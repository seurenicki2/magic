#include "stdafx.h"
#include "cHousingDecoPointDlg.h"
#include "WindowIDEnum.h"
#include "./interface/cListCtrl.h"
#include "cRitemEx.h"
#include "input/Mouse.h"
#include "cWindowManager.h"
#include "cMsgBox.h"
#include "../ChatManager.h"
#include "cHousingMgr.h"
#include "GameResourceManager.h"
#include "ItemManager.h"
#include "ObjectManager.h"

cHousingDecoPointDlg::cHousingDecoPointDlg()
{
	m_type = WT_HOUSING_DECOPOINT_DLG;
	m_nSelectBonus = -1;
}
cHousingDecoPointDlg::~cHousingDecoPointDlg()
{

}

//090818 pdy �Ͽ�¡ UI��ɺ��� cHousingDecoPointDlg�� �Ǵ��̾˷α׷� ����
void cHousingDecoPointDlg::Add(cWindow * window)
{
	if(window->GetType() == WT_PUSHUPBUTTON)
	{
		BYTE byCurIdx = 0 ;
		byCurIdx = curIdx1 ;

		AddTabBtn(curIdx1++, (cPushupButton * )window);

        if( byCurIdx >= 2 )
		{
			((cPushupButton*)window)->SetActive(FALSE) ;
		}
	}
	else if(window->GetType() == WT_DIALOG )
	{
		BYTE byCurIdx = 0 ;
		byCurIdx = curIdx2 ;

		AddTabSheet(curIdx2++, window);
	}
	else 
		cDialog::Add(window);
}

// 091211 pdy ��Ʈ ������ ListDlg�� 3���� Ȯ�� 
void cHousingDecoPointDlg::GetListDialogArrByTabNum(cListDialog** pArr,BYTE TabNum)
{
	cDialog* pCurDlg = (cDialog*)  GetTabSheet( TabNum ) ;

	if( pCurDlg->GetType() != WT_DIALOG )
	{
		pCurDlg = NULL;
	}

	cWindow* pTemp = NULL; 

	if( pCurDlg )
	{
		for( int i=0;i<eBonusListMaxType;i++ )
		{
			pTemp = pCurDlg->GetWindowForID( pCurDlg->GetID() + i +1 );

			if( pTemp && pTemp->GetType() != WT_LISTDIALOG )
			{
				pTemp = NULL ;
			}

			pArr[i] = (cListDialog*)pTemp;
		}
	}
	else
	{
		for( int i=0;i<eBonusListMaxType;i++ )
		{
			pArr[i] = NULL;
		}
	}
}

