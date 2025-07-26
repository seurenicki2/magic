#include "stdafx.h"
#include "KillMgr.h"
#include "GameIn.h"
#include "ObjectManager.h"

GLOBALTON(CKillManager)
CKillManager::CKillManager(void)
{
	m_dwKillPoint = 0;
	m_dwPoint = 0;
}

CKillManager::~CKillManager(void)
{
}
void CKillManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_KILL_POINT_ACK:				Kill_Point_Ack(pMsg);			break;
	default:																	break;
	}
}

void CKillManager::Kill_Point_Ack(void* pMsg)
{
	MSG_DWORD_PVP* pmsg = (MSG_DWORD_PVP*)pMsg;
	m_dwPoint = pmsg->dwDataTotal;
	if (pmsg->dwDataTotal >= MAX_FISHPOINT)
		return;
	if (HERO->GetID() == pmsg->dwObjectID )
	{
		m_dwKillPoint = pmsg->dwDataTotal;
		//Alemuri Pvp score-----------------------------
		m_dwPointFighter = pmsg->dwDataFighter;
		m_dwPointRogue = pmsg->dwDataRogue;
		m_dwPointMage = pmsg->dwDataMage;
		m_dwPointDevil = pmsg->dwDataDevil;
		//--------------------------------------
	}
	else
		return;//m_dwKillPoint = m_dwKillPoint;
	

	//Unused Code

	//char szValue[50];
	//sprintf( szValue, "%d", m_dwKillPoint);
	//GAMEIN->GetFishingDlg()->SetPointText(szValue);
}