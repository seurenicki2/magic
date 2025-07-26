// Quest.cpp: implementation of the CQuest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Quest.h"
#include "..\[CC]Quest\QuestInfo.h"
#include "..\[CC]Quest\SubQuestInfo.h"
#include "ObjectManager.h"
#include "StatusIconDlg.h"
#include "MHTimeManager.h"
#include "../[CC]Quest/QuestExecute.h"
#include "../[CC]Quest/QuestExecute_Item.h"
#include "GameIn.h"
#include "InventoryExDialog.h"
#include "../[CC]Quest/QuestNpcScript.h"
#include "QuestManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuest::CQuest( CQuestInfo* pQuestInfo )													// ������ �Լ�.
{
	m_pQuestInfo = pQuestInfo;																// ����Ʈ ������ �޾� ��� ������ �����Ѵ�.

	memset( m_CurSubQuestInfoArray, 0, sizeof(CSubQuestInfo*)*MAX_SUBQUEST );				// ���� ����Ʈ ������ ��� �迭�� �޸� ������ �Ѵ�.

	m_CurSubQuestInfoArray[0] = m_pQuestInfo->GetSubQuestInfo( 0 );							// ����Ʈ �������� 0��° ���� ����Ʈ ������ �޾� �迭0���� �����Ѵ�.

	m_dwSubQuestCount = m_pQuestInfo->GetSubQuestCount();									// ���� ����Ʈ ī��Ʈ�� �޾� ��� ī��Ʈ�� �����Ѵ�.


	memset( m_SubQuestTable, 0, sizeof(SUBQUEST)*MAX_SUBQUEST );							// ���� ����Ʈ ���̺��� �޸� �����Ѵ�.

	m_dwSubQuestFlag = m_dwData = 0;
	m_RegistTime = 0;
	
	m_bCheckTime = FALSE;																	// �ð�üũ ���θ� FALSE�� �����Ѵ�.
	m_CheckType = 0;																		// üũ Ÿ���� 0���� �����Ѵ�.
	m_CheckTime.value = 0;																	// �ð� ���� 0���� �����Ѵ�.

	m_dwCurSubQuestIdx = (DWORD)-1;

	InitDateTime();

	InitDemandQuestExecute();
}

CQuest::~CQuest()																			// �Ҹ��� �Լ�.
{
}

void CQuest::Initialize()																	// �ʱ�ȭ �Լ�.
{
	memset( m_CurSubQuestInfoArray, 0, sizeof(CSubQuestInfo*)*MAX_SUBQUEST );				// ���� ����Ʈ ������ ��� �迭�� �޸� �����Ѵ�.

	m_CurSubQuestInfoArray[0] = m_pQuestInfo->GetSubQuestInfo( 0 );							// ����Ʈ �������� 0��° ���� ����Ʈ ������ �޾� �迭0���� �����Ѵ�.

	m_dwSubQuestCount = m_pQuestInfo->GetSubQuestCount();									// ���� ����Ʈ ī��Ʈ�� �޾� ��� ī��Ʈ�� �����Ѵ�.

	memset( m_SubQuestTable, 0, sizeof(SUBQUEST)*MAX_SUBQUEST );							// ���� ����Ʈ ���̺��� �޸� �����Ѵ�.

	m_dwSubQuestFlag = m_dwData = 0;
	m_RegistTime = 0;

	m_bCheckTime = FALSE;																	// �ð� üũ ���θ� FALSE�� �����Ѵ�.
	m_CheckType = 0;																		// üũ Ÿ���� 0���� �����Ѵ�.
	m_CheckTime.value = 0;																	// �ð� ���� 0���� �����Ѵ�.

	m_dwCurSubQuestIdx = (DWORD)-1;
}

