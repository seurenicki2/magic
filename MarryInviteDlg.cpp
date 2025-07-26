/*************************************************************************************************************
 *
 *	File Name	::	MarryInviteDlg.h
 *
 *	Owner		::	Death
 *
 *	Purpose Application	::	Marry Inviting Dialog
 *
 *	Change Log	::
 *	
 *
 *
 *************************************************************************************************************/

#include "stdafx.h"
#include "MarryInviteDlg.h"
#include "WindowIDEnum.h"
#include "./Interface/cTextArea.h"
#include "./Interface/cStatic.h"
#include "ChatManager.h"
#include "cResourceManager.h"

CMarryInviteDlg::CMarryInviteDlg() :
mInviterPlayerIndex(0)
{
	m_type = WT_PARTYINVITEDLG;
}

CMarryInviteDlg::~CMarryInviteDlg()
{

}

void CMarryInviteDlg::Linking()
{	
	m_pInviter = (cTextArea*)GetWindowForID(MA_INVITER);
}

void CMarryInviteDlg::Update(const MSG_NAME_DWORD& message)
{
	mInviterPlayerIndex = message.dwData;
	
	TCHAR text[MAX_PATH] = {0};
	_sntprintf(
		text,
		_countof(text),
		CHATMGR->GetChatMsg(2491),
		message.Name);
	m_pInviter->SetScriptText(
		text);
}