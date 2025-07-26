#pragma once
#include "stdafx.h"

#define VIPMGR USINGTON(CVipManager)

class CVipManager
{
public:
	CVipManager(void);
	virtual ~CVipManager(void);

	void NetworkMsgParse(BYTE Protocol,void* pMsg);
	void Vip_Point_Ack(void* pMsg);

protected:
	DWORD m_dwVipPoint;
	DWORD m_dwPoint;

public:
	DWORD GetVipPoint()									{return m_dwVipPoint;}
	DWORD GetCurVipPoint()									{return m_dwPoint;}
};

EXTERNGLOBALTON(CVipManager)