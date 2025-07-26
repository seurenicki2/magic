// QuestManager.h: interface for the CQuestManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTMANAGER_H__AC4F30D2_DA87_4A14_8DF7_C2AFCBF24758__INCLUDED_)
#define AFX_QUESTMANAGER_H__AC4F30D2_DA87_4A14_8DF7_C2AFCBF24758__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\[CC]Quest\QuestDefines.h"															// ����Ʈ���� �ش������� �ҷ��´�.
#include "..\[CC]Quest\QuestEvent.h"															// ����Ʈ �̺�Ʈ �ش������� �ҷ��´�.
#include "QuestItemManager.h"																	// ����Ʈ ������ �Ŵ��� �ش� ������ �ҷ��´�.
#include "MiniMapIcon.h"
//#include "NewbieGuide.h"																		// �ʺ��� ���̵� ��� ������ �ҷ��´�.

enum ENUM_QUEST_TYPE
{
	e_QT_ERROR_TYPE = -1,
	e_QT_MULTY_HUNT = 0,
	e_QT_MEET_OTHER,
} ;


class CQuestInfo;																				// ����Ʈ ���� Ŭ������ �����Ѵ�.
class CSubQuestInfo;																			// ���� ����Ʈ ���� Ŭ������ �����Ѵ�.
class CQuestString;																				// ����Ʈ ��Ʈ�� Ŭ������ �����Ѵ�.
class CQuest;																					// ����Ʈ Ŭ������ �����Ѵ�.
class CQuestUpdater;																			// ����Ʈ �������� Ŭ������ �����Ѵ�.
//class CNewbieGuide;																				// �ʺ��� ���̵� Ŭ������ �����Ѵ�.
class CNpc;																						// NPCŬ������ �����Ѵ�.
class CQuestDialog ;
class CQuestQuickViewDialog ;

struct sQUEST_INFO ;

struct stNPCIDNAME																				// ����Ʈ â���� �ʿ��� npc ���� ����ü.
{
	char name[64] ;																				// npc �̸��� ���� ��Ʈ��.
	WORD wId ;																					// npc ���̵� ���� ����.

	stNPCIDNAME()
	{
		memset(name, 0, sizeof(name)) ;
		wId = 0 ;
	}
} ;

#define QUESTMGR	USINGTON(CQuestManager)														// ����Ʈ �Ŵ����� �۷ι��� ����.

#define MAX_NPCARRAY	40

#ifdef _TESTCLIENT_
	#define MAX_NPCIDNAME	2000
#else
	#define MAX_NPCIDNAME	500
#endif

class CQuestManager																				// ����Ʈ �Ŵ��� Ŭ����.
{
protected:
	CYHHashTable<CQuestInfo>	m_QuestInfoTable;												// ����Ʈ ������ ��� ���̺�.	// script information
	CYHHashTable<CQuestString>	m_QuestStringTable;												// ����Ʈ ��Ʈ���� ��� ���̺�.
	CYHHashTable<CQuest>		m_QuestTable;													// ����Ʈ�� ��� ���̺�.		// quest information
	CYHHashTable<QUESTNPCINFO>	m_QuestNpcTable;												// ����Ʈ NPC�� ��� ���̺�.	// quest npc

	typedef std::set< DWORD >	NpcHasQuestList;
	std::map< DWORD, NpcHasQuestList >	m_NpcHasQuestList;										// NPC�� ������ ����ƮIndex
	
	CQuestUpdater*				m_QuestUpdater;													// ����Ʈ ��������.
	CQuestItemManager			m_QuestItemMGR;													// ����Ʈ ������ �Ŵ���.

	CNpc*						m_pNpcArray[MAX_NPCARRAY];										// NCP ����.					// npc information
	DWORD						m_dwMaxNpc;														// �ִ� NPC �ε���.	

	//CNewbieGuide				m_NewbieGuide;													// �ʺ��� ���̵�.				// newbieguide
	
	DWORD						m_dwLastMsg;													// ������ �޽���.

	stNPCIDNAME					m_stNPCIDNAME[MAX_NPCIDNAME] ;									// ����Ʈ â���� �ʿ��� NPC ���� ����ü �迭 100��.

	DWORD						m_QuickViewQuestID[MAX_QUICKVIEW_QUEST] ;						// �� ��� ��� �Ǿ� �ִ� ����Ʈ ���̵� �迭.

	CQuestString*				m_TestString ;

	CQuestDialog*				m_QuestDialog ;
	CQuestQuickViewDialog*		m_QuickView ;