void CQuest::SetMainQuestData( DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime, BYTE bCheckType, DWORD dwCheckTime )	// ���� ����Ʈ �����͸� �����ϴ� �Լ�.
{
	m_CurSubQuestInfoArray[0] = NULL;														// ����, ���� ����Ʈ ���� �迭 0�� null ó�� �Ѵ�.
	m_dwSubQuestFlag = dwSubQuestFlag;														// ���� ����Ʈ �÷��׸� �����Ѵ�.
	m_dwData = dwData;																		// �����͸� �����Ѵ�.
	m_RegistTime = registTime;																		// �ð� ���� �����Ѵ�.

	m_CheckType = bCheckType;																// üũ Ÿ���� �����Ѵ�.
	m_CheckTime.value = dwCheckTime;														// üũ Ÿ���� �����Ѵ�.

	if( m_CheckType != 0 )																	// üũ Ÿ���� 0�� ���� ������,
	{
		m_bCheckTime = TRUE;																// �ð� üũ ���θ� TRUE�� �����Ѵ�.

		if( GetCurTime() < m_CheckTime.value )												// ���� �ð��� üũ Ÿ�Ӻ��� ������,
            STATUSICONDLG->AddQuestIconCount();												// ����Ʈ ������ ī��Ʈ�� �߰��Ѵ�.
	}
}

void CQuest::SetSubQuestData( DWORD dwSubQuestIdx, DWORD dwSubData, DWORD dwSubTime )		// ���� ����Ʈ �����͸� �����ϴ� �Լ�.
{
	m_CurSubQuestInfoArray[dwSubQuestIdx] = m_pQuestInfo->GetSubQuestInfo( dwSubQuestIdx );	// ���� ����Ʈ �ε����� �ش��ϴ�, ���� ����Ʈ �迭�� ����Ʈ ������ ��´�.
	m_SubQuestTable[dwSubQuestIdx].dwData = dwSubData;										// ���� ����Ʈ �����͸� �����Ѵ�.
	m_SubQuestTable[dwSubQuestIdx].dwTime = dwSubTime;										// ���� ����Ʈ �ð��� �����Ѵ�.
	m_SubQuestTable[dwSubQuestIdx].dwMaxCount = m_CurSubQuestInfoArray[dwSubQuestIdx]->GetMaxCount();	// ���� ����Ʈ ī��Ʈ�� �����Ѵ�.

	m_dwCurSubQuestIdx = dwSubQuestIdx ;																	// �����ϴ� ���� ����Ʈ �ε����� �����Ѵ�.

	InitDemandQuestExecute();
}

void CQuest::StartQuest( DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime )		// ����Ʈ�� �����ϴ� �Լ�.
{
	m_CurSubQuestInfoArray[0] = NULL;														// ���� ����Ʈ ���� �迭0�� NULL ó�� �Ѵ�.
	m_dwSubQuestFlag = dwSubQuestFlag;														// ���� ����Ʈ �÷��׸� �����Ѵ�.
	m_dwData = dwData;																		// �����͸� �����Ѵ�.
	m_RegistTime = registTime;																		// �ð��� �����Ѵ�.
}

void CQuest::EndQuest( DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime )					// ����Ʈ�� �����ϴ� �Լ�.
{
	if( dwSubQuestFlag == 0 )	// �ݺ�														// ���� ����Ʈ �÷��װ� 0�� ���ٸ�,
	{
		Initialize();																		// �ʱ�ȭ �Լ��� ȣ���Ѵ�.
	}
	else																					// �׷��� ������,
	{
		m_dwSubQuestFlag = dwSubQuestFlag;													// ���� ����Ʈ �÷��׸� �����Ѵ�.
		m_dwData = dwData;																	// �����͸� �����Ѵ�.
		m_RegistTime = registTime;															// �ð� ���� �����Ѵ�.

		m_bCheckTime = FALSE;																// �ð� üũ ���θ� FALSE�� �����Ѵ�.
		m_CheckType = 0;																	// üũ Ÿ���� 0���� �����Ѵ�.
		m_CheckTime.value = 0;																// �ð� ���� 0���� �����Ѵ�.
	}
}

void CQuest::StartSubQuest( DWORD dwSubQuestIdx, DWORD dwSubQuestFlag, DWORD dwSubData, DWORD dwSubTime )	// ���� ����Ʈ�� �����ϴ� �Լ�.
{
	m_CurSubQuestInfoArray[dwSubQuestIdx] = m_pQuestInfo->GetSubQuestInfo( dwSubQuestIdx );					// ���� ����Ʈ ������ �����Ѵ�.
	m_dwSubQuestFlag = dwSubQuestFlag;																		// ���� ����Ʈ �÷��׸� �����Ѵ�.
	m_SubQuestTable[dwSubQuestIdx].dwData = dwSubData;														// ���� ����Ʈ �����͸� �����Ѵ�.
	m_SubQuestTable[dwSubQuestIdx].dwTime = dwSubTime;														// ���� ����Ʈ �ð��� �����Ѵ�.
	m_SubQuestTable[dwSubQuestIdx].dwMaxCount = m_CurSubQuestInfoArray[dwSubQuestIdx]->GetMaxCount();		// ���� ����Ʈ ī��Ʈ�� �����Ѵ�.

	m_dwCurSubQuestIdx = dwSubQuestIdx ;																	// �����ϴ� ���� ����Ʈ �ε����� �����Ѵ�.

	InitDemandQuestExecute();
}

