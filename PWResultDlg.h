#pragma once
#include "stdAfx.h"
#include "./Interface/cDialog.h"

class cStatic;
class cListDialog;

struct TEAMSCOREINFO
{
	DWORD dwPlayerID;
	char szName[MAX_NAME_LENGTH+1];
	WORD wScore;
	WORD wTeam;

	TEAMSCOREINFO()
	{
		dwPlayerID = 0;
		ZeroMemory(szName, sizeof(szName));
		wScore = 0;
		wTeam = 0;
	}
};

class CPWResultDlg :
	public cDialog
{
	//FILE*							m_fp;
	cStatic*						m_pGuildName[2];
	cListDialog*					m_pMemberList[2];
	char							m_szGuildName[2][MAX_GUILD_NAME];

	CYHHashTable<TEAMSCOREINFO>	m_KillCountList;

public:
	CPWResultDlg(void);
	virtual ~CPWResultDlg(void);

	void Linking();
	void FillName();
	void SetGuildName(char* pTeam1Name, char* pTeam2Name);
	void SetKillCnt(DWORD dwPlayerID, DWORD dwCnt);
	void SetResult();
	void ClearResult();
	void RefreshList();
};