	// 100405 ShinJS --- �ð����� ����Ʈ
	std::set< DWORD > m_setDateLimitQuest;
public:
	CQuestManager();																			// ������ �Լ�.
	virtual ~CQuestManager();																	// �Ҹ��� �Լ�.

	void	Release();																			// ���� �Լ�.
	// script
	CQuestInfo*		GetQuestInfo( DWORD dwQuestIdx )	{ return m_QuestInfoTable.GetData( dwQuestIdx ); }		// ����Ʈ ������ �����ϴ� �Լ�.
	CQuestString*	GetQuestString( DWORD dwQuestIdx )	{ return m_QuestStringTable.GetData( dwQuestIdx );	}	// ����Ʈ ��Ʈ�� ������ �����ϴ� �Լ�.
	CQuest*			GetQuest( DWORD dwQuestIdx )		{ return m_QuestTable.GetData( dwQuestIdx ); }			// ����Ʈ�� �����ϴ� �Լ�.

	CYHHashTable<CQuest>*	GetQuestTable()				{ return &m_QuestTable; }								// ����Ʈ�� ��� �ִ� ����Ʈ ���̺��� �����ϴ� �Լ�.

	char*			GetQuestTitle( DWORD dwQuestIdx );															// ����Ʈ Ÿ��Ʋ�� �����ϴ� �Լ�.
	CQuestString* GetQuestString(DWORD dwMainQuestIdx, DWORD dwSubQuestIdx)	;									// ����Ʈ ��ȣ�� ���� �� ��ȣ�� ����Ʈ ��Ʈ���� ��ȯ�ϴ� �Լ�.

	BOOL	LoadQuestScript( char* strFileName );																// ����Ʈ ��ũ��Ʈ�� �ε��ϴ� �Լ�.
	BOOL	LoadQuestString( char* strFileName );																// ����Ʈ ��Ʈ���� �ε��ϴ� �Լ�.

	//void	LoadNewbieGuide( char* strFileName );																// �ʺ��� ���̵带 �ε��ϴ� �Լ�.

	void	LoadQuestNpc( char* strFileName );																	// ����Ʈ npc�� �ε��ϴ� �Լ�.

	void	CreateAllQuestFromScript();																			// script�� ���� ��� ����Ʈ�� �����ϴ� �Լ�.

	// quest information clear
	void	InitializeQuest();																					// ����Ʈ�� �ʱ�ȭ �ϴ� �Լ�.

	// quest set
	void	SetMainQuestData( DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t regisTime, BYTE bCheckType, DWORD dwCheckTime );	// ���� ����Ʈ�� �����ϴ� �Լ�.
	void	SetSubQuestData( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwData, DWORD dwTime );										// ���� ����Ʈ�� �����ϴ� �Լ�.

