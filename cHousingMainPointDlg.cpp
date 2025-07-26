#include "stdafx.h"
#include "cHousingMainPointDlg.h"
#include "WindowIDEnum.h"
#include "cImeEx.h"
#include "cWindowManager.h"
#include "cMsgBox.h"
#include "cHousingMgr.h"
#include "cHousingDecoPointDlg.h"
#include "GameIn.h"
#include "ChatManager.h"

//090409 pdy �Ͽ�¡ �Ͽ콺 �˻� UI�߰�
cHousingMainPointDlg::cHousingMainPointDlg()
{
	m_type = WT_HOUSING_MAINPOINTDLG;
}

cHousingMainPointDlg::~cHousingMainPointDlg()
{

}

void cHousingMainPointDlg::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
}

void cHousingMainPointDlg::Linking()
{
	 m_pHouseName_Static =  ( cStatic* )GetWindowForID( HOUSING_MAINPOINT_HOUSENAME_STATIC );
	 m_pVisitCount_Static =  ( cStatic* )GetWindowForID( HOUSING_MAINPOINT_VISIT_STATIC );
	 m_pDecoPoint_Static =  ( cStatic* )GetWindowForID( HOUSING_MAINPOINT_DECOPOINT_STATIC );
	 m_BtDecoPoint		=	( cButton* )GetWindowForID( HOUSING_DECOPOINT_BT );
}

void cHousingMainPointDlg::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	cDialog::SetActive(val);
	cDialog::SetActiveRecursive(val);

	if( ! HOUSINGMGR->IsHouseOwner() )
	{
		//�Ͽ콺 ������ �ƴѰ�� ������Ǵ� �ٹ̱� ����Ʈ ��� UI�� ���� 
		m_pDecoPoint_Static->SetActive(FALSE);
	}
}

void cHousingMainPointDlg::SetHouseName(char* szHouseName)
{
	m_pHouseName_Static->SetStaticText(szHouseName);
}

void cHousingMainPointDlg::SetVisitCount(DWORD dwVisitCount)
{
	char Buf[32]={0};

	sprintf(Buf,"%d",dwVisitCount);
	m_pVisitCount_Static->SetStaticText(Buf);
}

void cHousingMainPointDlg::SetDecoPoint(DWORD dwDecoPoint)
{
	char Buf[32]={0};

	sprintf(Buf,"%d",dwDecoPoint);
	m_pDecoPoint_Static->SetStaticText(Buf);
}

//091012 pdy �Ͽ�¡ �ٹ̱� ����Ʈ ��ư ��ȹ���� 
void cHousingMainPointDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	if(! HOUSINGMGR->IsHouseInfoLoadOk())
	{
		//�Ͽ콺 �ε��� �Ǿ����� ������ ���� 
		return;
	}

	switch(lId)
	{
		case HOUSING_DECOPOINT_BT :
			{
				if(  HOUSINGMGR->IsHouseOwner() )
				{
					//�ٹ̱⺸�ʽ� UI�� ����
					cHousingDecoPointDlg* pDlg = GAMEIN->GetHousingDecoPointDlg() ;

					if( pDlg )
					{
						pDlg->SetActive( TRUE ) ; 
					}
				}
				else
				{
					CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1887) ); //1887	"�������� �ƴҽ� ����Ͻ� �� �����ϴ�"
				}
			}			
			break;
	}
}