DWORD cHousingDecoPointDlg::ActionEvent(CMouse * mouseInfo)
{
	DWORD we = WE_NULL;
	if( !m_bActive )		return we;
	we = cTabDialog::ActionEvent(mouseInfo);

	cDialog* pCurDlg = (cDialog*)  GetTabSheet( m_bSelTabNum ) ;

	if( pCurDlg == NULL )
	{
		return we;
	}

	// 091211 pdy ��Ʈ ������ ListDlg�� 3���� Ȯ�� 
	cListDialog* pCurListDlgArr[eBonusListMaxType] = {0,};

	GetListDialogArrByTabNum(pCurListDlgArr,m_bSelTabNum);

	// ù��° ����Ʈ�� �������θ� �˻��Ѵ�.
	if( pCurListDlgArr[0] && pCurListDlgArr[0]->PtInWindow(mouseInfo->GetMouseX(), mouseInfo->GetMouseY()) )
	{
		if( we & WE_LBTNDBLCLICK )
		{
			cListDialog* pCurDecoPointListDlg = pCurListDlgArr[0];
			if( !pCurDecoPointListDlg )	
				return NULL ;

			int nSelectedLine = pCurDecoPointListDlg->GetCurSelectedRowIdx( ) ;

			ITEM* pItem = NULL ;
			pItem = pCurDecoPointListDlg->GetItem(nSelectedLine) ;
			if( ! pItem )
				return we;

			m_nSelectBonus = pItem->line ;

			//090527 pdy �Ͽ�¡ �ý��۸޼��� �ٹ̱� ����Ͻ� ���ѻ��� [�Ͽ�¡ Ȯ��]
			if( HERO->GetState() == eObjectState_Housing )
			{
				CHATMGR->AddMsg(CTC_SYSMSG,CHATMGR->GetChatMsg( 1874  ) );	//1874	"�ٹ̱� ��忡�� ����Ͻ� �� �����ϴ�."
				return we;
			}
			stHouseBonusInfo* pBonusInfo = GetCurSelectBonusInfo();

			DWORD dwErr = 0;
			if ( FALSE == HOUSINGMGR->CanUseDecoBonus(pBonusInfo,&dwErr) )
			{
				//����Ҽ����� ���ʽ��϶� 
				if( 1 == dwErr )
				{
					//�������� �ƴҶ� 
					//090527 pdy �Ͽ�¡ �ý��۸޼��� �������� �ƴҽ� ���ѻ��� [�ٹ̱⺸�ʽ����]
					CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1887) ); //1887	"�������� �ƴҽ� ����Ͻ� �� �����ϴ�"
					return we;

				}
				else if( 2 == dwErr )
				{
					//�ٹ̱�����Ʈ�� �����Ҷ�
					//090527 pdy �Ͽ�¡ �ý��۸޼��� �ٹ̱�����Ʈ ���� ���ѻ��� [�ٹ̱⺸�ʽ����]
					CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1863) ); //1863	"�ٹ̱� ����Ʈ�� �����մϴ�."
					return we;;
				}
				else if( 3 == dwErr )
				{
					//������Ʈ�� �����Ҷ�
					//090527 pdy �Ͽ�¡ �ý��۸޼��� ������Ʈ ���� ���ѻ��� [�ٹ̱⺸�ʽ����]
					CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1862) );	//1862	"�� ����Ʈ�� �����մϴ�."
					return we;;
				}
				else if( 4 == dwErr )
				{
					//������Ʈ�� �����Ҷ�
					//090527 pdy �Ͽ�¡ �ý��۸޼��� ��Ŀ�� �ƴҶ� ���ѻ��� [�ٹ̱⺸�ʽ����]
					CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1905) );	//1905	"��Ŀ�� �ƴϸ� ����Ҽ� �����ϴ�."
					return we;;
				}
			}

			char MsgBuf[128] = {0,};
			sprintf(MsgBuf,CHATMGR->GetChatMsg(1891), pBonusInfo->szName ); //1891	"%s�� �����Ͻðڽ��ϱ�?"

			//090527 pdy �Ͽ�¡ �˾�â [�ٹ̱⺸�ʽ����]
			WINDOWMGR->MsgBox( MBI_USEDECOBONUS_AREYOUSURE, MBT_YESNO,MsgBuf ) ;
			SetDisable(TRUE) ;
		}
	}

	return we;
}

// 091211 pdy ��Ʈ ������ ListDlg�� 3���� Ȯ�� 
void cHousingDecoPointDlg::OnActionEvent( LONG id, void* p, DWORD event )
{
	cListDialog* pCurListDlgArr[eBonusListMaxType] = {0,};

	GetListDialogArrByTabNum(pCurListDlgArr,m_bSelTabNum);

	if( pCurListDlgArr[0] == NULL )
		return; 

	switch( id )
	{
		case HOUSING_DECOPOINT_LIST01_01:
		case HOUSING_DECOPOINT_LIST02_01:
		{
			switch( event )
			{
			case WE_ROWCLICK:
				{
					int nClickIdx = pCurListDlgArr[0]->GetClickedRowIdx();

					for( int i = 1 ; i < eBonusListMaxType ; i ++ )
					{
						if( pCurListDlgArr[i] && pCurListDlgArr[i]->GetClickedRowIdx() != nClickIdx )
							pCurListDlgArr[i]->SetCurSelectedRowIdxWithoutRowMove( nClickIdx );
					}
				}
				break;
			case WE_TOPROWCHANGE:
				{
					int nTopListItemIdx = pCurListDlgArr[0]->GetTopListItemIdx();

					for( int i = 1 ; i < eBonusListMaxType ; i ++ )
					{
						if( pCurListDlgArr[i] && pCurListDlgArr[i]->GetTopListItemIdx() != nTopListItemIdx )
							pCurListDlgArr[i]->SetTopListItemIdx( nTopListItemIdx );

					}
				}
				break;
			}
		}
		break;
	}
}