void CQuest::EndSubQuest( DWORD dwSubQuestIdx, DWORD dwSubQuestFlag, DWORD dwSubData, DWORD dwSubTime )		// ���� ����Ʈ�� �����ϴ� �Լ�.
{
	m_CurSubQuestInfoArray[dwSubQuestIdx] = NULL;															// ���� ����Ʈ ������ null ó���� �Ѵ�.
	m_dwSubQuestFlag = dwSubQuestFlag;																		// ���� ����Ʈ �÷��׸� �����Ѵ�.
	m_SubQuestTable[dwSubQuestIdx].dwData = dwSubData;														// ���� ����Ʈ �����͸� �����Ѵ�.
	m_SubQuestTable[dwSubQuestIdx].dwTime = dwSubTime;														// ���� ����Ʈ �ð��� �����Ѵ�.

	m_dwCurSubQuestIdx = dwSubQuestIdx ;																	// �����ϴ� ���� ����Ʈ �ε����� �����Ѵ�.

	InitDemandQuestExecute();
}

void CQuest::UpdateSubQuestData( DWORD dwSubQuestIdx, DWORD dwMaxCount, DWORD dwSubData, DWORD dwSubTime )	// ���� ����Ʈ�� ������Ʈ �ϴ� �Լ�.
{
	m_SubQuestTable[dwSubQuestIdx].dwMaxCount = dwMaxCount;													// ���� ����Ʈ �ִ� ī��Ʈ�� �����Ѵ�.
	m_SubQuestTable[dwSubQuestIdx].dwData = dwSubData;														// ���� ����Ʈ �����͸� �����Ѵ�.
	m_SubQuestTable[dwSubQuestIdx].dwTime = dwSubTime;														// ���� ����Ʈ �ð��� �����Ѵ�.

	m_dwCurSubQuestIdx = dwSubQuestIdx ;																	// �����ϴ� ���� ����Ʈ �ε����� �����Ѵ�.

	InitDemandQuestExecute();
}

DWORD CQuest::GetQuestIdx()																					// ����Ʈ �ε����� ��ȯ�ϴ� �Լ�.
{
	return m_pQuestInfo->GetQuestIdx();																		// ����Ʈ ������ ����Ʈ �ε����� ��ȯ�Ѵ�.
}

BOOL CQuest::IsNpcRelationQuest( DWORD dwNpcIdx )															// npc���� ����Ʈ���� üũ�ϴ� �Լ�.
{
	for( DWORD i = 0; i < m_dwSubQuestCount; ++i )															// ���� ����Ʈ ����ŭ for���� ������.
	{
		if( m_CurSubQuestInfoArray[i] )																		// ī��Ʈ�� �ش��ϴ� ���� ����Ʈ ������ ��ȿ�ϸ�,
		if( m_CurSubQuestInfoArray[i]->IsNpcRelationQuest( dwNpcIdx ) )										// npc ���� ����Ʈ�� �´ٸ�,
			return TRUE;																					// TRUE�� �����Ѵ�.
	}
	return FALSE;																							// FALSE�� �����Ѵ�.
}

DWORD CQuest::GetNpcScriptPage( DWORD dwNpcIdx )															// NPC ��ũ��Ʈ �������� ��ȯ�ϴ� �Լ�.
{
	for( DWORD i = 0; i < m_dwSubQuestCount; ++i )															// ���� ����Ʈ ����ŭ for���� ������.
	{
		CSubQuestInfo* const info = m_CurSubQuestInfoArray[i];

		if(0 == info)
		{
			continue;
		}

		const DWORD dwPage = info->GetNpcScriptPage(dwNpcIdx);

		if(0 == dwPage)
		{
			continue;
		}

		return dwPage;
	}
	return 0;																							// �������� �����Ѵ�.
}

