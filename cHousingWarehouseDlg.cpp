#include "stdafx.h"
#include "cHousingWarehouseDlg.h"
#include "WindowIDEnum.h"
#include "Interface/cWindowManager.h"
#include "Interface/cIconGridDialog.h"
#include "cHousingMgr.h"
#include "ChatManager.h"
#include "cScrollIconGridDialog.h"
#include "cHousingDecoPointDlg.h"
#include "GameIn.h"
#include "PKManager.h"
#include "Item.h"
#include "cHousingStoredIcon.h"
#include "ItemManager.h"
#include "cMsgBox.h"
#include "ObjectStateManager.h"
#include "ObjectManager.h"

cHousingWarehouseDlg::cHousingWarehouseDlg()
{
	m_type				= WT_HOUSING_WAREHOUSE_DLG;

	m_StoredIconHash.Initialize( 100 );
}

cHousingWarehouseDlg::~cHousingWarehouseDlg()
{
	m_StoredIconHash.RemoveAll();
}
void cHousingWarehouseDlg::Add(cWindow * window)
{
// 071210 LYW --- InventoryExDialog : �κ��丮 Ȯ�忡 ���� Ȯ��������� Ȱ��ȭ ���� ó��.
	if(window->GetType() == WT_PUSHUPBUTTON)
	{
		BYTE byCurIdx = 0 ;
		byCurIdx = curIdx1 ;

		AddTabBtn(curIdx1++, (cPushupButton * )window);

        if( byCurIdx >= 2 )
		{
			//window->SetActive(FALSE) ;
			((cPushupButton*)window)->SetActive(FALSE) ;
		}
	}
	else if(window->GetType() == WT_SCROLLICONGRIDDLG)
	{
		BYTE byCurIdx = 0 ;
		byCurIdx = curIdx2 ;

		AddTabSheet(curIdx2++, window);
		((cIconGridDialog*)window)->SetDragOverIconType( WT_ITEM );
	}
	else 
		cDialog::Add(window);
}

DWORD cHousingWarehouseDlg::ActionEvent(CMouse * mouseInfo)
{
	return cTabDialog::ActionEvent( mouseInfo );	
}

void cHousingWarehouseDlg::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	if(! HOUSINGMGR->IsHouseInfoLoadOk() || ! HOUSINGMGR->IsHouseOwner() )
	{
		//�Ͽ콺 �ε��� �Ǿ����� ������ ���� (�Ͽ�¡ �ʿ� ������ �ε��� �Ϸ�)
		//�������� �ƴϸ� ����  
		val = FALSE;
	}

	cTabDialog::SetActive(val);
}

void cHousingWarehouseDlg::Render()
{
	cDialog::RenderWindow();
	cDialog::RenderComponent();
	cTabDialog::RenderTabComponent();
}

void cHousingWarehouseDlg::Linking()
{
	//m_BtDecorationMode  = (cButton*)GetWindowForID(HOUSING_WH_DECOMODE_BTN);
	//m_BtDecoPoint		= (cButton*)GetWindowForID(HOUSING_WH_DECOPOINT_BTN);
	m_BtLeftScroll		= (cButton*)GetWindowForID(HOUSING_WH_LEFT_BTN);
	m_BtRightScroll		= (cButton*)GetWindowForID(HOUSING_WH_RIGHT_BTN);
	m_BtUpScroll		= (cButton*)GetWindowForID(HOUSING_WH_UP_BTN);
	m_BtDownScroll		= (cButton*)GetWindowForID(HOUSING_WH_DOWN_BTN);
	m_pStatic_Basic		= (cStatic*) GetWindowForID( HOUSING_WH_STATIC_TABBTN_BASIC_TEXTPOS ) ;
	m_pStatic_Push		= (cStatic*) GetWindowForID( HOUSING_WH_STATIC_TABBTN_PUSH_TEXTPOS ) ;

	//090714 pdy �ǹ�ư ��� ����
	//���ʿ� �ѹ� ����Ʈ���� ������ ������ �ǹ�ư�� �ؽ�Ʈ ��ġ�����̴�
	SelectTab(m_bSelTabNum);
}


