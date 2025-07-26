// UserInfoManager.cpp: implementation of the CUserInfoManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UserInfoManager.h"

#include "WindowIDEnum.h"
#include "./Interface/cWindowManager.h"
#include "GameIn.h"
#include "MHCamera.h"
#include "QuestManager.h"
#include "../[CC]Header/GameResourceManager.h"
#include "QuestQuickViewDialog.h"

GLOBALTON(CUserInfoManager)

#define USERINFO_USERSTATE_VER 0x0006060801

CUserInfoManager::CUserInfoManager()
{
	m_strUserID[0] = 0;
	m_bMapChange = FALSE;
	m_dwSaveFolderName = 0;
}

CUserInfoManager::~CUserInfoManager()
{
	
}

void CUserInfoManager::SetSaveFolderName( DWORD dwUserID )
{
	const DWORD Key = 740705;

	m_dwSaveFolderName = dwUserID + Key;
}

void CUserInfoManager::LoadUserInfo( DWORD dwFlag )
{
	char strFilePath[MAX_PATH];

	if( dwFlag & eUIK_USERSTATE )
	{
		wsprintf( strFilePath, "%s\\data\\interface\\log\\%d\\%d.UIS", DIRECTORYMGR->GetFullDirectoryName(eLM_Root), m_dwSaveFolderName, m_dwSaveFolderName );
		LoadUserState( strFilePath );
	}

	if(0 < gHeroID)
	{
		if( dwFlag & eUIK_USERQUEST )
		{
			wsprintf(
				strFilePath,
				"%s\\data\\interface\\log\\%d\\%d.UIQ",
				DIRECTORYMGR->GetFullDirectoryName(eLM_Root),
				m_dwSaveFolderName,
				gHeroID);
			LoadUserQuestQuickView( strFilePath ) ;
		}
	}
}

void CUserInfoManager::SaveUserInfo( DWORD dwFlag )
{
	if( dwFlag == eUIK_NONE ) return;

	char strFilePath[MAX_PATH];

	//make directory
	wsprintf( strFilePath, "%s\\data\\interface\\log", DIRECTORYMGR->GetFullDirectoryName(eLM_Root) );
	CreateDirectory( strFilePath, NULL );
	wsprintf( strFilePath, "%s\\data\\interface\\log\\%d", DIRECTORYMGR->GetFullDirectoryName(eLM_Root), m_dwSaveFolderName );
	CreateDirectory( strFilePath, NULL );

	if(0 < gHeroID)
	{
		if( dwFlag & eUIK_USERSTATE )
		{
			wsprintf( strFilePath, "%s\\data\\interface\\log\\%d\\%d.UIS", DIRECTORYMGR->GetFullDirectoryName(eLM_Root), m_dwSaveFolderName, m_dwSaveFolderName );
			SaveUserState( strFilePath );
		}

		if( dwFlag & eUIK_USERQUEST )
		{
			wsprintf(
				strFilePath,
				"%s\\data\\interface\\log\\%d\\%d.UIQ",
				DIRECTORYMGR->GetFullDirectoryName(eLM_Root),
				m_dwSaveFolderName,
				gHeroID);
			SaveUserQuestQuickView( strFilePath );
		}
	}
}

void CUserInfoManager::LoadUserState( char* strFilePath )
{
	HANDLE hFile = CreateFile( strFilePath, GENERIC_READ, 0, NULL, 
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE ) return;

	DWORD dwSize;
	DWORD dwVersion;
	if( !ReadFile( hFile, &dwVersion, sizeof(DWORD), &dwSize, NULL ) )
	{
		CloseHandle( hFile );
		return;
	}
	if( dwVersion != USERINFO_USERSTATE_VER )
	{
		CloseHandle( hFile );
		return;
	}

	sUSERINFO_USERSTATE UIS;
	if( !ReadFile( hFile, &UIS, sizeof(UIS), &dwSize, NULL ) )
	{
		CloseHandle( hFile );
		return;
	}

	CloseHandle( hFile );

	if(m_bMapChange && gHeroID)
	{
		CAMERA->SetDistance( 0, UIS.fZoomDistance, 0 );
	}
}

void CUserInfoManager::SaveUserState( char* strFilePath )
{
	if(0 == gHeroID)
	{
		return;
	}

	sUSERINFO_USERSTATE UIS;
	UIS.fZoomDistance = CAMERA->GetDistance(0);

	HANDLE hFile = CreateFile( strFilePath, GENERIC_WRITE, 0, NULL, 
								CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE ) return;

	DWORD dwSize;
	DWORD dwVersion = USERINFO_USERSTATE_VER;
	WriteFile( hFile, &dwVersion, sizeof(DWORD), &dwSize, NULL );
	WriteFile( hFile, &UIS, sizeof(UIS), &dwSize, NULL );

	CloseHandle( hFile );
}

