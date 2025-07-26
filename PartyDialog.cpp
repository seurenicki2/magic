// PartyDialog.cpp: implementation of the CPartyDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PartyDialog.h"
#include "PartyMemberDlg.h"
#include "PartyBtnDlg.h"

#include "PartyManager.h"
#include "ObjectManager.h"
#include "WindowIDEnum.h"
#include "./Interface/cWindowManager.h"

#include "GameIn.h"

// 080318 LUJ, ���� ��ų ǥ������ ����
#include "..\interface\cCheckBox.h"
#include "StatusIconDlg.h"
#include "../[cc]skill/Client/Info/BuffSkillInfo.h"
#include "../[cc]skill/Client/Object/BuffSkillObject.h"
#include "../[cc]skill/client/manager/skillmanager.h"

// 061220 LYW --- Delete this code.
//#include "MainBarDialog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPartyDialog::CPartyDialog() 
{
	m_bActive = FALSE;
	m_ClickedMemberID = 0;
	m_bShowMember = TRUE;
	m_bOption = TRUE;
}

CPartyDialog::~CPartyDialog()
{}

void CPartyDialog::RegistMemberDlg(int i, CPartyMemberDlg* pDlg)
{
	m_MemberDlg[i] = pDlg;
	m_MemberDlg[i]->Linking(i);
//	m_MemberDlg[i]->SetPartyBtnDlg( m_PartyBtnDlg );
}

void CPartyDialog::RegistBtnDlg(CPartyBtnDlg* pDlg)
{
	m_PartyBtnDlg = pDlg;
	m_PartyBtnDlg->Linking();
}

BOOL CPartyDialog::IsActive()
{
	return m_bActive;
}

void CPartyDialog::SetActive(BOOL bState)
{
	m_bActive = bState;

	if(bState == TRUE)
	{
		if(PARTYMGR->CanActivate() == FALSE)
		{
			m_bActive = FALSE;
		}
	}

	m_PartyBtnDlg->SetActive(m_bActive);
}

void CPartyDialog::RefreshDlg()
{
	for(int i=0; i<MAX_PARTY_LISTNUM-1; ++i)
		m_MemberDlg[i]->SetMemberData(NULL);

	int membercount=0;

	for(int i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		const DWORD PlayerID = PARTYMGR->GetPartyMemberID(i);

		if(0 == PlayerID)
		{
			continue;
		}
		else if(PlayerID == gHeroID)
		{
			continue;
		}

		m_MemberDlg[membercount++]->SetMemberData(PARTYMGR->GetPartyMemberInfo(i));
	}

	// 090604 ShinJS --- m_ClickedMemberID �ʱ�ȭ �߰�
	m_ClickedMemberID = 0;

	m_PartyBtnDlg->RefreshDlg();
}

void CPartyDialog::MemberBtnPushUp(DWORD id)
{
	for(int i=0; i<MAX_PARTY_LISTNUM-1; ++i)
	{
		if(m_MemberDlg[i]->m_MemberID ==id)
		{
			m_MemberDlg[i]->SetNameBtnPushUp(TRUE);
			m_ClickedMemberID = id;
		}
		else
			m_MemberDlg[i]->SetNameBtnPushUp(FALSE);
	}
}

void CPartyDialog::SetClickedMemberID(DWORD id)
{
	if( id == 0 )
	{
		for( int i = 0; i <MAX_PARTY_LISTNUM-1; ++i )
			m_MemberDlg[i]->SetNameBtnPushUp( FALSE );
		m_ClickedMemberID = 0;
		return;
	}
	else
	{
		CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( id );

		MemberBtnPushUp(id);

		if(pPlayer)
		{
			pPlayer->ShowObjectName( TRUE, NAMECOLOR_SELECTED );

			OBJECTMGR->SetSelectedObject( pPlayer );
		}
	}
}

DWORD CPartyDialog::GetClickedMemberID()
{
	return m_ClickedMemberID;
}

const PARTY_MEMBER& CPartyDialog::GetClickedMember()
{
	for(int i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		const PARTY_MEMBER* const member = PARTYMGR->GetPartyMemberInfo(i);

		if(member->dwMemberID == m_ClickedMemberID)
		{
			return *member;
		}
	}

	static const PARTY_MEMBER emptyMember = {0};
	return emptyMember;
}

// 070420 LYW --- PartyDialog : Add function to return member dialog.
CPartyMemberDlg* CPartyDialog::GetMemberDlg( BYTE index )
{
	if(index<0 || index >= 6) return NULL ;
	
	return m_MemberDlg[index] ;
}


