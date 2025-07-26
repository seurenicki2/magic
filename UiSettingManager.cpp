#include "stdafx.h"
#include "UiSettingManager.h"
#include "./Interface/cWindowManager.h"
#include "../[CC]Header/GameResourceManager.h"
#include "interface/cDialog.h"
#include "WindowIDEnum.h"
#include "QuickSlotDlg.h"
#include "MHAudioManager.h"

GLOBALTON(cUiSettingManager)

cUiSettingManager::cUiSettingManager(void)
{
	LoadSettingInfo(); // never load ui position
}

cUiSettingManager::~cUiSettingManager(void)
{}

// 091207 ONS Ȱ��ȭ �������� ó�� �߰�
void cUiSettingManager::SetSettingList( DWORD dwWindowID, BOOL bIsSavePos, BOOL bIsSaveActivity )
{
	if( !dwWindowID )
		return;

	// UI���û��� ���̺��� �����Ѵ�.
	stUiSettingState pState;
	ZeroMemory(&pState, sizeof(stUiSettingState));
	pState.dwID = dwWindowID;
	pState.bSavePosition = bIsSavePos;
	pState.bSaveActivity = bIsSaveActivity;
	m_UiSetting.push_back(pState);

}

void cUiSettingManager::ResetUiSettingInfo()
{
	if(m_UiSetting.size() > m_UiSettingMap.size())
	{
		// ���ο� UI������ �߰��� ��� ���������� �߰��Ѵ�.
		std::vector<stUiSettingState>::iterator iter;
		for( iter  = m_UiSetting.begin(); iter != m_UiSetting.end(); ++iter )
		{
			stUiSettingState pState = *iter;
			
			const stSettingProperty* pProperty = GetUiSettingInfo( pState.dwID );
			if( pProperty )
				continue;

			cDialog* dlg = WINDOWMGR->GetWindowForID( pState.dwID );
			if( !dlg )
				continue;
			
			// ������ ���������� ���Ե��� ������ �߰��Ѵ�.
			stUiSettingInfo stInfo;
			ZeroMemory(&stInfo, sizeof(stUiSettingInfo));

			stInfo.dwID = pState.dwID;
			if(pState.bSavePosition)
			{
				stInfo.property.vPos.x = dlg->GetAbsX();
				stInfo.property.vPos.y = dlg->GetAbsY();
			}
			if(pState.bSaveActivity)
			{
				stInfo.property.bActivity = dlg->IsActive();
			}
			AddSettingInfo( &stInfo );
		}		
	}
	else
	{
		// UI������ ����������� ���������� �����Ѵ�.
		std::vector<stUiSettingInfo> vTempInfo;
		std::vector<stUiSettingState>::iterator iter;
		for( iter  = m_UiSetting.begin(); iter != m_UiSetting.end(); ++iter )
		{
			stUiSettingState pState = *iter;
			const stSettingProperty* pProperty = GetUiSettingInfo( pState.dwID );
			if( pProperty )
			{
				// �ش� ���������� �����ϸ� �����Ѵ�.
				stUiSettingInfo stInfo;
				ZeroMemory(&stInfo, sizeof(stUiSettingInfo));

				stInfo.dwID = pState.dwID;
				stInfo.property.bActivity = pProperty->bActivity;
				stInfo.property.vPos.x = pProperty->vPos.x;
				stInfo.property.vPos.y = pProperty->vPos.y;

				vTempInfo.push_back(stInfo);
			}
		}
		m_UiSettingMap.clear();

		for(int i = 0; i< (int)vTempInfo.size(); i++)
		{
			m_UiSettingMap[vTempInfo[i].dwID] = vTempInfo[i].property;
		}
		vTempInfo.clear();
	}
}