void cHousingWarehouseDlg::OnActionEvent(LONG lId, void * p, DWORD we)
{
	switch(lId)
	{
		case HOUSING_WH_LEFT_BTN:
			{
				cScrollIconGridDialog* pDlg = (cScrollIconGridDialog*)GetTabSheet( GetCurTabNum() );
				pDlg->SetWheelProcessXY(1,0);
				pDlg->WheelEventProcess(1);
				pDlg->SetWheelProcessXY(0,1);
				pDlg->SortShowGridIconPos();
			}
			break;
		case HOUSING_WH_RIGHT_BTN:
			{
				cScrollIconGridDialog* pDlg = (cScrollIconGridDialog*)GetTabSheet( GetCurTabNum() );
				pDlg->SetWheelProcessXY(1,0);
				pDlg->WheelEventProcess(-1);
				pDlg->SetWheelProcessXY(0,1);
				pDlg->SortShowGridIconPos();
			}
			break;
		case HOUSING_WH_UP_BTN:
			{
				cScrollIconGridDialog* pDlg = (cScrollIconGridDialog*)GetTabSheet( GetCurTabNum() );
				pDlg->SetWheelProcessXY(0,1);
				pDlg->WheelEventProcess(1);
				pDlg->SortShowGridIconPos();
			}
			break;
		case HOUSING_WH_DOWN_BTN:
			{
				cScrollIconGridDialog* pDlg = (cScrollIconGridDialog*)GetTabSheet( GetCurTabNum() );
				pDlg->SetWheelProcessXY(0,1);
				pDlg->WheelEventProcess(-1);
				pDlg->SetWheelProcessXY(1,0);
				pDlg->SortShowGridIconPos();
			}
			break;
		//case HOUSING_WH_DECOPOINT_BTN:
		//	{
		//		cHousingDecoPointDlg* pDlg = (cHousingDecoPointDlg*) GAMEIN->GetHousingDecoPointDlg();
		//		pDlg->SetActive(TRUE);
		//	}
		//	break;
		case HOUSING_WH_TABDLG1:
		case HOUSING_WH_TABDLG2:
		case HOUSING_WH_TABDLG3:
		case HOUSING_WH_TABDLG4:
		case HOUSING_WH_TABDLG5:
		case HOUSING_WH_TABDLG6:
		case HOUSING_WH_TABDLG7:
		case HOUSING_WH_TABDLG8:
		case HOUSING_WH_TABDLG9:
			{
				if(we == WE_LBTNDBLCLICK)
				{
					DWORD CurTabNum = GetCurTabNum();
					cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet(CurTabNum);
					if( ! pGridDlg)
						return; 

					int CurSelCellPos = pGridDlg->GetCurSelCellPos();
					if( CurSelCellPos == -1 )
						return;

					cIcon* pIcon = pGridDlg->GetIconForIdx(CurSelCellPos);
					if(!pIcon) 
						return;

					UseIcon(pGridDlg->GetIconForIdx(CurSelCellPos));
						//if ( �۴��� ����Ʈ[GetCurTabNum()][CurSelCellPos].������Ʈ == ���� ) 
						//�������̸� �ٹ̱���� (���̸����..)
						//else if( �۴��� ����Ʈ[GetCurTabNum()][CurSelCellPos].������Ʈ == ��ġ )
								//��ġ���� ��Ŷ�� ������ 
						//}

				}
			}
			break;
	}
}

BOOL cHousingWarehouseDlg::FakeMoveIcon(LONG x, LONG y, cIcon * icon)
{
	ASSERT(icon);
	if( m_bDisable )	return FALSE;

	if( PKMGR->IsPKLooted() )	return FALSE;	//�׾����� �ٸ�����.. //KES 040801

	if( icon->GetType() == WT_STALLITEM || icon->GetType() == WT_EXCHANGEITEM ) return FALSE;

	if(icon->GetType() == WT_ITEM)
	{
		if( FALSE == FakeMoveItem(x, y, (CItem *)icon) )
		{
			//090527 pdy �Ͽ�¡ �ý��۸޼��� [������������]
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1873 ) ); //1873	"�ش� �������� �̵� �� �� �����ϴ�."
		}
	}
	return FALSE;
}