// 080318 LUJ, ��Ƽ�� ���� ��ų ǥ�� �߰�
void CPartyDialog::AddSkill( const SKILLOBJECT_INFO& info )
{
	const BYTE size = sizeof( m_MemberDlg ) / sizeof( *m_MemberDlg );

	for( BYTE index = 0; size > index; ++index )
	{
		CPartyMemberDlg* memberDialog = GetMemberDlg( index );

		if(	!	memberDialog ||
				memberDialog->GetPlayerIndex() != info.MainTarget.dwMainTargetID )
		{
			continue;
		}

		CStatusIconDlg* statusIconDialog = memberDialog->GetStatusIconDialog();

		if( ! statusIconDialog )
		{
			continue;
		}

		statusIconDialog->AddSkillIcon( info.SkillObjectIdx, info.SkillIdx, info.RemainTime, info.Count );
		break;
	}
}


// 080318 LUJ, ��Ƽ�� ���� ��ų ǥ�� ����
void CPartyDialog::RemoveSkill( const SKILLOBJECT_INFO& info )
{
	const BYTE size = sizeof( m_MemberDlg ) / sizeof( *m_MemberDlg );

	for( BYTE index = 0; size > index; ++index )
	{
		CPartyMemberDlg* memberDialog = GetMemberDlg( index );

		if(	!	memberDialog ||
				memberDialog->GetPlayerIndex() != info.MainTarget.dwMainTargetID )
		{
			continue;
		}

		CStatusIconDlg* statusIconDialog = memberDialog->GetStatusIconDialog();

		if( ! statusIconDialog )
		{
			continue;
		}

		statusIconDialog->RemoveSkillIcon( info.SkillObjectIdx, info.SkillIdx );
		break;
	}
}


// 080318 LUJ, �ش� ��Ƽ���� ���� ��� ��ų ǥ�ø� �����Ѵ�
void CPartyDialog::RemoveAllSkill( DWORD playerIndex )
{
	const BYTE size = sizeof( m_MemberDlg ) / sizeof( *m_MemberDlg );

	for( BYTE index = 0; size > index; ++index )
	{
		CPartyMemberDlg* memberDialog = GetMemberDlg( index );

		if( !	memberDialog ||
				memberDialog->GetPlayerIndex() != playerIndex )
		{
			continue;
		}

		CStatusIconDlg* statusIconDialog = memberDialog->GetStatusIconDialog();

		if( ! statusIconDialog )
		{
			continue;
		}

		statusIconDialog->Release();
		break;
	}
}


// 080318 LUJ, ��Ƽ���� ���� ��ų ǥ�ø� ����Ѵ�
void CPartyDialog::OnActionEvent( LONG lId, void* p, DWORD we )
{
	switch( lId )
	{
	case PA_SKILL_ICON_BUTTON:
		{
			cCheckBox* checkBox = 0;
			{
				cDialog*	dialog	= WINDOWMGR->GetWindowForID( PA_BTNDLG );
				checkBox			= ( dialog ? ( cCheckBox* )dialog->GetWindowForID( PA_SKILL_ICON_BUTTON ) : 0 );

				if( !	checkBox						||
						checkBox->GetType() != WT_CHECKBOX )
				{
					break;
				}
			}

			const BYTE size = sizeof( m_MemberDlg ) / sizeof( *m_MemberDlg );

			for( BYTE index = 0; size > index; ++index )
			{
				CPartyMemberDlg* dialog = GetMemberDlg( index );

				if( dialog )
				{
					dialog->SetVisibleSkill( checkBox->IsChecked() );
				}
			}

			break;
		}
	case PA_MEMBER1_SKILL_ICON_BUTTON:
		{
			CPartyMemberDlg* dialog = GetMemberDlg( 0 );

			if( dialog )
			{
				dialog->SetVisibleSkill( ! dialog->IsVisibleSkill() );
			}

			break;
		}
	case PA_MEMBER2_SKILL_ICON_BUTTON:
		{
			CPartyMemberDlg* dialog = GetMemberDlg( 1 );

			if( dialog )
			{
				dialog->SetVisibleSkill( ! dialog->IsVisibleSkill() );
			}

			break;
		}
	case PA_MEMBER3_SKILL_ICON_BUTTON:
		{
			CPartyMemberDlg* dialog = GetMemberDlg( 2 );

			if( dialog )
			{
				dialog->SetVisibleSkill( ! dialog->IsVisibleSkill() );
			}

			break;
		}
	case PA_MEMBER4_SKILL_ICON_BUTTON:
		{
			CPartyMemberDlg* dialog = GetMemberDlg( 3 );

			if( dialog )
			{
				dialog->SetVisibleSkill( ! dialog->IsVisibleSkill() );
			}

			break;
		}
	case PA_MEMBER5_SKILL_ICON_BUTTON:
		{
			CPartyMemberDlg* dialog = GetMemberDlg( 4 );

			if( dialog )
			{
				dialog->SetVisibleSkill( ! dialog->IsVisibleSkill() );
			}

			break;
		}
	case PA_MEMBER6_SKILL_ICON_BUTTON:
		{
			CPartyMemberDlg* dialog = GetMemberDlg( 5 );

			if( dialog )
			{
				dialog->SetVisibleSkill( ! dialog->IsVisibleSkill() );
			}

			break;
		}
	}
}