	void	DeleteQuest( DWORD dwQuestIdx, BOOL bCheckTime = FALSE );																	// ����Ʈ�� �����ϴ� �Լ�.
	void	StartQuest( DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime );									// ����Ʈ�� �����ϴ� �Լ�.
	void	EndQuest( DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime );									// ����Ʈ�� �����ϴ� �Լ�.
	void	StartSubQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwSubQuestFlag, DWORD dwSubData, DWORD dwSubTime );				// ���� ����Ʈ�� �����ϴ� �Լ�.
	void	EndSubQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwSubQuestFlag, DWORD dwSubData, DWORD dwSubTime );				// ���� ����Ʈ�� �����ϴ� �Լ�.
	void	UpdateSubQuestData( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount, DWORD dwSubData, DWORD dwSubTime );			// ���� ����Ʈ �����͸� ������Ʈ �ϴ� �Լ�.

	void	QuestItemAdd( DWORD dwItemIdx, DWORD dwCount );							// ����Ʈ �������� �߰��ϴ� �Լ�.
	void	QuestItemDelete( DWORD dwItemIdx );										// ����Ʈ �������� �����ϴ� �Լ�.
	void	QuestItemUpdate( DWORD dwType, DWORD dwItemIdx, DWORD dwCount );		// ����Ʈ �������� ������Ʈ �ϴ� �Լ�.
	QUEST_ITEM_INFO* GetQuestItem( DWORD dwItemIdx );
	
	// NpcScript
	BOOL	IsNpcRelationQuest( DWORD dwNpcIdx );									// npc ���� ����Ʈ ���� üũ�ϴ� �Լ�.
	DWORD	GetNpcScriptPage( DWORD dwNpcIdx );										// npc ��ũ��Ʈ �������� �����ϴ� �Լ�.
	void	SendNpcTalkEvent( DWORD dwNpcIdx, DWORD dwNpcScriptIdx, DWORD dwRequitalIndex = 0, DWORD dwRequitalCount = 0 );				// npc ��ȭ �̺�Ʈ�� ������ �Լ�.
	void	SendNpcTalkEventForEvent( DWORD dwNpcIdx, DWORD dwNpcScriptIdx );		// �̺�Ʈ�� npc ��ȭ �̺�Ʈ�� ������ �Լ�.
	// NpcMark
	void	SetNpcData( CNpc* pNpc );												// npc �����͸� �����ϴ� �Լ�.
	void	ProcessNpcMark();														// npc ��ũ ���μ��� �Լ�.
	// newbieguide
	//void	CheckGameEvent( GAMEEVENT* pGE )	{ m_NewbieGuide.CheckEvent( pGE ); }	// ���� �̺�Ʈ�� üũ�ϴ� �Լ�.
	// quest npc
	void	AddQuestNpc( CSubQuestInfo* pSubQuestInfo );								// ����Ʈ npc�� �߰��ϴ� �Լ�.
	
	BOOL	IsQuestStarted( DWORD dwQuestIdx );											// ����Ʈ�� ���۵Ǿ����� üũ�ϴ� �Լ�.

	BOOL IsQuestComplete(DWORD dwQuestIdx);												// ����Ʈ�� �Ϸ�Ǿ����� üũ�ϴ� �Լ�.

	// Network
	void	NetworkMsgParse( BYTE Protocol, void* pMsg );								// ��Ʈ���� �޽��� �Ľ� �Լ�.
	void	QuestAbandon();																// ����Ʈ ���� ó�� �Լ�.
	void	QuestAbandon( DWORD dwQuestIdx );											// ����Ʈ ���� ó�� �Լ�.
	void	QuestTimeLimit( DWORD dwQuestIdx, DWORD dwTime );							// ����Ʈ �ð� ���� ó�� �Լ�.
	void	RegistCheckTime( DWORD dwQuestIdx, BYTE bCheckType, DWORD dwCheckTime );	// ����Ʈ �ð� ��� �Լ�.
	void	UnRegistCheckTime( DWORD dwQuestIdx );										// ����Ʈ �ð� ���� �Լ�.

	void	QuestEventAck( DWORD dwQuestIdx );											// ����Ʈ �̺�Ʈ ���� �Լ�.
	void	QuestEventNack( DWORD dwQuestIdx, DWORD dwError );							// ����Ʈ �̺�Ʈ ���� �Լ�.
	// 070205 LYW --- Add functions to process network msg.
