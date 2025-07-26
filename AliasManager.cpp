/*************************************************************************************************************
 *
 *	File Name	::	AliasManager.cpp
 *
 *	Owner		::	Death
 *
 *	Purpose Application	::	Alias Manager 
 *
 *	Change Log	::
 *	
 *
 *
 *************************************************************************************************************/
#include "stdafx.h"
#include "GameResourceManager.h"
#include "GameIn.h"
#include "MHMap.h"
#include "AliasManager.h"
#include "AliasMarkImg.h"
#include "AliasMark.h"
#include "cImageSelf.h"
#include "cScriptManager.h"
#include "../ChatManager.h"
#include "ExitManager.h"

extern int g_nServerSetNum;

/// Constructor
CAliasManager* CAliasManager::GetInstance()
{
	static CAliasManager instance;

	return &instance;
}
CAliasManager::CAliasManager(void) :
mDirectory( "data\\interface\\2dimage\\guildmark" )
{

}

CAliasManager::~CAliasManager(void)
{

}


CAliasMark* CAliasManager::GetAliasMark(eALIASTYPE type)
{
	CAliasMark* pMark = new CAliasMark();
	int Image_Hard_Idx = 0;

	switch (type)
	{
		case NoAlias: return NULL;break;
		case PVPHighest: Image_Hard_Idx = 182;break;
		case SiegeWarHighest: Image_Hard_Idx =183; break;
		case FamilyPointHighest: Image_Hard_Idx = 184;break;
		case GuildTourWinCountHighest: Image_Hard_Idx = 185;break;
		case TrueRefillHightest: Image_Hard_Idx = 186;break;
		case GM:Image_Hard_Idx = 187;break;
			//aziz
		case TopPlayer: Image_Hard_Idx = 192;break;
		case FishMaster: Image_Hard_Idx = 193;break;
		case VIP: Image_Hard_Idx = 194;break;
		case ImCute: Image_Hard_Idx = 195;break;
		default:  Image_Hard_Idx = type;
	}
	// 마크 이미지를 로딩한다
	CAliasMarkImg* pMarkImg = LoadMarkImg(Image_Hard_Idx,type);
	pMark->SetAliasMarkImg(pMarkImg);

	return pMark;

}

CAliasMarkImg* CAliasManager::LoadMarkImg(DWORD Image_Hard_Idx,eALIASTYPE type)
{
	cImage* pImg = new cImage;
	SCRIPTMGR->GetImage( Image_Hard_Idx, pImg, PFT_HARDPATH );		
	return new CAliasMarkImg(type,pImg);
}

// PART 2 :: Item Alias Support Function
void CAliasManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
		case MP_ALIAS_ITEM_ACK:				ItemKindAliasAck( pMsg );			break;					
	} 
}

void CAliasManager::ItemKindAliasSyn(DWORD ItemSupplyType, DWORD itemIdx, POSTYPE itemPosition)
{	
	MSG_DWORD3 msg;
	msg.Category	= MP_ALIAS;
	msg.Protocol	= MP_ALIAS_ITEM_SYN;
	msg.dwObjectID	= HEROID;
	msg.dwData1		= ItemSupplyType;
	msg.dwData2		= itemIdx;	//Alemuri Alias block Cheat
	msg.dwData3		= itemPosition;	//Alemuri Alias block Cheat
	NETWORK->Send(&msg,sizeof(msg));
}

void CAliasManager::ItemKindAliasAck(void* pMsg)
{	
	/*
	MSG_DWORD msg;
	msg.Category	= MP_ALIAS;
	msg.Protocol	= MP_ALIAS_ITEM_ACK;
	msg.dwObjectID	= HEROID;
	msg.dwData		= ItemSupplyType;
	NETWORK->Send(&msg,sizeof(msg));
	*/
	CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(5114));
	EXITMGR->SendExitMsg(eEK_CHARSEL);
}