void cHousingDecoPointDlg::Linking()
{
	m_pStarPointStatic = (cStatic*)GetWindowForID(HOUSING_STARPOINT_STATIC); 
	m_pStarPointStatic->SetStaticText("");

	//090818 pdy �Ͽ�¡ UI��ɺ��� cHousingDecoPointDlg�� �Ǵ��̾˷α׷� ����
	for(int i = 0 ; i < 2 ; i ++ )
	{
		cDialog* pCurDlg = (cListDialog*) GetTabSheet( i ) ;
		
		if( !pCurDlg )	continue;

		cListDialog* pCurListDlgArr[eBonusListMaxType] = {0,};

		GetListDialogArrByTabNum(pCurListDlgArr,m_bSelTabNum);

		for( int j = 0 ; j < eBonusListMaxType ; j ++ )
		{
			if( pCurListDlgArr[j] )
			{
				 pCurListDlgArr[j]->SetShowSelect( TRUE );
				 pCurListDlgArr[j]->SetAutoScroll( FALSE );
			}
		}
	}

	InitBonusList();
}

//090818 pdy �Ͽ�¡ UI��ɺ��� cHousingDecoPointDlg�� �Ǵ��̾˷α׷� ����
void cHousingDecoPointDlg::SelectTab(BYTE idx)
{
	cTabDialog::SelectTab( idx ) ;

	//���� �ٲ�� ���õ� ���ʽ� ������ �ʱ�ȭ
	m_nSelectBonus = -1;
}

//090818 pdy �Ͽ�¡ UI��ɺ��� cHousingDecoPointDlg�� �Ǵ��̾˷α׷� ����
void cHousingDecoPointDlg::Render()
{
	cDialog::RenderWindow();
	cDialog::RenderComponent();
	cTabDialog::RenderTabComponent();
}

stHouseBonusInfo* cHousingDecoPointDlg::GetCurSelectBonusInfo()
{	
	if( m_nSelectBonus == -1 )
		return NULL;

	cListDialog* pCurListDlgArr[eBonusListMaxType] = {0,};

	GetListDialogArrByTabNum(pCurListDlgArr,m_bSelTabNum);

	cListDialog* pCurDecoPointListDlg = pCurListDlgArr[0] ;

	if( !pCurDecoPointListDlg )
		return NULL ;

	ITEM* pItem = NULL ;
	pItem = pCurDecoPointListDlg->GetItem(m_nSelectBonus) ;
	if( ! pItem )
		return NULL;

	cPtrList* pCurList = NULL;

	switch( m_bSelTabNum )
	{
	case 0:		pCurList = &m_DecoNormalBonusList;break;
	case 1:		pCurList = &m_DecoSpacialBonusList;break;
	}

	if( ! pCurList )
		return NULL;

	PTRLISTPOS Pos = pCurList->FindIndex(m_nSelectBonus);

	if(! Pos )
		return NULL;

	return (stHouseBonusInfo*)pCurList->GetAt(Pos);
}