public :
	void Quest_Maindata_Load( void* pMsg ) ;											// ���� ������ �ε� �Լ�.
	void Quest_Subdata_Load( void* pMsg ) ;												// ���� ������ �ε� �Լ�.
	void Quest_Item_Load( void* pMsg ) ;												// ����Ʈ ������ �ε� �Լ�.
	void Quest_Start_Ack( void* pMsg ) ;												// ����Ʈ ��ŸƮ �㰡 �Լ�.
	void SubQuest_Start_Ack( void* pMsg ) ;												// ���� ����Ʈ ��ŸƮ �㰡 �Լ�.
	void Quest_End_Ack( void* pMsg ) ;													// ����Ʈ ���� �㰡 �Լ�.
	void SubQuest_End_Ack( void* pMsg ) ;												// ���� ����Ʈ ���� �㰡 �Լ�.
	void SubQuest_Update( void* pMsg ) ;												// ���� ����Ʈ ������Ʈ �Լ�.
	void QuestItem_Give( void* pMsg ) ;													// ����Ʈ ������ ��ȯ �Լ�.
	void QuestItem_Take( void* pMsg ) ;													// ����Ʈ ������ ȹ�� �Լ�.
	void QuestItem_Update( void* pMsg ) ;												// ����Ʈ ������ ������Ʈ �Լ�.
	void Quest_Delete_Ack( void* pMsg ) ;												// ����Ʈ ���� �㰡 �Լ�.
	void Quest_TakeItem_Ack( void* pMsg ) ;												// ����Ʈ ������ ȹ�� �㰡 �Լ�.
	void Quest_Takemoney_Ack( void* pMsg ) ;											// ����Ʈ �Ӵ� ȹ�� �㰡 �Լ�.
	void Quest_TakeExp_Ack( void* pMsg ) ;												// ����Ʈ ����ġ ȹ�� �㰡 �Լ�.
	void Quest_TakesExp_Ack( void* pMsg ) ;												// ����Ʈ sp ����Ʈ ȹ�� �㰡 �Լ�.
	void Quest_GiveItem_Ack( void* pMsg ) ;												// ����Ʈ ������ ��ȯ �㰡 �Լ�.
	// 071026 LYW --- QuestManager : Add function to decrease item.
	void Quest_GiveItems_Ack( void* pMsg ) ;											// ����Ʈ ���� ������ ���� ��ȯ �Լ�.
	void Quest_GiveMoney_Ack( void* pMsg ) ;											// ����Ʈ �Ӵ� ��ȯ �㰡 �Լ�.
	void Quest_Time_Limit( void* pMsg ) ;												// ����Ʈ �ð� ���� �Լ�.
	void Quest_Regist_Checktime( void* pMsg ) ;											// ����Ʈ �ð� üũ ��� �Լ�.
	void Quest_Unregist_Checktime( void* pMsg ) ;										// ����Ʈ �ð� üũ ���� �Լ�.
	void QuestEvent_NpcTalk_Ack( void* pMsg ) ;											// ����Ʈ �̺�Ʈ npc ��ȭ �㰡 �Լ�.
	void QuestEvent_NpcTalk_Nack( void* pMsg ) ;										// ����Ʈ �̺�Ʈ npc ��ȭ ���� �Լ�.
	void Quest_Execute_Error( void* pMsg ) ;											// ����Ʈ ���� ���� �Լ�.
	void Quest_Error_Ext( void* pMsg ) ;												// ����Ʈ ��Ÿ ���� ó�� �Լ�.
	void Quest_Error_FullQuest( void* pMsg ) ;											// ����Ʈ ����� �������� ���̻� ������ �� ���ٴ� ���� �޽��� ó��.

	// 071028 LYW --- CQuestManager : Add function to process give item error.
	void Quest_GiveItem_Error( void* pMsg ) ;											// ����Ʈ ������ �ݳ��� ������ ���� ó���� �ϴ� �Լ�.

	// 070329 LYW --- QuestManager : Add function to make string.
	void CompositString( char* inputStr, char* str, ... ) ;								// ����Ʈ ��� ���� ���� �Լ�.

	void AddMapTree(int nMapNum) ;														// �� Ʈ���� �߰��ϴ� �Լ�.
	void DeleteMapTree(int nMapNum) ;													// �� Ʈ���� �����ϴ� �Լ�.

	void AddQuestTitle(int nMapNum, DWORD dwQuestIdx) ;									// ����Ʈ ���̾�α׿� ����Ʈ ������ �߰��ϴ� �Լ�.
	void DeleteQuestTitle(int nMapNum, DWORD dwQuestIdx) ;								// ����Ʈ ����Ʈ���� ����Ʈ ������ �����ϴ� �Լ�.

	void SetNpcIDName(WORD wIdx, stNPCIDNAME* pInfo) ;									// npc �̸��� ���̵� �����ϴ� �Լ�.

	stNPCIDNAME* GetNpcIdName(DWORD npcIdx) ;

	QUESTNPCINFO* GetQusetNpcInfo(DWORD npcIdx);
	void LoadQuickViewInfoFromFile(DWORD* pIdx);
	int CheckQuestType(DWORD wQuestIdx, DWORD wSubQuestIdx);
	void SetQuestDialog(CQuestDialog* pQuestDialog) { m_QuestDialog = pQuestDialog ; }
	void SetQuickViewDialog(CQuestQuickViewDialog* pQuickView) { m_QuickView = pQuickView ; }

	// 100410 ShinJS --- �ð� ���� ��� �߰�
	void NotifyChangedDateTime( const DWORD dwQuestIdx );								// ��ȿ �ð� ���°� ����� ��츦 �˷��ش�.
	void Process();
	void InitDateTime();																// ���� �����ð����� �ð����� ����Ʈ���� �ʱ�ȭ�Ѵ�.
	BOOL IsValidDateTime( DWORD dwQuestIdx );

	// 100517 ShinJS --- ����Ʈ �䱸������ �Ϸ��ߴ��� �˻�(�Ϲ� ������ ��)
	BOOL IsCompletedDemandQuestExecute( DWORD dwQuestIdx );

	// 100414 ONS ����Ʈ ������Ʈ���� ������ �����´�.
	cPtrList*	GetLastSubQuestTrigger( DWORD dwQuestIdx );

	eQUEST_MARK GetQuestMarkState( DWORD dwUniqueNpcIdx, DWORD dwNpcObjectIdx );

	void AddNpcHasQuest( DWORD dwUniqueNpcIdx, DWORD dwQuestIdx );
};

EXTERNGLOBALTON(CQuestManager);

#endif // !defined(AFX_QUESTMANAGER_H__AC4F30D2_DA87_4A14_8DF7_C2AFCBF24758__INCLUDED_)
