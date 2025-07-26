#pragma once
#include "stdafx.h"
#include "CommonStruct.h"

#define DungeonMGR USINGTON(CDungeonMgr)

class CDungeonMgr
{
private:
	CYHHashTable<stWarpInfo>		m_WarpList;
	CYHHashTable<stSwitchNpcInfo>	m_SwitchNpcList;
	stDungeon						m_CurDungeon;
	std::multimap< DWORD, DWORD >	m_mapEntranceNpcJob;

public:
	CDungeonMgr(void);
	virtual ~CDungeonMgr(void);

	void	Init();
	void	Process();
	void	NetworkMsgParse(BYTE Protocol,void* pMsg);	


	void	LoadDungeonInfo();
	void	SetWarp(DWORD dwIndex, BOOL bActive);
	void	SetSwitchNpc(WORD dwIndex, BOOL bActive);

	BOOL	IsDungeonMap(MAPTYPE map);

	void	ZoneBlock(float posx, float posz, float range);
	void	ZoneFree(float posx, float posz, float range);
	void	EdgeBlock(float posx, float posz, float range);
	void	EdgeFree(float posx, float posz, float range);

	// 091119 ONS �δ� ���̵幫���� ó���Լ� �߰�
	void	AfterWarpAction( DWORD dwWarpIndex );

	void	ClearCurDungeon();

	// �δ� ���� ������ ���� ���� NPC���� �߰�
	void	AddEntranceNpc( DWORD dwDungeonKey, DWORD dwNpcJob );
	BOOL	IsDungeonEntranceNpc( const DWORD dwNpcJob ) const;
	std::multimap< DWORD, DWORD >& GetEntranceNpcJobMap() { return m_mapEntranceNpcJob; }
};

EXTERNGLOBALTON(CDungeonMgr)