BOOL cHousingWarehouseDlg::FakeMoveItem(LONG mouseX, LONG mouseY, CItem * pFromItem)
{
	//���� �������̾ƴϸ� �ȵ� 
	if( pFromItem->GetItemInfo()->SupplyType != ITEM_KIND_FURNITURE ) 
		return FALSE;

	if( pFromItem->IsLocked() ) return FALSE;

		//���ö��� ������ �۴��� �ε����� ��������. 
	DWORD dwFromItemCategory = Get_HighCategory( pFromItem->GetItemInfo()->SupplyValue );

	if( (dwFromItemCategory) >= eFN_HighCatefory_Max ) 
		return FALSE;


	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet(dwFromItemCategory-1);	
	if( ! pGridDlg)
		return FALSE; 

	WORD ToPos = 0;

	//if( dwCategoryTabNum == m_bSelTabNum )
	//{
	//	//������� ���ϴ� ��ġ�� �������� �ִ� ������<->���������� ��ü�� �ɼ��� �ִ�
	//	//�ϴ� ���ϴ���ġ�� ���ڸ��϶��� ó�������� 
	//	pGridDlg->GetPositionForXYRef(mouseX,mouseY,ToPos);
	//	cIcon* pIcon = pGridDlg->GetIconForIdx(ToPos);
	//	if( pIcon )
	//	{
	//		//������ < - > ���������� ���� ���߿� �߰��ɱ�? 
	//		return FALSE;
	//	}
	//}
	//else if(HOUSINGMGR->GetStoredFunitureNumByCategory( dwCategoryTabNum+1 ) > MAX_HOUSING_SLOT_NUM)
	//{
	//	//ī�װ��� ���������� ���ڸ��� ���ٸ� ���� 
	//	return FALSE;
	//}

	DWORD dwCurActivedCategory = GetCurTabNum()+1;

	if( HOUSINGMGR->GetStoredFunitureNumByCategory( dwCurActivedCategory ) >= MAX_HOUSING_SLOT_NUM)
	{
		//���� ���� ���� ������ ������� �ʴٸ� ���� 
		return FALSE;
	}

	if( dwFromItemCategory != dwCurActivedCategory ) 
	{
		//���� ���� �ܰ� �������� ������ ���� �ٸ��ٸ� 
		//������ ���� ������ �ִ��� Ȯ��
		if( HOUSINGMGR->GetStoredFunitureNumByCategory( dwFromItemCategory ) >= MAX_HOUSING_SLOT_NUM)
		{
			return FALSE;
		}
	}

	//����� ������� �󽽷���ġ�� ������ ������ ��û���� 
	ToPos = HOUSINGMGR->GetBlankSlotIndexFromStoredFunitureListByCategory( dwFromItemCategory );
	return HOUSINGMGR->RequestStoredItem(ToPos,pFromItem);
}

BOOL cHousingWarehouseDlg::AddIcon(stFurniture* pstFuniture)
{
	if( !pstFuniture )
		return FALSE;

	cHousingStoredIcon* pNewIcon = MakeHousingStoredIcon(pstFuniture);

	return AddIcon(pNewIcon);
}

BOOL cHousingWarehouseDlg::AddIcon(cIcon* pIcon)
{
	if(!pIcon || pIcon->GetType() != WT_HOUSING_STORED_ICON)
		return FALSE;

	cHousingStoredIcon* pHsStoredIcon =  (cHousingStoredIcon*)pIcon;

	stFurniture* pFurniture = pHsStoredIcon->GetLinkFurniture();
	if( ! pFurniture)
		return FALSE;

	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet((pFurniture->wCategory-1));
	if( ! pGridDlg )
		return FALSE;

	//090713 pdy �Ͽ�¡ â�� ���� ��� �߰� -- �ǵڷ� Add�� ���� 
	if( ! pGridDlg->AddIcon( MAX_HOUSING_SLOT_NUM - 1 , pIcon) )
	{
		WINDOWMGR->DeleteWindow(pIcon);
		return FALSE;
	}

	m_StoredIconHash.Add( pHsStoredIcon , pFurniture->dwObjectIndex );

	//Refresh�Ǹ� �ȿ��� ���¿� �°� ���ĵ� ���ش�. ������ ��ĭ���� ���º��� ��������..
	RefreshIcon( pFurniture );

	return TRUE;
}

