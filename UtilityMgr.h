#pragma once
#include "stdafx.h"

#define UTILITYMGR USINGTON(CUtilityManager)

class CUtilityManager
{
public:
	CUtilityManager(void);
	virtual ~CUtilityManager(void);

	void NetworkMsgParse(BYTE Protocol,void* pMsg);
	void Reborn_Data_Ack(void* pMsg);
	void Reborn_Point_Ack(void* pMsg);

protected:
	DWORD m_dwRebornData;
	//aziz Reborn Point 13 Oct
	DWORD m_dwRebornPoint;
	DWORD m_dwPoint1;
	DWORD m_dwPoint2;
public:
	DWORD GetRebornData()									{return m_dwRebornData;}
	DWORD GetRebornPoint()									{return m_dwRebornPoint;}
	DWORD GetCurRebornData()									{return m_dwPoint1;}
	DWORD GetCurRebornPoint()									{return m_dwPoint2;}
};

EXTERNGLOBALTON(CUtilityManager)