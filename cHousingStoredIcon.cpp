#include "stdafx.h"
#include "cHousingStoredIcon.h"
#include "../[cc]Header/CommonStruct.h"
#include "ItemManager.h"
#include "cScriptManager.h"
#include "ChatManager.h"


cHousingStoredIcon::cHousingStoredIcon( stFurniture* pstFurniture  )
{
	m_type = WT_HOUSING_STORED_ICON;
	m_pLinkstFurniture = pstFurniture;
}

cHousingStoredIcon::~cHousingStoredIcon()
{

}
void cHousingStoredIcon::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cIcon::Init( x,  y,  wid,  hei,  basicImage, ID);
}

BOOL cHousingStoredIcon::CanDelete()
{
	if( m_pLinkstFurniture->bNotDelete )				//�⺻��ġ ǰ���ϰ�� �����ۻ��� ���� 
	{
		//090527 pdy �Ͽ�¡ �ý��۸޼��� �⺻�������� �������� [��������]
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1870 ) );	//1870	"�⺻ ��ǰ�� ���� �Ͻ� �� �����ϴ�."
		return FALSE;
	}
		
	if( m_pLinkstFurniture->wState == eHOUSEFURNITURE_STATE_INSTALL	)
	{
		//090527 pdy �Ͽ�¡ �ý��۸޼��� ��ġ���ΰ��� �������� [��������]
		CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(1905) );	//1905	"��ġ���� ǰ���� �ٷ� �����ϽǼ� �����ϴ�."
		return FALSE;
	}

	return TRUE;
}

BOOL cHousingStoredIcon::CanMoveToDialog( cDialog* ptargetdlg )
{
	//â�� ������������ �κ��丮�θ� �̵������ϴ�.
	if(	ptargetdlg->GetType() == WT_INVENTORYDIALOG )
		return TRUE;

	return FALSE;
}

DWORD cHousingStoredIcon::ActionEvent(CMouse* mouseInfo)
{
	DWORD we = WE_NULL;

	if( ! m_bActive )
		return we;

	we |= cWindow::ActionEvent(mouseInfo);

	if( !m_bDisable )
		we |= ActionEventWindow(mouseInfo);

	we |= ActionEventComponent(mouseInfo);

	if( we == WE_MOUSEOVER )
	{
		const ITEM_OPTION pOption ={0,}; 
		ITEMMGR->AddTimeToolTip( this, pOption, FALSE);
	}

	return we ;
}

void cHousingStoredIcon::Render()
{
	if( ! m_bActive )
		return;

	cIcon::Render();
}

void cHousingStoredIcon::Refresh()
{
	//���� ���¿� ���� ����ǥ�� 
	switch(m_pLinkstFurniture->wState)
	{
		case eHOUSEFURNITURE_STATE_KEEP :
			{
				ClearStateImage();
			}
			break;
		case eHOUSEFURNITURE_STATE_INSTALL :
			{
				ClearStateImage();
				cImage dieimage;
				SCRIPTMGR->GetImage( 3, &dieimage );
				SetStateImage( &dieimage );
			}
			break;
		case eHOUSEFURNITURE_STATE_UNINSTALL :
			{
				ClearStateImage();
				cImage dieimage;
				SCRIPTMGR->GetImage( 145, &dieimage );
				SetStateImage( &dieimage );
			}
			break;
	}
}