cHousingStoredIcon* cHousingWarehouseDlg::MakeHousingStoredIcon(stFurniture* pstFuniture)
{
	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo( pstFuniture->dwFurnitureIndex );
	if(!stFurnitureInfo )
	{
		return NULL;
	}

	cHousingStoredIcon* pNewIcon = new cHousingStoredIcon( pstFuniture );
	pNewIcon->SetSeal(FALSE);

	cImage image;

	long IconID = IG_HS_STOREDICON_START + ( MAX_HOUSING_SLOT_NUM * (pstFuniture->wCategory-1) ) + pstFuniture->wSlot;
	pNewIcon->Init(
		0,
		0,
		DEFAULT_ICONSIZE,
		DEFAULT_ICONSIZE,
		ITEMMGR->GetIconImage(  pstFuniture->dwLinkItemIndex , &image ),
		IconID );
	pNewIcon->SetData( pstFuniture->dwLinkItemIndex );
	WINDOWMGR->AddWindow(pNewIcon);

	return pNewIcon;
}

//BOOL cHousingWarehouseDlg::DeleteIcon(WORD wTapNum , WORD wSlot)
//{	
//	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet(wTapNum);
//	if( ! pGridDlg)
//		return FALSE; 
//
//	cIcon* pIcon = pGridDlg->GetIconForIdx(wSlot);
//
//	if(!pIcon )
//		return FALSE;
//
//	pGridDlg->DeleteIcon( pIcon  );
//	WINDOWMGR->DeleteWindow(pIcon);
//
//	return FALSE;
//}

//090708 pdy ��ȹ�� ��û������ ���� ���¿����� ���ı�� �߰� 
BOOL cHousingWarehouseDlg::DeleteIcon(stFurniture* pFurniture)
{
	if( pFurniture == NULL )
		return FALSE;

	WORD wTapNum = pFurniture->wCategory -1 ;

	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet(wTapNum);
	if( ! pGridDlg)
		return FALSE; 

	cHousingStoredIcon* pResultIcon = m_StoredIconHash.GetData( pFurniture->dwObjectIndex );
	
	if( pResultIcon == NULL )
		return FALSE;
	
	m_StoredIconHash.Remove( pFurniture->dwObjectIndex );
	pGridDlg->DeleteIcon( pResultIcon );
	WINDOWMGR->DeleteWindow( pResultIcon );

	//�������� �����Ǹ� �ش� ���� ���Ľ�������.
	SortIconByTapNum( wTapNum );

	return TRUE;
}

void cHousingWarehouseDlg::SelectTab(BYTE idx)
{
	//090714 pdy �ǹ�ư ��� ����
	BYTE byOldTab = GetCurTabNum();
	BYTE byNewTab = idx;

	if(  m_pStatic_Basic && m_pStatic_Push )
	{
		// �ǹ�ư�� ���¿����� �̹����� �ٸ��� (Ǫ���ɶ� �������� �̵�) 
		// ���¿� �°� �ؽ�Ʈ�� �̵���Ű��.
		cPushupButton* pPushBt			  = (cPushupButton*) GetWindowForID( HOUSING_WH_TABBTN1 + byNewTab ) ;
		cPushupButton* pOldPushBt		  = (cPushupButton*) GetWindowForID( HOUSING_WH_TABBTN1 + byOldTab ) ;

		if( pPushBt )
		{
			pPushBt->SetTextXY( m_pStatic_Push->GetRelX() , m_pStatic_Push->GetRelY() );
			pPushBt->SetRenderArea();
		}

		if( pOldPushBt && byOldTab != byNewTab )
		{
			pOldPushBt->SetTextXY( m_pStatic_Basic->GetRelX() , m_pStatic_Basic->GetRelY() );
			pOldPushBt->SetRenderArea();
		}
	}
	cTabDialog::SelectTab(idx);

	cScrollIconGridDialog* pDlg = (cScrollIconGridDialog*)GetTabSheet( idx );
	pDlg->InitScrollXY();
}