// 080318 LUJ, ��Ƽ���� ���� Ŭ���̾�Ʈ�� ���� ������ ���� ������ â�� ǥ���Ѵ�
void CPartyDialog::InitializeSkill( const PARTY_MEMBER& member )
{
	// 080318 LUJ, ������ ����â�� ���´�
	CStatusIconDlg* statusIconDialog = 0;
	{
		for(
			BYTE index = sizeof( m_MemberDlg ) / sizeof( *m_MemberDlg );
			index--; )
		{
			CPartyMemberDlg* dialog = GetMemberDlg( index );

			if( dialog &&
				dialog->GetPlayerIndex() == member.dwMemberID )
			{
				statusIconDialog = dialog->GetStatusIconDialog();
				break;
			}
		}

		if( ! statusIconDialog )
		{
			return;
		}

		statusIconDialog->Release();
		statusIconDialog->SetObject( HERO );
	}

	// 080318 LUJ, ��ų ������Ʈ�� ��� �˻��ؼ� Ÿ���� �߰��� ��Ƽ���� ���, �߰����ش�
	{
		CYHHashTable< cSkillObject >& hashTable = SKILLMGR->GetSkillObjectHashTable();
		hashTable.SetPositionHead();
		
		for(cSkillObject* object = hashTable.GetData();
			0 < object;
			object = hashTable.GetData())
		{
			const sSKILL_CREATE_INFO& createInfo = object->GetSkillObjectInfo();

			if(createInfo.mainTarget.dwMainTargetID != member.dwMemberID)
			{
				continue;
			}

			cBuffSkillInfo* skillInfo = ( cBuffSkillInfo* )createInfo.pSkillInfo;

			if( !	skillInfo ||
					skillInfo->GetKind() != SKILLKIND_BUFF )
			{
				continue;
			}

			cBuffSkillObject*		buffSkillObject = ( cBuffSkillObject* )object;
			const BUFF_SKILL_INFO*	buffSkillInfo	= skillInfo->GetSkillInfo();

			if( ! buffSkillInfo )
			{
				continue;
			}

			DWORD time = 0;

			// 080318 LUJ, ���� ��ų�� ��ȿ �ð��� Ŭ���̾�Ʈ���� ������Ʈ���� �����Ƿ�, ���� �ð��� �������� ����Ѵ�
			if(createInfo.remainTime)
			{
				time = createInfo.remainTime + buffSkillObject->GetBeginTime();
			}
			else if( buffSkillInfo->DelayTime )
			{
				time = buffSkillInfo->DelayTime + buffSkillObject->GetBeginTime();
			}

			statusIconDialog->AddSkillIcon( object->GetID(), skillInfo->GetIndex(), time, buffSkillInfo->Count );
		}
	}
}

