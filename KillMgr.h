#pragma once
#include "stdafx.h"

#define KILLMGR USINGTON(CKillManager)

class CKillManager
{
public:
	CKillManager(void);
	virtual ~CKillManager(void);

	void NetworkMsgParse(BYTE Protocol,void* pMsg);
	void Kill_Point_Ack(void* pMsg);
	
protected:
	DWORD m_dwKillPoint;
	DWORD m_dwPoint;

	//Alemuri Pvp score--------------
	DWORD m_dwPointFighter;
	DWORD m_dwPointRogue;
	DWORD m_dwPointMage;
	DWORD m_dwPointDevil;
	//-----------------------

public:
	DWORD GetKillPoint()									{return m_dwKillPoint;}
	DWORD GetCurKillPoint() {return m_dwPoint;}

	//Alemuri Pvp score------------------------------------------------------------------------------
	DWORD GetKillPointFighter()									{return m_dwPointFighter;}
	DWORD GetKillPointRogue()									{return m_dwPointRogue;}
	DWORD GetKillPointMage()									{return m_dwPointMage;}
	DWORD GetKillPointDevil()									{return m_dwPointDevil;}
	//----------------------------------------------------------------------------------------

};

EXTERNGLOBALTON(CKillManager)