void cHousingWarehouseDlg::UseIcon(cIcon* pIcon)
{
	if(!pIcon || pIcon->GetType() != WT_HOUSING_STORED_ICON)
		return;

	cHousingStoredIcon* pStoredIcon = (cHousingStoredIcon*)pIcon;
	stFurniture* pstFuniture = pStoredIcon->GetLinkFurniture();

	if(! pstFuniture)
		return;

	if(HOUSINGMGR->IsDoDecoration() &&
		(pstFuniture->wState == eHOUSEFURNITURE_STATE_UNINSTALL ||
		pstFuniture->wState == eHOUSEFURNITURE_STATE_KEEP))
	{
		HOUSINGMGR->CancelDecoration();
	}
	else if(pstFuniture->wState == eHOUSEFURNITURE_STATE_INSTALL)
	{
		return;
	}

	//091224 NYJ eObjectState_None�� �ƴϸ� ����.
	if( (pstFuniture->wState == eHOUSEFURNITURE_STATE_UNINSTALL ||
		pstFuniture->wState == eHOUSEFURNITURE_STATE_KEEP) &&
		(HERO->GetState() != eObjectState_None &&
		HERO->GetState() != eObjectState_Housing))
	{
		CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(2010) /*�������� ��ġ�� �� ���� �����Դϴ�.��*/ );
		return;
	}

	//090527 pdy �Ͽ�¡ �ý��۸޼��� �ٹ̱� ��尡 �ƴҽ� ���ѻ��� [���͸���ü]
	if( (pstFuniture->wState == eHOUSEFURNITURE_STATE_UNINSTALL ||
		pstFuniture->wState == eHOUSEFURNITURE_STATE_KEEP) &&
		HERO->GetState() != eObjectState_Housing )
	{
		//CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1878) );	//1878	"�ٹ̱� ����϶��� ����Ͻ� �� �ֽ��ϴ�."
		//return;

		//091224 NYJ �ٹ̱��尡 �ƴϸ� �ٹ̱� ���� ��ȯ
		HOUSINGMGR->RequestDecoRationMode();
		HOUSINGMGR->SetDecorationModeTemp();
	}

//	pStoredIcon->SetLock(TRUE) ;								// ����� �������� ��ٴ�.

	if( pstFuniture->bNotDelete )								//�⺻��ġ ǰ���ϰ�� �����ۻ�� ���� 
	if(! IsDoor( pstFuniture->dwFurnitureIndex)  )				//�⺻��ġ���� �����ۻ�밡�� 
	if(! IsStart( pstFuniture->dwFurnitureIndex) )				//�⺻��ġ ��ŸƮ�� �����ۻ�밡�� 
		return;

	/*
	if( pstFuniture->wState == eHOUSEFURNITURE_STATE_INSTALL )
	{
		//�⺻��ġ ǰ���̳� ������ ��ġ �Ұ� 
		if( pstFuniture->bNotDelete	|| IsDoor( pstFuniture->dwFurnitureIndex ) )
		{
			//090527 pdy �Ͽ�¡ �ý��۸޼��� �⺻�������� [��ġ����]
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1888 ) );	//1888	"�⺻ ��ǰ�� ��ġ���� �� �� �����ϴ�."
		}
		////091223 NYJ - ����â���� ����Ŭ������ ����ȸ����� ����.
		//else
		//{
		//	HOUSINGMGR->RequestUninstallDecoFromUseIcon(pstFuniture);
		//}
		return;
	}
	*/

	//��ġ���̾ƴϸ� �����ٹ̱� ����
	HOUSINGMGR->StartDecoration(pstFuniture);
}

//void cHousingWarehouseDlg::RefreshIcon(WORD wTapNum , WORD wSlot)
//{
//	cHousingStoredIcon* pStoredIcon = NULL;
//
//	pStoredIcon =GetStoredIconIcon(wTapNum , wSlot);
//
//	if( pStoredIcon ) 
//		pStoredIcon->Refresh();
//}