void cUiSettingManager::ApplySettingInfo()
{
	// UI���������� ��ȭ�� �ִٸ� ���������� �����Ѵ�.
	if(m_UiSetting.size() != m_UiSettingMap.size())
	{
		ResetUiSettingInfo();
	}

	// LUJ, �������̽��� �����ϴ� ����, ȿ������ ������ �ʵ��� �Ѵ�.
	//		���� �� ���� �������� �ǵ�����.
	const float soundVolume = AUDIOMGR->GetSoundVolume(
		1);
	AUDIOMGR->SetSoundVolume(
		0);

	for(std::vector<stUiSettingState>::iterator iter  = m_UiSetting.begin();
		iter != m_UiSetting.end();
		++iter)
	{
		stUiSettingState pState = *iter;
		cDialog* dlg = WINDOWMGR->GetWindowForID( pState.dwID );
		if( NULL == dlg )
			continue;
		
		// 091207 ONS Ȱ��ȭ �������� ó�� �߰�
		const stSettingProperty* pProperty = GetUiSettingInfo( pState.dwID );
		if( NULL == pProperty )
			continue;
		
		if(pState.bSavePosition)
		{
			VECTOR2 vPos = {0};
			// 100510 ONS Ȯ�� �������� ��� ������ ��ġ�� ����Ұ�� �ػ󵵰� ����ʿ����� �⺻�����԰� �Ÿ��� ��������
			// �ǹǷ� �⺻������ ��ġ�� �������� ������ġ�� �����Ѵ�.
			if( pState.dwID == QI2_QUICKSLOTDLG )
			{
				cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
				if( !dialog )
				{
					continue;
				}

				DWORD dwQuickSlotHeight = dlg->GetHeight();
				vPos.x = dialog->GetAbsX();
				vPos.y = dialog->GetAbsY() - dwQuickSlotHeight;
			}
			else
			{
				// 100127 ONS �ػ󵵺��濡 ���� ���̾�α��� ��ġ�� �����Ų��.
				// �ػ󵵰� �پ����� ��� ȭ��ۿ� ��� ���̾�α��� ��ġ�� �����Ѵ�.
				const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
				// ����� ���̾�α� X��ǥ�� �ػ󵵺��� Ŭ��� �����Ѵ�.
				if(pProperty->vPos.x + dlg->GetWidth() > dispInfo.dwWidth)
				{
					vPos.x = float(dispInfo.dwWidth - dlg->GetWidth());
				}
				else
				{
					vPos.x = pProperty->vPos.x;
				}

				// ����� ���̾�α� Y��ǥ�� �ػ󵵺��� Ŭ��� �����Ѵ�.
				if(pProperty->vPos.y + dlg->GetHeight() > dispInfo.dwHeight)
				{
					vPos.y = float(dispInfo.dwHeight - dlg->GetHeight());
				}
				else
				{
					vPos.y = pProperty->vPos.y;
				}
			}

			dlg->SetAbsXY((LONG)vPos.x, (LONG)vPos.y);
		}
		if(pState.bSaveActivity)
		{
			dlg->SetActive(pProperty->bActivity);
		}
	}

	AUDIOMGR->SetSoundVolume(
		soundVolume);
}

void cUiSettingManager::UpdateSettingInfo()
{
	// �ʿ��� ������ ����� UI������ �����Ѵ�.
	std::vector<stUiSettingState>::iterator iter;
	for( iter  = m_UiSetting.begin(); iter != m_UiSetting.end(); ++iter )
	{
		stUiSettingState pState = *iter;
		
		cDialog* dlg = WINDOWMGR->GetWindowForID( pState.dwID );
		if( NULL == dlg )
			continue;
		
		if(pState.bSavePosition)
		{
			SetUiPosition( pState.dwID, dlg->GetAbsX(), dlg->GetAbsY() );
		}
		// 091207 ONS Ȱ��ȭ �������� ó�� �߰�
		if(pState.bSaveActivity)
		{
			SetUiCurActivity(pState.dwID, dlg->IsActive());
		}
	}
	if( !m_UiSetting.empty() )
	{
		m_UiSetting.clear();
	}
}

