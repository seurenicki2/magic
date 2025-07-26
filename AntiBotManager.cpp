/*************************************************************************************************************
 *
 *	File Name	::	AntiBotManager.cpp
 *
 *	Owner		::	Death
 *
 *	Purpose Application	::	AntiBot , Auto Joy By has dialog display if player atk more one hour
 *
 *	Change Log	::
 *
 *	Death , Sep 05		Intitial Version
 *
 *	Death , Sep 08		Add New 3 Condition to Not Let Anti bot Work
 *						- Map Alker , Alker Farm , Guild Tournament and Castle
 *						- Change to 2 Hour per time to AntiBot Work
 *						- Get GodStatus while Answer the antibot
 *
 *************************************************************************************************************/
#include "stdafx.h"
#include "AntiBotManager.h"
#include "GameResourceManager.h"
#include "cAntiBotDialog.h"
#include "GameIn.h"
#include "MHTimeManager.h"
#include "cWindowManager.h"
#include "ObjectStateManager.h"
#include "ObjectManager.h"
#include "MHMap.h"


/// Constructor
CAntiBotManager* CAntiBotManager::GetInstance()
{
	static CAntiBotManager instance;

	return &instance;
}
CAntiBotManager::CAntiBotManager(void)
{
	AttackMonsterCount = 0;	
	CurrentServerTime = 0;
	NextCheckBotTime = 0;
}

CAntiBotManager::~CAntiBotManager(void)
{

}

/// Public Function
void CAntiBotManager::Process()
{
	// Temp Fix Always Return
	return;


	if (GAMEIN->GetAntiBotDlg()->IsActive())
		return;
	
	MAPTYPE maptype = MAP->GetMapNum();
	if (maptype == Alker_Harbor || 
		maptype == The_Town_of_Nera_Castle ||
		maptype == GuildTournamentStadium || 
		maptype == Alker_Farm || 
		maptype == The_Castle_of_Rushen ||
		maptype == The_Castle_of_Zevyn ||
		maptype == The_Culvert				
		)
		return;
	
	if (RequireCheckBotDialog())
	{
		RequestAntiBotBuffToServer();
		if (HERO->GetAbnormalStatus()->IsNoRequireCheckBot)
			return;
		DisplayCheckBotDialog();
	}
}

BOOL CAntiBotManager::RequireCheckBotDialog()
{
	CurrentServerTime = MHTIMEMGR->GetServerTime();
	MAP->GetMapNum();
	if (AttackMonsterCount >= MONSTERHITACCEPTCHECKBOT && NextCheckBotTime < CurrentServerTime )
	{
		// Require to Bot Check
		NextCheckBotTime = CurrentServerTime + SECONDTORECHECKBOT;		
		AttackMonsterCount = 0;
		return true;
	}
	else if (NextCheckBotTime < CurrentServerTime)
	{
		// Require to Reset Counter
		NextCheckBotTime = CurrentServerTime + SECONDTORECHECKBOT;		
		AttackMonsterCount = 0;	
	}

	return false;
}

void CAntiBotManager::DisplayCheckBotDialog()
{	

	WINDOWMGR->HideAllWindow();
	GAMEIN->GetAntiBotDlg()->MakeRandomNumber();
	GAMEIN->GetAntiBotDlg()->SetActive(TRUE);	
	OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal);
	
}

void CAntiBotManager::RequestAntiBotBuffToServer()
{	
	MSGBASE msg;
	msg.Category = MP_ANTIBOT;
	msg.Protocol = MP_ANTIBOT_REQUESTBUFF_SYN;
	msg.dwObjectID = HEROID;
	NETWORK->Send(&msg,sizeof(msg));		
}


void CAntiBotManager::IncreseAttackMonsterCount()
{
	AttackMonsterCount++;
}

void CAntiBotManager::SetAttackMonsterCount(DWORD AtkCount)
{
	AttackMonsterCount = AtkCount;
}



