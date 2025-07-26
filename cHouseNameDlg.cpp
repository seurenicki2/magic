#include "stdafx.h"
#include "cHouseNameDlg.h"
#include "WindowIDEnum.h"
#include "cImeEx.h"
#include "cWindowManager.h"
#include "cMsgBox.h"
#include "../ChatManager.h"

//090410 pdy �Ͽ�¡ �Ͽ콺 �̸����� UI
cHouseNameDlg::cHouseNameDlg()
{
	m_type = WT_HOUSE_NAME_DLG;
}

cHouseNameDlg::~cHouseNameDlg()
{

}

void cHouseNameDlg::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
	m_type = WT_HOUSE_NAME_DLG;
}

void cHouseNameDlg::Linking()
{
	m_pHouseName = (cStatic*)GetWindowForID(HOUSE_SEARCH_NAME);
	m_pHouseNameEdit = (cEditBox*)GetWindowForID(HOUSE_NAME_EDIT);
	m_pHouseNameEdit->SetValidCheck(/*VCM_CHARNAME*/VCM_DEFAULT);
	m_pHouseNameEdit->SetEditText("");
	m_pHouseNameOkBtn = (cButton*)GetWindowForID(HOUSE_NAME_OKBTN);
}

void cHouseNameDlg::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	cDialog::SetActive(val);
	cDialog::SetActiveRecursive(val);

	m_pHouseNameEdit->SetFocusEdit(val);

	if(val)
		m_pHouseNameEdit->SetEditText("");
}

void cHouseNameDlg::SetName(char* Name)
{
	m_pHouseNameEdit->SetEditText(Name);
}

char* cHouseNameDlg::GetName()
{
	return m_pHouseNameEdit->GetEditText();
}

void cHouseNameDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	switch(lId)
	{
		case HOUSE_NAME_OKBTN :
			{
				//�̸��� äũ�ϰ� 
				char* name = "";
				name = m_pHouseNameEdit->GetEditText();

				char MsgBuf[128] = {0,};
				sprintf(MsgBuf,CHATMGR->GetChatMsg(1889), name); //1889	"%s��(��) ����Ͻðڽ��ϱ�?"

				//090608 pdy �Ͽ콺�̸� äũ �߰� 
				if(IsCharInString(name, "'"))
				{
					//090605 pdy �Ͽ�¡ �ý��۸޼��� HouseNameUI ����Ҽ����� �̸� [�Ͽ콺 �̸�]  
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 551 ) );		//551	"����� �� ���� �̸��Դϴ�."
					return;
				}

				//090527 pdy �Ͽ�¡ �˾�â [�Ͽ콺�̸����ϱ�]
				WINDOWMGR->MsgBox( MBI_HOUSENAME_AREYOUSURE, MBT_YESNO, MsgBuf) ; 
				SetDisable(TRUE) ;
			}
			break;
	}

}