// 080613 LUJ, ��Ƽ ���� ǥ�� ��ġ�� �ٽ� �����Ѵ�. ���� �������� ���� ���� ó�� �ð��� �����ǹǷ�, ���μ��� �ܿ����� ȣ������ �ʵ��� ����.
//
//	����: 	���� RefreshDlg()�� �־�����, ���� ȣ��Ǿ� Ŭ���̾�Ʈ ó�� �ӵ��� ���ϵǾ���. ������ �Լ��� �и��ϰ� �̺�Ʈ �߻� �ÿ���
//			ó���ϵ��� ��
void CPartyDialog::RelocateSkill()
{
	// 080318 LUJ, ��ų ���� ����
	//typedef std::list< StatusInfo >	SkillList;

	// 080318 LUJ, ��Ƽ�� �� ǥ�� ���� ���� ��ų�� �����Ѵ�
	//typedef std::map< DWORD, SkillList >	SkillMap;
	SkillMap								skillMap;

	// 080318 LUJ,	������ �����ϱ� ���� ���� â���� ��ų���� �����ͼ� �÷��̾� �ε��� ���� ��������
	//				�׷��� ��Ƽ ��� ���� â ǥ�� ��ġ�� ����Ǿ ��ų ��ġ�� ���������� ǥ�õǵ��� �Ѵ�.
	for(
		BYTE index = sizeof( m_MemberDlg ) / sizeof( *m_MemberDlg );
		index--; )
	{
		CPartyMemberDlg* memberDialog = GetMemberDlg( index );

		if( !	memberDialog	||
			!	memberDialog->IsActive() )
		{
			continue;
		}

		CStatusIconDlg* statusIconDialog = memberDialog->GetStatusIconDialog();

		if( ! statusIconDialog )
		{
			continue;
		}

		SkillList& skillList = skillMap[ memberDialog->GetPlayerIndex() ];

		statusIconDialog->Copy( skillList );
		statusIconDialog->Release();
	}

	// 080318 LUJ,	��Ƽ���� Ż��/�����ϸ� ��ġ�� �����Ǿ� ��ų ǥ�� ��ġ�� �ٲپ��־�� �Ѵ�.
	//				�����͸� �����ؼ� �Űܾ��Ѵ�.
	for(
		BYTE index = sizeof( m_MemberDlg ) / sizeof( *m_MemberDlg );
		index--; )
	{
		CPartyMemberDlg* memberDialog = GetMemberDlg( index );

		if( ! memberDialog )
		{
			continue;
		}

		SkillMap::const_iterator list_it = skillMap.find( memberDialog->GetPlayerIndex() );

		if( skillMap.end() == list_it )
		{
			continue;
		}

		CStatusIconDlg* statusIconDialog = memberDialog->GetStatusIconDialog();

		if( ! statusIconDialog )
		{
			continue;
		}

		statusIconDialog->Release();
		statusIconDialog->SetObject( HERO );

		const SkillList& skillList = list_it->second;

		// 080318 LUJ, ���� ǥ�� ���� ��ų�� ������ ���� �߰��Ѵ�
		for(
			SkillList::const_iterator it = skillList.begin();
			skillList.end() != it;
		++it )
		{
			const StatusInfo& info = *it;

			statusIconDialog->AddSkillIcon( info.ID, info.Index, info.Time, info.Count );
		}
	}
}

void CPartyDialog::BackupMemberSkillList()
{
	m_skillMap.clear();

	for(BYTE i=0; i<MAX_PARTY_LISTNUM; ++i)
	{
		CPartyMemberDlg* memberDialog = GetMemberDlg( i );

		if( !	memberDialog	||
			!	memberDialog->IsActive() )
		{
			continue;
		}

		CStatusIconDlg* statusIconDialog = memberDialog->GetStatusIconDialog();

		if( ! statusIconDialog )
		{
			continue;
		}

		SkillList& skillList = m_skillMap[ memberDialog->GetPlayerIndex() ];

		statusIconDialog->Copy( skillList );
		statusIconDialog->Release();
	}
}

void CPartyDialog::UpdateMemberSkillList()
{
	for(BYTE i=0; i<MAX_PARTY_LISTNUM; ++i)
	{
		CPartyMemberDlg* memberDialog = GetMemberDlg( i );

		if( ! memberDialog )
		{
			continue;
		}

		SkillMap::const_iterator list_it = m_skillMap.find( memberDialog->GetPlayerIndex() );

		if( m_skillMap.end() == list_it )
		{
			continue;
		}

		CStatusIconDlg* statusIconDialog = memberDialog->GetStatusIconDialog();

		if( ! statusIconDialog )
		{
			continue;
		}

		statusIconDialog->Release();
		statusIconDialog->SetObject( HERO );

		const SkillList& skillList = list_it->second;

		// ���� ǥ�� ���� ��ų�� ������ ���� �߰��Ѵ�
		for(
			SkillList::const_iterator it = skillList.begin();
			skillList.end() != it;
		++it )
		{
			const StatusInfo& info = *it;

			statusIconDialog->AddSkillIcon( info.ID, info.Index, info.Time, info.Count );
		}
	}
}

void CPartyDialog::RemoveMemberSkillList(DWORD dwPlayerIdx)
{
	SkillMap::iterator list_it = m_skillMap.find( dwPlayerIdx );
	if(list_it != m_skillMap.end())
	{
		m_skillMap.erase(list_it);
	}
}

void CPartyDialog::ClearMemberSkillList()
{
	m_skillMap.clear();
}