//090708 pdy ��ȹ�� ��û������ ���� ���¿����� ���ı�� �߰� 
void cHousingWarehouseDlg::RefreshIcon(stFurniture* pFurniture)
{
	if( pFurniture == NULL )
		return;

	WORD wTapNum = pFurniture->wCategory -1 ;

	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet(wTapNum);
	if( ! pGridDlg)
		return; 

	//���ı�ɶ����� ������ ���� ������ġ�� ������UI���� ������ġ�� ���� �ʴ�. 
	//������ ������ �ؽÿ��� ���� Obj�ε����� �������� ��������  

	cHousingStoredIcon* pResultIcon = m_StoredIconHash.GetData( pFurniture->dwObjectIndex );

	if( pResultIcon == NULL )
		return;

	//������ ���°� �ٲ�� ������UI���� �ش����� ��ġ������ �ٽ� ���־�� �Ѵ�. 
	SortIconByTapNum(wTapNum) ;

	pResultIcon->Refresh();

	WORD wPosition = 0; pGridDlg->GetPositionForCell( pResultIcon->GetCellX() ,pResultIcon->GetCellY() );

	if( IsDoor( pFurniture->dwFurnitureIndex ) )
	{
		for(int i = 0; i < MAX_HOUSING_SLOT_NUM ; i++ )
		{
			cHousingStoredIcon* pDoorIcon = (cHousingStoredIcon*)pGridDlg->GetIconForIdx( i );

			if( pDoorIcon == NULL || IsDoor( pDoorIcon->GetLinkFurniture()->dwFurnitureIndex ) == FALSE )
				continue;

			if( pDoorIcon->GetLinkFurniture()->wState == eHOUSEFURNITURE_STATE_INSTALL )
			{
				wPosition = pGridDlg->GetPositionForCell( pDoorIcon->GetCellX() ,pDoorIcon->GetCellY() );
				break;
			}
		}
	}
	else 
	{
		wPosition = pGridDlg->GetPositionForCell( pResultIcon->GetCellX() ,pResultIcon->GetCellY() );
	}
	
	//������ ������ ���� ���õ� ����ġ�� �ٽ� ���߾� �ش�.
	pGridDlg->SetCurSelCellPos( wPosition );
}

cHousingStoredIcon* cHousingWarehouseDlg::GetStoredIconIcon(WORD wTapNum , WORD wSlot)
{
	cHousingStoredIcon* pStoredIcon = NULL;

	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet(wTapNum);
	if( ! pGridDlg)
		return NULL; 

	pStoredIcon =(cHousingStoredIcon*) pGridDlg->GetIconForIdx( wSlot ) ;

	return pStoredIcon;
}

void cHousingWarehouseDlg::FakeDeleteIcon(cHousingStoredIcon* pStoredIcon)
{
	stFurniture* pstFurniture = pStoredIcon->GetLinkFurniture();

	if(! pstFurniture )
		return ;

	if( pstFurniture->bNotDelete )
		return;

	if( pstFurniture->wState == eHOUSEFURNITURE_STATE_INSTALL )
	{
		return;
	}

	Set_QuickSelectedIcon(pStoredIcon) ;

	//090527 pdy �Ͽ�¡ �˾�â [��������]
	WINDOWMGR->MsgBox( MBI_HOUSE_DESTROY_FUTNITURE_AREYOUSURE , MBT_YESNO, CHATMGR->GetChatMsg(209)) ;
}

//090708 pdy ��ȹ�� ��û������ ���� ���¿����� ���ı�� �߰� 
void cHousingWarehouseDlg::SortIconByTapNum(WORD wTapNum)
{
	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet(wTapNum);
	if( ! pGridDlg)
		return;

	//�ش� ���� ��� �������� ����鼭 ������ �ּҸ� ��� �Ѵ�.
	cIcon* TempArr[MAX_HOUSING_SLOT_NUM] = {0,};

	for(int i = 0; i < MAX_HOUSING_SLOT_NUM ; i++ )
	{
		pGridDlg->DeleteIcon( i , &TempArr[i] );
	}

	//�������� ����Ͽ����� ���� ���°��� ���� ��ĭ���� �������� �������ش�. 

	//�켱���� 1 : ��ġ���� ���� ������
	//�켱���� 2 : ��ġ �� �� ������ ���� ������ 
	//�켱���� 3 : ��ġ ���� ���� ������ 

	//�켱������ ���� Temp����Ʈ�� ���� �����
	cPtrList TempListNo1, TempListNo2, TempListNo3;

	for(int i=0; i < MAX_HOUSING_SLOT_NUM ; i++ )
	{
		if( TempArr[i] == NULL ||  TempArr[i]->GetType() != WT_HOUSING_STORED_ICON )
			continue;

		cHousingStoredIcon* pHsStoredIcon = (cHousingStoredIcon*) TempArr[i] ;

		stFurniture* pLinkFurniture = pHsStoredIcon->GetLinkFurniture();

		if( pLinkFurniture == NULL || pLinkFurniture->wState == eHOUSEFURNITURE_STATE_UNKEEP)
			continue;


		switch(pLinkFurniture->wState)
		{
			case eHOUSEFURNITURE_STATE_KEEP:		//��ġ ���� ���� 
				{
					TempListNo1.AddTail( TempArr[i] );
				}
				break;
			case eHOUSEFURNITURE_STATE_UNINSTALL :	//��ġ �� �� ������ ���� ������
				{
					TempListNo2.AddTail( TempArr[i] );
				}
				break;
			case eHOUSEFURNITURE_STATE_INSTALL:		//��ġ ���� ���� ������ 
				{
					TempListNo3.AddTail( TempArr[i] );
				}
				break;
		}
	}

	//�켱������ �°� ����Ʈ�� ��Ҵٸ� ������� �ٽ� ADD 
	WORD wIconPos = 0;

	PTRLISTPOS pos = TempListNo1.GetHeadPosition();
	while( pos )
	{
		cIcon* pIcon = (cIcon*)TempListNo1.GetNext( pos ) ;
		pGridDlg->AddIcon( wIconPos , pIcon );
		wIconPos++;
	}

	pos = TempListNo2.GetHeadPosition();
	while( pos )
	{
		cIcon* pIcon = (cIcon*)TempListNo2.GetNext( pos ) ;
		pGridDlg->AddIcon( wIconPos , pIcon );
		wIconPos++;
	}

	pos = TempListNo3.GetHeadPosition();
	while( pos )
	{
		cIcon* pIcon = (cIcon*)TempListNo3.GetNext( pos ) ;
		pGridDlg->AddIcon( wIconPos , pIcon );
		wIconPos++;
	}

	TempListNo1.RemoveAll();
	TempListNo2.RemoveAll();
	TempListNo3.RemoveAll();

}