BOOL CQuest::IsSubQuestComplete( DWORD dwSubQuestIdx )														// ���� ����Ʈ�� �Ϸ� ���θ� �����ϴ� �Լ�.
{
	return ( m_dwSubQuestFlag & ( 1 << (31-dwSubQuestIdx) ) ) ? TRUE : FALSE;							// ���� ����Ʈ �÷��׿� ���� ���� �����Ѵ�.
}

DWORD CQuest::GetNpcMarkType( DWORD dwNpcIdx )																// npc ��ũ Ÿ���� ��ȯ�Ѵ�.
{
	for( DWORD i = 0; i < m_dwSubQuestCount; ++i )															// ���� ����Ʈ ī��Ʈ ��ŭ for���� ������.
	{
		CSubQuestInfo* const info = m_CurSubQuestInfoArray[i];

		if(0 == info)
		{
			continue;
		}

		const DWORD dwNpcMarkType = info->GetNpcMarkType(dwNpcIdx);

		if(0 == dwNpcMarkType)
		{
			continue;
		}

		return dwNpcMarkType;
	}

	return 0;
}

CSubQuestInfo* CQuest::GetSubQuestInfo( DWORD dwSubQuestIdx )												// ���� ����Ʈ ������ �����ϴ� �Լ�.
{
	return m_pQuestInfo->GetSubQuestInfo( dwSubQuestIdx );													// ���� ���� ����Ʈ �ε����� ���� ����Ʈ ������ ��ȯ�Ѵ�.
}

eQuestState CQuest::IsQuestState( DWORD dwNpcIdx )																	// ����Ʈ ���¸� ��ȯ�ϴ� �Լ�.
{
	for( DWORD i = 0; i < m_dwSubQuestCount; ++i )															// ���� ����Ʈ ī��Ʈ ��ŭ for���� ������.
	{
		if( m_CurSubQuestInfoArray[i] )																		// ���� ����Ʈ ������ ��ȿ�ϸ�,
		{
			eQuestState state = m_CurSubQuestInfoArray[i]->IsQuestState( dwNpcIdx );						// ���� ���� ����Ʈ�� ���� ���� ����� �޴´�.
			if( state != eQuestState_ImPossible )
				return state;
		}
	}
	return eQuestState_ImPossible;
}

BOOL CQuest::CanItemUse( DWORD dwQuestIdx )																	// ������ ��뿩�θ� �����Ѵ�.
{
	if( m_CurSubQuestInfoArray[0] )																			// ���� ����Ʈ ���� �迭 0���� ��ȿ�ϸ�,
	{
		return m_CurSubQuestInfoArray[0]->CheckLimitCondition( dwQuestIdx );								// ����Ʈ ������ üũ�Ͽ� �����Ѵ�.
	}
	return FALSE;																							// FALSE�� �����Ѵ�.
}

void CQuest::RegistCheckTime( BYTE bCheckType, DWORD dwCheckTime )											// �ð� üũ�� ����ϴ� �Լ�.
{
	m_bCheckTime = TRUE;																					// �ð� üũ ���θ� TRUE�� �����Ѵ�.
	m_CheckType = bCheckType;																				// üũ Ÿ���� �����Ѵ�.
	m_CheckTime.value = dwCheckTime;																		// Ÿ�� ���� �����Ѵ�.
}

void CQuest::UnRegistCheckTime()																			// �ð� üũ�� ���� �ϴ� �Լ�.
{
	m_bCheckTime = FALSE;																					// �ð� üũ ���θ� FALSE�� �����Ѵ�.
	m_CheckType = 0;																						// üũ Ÿ���� 0���� �����Ѵ�.
	m_CheckTime.value = 0;																					// Ÿ�� ���� 0���� �����Ѵ�.
}

void CQuest::InitDateTime()
{
	if( !m_pQuestInfo->HasDateLimit() )
	{
		m_NextDateTime = 0;
		m_IsValidDateTime = TRUE;
		return;
	}

	// ��ȿ�ð����� ����
	m_IsValidDateTime = m_pQuestInfo->IsValidDateTime();
	m_NextDateTime = ( m_IsValidDateTime == TRUE ? m_pQuestInfo->GetNextEndDateTime() : m_pQuestInfo->GetNextStartDateTime() );
}