// 100304 ONS �ػ󵵸� �����Կ����� ���̾�α��� ��ġ�� ������������ ǥ�õǴ� ������ �ذ��ϱ�����
// ����ȭ���� 4�и����� ������ ������ �𼭸��� �������� ���̾�α׸� ��ġ��Ű���� ����.
void cUiSettingManager::LoadSettingInfo()
{
	// ȭ���� 4����� ������ �����Ѵ�.
/*	SetScreenRect();

	// ���� �⵿�� UI���������� �ε��Ѵ�.
	FILE* const file = fopen( "./system/UiSetting.txt", "rt");
	if( !file )	return;

	const char* delimit = " \n\t=,~";
	const char* markBegin = "{";
	const char* markEnd = "}";
	const BYTE	LOAD_SUCCESS = 5;
	    	
	DWORD		dwOpenCnt = 0;
	DWORD		dwWindowID = 0;
	DWORD		dwWidth = 0;
	DWORD		dwHeight = 0;
	BOOL		bIsComment = FALSE;
	BOOL		bIsDlgBlock = FALSE;
	BYTE		byActivity = FALSE;
	BYTE		byLoadCount = 0;
	VECTOR2		posRate = {0};
	eDispSector sector = eDispSector_None;

	while(0 == feof( file ))
	{
		char buff[ MAX_PATH ] = {0,};
		fgets( buff, sizeof( buff ) / sizeof( *buff ), file );

		char* token = strtok( buff, delimit );
		if( ! token )
		{
			continue;
		}
		else if( bIsComment )
		{
			continue;
		}
		else if( ! stricmp( "DLG", token ) )
		{
			bIsDlgBlock = TRUE;
			byLoadCount = 0;
		}
		else if( ! strnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++dwOpenCnt;
		}
		else if( ! strnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			--dwOpenCnt;
			bIsDlgBlock = FALSE;
		}
		else if( !bIsDlgBlock )
		{
			continue;
		}

		// ���̾�α� ���������� �о���δ�.
        if( stricmp( token, "ID" ) == 0 )
		{
			token = strtok( 0, delimit );
			if( token )
			{
				dwWindowID = (DWORD)atoi( token );
				byLoadCount++;
			}
		}
		else if( stricmp( token, "Sector" ) == 0 )
		{
			token = strtok( 0, delimit );
			if( token )
			{
				sector = (eDispSector)atoi( token );
				byLoadCount++;
			}
		}
		else if( stricmp( token, "Rate" ) == 0 )
		{
			token = strtok( 0, delimit );
			if( token )
			{
				posRate.x = (float)atof( token );
				if( posRate.x < 0.05f )	
					posRate.x = 0.0f;

				token = strtok( 0, delimit );
				if( token )
				{
					posRate.y = (float)atof( token );
					if( posRate.y < 0.05f )	
						posRate.y = 0.0f;

					byLoadCount++;
				}
            }
		}
		else if( stricmp( token, "Size" ) == 0 )
		{
			token = strtok( 0, delimit );
			if( token )
			{
				dwWidth = (DWORD)atoi( token );

				token = strtok( 0, delimit );
				if( token )
				{
					dwHeight = (DWORD)atoi( token );
					byLoadCount++;
				}
			}
		}
		else if( stricmp( token, "Activity" ) == 0 )
		{
			token = strtok( 0, delimit );
			if( token )
			{
				byActivity = (BYTE)atoi( token );
				byLoadCount++;
			}
		}

		// �ε��׸��� ������ üũ�� �� �����Ѵ�.
		if( LOAD_SUCCESS == byLoadCount )
		{
			stUiSettingInfo stInfo;
			ZeroMemory(&stInfo, sizeof(stUiSettingInfo));

			stInfo.dwID = dwWindowID;
			stInfo.property.bActivity = byActivity;

			// ȭ���� 4�������� �������� ���̾�α��� ��ġ ������ǥ�� �����Ѵ�.	
			const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
			switch(sector)
			{
			case eDispSector_1:
				{
					stInfo.property.vPos.x = m_screenRect[sector].left + ( dispInfo.dwWidth/2 * posRate.x );
					stInfo.property.vPos.y = m_screenRect[sector].top + ( dispInfo.dwHeight/2 * posRate.y );
				}
				break;
			case eDispSector_2:
				{
					stInfo.property.vPos.x = m_screenRect[sector].right - ( dispInfo.dwWidth/2 * posRate.x ) - dwWidth;
					stInfo.property.vPos.y = m_screenRect[sector].top + ( dispInfo.dwHeight/2 * posRate.y );
				}
				break;
			case eDispSector_3:
				{
					stInfo.property.vPos.x = m_screenRect[sector].left + ( dispInfo.dwWidth/2 * posRate.x );
					stInfo.property.vPos.y = m_screenRect[sector].bottom - ( dispInfo.dwHeight/2 * posRate.y ) - dwHeight;
				}
				break;
			case eDispSector_4:
				{
					stInfo.property.vPos.x = m_screenRect[sector].right - ( dispInfo.dwWidth/2 * posRate.x ) - dwWidth;
					stInfo.property.vPos.y = m_screenRect[sector].bottom - ( dispInfo.dwHeight/2 * posRate.y ) - dwHeight;
				}
				break;
			}

			AddSettingInfo(&stInfo);
		}
	}
	fclose(file);*/
} 

