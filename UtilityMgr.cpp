#include "stdafx.h"
#include "UtilityMgr.h"
#include "GameIn.h"
#include "ObjectManager.h"
GLOBALTON(CUtilityManager)
CUtilityManager::CUtilityManager(void)
{
	m_dwRebornData = 0;
	m_dwRebornPoint = 0;
	m_dwPoint1 = 0;
	m_dwPoint2 = 0;
}

CUtilityManager::~CUtilityManager(void)
{
}
void CUtilityManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_REBORN_DATA_ACK:				Reborn_Data_Ack(pMsg);			break;
	case MP_REBORNPOINT_ACK:				Reborn_Point_Ack(pMsg);			break;
	default:																	break;
	}
}

void CUtilityManager::Reborn_Data_Ack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	m_dwPoint1 = pmsg->dwData;
	if (pmsg->dwData >= MAX_FISHPOINT)
		return;
	if (HERO->GetID() == pmsg->dwObjectID )
		m_dwRebornPoint = pmsg->dwData;
	else
		return;
	//m_dwRebornData = pmsg->dwData;

	//char szValue[50];
	//sprintf( szValue, "%d", m_dwRebornData);
}
void CUtilityManager::Reborn_Point_Ack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	m_dwPoint2 = pmsg->dwData;
	if (pmsg->dwData >= MAX_FISHPOINT)
		return;
	if (HERO->GetID() == pmsg->dwObjectID )
		m_dwRebornPoint = pmsg->dwData;
	else
		return;
	//m_dwRebornPoint = pmsg->dwData;

	//char szValue[50];
	//sprintf( szValue, "%d", m_dwRebornPoint);
}