void CUserInfoManager::SaveUserQuestQuickView( char* strFilePath )
{
	sUSERINFO_QUESTQUICKVIEW UIQ = {0};
	CQuestQuickViewDialog* pQuickView = GAMEIN->GetQuestQuickViewDialog();

	if( !pQuickView ) return ;

	cPtrList* pList = NULL ;
	pList = pQuickView->GetViewList() ;

	if( !pList ) return ;

	DWORD* pIdx ;

	int nCount = 0 ;

	PTRLISTPOS pos = NULL ;
	pos = pList->GetHeadPosition() ;

	while(pos)
	{
		pIdx = NULL ;
		pIdx = (DWORD*)pList->GetNext(pos) ;

		if( !pIdx ) continue ;

		UIQ.dwQuestID[nCount] = *pIdx ;

		++nCount ;
	}

	HANDLE hFile = CreateFile( strFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,		// ���� �н��� ���� ������ �����Ѵ�.
							   FILE_ATTRIBUTE_NORMAL, NULL ) ;

	if( hFile == INVALID_HANDLE_VALUE )													// ����� ���� ������ ���� ó���� �Ѵ�.
	{
		return ;
	}

	DWORD dwSize = 0;
	DWORD dwVersion = gHeroID;
	WriteFile( hFile, &dwVersion, sizeof(DWORD), &dwSize, NULL ) ;						// ������ ����� ����Ѵ�.
	WriteFile( hFile, &UIQ, sizeof(UIQ), &dwSize, NULL ) ;								// ����Ʈ �ε��� ������ ����Ѵ�.

	CloseHandle( hFile ) ;																// ������ �ݴ´�.
}

void CUserInfoManager::LoadUserQuestQuickView( char* strFilePath ) 						// ������ ����Ʈ �˸��� ������ �ε��ϴ� �Լ�.
{
	HANDLE hFile = CreateFile( strFilePath, GENERIC_READ, 0, NULL,						// ������ ���� �н��� ������ �д´�.
							   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ) ;

	if( hFile == INVALID_HANDLE_VALUE )													// �����ϸ�, 
	{
		return ;																		// ���� ó���� �Ѵ�.
	}

	DWORD dwSize ;																		// ����� ���� ������ �����Ѵ�.
	DWORD dwVersion ;																	// ������ ���� ������ �����Ѵ�.

	if( !ReadFile( hFile, &dwVersion, sizeof(DWORD), &dwSize, NULL ) )					// ������� ������ �о���δ�. �����ϸ�,
	{
		CloseHandle( hFile ) ;															// ������ �ݴ´�.
		return ;																		// ���� ó���� �Ѵ�.
	}
	else if(dwVersion != gHeroID)
	{
		CloseHandle( hFile ) ;															// ������ �ݴ´�.
		return ;																		// ���� ó���� �Ѵ�.
	}

	sUSERINFO_QUESTQUICKVIEW UIQ ;														// ����Ʈ �˸��̿� ��� �� ����Ʈ ���̵� �����ϴ� ����ü.

	if( !ReadFile( hFile, &UIQ, sizeof(UIQ), &dwSize, NULL ) )							// ����Ʈ ���̵� �о���δ�. �����ϸ�,
	{
		CloseHandle( hFile ) ;															// ������ �ݴ´�.
		return ;																		// ���� ó���� �Ѵ�.
	}

	CloseHandle( hFile ) ;																// ������ �ݴ´�.

	QUESTMGR->LoadQuickViewInfoFromFile(UIQ.dwQuestID) ;				// ����Ʈ �Ŵ����� �о���� ������ �����Ѵ�.
}

void CUserInfoManager::DeleteUserQuestInfo(DWORD playerIndex)
{
	char strFilePath[MAX_PATH];
	wsprintf(
		strFilePath,
		"%s\\data\\interface\\log\\%d\\%d.UIQ",
		DIRECTORYMGR->GetFullDirectoryName(eLM_Root),
		m_dwSaveFolderName,
		playerIndex);
	DeleteFile(strFilePath) ;
}


void CUserInfoManager::SetUserID( char* strID )
{
	strcpy( m_strUserID, strID );
}


//for Interface
BOOL CUserInfoManager::IsValidInterfacePos( RECT* prcCaption, LONG lX, LONG lY )
{
	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();

	if( lX + prcCaption->left < 0 )		return FALSE;
	if( lX + prcCaption->right > (LONG)(dispInfo.dwWidth) ) return FALSE;
	if( lY + prcCaption->top < 0 )		return FALSE;
	if( lY + prcCaption->bottom > (LONG)(dispInfo.dwHeight) ) return FALSE;

	// 070202 LYW --- End.

	return TRUE;
}