// 100304 ONS �ػ󵵸� �����Կ����� ���̾�α��� ��ġ�� ������������ ǥ�õǴ� ������ �ذ��ϱ�����
// ����ȭ���� 4�и����� ������ ������ �𼭸��� �������� ���̾�α׸� ��ġ��Ű���� ����.
void cUiSettingManager::SaveSettingInfo()
{
	// ���� ����� UI���������� �����Ѵ�.
/*	FILE* const file = fopen( "./system/UiSetting.txt", "wt");
	if( !file )		return;
    
	stdext::hash_map< DWORD, stSettingProperty >::iterator it;
	for( it  = m_UiSettingMap.begin(); it != m_UiSettingMap.end(); ++it )
	{
		DWORD dwWindowID = (it->first);
		const stSettingProperty* pProperty = GetUiSettingInfo( dwWindowID );
		if( NULL == pProperty )
		{
			continue;
		}

		stUiSettingInfo stInfo;
		ZeroMemory(&stInfo, sizeof(stUiSettingInfo));

		stInfo.dwID = dwWindowID;
		memcpy(&stInfo.property, pProperty, sizeof(stSettingProperty));

		// ���̾�αװ� ��ġ�� ���Ϳ� �ش� ���Ϳ����� ��ġ�� ������ ���Ѵ�.
		const eDispSector Sector = (eDispSector)GetDisplaySector( pProperty );
		if( eDispSector_None != Sector )
		{
			const VECTOR2&	  PosRate = SetPositionRate( pProperty, Sector );
			fprintf(file, "DLG\n{");
			fprintf(file, "\n\tID = %d", dwWindowID);
			fprintf(file, "\n\tSector = %d", int(Sector));
			fprintf(file, "\n\tRate = %f, %f", PosRate.x, PosRate.y);
			fprintf(file, "\n\tSize = %d, %d", pProperty->dwWidth, pProperty->dwHeight);
			fprintf(file, "\n\tActivity = %d", pProperty->bActivity);
			fprintf(file, "\n}\n");
		}
	}
	fclose(file); */
}

void cUiSettingManager::AddSettingInfo( stUiSettingInfo* pInfo )
{
	// UI���� ����(��ġ, Ȱ��ȭ �Ӽ�)�� �߰��Ѵ�.
	stSettingProperty* pProperty = NULL;
	pProperty = &( m_UiSettingMap[ pInfo->dwID ] );
	pProperty->vPos.x = pInfo->property.vPos.x;
	pProperty->vPos.y = pInfo->property.vPos.y;
	pProperty->bActivity = 	pInfo->property.bActivity;
}

const stSettingProperty* cUiSettingManager::GetUiSettingInfo(DWORD dwWindowID)
{
	// �ش� �������� UI���� ����(��ġ, Ȱ��ȭ �Ӽ�)�� �����´�.
	UiSettingMap::const_iterator it = m_UiSettingMap.find(dwWindowID);
	return m_UiSettingMap.end() == it ? 0 : &( it->second );
}

void cUiSettingManager::SetUiPosition( DWORD dwWindowID, float fPosX, float fPosY )
{
	// ����� UI��ġ������ �����Ѵ�.
	UiSettingMap::iterator it = m_UiSettingMap.find(dwWindowID);
	stSettingProperty* pProperty = &(it->second);
	if( NULL == pProperty )
		return;

	pProperty->vPos.x = fPosX;
	pProperty->vPos.y = fPosY;

	cDialog* dlg = WINDOWMGR->GetWindowForID( dwWindowID );
	if( dlg )
	{
		pProperty->dwWidth = dlg->GetWidth();
		pProperty->dwHeight = dlg->GetHeight();
	}
}

void cUiSettingManager::SetUiCurActivity( DWORD dwWindowID, BOOL bCurActivity )
{
	// ����� UIȰ��ȭ���� ������ �����Ѵ�.
	UiSettingMap::iterator it = m_UiSettingMap.find(dwWindowID);
	stSettingProperty* pProperty = &(it->second);
	if( NULL == pProperty )
		return;
	pProperty->bActivity = bCurActivity;
}