BOOL CQuest::IsValidDateTime()
{
	if( !m_pQuestInfo->HasDateLimit() )
		return TRUE;

	__time64_t curTime = MHTIMEMGR->GetServerTime();

	// ���� ����/���� �ð� �˻�
	if( m_NextDateTime < curTime )
	{
		InitDateTime();
	}

	return m_IsValidDateTime;
}

BOOL CQuest::HasDateLimit() const
{
	return m_pQuestInfo->HasDateLimit();
}

void CQuest::InitDemandQuestExecute()
{
	// ������ �ʿ��� CQuestExecute ��
	int nNeedRefreshQuestExecute[] = {
		eQuestExecute_GiveItem,
	};

	m_DemandQuestExecuteMap.clear();


	for( DWORD i = 0; i < m_dwSubQuestCount; ++i )
	{
		if( !m_CurSubQuestInfoArray[i] )
			continue;

		for( int nLoop=0 ; nLoop < sizeof(*nNeedRefreshQuestExecute)/sizeof(nNeedRefreshQuestExecute) ; ++nLoop )
		{
			m_CurSubQuestInfoArray[i]->StartLoopByQuestExecuteKind( nNeedRefreshQuestExecute[ nLoop ] );
			CQuestExecute* pQuestExecute = NULL;
			while( (pQuestExecute = m_CurSubQuestInfoArray[i]->GetNextQuestExecute()) != NULL )
			{
				DemandQuestExecuteKey key;
				key.first = m_pQuestInfo->GetQuestIdx();
				key.second = i;

				m_DemandQuestExecuteMap.insert( std::make_pair( key, pQuestExecute ) );
			}
		}
	}
}

BOOL CQuest::IsCompletedDemandQuestExecute() const
{
	if( m_DemandQuestExecuteMap.empty() )
		return TRUE;

	typedef std::multimap< DemandQuestExecuteKey, CQuestExecute* >::const_iterator QuestExecuteIter;
	std::pair<QuestExecuteIter, QuestExecuteIter> equalRange;

	DemandQuestExecuteKey key;
	key.first = m_pQuestInfo->GetQuestIdx();
	key.second = m_dwCurSubQuestIdx;
    equalRange = m_DemandQuestExecuteMap.equal_range( key );

	if( equalRange.first == m_DemandQuestExecuteMap.end() &&
		equalRange.second == m_DemandQuestExecuteMap.end() )
		return TRUE;

	for( QuestExecuteIter iter = equalRange.first ; iter != equalRange.second ; ++iter )
	{
		CQuestExecute* pQuestExecute = iter->second;

		switch( pQuestExecute->GetQuestExecuteKind() )
		{
		case eQuestExecute_GiveItem:
			{
				CQuestExecute_Item* pQuestExecute_Item = (CQuestExecute_Item*)pQuestExecute;
				const DWORD dwItemIdx = pQuestExecute_Item->GetItemKind();
				const DWORD dwNeedCnt = pQuestExecute_Item->GetItemNum();
				
				const DWORD dwInvenCnt = GAMEIN->GetInventoryDialog()->GetTotalItemDurability( dwItemIdx );
				if( dwInvenCnt < dwNeedCnt )
					return FALSE;
			}
		}
	}

	return TRUE;
}

void CQuest::AddNpcHasQuest()
{
	for( DWORD i = 0; i < m_pQuestInfo->GetSubQuestCount(); ++i )
	{
		CSubQuestInfo* pSubQuestInfo = m_pQuestInfo->GetSubQuestInfo( i );
		if( pSubQuestInfo == NULL )
			continue;

		const std::map< DWORD, CQuestNpcScript* >& questNpcScriptList = pSubQuestInfo->GetQuestNpcScriptList();
		for( std::map< DWORD, CQuestNpcScript* >::const_iterator questNpcScriptIter = questNpcScriptList.begin() ; questNpcScriptIter != questNpcScriptList.end() ; ++questNpcScriptIter )
		{
			const CQuestNpcScript* const pQuestNpcScript = questNpcScriptIter->second;
			const DWORD dwNpcUniqueIdx = pQuestNpcScript->GetNpcIdx();
			const DWORD dwQuestIdx = GetQuestIdx();

			QUESTMGR->AddNpcHasQuest( dwNpcUniqueIdx, dwQuestIdx );
		}
	}
}