cHousingStoredIcon* cHousingWarehouseDlg::GetStoredIconByFurniture(stFurniture* pFurniture)
{
	if(! pFurniture )
		return NULL;

	return m_StoredIconHash.GetData( pFurniture->dwObjectIndex );
}

cHousingStoredIcon* cHousingWarehouseDlg::GetStoredIconByObjectIdx(DWORD dwObjectIdx)
{
	return m_StoredIconHash.GetData( dwObjectIdx );
}

void cHousingWarehouseDlg::FocusOnByFurniture(stFurniture* pFurniture)
{
	//   < ���������ܿ� ��Ŀ���� �����ش� >  
	// * �Ǽ����� ���� ������ ���� �ǿ� �����ֱ� 
	// * ���� �ٲ�� �ǹ�ư�� �ý�Ʈ ��ġ �ٲ��ֱ� 
	// * UI�� ��ũ���� ���� �������� ��ġ�� �����ֱ�

	if( ! pFurniture )
		return;

	cHousingStoredIcon* pIcon = m_StoredIconHash.GetData( pFurniture->dwObjectIndex );

	if( ! pIcon )
		return;

	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet( pFurniture->wCategory-1 );

	if( ! pGridDlg )
		return;

	BYTE byOldTab = m_bSelTabNum;
	BYTE byNewTab = pFurniture->wCategory - 1;

	if( byOldTab != byNewTab )								// ���� ���� ������ ������ ���� �ʴ´ٸ� 
	{
		if(  m_pStatic_Basic && m_pStatic_Push )			// ���� �ٲ�� �ǹ�ư �ؽ�Ʈ ��ġ�� �ٲ��ش� 
		{
			cPushupButton* pPushBt			  = (cPushupButton*) GetWindowForID( HOUSING_WH_TABBTN1 + byNewTab ) ;
			cPushupButton* pOldPushBt		  = (cPushupButton*) GetWindowForID( HOUSING_WH_TABBTN1 + byOldTab ) ;

			if( pPushBt )
			{
				pPushBt->SetTextXY( m_pStatic_Push->GetRelX() , m_pStatic_Push->GetRelY() );
				pPushBt->SetRenderArea();
			}

			if( pOldPushBt )
			{
				pOldPushBt->SetTextXY( m_pStatic_Basic->GetRelX() , m_pStatic_Basic->GetRelY() );
				pOldPushBt->SetRenderArea();
			}
		}

		cTabDialog::SelectTab(byNewTab);						// ���������� �´� ���� �������ش�. 
	}

	WORD pos = pGridDlg->GetPositionForCell( pIcon->GetCellX() , pIcon->GetCellY() );

	pGridDlg->InitScrollXY( pos );								//��ũ���� �����ܿ� ��ġ�� �����ش�  
	pGridDlg->SetCurSelCellPos( pos );							//Ŀ���� ������ ��ġ�� �����ش�. 
}