// 100304 ONS �ش� ���̾�αװ� ��ġ�ϴ� �κ��� ���Ѵ�.(1/4, 2/4, 3/4, 4/4�и�)
int cUiSettingManager::GetDisplaySector( const stSettingProperty* pProperty )
{
	for( BYTE sector = eDispSector_1; sector < eDispSector_None; sector++ )
	{
		if( m_screenRect[sector].left	<= pProperty->vPos.x	&& 
			m_screenRect[sector].right	>= pProperty->vPos.x	&&
			m_screenRect[sector].top	<= pProperty->vPos.y	&&
			m_screenRect[sector].bottom >= pProperty->vPos.y )
				return sector;
	}
	return eDispSector_None;
}

// 100304 ONS ���̾�α��� ��ġ�� �� �𼭸��� �������� ������ �����Ѵ�.
VECTOR2 cUiSettingManager::SetPositionRate( const stSettingProperty* pProperty, eDispSector Sector )
{
	VECTOR2 posRate = {0};

	switch( Sector )
	{
	case eDispSector_1:
		{
			posRate.x = (pProperty->vPos.x - m_screenRect[Sector].left) / (m_screenRect[Sector].right - m_screenRect[Sector].left);
			posRate.y = (pProperty->vPos.y - m_screenRect[Sector].top) / (m_screenRect[Sector].bottom - m_screenRect[Sector].top);
		}
		break;
	case eDispSector_2:
		{
			posRate.x = (m_screenRect[Sector].right - (pProperty->vPos.x + pProperty->dwWidth)) / (m_screenRect[Sector].right - m_screenRect[Sector].left);
			posRate.y = (pProperty->vPos.y - m_screenRect[Sector].top) / (m_screenRect[Sector].bottom - m_screenRect[Sector].top);
		}
		break;
	case eDispSector_3:
		{
			posRate.x = (pProperty->vPos.x - m_screenRect[Sector].left) / (m_screenRect[Sector].right - m_screenRect[Sector].left);
			posRate.y = (m_screenRect[Sector].bottom - (pProperty->vPos.y + pProperty->dwHeight)) / (m_screenRect[Sector].bottom - m_screenRect[Sector].top);
		}
		break;
	case eDispSector_4:
		{
			posRate.x = (m_screenRect[Sector].right - (pProperty->vPos.x + pProperty->dwWidth)) / (m_screenRect[Sector].right - m_screenRect[Sector].left);
			posRate.y = (m_screenRect[Sector].bottom - (pProperty->vPos.y + pProperty->dwHeight)) / (m_screenRect[Sector].bottom - m_screenRect[Sector].top);
		}
		break;
	}

	// ȭ���� ����� ��� ��ġ�� �����Ѵ�.
	if(posRate.x < 0)
	{
		posRate.x = 0.0f;
	}
	if(posRate.y < 0)
	{
		posRate.y = 0.0f;
	}

	return posRate;
}

// 100304 ONS ȭ���� 4����� ������ �����Ѵ�.
void cUiSettingManager::SetScreenRect()
{
	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();	

	m_screenRect[eDispSector_1].left = 0;
	m_screenRect[eDispSector_1].top = 0;
	m_screenRect[eDispSector_1].right = dispInfo.dwWidth/2;
	m_screenRect[eDispSector_1].bottom = dispInfo.dwHeight/2;
	
	m_screenRect[eDispSector_2].left = dispInfo.dwWidth/2+1;
	m_screenRect[eDispSector_2].top = 0;
	m_screenRect[eDispSector_2].right = dispInfo.dwWidth;
	m_screenRect[eDispSector_2].bottom = dispInfo.dwHeight/2;

	m_screenRect[eDispSector_3].left = 0;
	m_screenRect[eDispSector_3].top = dispInfo.dwHeight/2+1;
	m_screenRect[eDispSector_3].right = dispInfo.dwWidth/2;
	m_screenRect[eDispSector_3].bottom = dispInfo.dwHeight;	

	m_screenRect[eDispSector_4].left = dispInfo.dwWidth/2+1;
	m_screenRect[eDispSector_4].top = dispInfo.dwHeight/2+1;
	m_screenRect[eDispSector_4].right = dispInfo.dwWidth;
	m_screenRect[eDispSector_4].bottom = dispInfo.dwHeight;	
}