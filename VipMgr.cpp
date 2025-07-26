#include "stdafx.h"
#include "VipMgr.h"
#include "GameIn.h"
#include "ObjectManager.h"
GLOBALTON(CVipManager)
CVipManager::CVipManager(void)
{
	m_dwVipPoint = 0;
	m_dwPoint = 0;
}

CVipManager::~CVipManager(void)
{
}
void CVipManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_VIP_POINT_ACK:				Vip_Point_Ack(pMsg);			break;
	default:																	break;
	}
}

void CVipManager::Vip_Point_Ack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	m_dwPoint = pmsg->dwData;
	if (pmsg->dwData >= MAX_FISHPOINT)
		return;
	if (HERO->GetID() == pmsg->dwObjectID )
		m_dwVipPoint = pmsg->dwData;
	else
		return;//m_dwVipPoint = m_dwVipPoint;

	//char szValue[50];
	//sprintf( szValue, "%d", m_dwVipPoint);
	//GAMEIN->GetFishingDlg()->SetPointText(szValue);
}