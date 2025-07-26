#include "stdafx.h"
#include "cHousingWarehouseButtonDlg.h"
#include "WindowIDEnum.h"
#include "cImeEx.h"
#include "cWindowManager.h"
#include "cMsgBox.h"
#include "cHousingWarehouseDlg.h"
#include "cHousingMgr.h"
#include "GameIn.h"


cHousingWarehouseButtonDlg::cHousingWarehouseButtonDlg()
{
	m_type = WT_HOUSING_WAREHOUSE_BUTTON_DLG;
}

cHousingWarehouseButtonDlg::~cHousingWarehouseButtonDlg()
{

}

void cHousingWarehouseButtonDlg::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
}

void cHousingWarehouseButtonDlg::Linking()
{
	 m_pWarehouseBtn =  ( cButton* )GetWindowForID( HOUSING_WH_BT );
}

void cHousingWarehouseButtonDlg::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	if(! HOUSINGMGR->IsHouseOwner() )
	{
		//�������� �ƴϸ� ����  
		val = FALSE;
	}

	cDialog::SetActive(val);
	cDialog::SetActiveRecursive(val);
}

void cHousingWarehouseButtonDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	if(! HOUSINGMGR->IsHouseInfoLoadOk())
	{
		//�Ͽ콺 �ε��� �Ǿ����� ������ ���� 
		return;
	}

	switch(lId)
	{
		case HOUSING_WH_BT :
			{
				// 091214 �Ͽ�¡ â���ư ���� �ݱ� �Ѵ� �����ϵ��� ���� 
				BOOL bActived = GAMEIN->GetHousingWarehouseDlg()->IsActive() ;
				//����â�� ���� 
				GAMEIN->GetHousingWarehouseDlg()->SetActive( !bActived );
			}
			break;
	}
}