void cHousingDecoPointDlg::InitBonusList()
{
	CYHHashTable<stHouseBonusInfo>* pBonusList = GAMERESRCMNGR->GetHouseBonusList();
	pBonusList->SetPositionHead();

	for(stHouseBonusInfo* pInfo = pBonusList->GetData();
		0 < pInfo;
		pInfo = pBonusList->GetData())
	{
		BOOL bInsert = FALSE;
		cPtrList* pCurList = (pInfo->byKind == 0 )? &m_DecoNormalBonusList : &m_DecoSpacialBonusList;

		//�ٹ̱�����Ʈ����� ���Ͽ� ���� 
		PTRLISTPOS pos = pCurList->GetHeadPosition();
		while( pos )
		{
			PTRLISTPOS OldPos = pos;
			stHouseBonusInfo * pOldInfo = (stHouseBonusInfo*)pCurList->GetNext( pos ) ;
			if( pInfo->dwDecoPoint < pOldInfo->dwDecoPoint )
			{
				pCurList->InsertBefore(OldPos ,pInfo );
				bInsert = TRUE;
				break;
			}
		}
		if( ! bInsert )
			pCurList->AddTail(pInfo);
	}

	cPtrList* pBonusListArr[2] = { &m_DecoNormalBonusList , &m_DecoSpacialBonusList };

	for( int j= 0; j < 2 ; j++ )
	{
		cPtrList* pCurBonusList =  pBonusListArr[j];

		cListDialog* pCurListDlgArr[eBonusListMaxType] = {0,};
		GetListDialogArrByTabNum(pCurListDlgArr,j);

		BOOL bCheck = FALSE ;
		for( int cnt = 0; cnt < eBonusListMaxType ; cnt ++ )
		{
			if( pCurListDlgArr[cnt] == NULL )
			{
				bCheck = TRUE ;
				break;
			}
		}

		if(  bCheck == TRUE )
			continue;

		//����Ʈ��Ʈ�ѿ� ���ĵ� ����Ʈ�� ����
		char buf[256] = {0,};
		DWORD color = RGB_HALF( 255, 0, 0 );

		int i=0;
		PTRLISTPOS pos = pCurBonusList->GetHeadPosition();
		while( pos )
		{
			char NameBuf[64] = {0,};
			stHouseBonusInfo* pBonus = (stHouseBonusInfo*)pCurBonusList->GetNext( pos ) ;
			sprintf( NameBuf,"%s",pBonus->szName);

			char PointBuf[32];	//����Ʈ�� 7�ڸ�����
			memset( PointBuf , ' ',sizeof( PointBuf ) );
			sprintf( buf, "%d" , pBonus->dwDecoPoint);
			sprintf( PointBuf+ (7-strlen(buf)),"%s",buf);		//���ڴϱ� ���ڸ�����

			char UseBuf[32];	//�Ҹ�����Ʈ�� 3�ڸ�����
			memset( UseBuf , ' ',sizeof( UseBuf ) );
			sprintf( buf, "%d" , pBonus->dwUsePoint);
			sprintf( UseBuf+ (3-strlen(buf)),"%s",buf);				//���ڴϱ� ���ڸ�����


			cListDialog::ToolTipTextList ToolTipTextList;
			const std::vector< std::string >& cutTooltipList = ITEMMGR->GetToolTipMsg( pBonus->dwTooltipIndex );
			if( cutTooltipList.empty() )
			{
				pCurListDlgArr[eBonusNameList]->AddItem(NameBuf, color ,i);
				pCurListDlgArr[eDecoPointList]->AddItem(PointBuf, color ,i);
				pCurListDlgArr[eUsePointList]->AddItem(UseBuf, color ,i);
			}
			else
			{
				for( std::vector< std::string >::const_iterator cutTooltipListIter = cutTooltipList.begin() ; cutTooltipListIter != cutTooltipList.end() ; ++cutTooltipListIter )
				{
					const std::string strTooltip = *cutTooltipListIter;
					ToolTipTextList.push_back( strTooltip.c_str() );
				}

				pCurListDlgArr[eBonusNameList]->AddItem(NameBuf, color ,ToolTipTextList,i);
				pCurListDlgArr[eDecoPointList]->AddItem(PointBuf, color ,ToolTipTextList,i);
				pCurListDlgArr[eUsePointList]->AddItem(UseBuf, color ,ToolTipTextList,i);
			}

			i++;
		}
	}
}

