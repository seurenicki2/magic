/*************************************************************************************************************
 *
 *	File Name	::	MarryManager.cpp
 *
 *	Owner		::	Death
 *
 *	Purpose Application	::	Marry Manager 
 *
 *	Change Log	::
 *	
 *	14-Feb-2014 Death
 *	- Intialize
 *	
 *	17-Feb-2014 Death
 *	- Enable Love Effect when MarryPlayer nearby
 *
 *************************************************************************************************************/
#include "stdafx.h"
#include "GameResourceManager.h"
#include "MarryManager.h"
#include "MarryInviteDlg.h"
#include "GameIn.h"
#include "cScriptManager.h"
#include "ObjectManager.h"
#include "../ChatManager.h"
#include "CharacterDialog.h"
#include "WindowIDEnum.h"
#include "Interface/cWindowManager.h"

/// Constructor
CMarryManager* CMarryManager::GetInstance()
{
	static CMarryManager instance;

	return &instance;
}
CMarryManager::CMarryManager(void)
{
	//MARRYEFFECT = 10384;
}

CMarryManager::~CMarryManager(void)
{

}

void CMarryManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
		case MP_MARRY_ADD_INVITE: 				Marry_Add_Invite( pMsg );			break;		
		case MP_MARRY_INVITE_ACCEPT_ACK:		Marry_Invite_Accept_Ack( pMsg );	break;
		case MP_MARRY_INVITE_ACCEPT_NACK:		Marry_Invite_Accept_Nack( pMsg );	break;		
		case MP_MARRY_CREATE_ACK:				Marry_Create_Ack( pMsg );			break;
		case MP_MARRY_CREATE_NOTIFY:			Marry_Create_Notify( pMsg );		break;
		case MP_MARRY_INVITE_DENY_ACK:			Marry_Invite_Deny_Ack( pMsg );		break;		

		case MP_MARRY_ABORT_ACK:				Marry_Abort_Ack( pMsg );			break;		
			
	} 
}

void CMarryManager::MarryInviteAccept()
{
	MSG_DWORD msg;
	msg.Category = MP_MARRY;
	msg.Protocol = MP_MARRY_INVITE_ACCEPT_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData = GetRequestMarryPlayerID();	
	
	NETWORK->Send(&msg,sizeof(msg));
	
}

void CMarryManager::MarryInviteDeny()
{
	MSG_DWORD msg;
	msg.Category = MP_MARRY;
	msg.Protocol = MP_MARRY_INVITE_DENY_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData = GetRequestMarryPlayerID();
	
	NETWORK->Send(&msg,sizeof(msg));		
}

void CMarryManager::RequestMarrySyn(CObject* RequestMarryToPlayerobj, DWORD itemIdx, DWORD itemPosition)
{
	if(RequestMarryToPlayerobj->GetID() == 0)
	{
		ASSERT(0);
		return;
	}

	MSG_DWORD3 msg;
	msg.Category	= MP_MARRY;
	msg.Protocol	= MP_MARRY_REQUEST_SYN;
	msg.dwObjectID	= HEROID;
	msg.dwData1		= RequestMarryToPlayerobj->GetID();			
	msg.dwData2		= itemIdx;
	msg.dwData3		= itemPosition;
	NETWORK->Send(&msg,sizeof(msg));
	
}

void CMarryManager::AbortMarrySyn(DWORD itemIdx, DWORD itemPosition)
{
	MSG_DWORD3 msg;
	msg.Category	= MP_MARRY;
	msg.Protocol	= MP_MARRY_ABORT_SYN;
	msg.dwObjectID	= HEROID;
	msg.dwData1		= HERO->GetMarryPlayerID();
	msg.dwData2		= itemIdx;
	msg.dwData3		= itemPosition;

	NETWORK->Send(&msg,sizeof(msg));

}

void CMarryManager::MarrySkillUpgradeSyn()
{
	MSG_DWORD msg;
	msg.Category	= MP_MARRY;
	msg.Protocol	= MP_MARRY_SKILLUPGRADE_SYN;
	msg.dwObjectID	= HEROID;
	
	NETWORK->Send(&msg,sizeof(msg));
}


void CMarryManager::Marry_Add_Invite( void* pMsg )
{	
	MSG_NAME_DWORD* pmsg = (MSG_NAME_DWORD*)pMsg;
	SetRequestMarryPlayerID(pmsg->dwData);
	
	GAMEIN->GetMarryInviteDialog()->Update(
		*pmsg);
	GAMEIN->GetMarryInviteDialog()->SetActive(
		TRUE);

}

void CMarryManager::Marry_Invite_Accept_Ack( void* pMsg )
{
	CHATMGR->AddMsg( CTC_SYSMSG,CHATMGR->GetChatMsg(2494));	
}

void CMarryManager::Marry_Invite_Accept_Nack( void* pMsg )
{
	CHATMGR->AddMsg( CTC_SYSMSG,CHATMGR->GetChatMsg(2495));	
}

void CMarryManager::Marry_Invite_Deny_Ack( void* pMsg )
{
	CHATMGR->AddMsg( CTC_SYSMSG,CHATMGR->GetChatMsg(2492));	
}

void CMarryManager::Marry_Create_Ack( void* pMsg )
{
	MSG_DWORD2_NAME* pmsg = (MSG_DWORD2_NAME*)pMsg;

	CHATMGR->AddMsg( CTC_SYSMSG,CHATMGR->GetChatMsg(2493),pmsg->dwData1);

	CCharacterDialog* pWindow = (CCharacterDialog*)WINDOWMGR->GetWindowForID(CI_CHARDLG) ;
//	pWindow->SetMarryName( pmsg->Name ) ;
	HERO->SetMarryPlayerID(pmsg->dwData2);
	HERO->SetMarryPlayerName(pmsg->Name);
}

void CMarryManager::Marry_Create_Notify( void* pMsg )
{
	MSG_NAME2_DWORD* pmsg = ( MSG_NAME2_DWORD* )pMsg;

	CHATMGR->AddMsg( CTC_OPERATOR2, CHATMGR->GetChatMsg( 2496 ), pmsg->Name1 ,  pmsg->Name2 );
}

void CMarryManager::Marry_Abort_Ack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwObjectID);
	if(! pPlayer)
		return;

	// Get Another Abort Message
	if ( pPlayer->GetID() != HEROID )
	{
		pPlayer->SetMarryPlayerID(0);
	}
	else
	{
		CHATMGR->AddMsg( CTC_SYSMSG,CHATMGR->GetChatMsg(2499));
		CCharacterDialog* pWindow = (CCharacterDialog*)WINDOWMGR->GetWindowForID(CI_CHARDLG) ;
//		pWindow->SetMarryName("") ;

		HERO->SetMarryPlayerName("");
		HERO->SetMarryPlayerID(0);
	}
}

