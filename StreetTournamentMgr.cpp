#include "stdafx.h"
#include "StreetTournamentMgr.h"
#include "WindowIDEnum.h"
#include "ChatManager.h"
#include "GameIn.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"
#include "MHMap.h"
#include "Player.h"
#include "interface/cFont.h"
#include "ImageNumber.h"
#include "./interface/cWindowManager.h"
#include "./Interface/cScriptManager.h"
#include "../[CC]Header/GameResourceManager.h"
#include "Trigger/TriggerTimerDlg.h"


CStreetTournamentMgr::CStreetTournamentMgr()
{
}

CStreetTournamentMgr::~CStreetTournamentMgr()
{
	m_dwState = 0;
	last_updatetime = 0;
	m_dwStage = 0;
	m_aplyname = FALSE;
	//OnActiveNumber();
}

BOOL CStreetTournamentMgr::isEnemy(CPlayer* Operator, CPlayer* Target)
{
	if(!Operator || !Target) return FALSE;
	
	if(m_dwState == 3 && MAP->GetMapNum() == StreetTournament)
	{

		if(Operator->IsRegisterStreetTournament() && Target->IsRegisterStreetTournament())
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CStreetTournamentMgr::isStreetTournament()
{

	if(MAP->GetMapNum() == StreetTournament)
	{
		return TRUE;
	}
	
	return FALSE;
}

void CStreetTournamentMgr::OnActiveNumber()
{
	
}

void CStreetTournamentMgr::SetState(WORD State)
{
	m_dwState = State;
	last_updatetime = gCurTime;

	if(isStreetTournament())
	{
		if(m_dwState == 2 && last_updatetime != 0)
		{
			GAMEIN->GetTriggerTimerDlg()->Start((eTimerType)0, 30000, 0);
		}
		else if(m_dwState == 3 && last_updatetime != 0)
		{
			GAMEIN->GetTriggerTimerDlg()->Start((eTimerType)0, 3600000, 0);//3600000
		}
		else if(m_dwState == 4 && last_updatetime != 0)
		{
			GAMEIN->GetTriggerTimerDlg()->Start((eTimerType)0, 30000, 0);
		}
	}
}

void CStreetTournamentMgr::Proses()
{
	if(MAP->GetMapNum() == StreetTournament)
	{
		if(m_dwState == 3 && (gCurTime - last_updatetime) >= 2000 && !m_aplyname)
		{
			OBJECTMGR->ApplyOverInfoOptionToAll();
			m_aplyname = TRUE;
		}
	}
}