void cHousingDecoPointDlg::SetActive( BOOL val )
{
	if( m_bDisable ) return;

	if(! HOUSINGMGR->IsHouseInfoLoadOk() || ! HOUSINGMGR->IsHouseOwner() )
	{
		//�Ͽ콺 �ε��� �Ǿ����� ������ ���� (�Ͽ�¡ �ʿ� ������ �ε��� �Ϸ�)
		//�������� �ƴϸ� ����  
		val = FALSE;
	}

	for( int i = 0 ; i < 2 ; i ++ )
	{
		cListDialog* pCurListDlgArr[eBonusListMaxType] = {0,};
		GetListDialogArrByTabNum(pCurListDlgArr,i);

		for( int j = 0 ; j < eBonusListMaxType ; j++ )
		{
			if( pCurListDlgArr[j] )
				pCurListDlgArr[j]->SetTopListItemIdx( 0 ) ;
		}
	}

	m_nSelectBonus = -1;

	cTabDialog::SetActive(val);
}

void cHousingDecoPointDlg::RefreshBonusList(DWORD dwDecoPoint,DWORD dwStarPoint)
{
	cPtrList* pBonusListArr[2] = { &m_DecoNormalBonusList , &m_DecoSpacialBonusList };

	for( int j = 0; j < 2 ; j++ )
	{
		cPtrList* pCurBonusList =  pBonusListArr[j];

		cListDialog* pCurListDlgArr[eBonusListMaxType] = {0,};
		GetListDialogArrByTabNum(pCurListDlgArr,j);

		BOOL bCheck = FALSE ;
		for( int cnt = 0; cnt < eBonusListMaxType ; cnt ++ )
		{
			if( pCurListDlgArr[cnt] == NULL )
			{
				bCheck = TRUE ;
				break;
			}
		}

		if(  bCheck == TRUE )
			continue;

		PTRLISTPOS pos = pCurBonusList->GetHeadPosition();

		char Buf[32]={0};

		sprintf(Buf,"%d",dwStarPoint);
		m_pStarPointStatic->SetStaticText(Buf);

		//����Ʈ, ���ʽ� ������ �°� ������ �ٲ��� 
		int i=0;
		while(pos)
		{
			stHouseBonusInfo* pBonus = (stHouseBonusInfo*)pCurBonusList->GetNext( pos ) ;
			ITEM* pCurItem[eBonusListMaxType] = {0,};
			for( int j=0; j < eBonusListMaxType ; j++ )
			{
				pCurItem[j] = pCurListDlgArr[j]->GetItem(i);
			}

			if( !pCurItem )
				continue;

			BOOL bCanUseBonus = TRUE;

			if( pBonus->dwDecoPoint > dwDecoPoint ||  pBonus->dwUsePoint > dwStarPoint )
				bCanUseBonus = FALSE;

			if( pBonus->byKind == 1 && ! HOUSINGMGR->IsRankHouse() )
				bCanUseBonus = FALSE;

			if( bCanUseBonus )
			{
				////Ÿ�Կ� �°� ������ �ٲ��� 
				//switch(pBonus->byKind)
				//{
				//case 1:	//��Ŀ���� ���ʽ� 
				//	{
				//		pCurItem->rgb = RGB_HALF( 255, 255, 0 );
				//	}
				//	break;
				//default:
					for( int j=0; j < eBonusListMaxType ; j++ )
					{
						pCurItem[j]->rgb = RGB_HALF( 255, 255, 255 );
					}
				//	break;
				//}
			}
			else	//��밡��
			{
				//ȸ������ �ٲ���
				for( int j=0; j < eBonusListMaxType ; j++ )
				{
					pCurItem[j]->rgb = RGB_HALF( 150 , 150 ,150  );
				}
			}

			i++;
		}
	}
}
