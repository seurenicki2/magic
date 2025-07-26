#pragma once


#include "Object.h"


#define MAX_NPCQUEST_LIST 50


struct NPC_LIST;


class CNpc : public CObject  
{
	NPC_TOTALINFO m_NpcInfo;
	int		m_QuestList[MAX_NPCQUEST_LIST];
	int		m_QuestCount;
	
	NPC_LIST * m_pSInfo;

	// 090925 ShinJS --- NPC Notice Dialog ��뿩��
	BOOL	m_bUseNoticeDlg;

	void InitNpc(NPC_TOTALINFO* pTotalInfo);
public:
	CNpc();
	virtual ~CNpc();
	virtual void Die(CObject* pAttacker,BOOL bFatalDamage,BOOL bCritical, BOOL bDecisive);
	NPC_LIST * GetSInfo() { return m_pSInfo; }

	void Process();
	WORD GetNpcJob(){return m_NpcInfo.NpcJob;}
	friend class CObjectManager;

	WORD GetNpcKind()		{	return m_NpcInfo.NpcKind;	}
	WORD GetNpcUniqueIdx()		{	return m_NpcInfo.NpcUniqueIdx;	}
	void AddQuestIndex(int Idx);
	int  GetQuestIdx(int Idx)		{	return m_QuestList[Idx];	}
	int  GetQuestCount()			{	return m_QuestCount;	}

	void OnClicked();
	NPC_TOTALINFO* GetNpcTotalInfo() { return &m_NpcInfo; }
	NPC_LIST* GetNpcListInfo() { return m_pSInfo; }
	virtual void SetMotionInState(EObjectState);
	DWORD m_dwLastMotionTime;
	int m_nNextMotionTime;

	// 090227 ShinJS --- �̵�NPC�� �̵��� �غ��۾������� �Ǵ�
private:
	BOOL m_bReadyToMove;
public:
	BOOL IsReadyToMove() const { return m_bReadyToMove; }
	void SetReadyToMove( BOOL bReadyToMove ) { m_bReadyToMove = bReadyToMove; }

	// 090916 ShinJS --- Npc Notice Dialog�� ��� ���� ����/��ȯ �߰�
	void SetUseNoticeDlg( BOOL bUseNoticeDlg ) { m_bUseNoticeDlg = bUseNoticeDlg; }
	BOOL IsUseNoticeDlg() const { return m_bUseNoticeDlg; }
};