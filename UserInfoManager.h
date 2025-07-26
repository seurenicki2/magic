// UserInfoManager.h: interface for the CUserInfoManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERINFOMANAGER_H__D1F0B1D0_16F6_4DA1_ABA7_278233F2BEB1__INCLUDED_)
#define AFX_USERINFOMANAGER_H__D1F0B1D0_16F6_4DA1_ABA7_278233F2BEB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


enum eUSERINFO_KIND
{
	eUIK_NONE		= 0,
	eUIK_INTERFACE	= 1,
	eUIK_USERSTATE	= 2,
	eUIK_USERQUEST	= 3,
	eUIK_ALL		= 0xFFFFFFFF,
};

struct sDIALOG_INFO
{
	LONG	lPosX;
	LONG	lPosY;
	DWORD	dwFlag;
	DWORD	dwParam;
};

struct sUSERINFO_USERSTATE
{
	float fZoomDistance;
};


struct sQUEST_INFO																		// ����Ʈ ���� ����ü.
{
	DWORD dwQuestIdx ;																	// ���� ����Ʈ �ε���.
	//DWORD dwSubQuestIdx[32] ;															// ���� ����Ʈ �ε��� �迭.

	BOOL  bCompleteMainQuest ;

    BOOL  bComplete[32] ;																// ���� ����Ʈ�� �Ϸ� ���� �迭.

	int   nCount[32] ;																	// ���� ����Ʈ�� ���� ī��Ʈ �迭.
	int   nTotalCount[32] ;																// ���� ����Ʈ�� ��ǥ ī��Ʈ �迭.

	sQUEST_INFO()																		// �ʱ�ȭ �Լ�.
	{
		dwQuestIdx = 0 ;																// ���� ����Ʈ �ε����� 0���� �����Ѵ�.

		for( int count = 0 ; count < 32; ++count )										// �ִ� ũ���� ���� ����Ʈ�� 32, ��ŭ for���� ������.
		{
		//	dwSubQuestIdx[count] = 0 ;													// ���� ����Ʈ �ε����� 0���� �����Ѵ�.

			bComplete[count] = FALSE ;													// ���� ����Ʈ �� �Ϸ� ���θ� false�� �����Ѵ�.

			nCount[count] = 0 ;															// ���� ����Ʈ �� ���� ī��Ʈ�� 0���� �����Ѵ�.
			nTotalCount[count] = 0 ;													// ���� ����Ʈ �� ��ǥ ī��Ʈ�� 0���� �����Ѵ�.
		}
	} 
} ;

struct sUSERINFO_QUESTQUICKVIEW															// ����Ʈ �˸��̿� ��� �� ����Ʈ ���̵� �����ϴ� ����ü.
{
	DWORD dwQuestID[MAX_QUICKVIEW_QUEST] ;												// ����Ʈ �ִ� ��� ���� �� ũ���� �迭�� ����� ���´�.

	//cPtrList list ;

	//sQUEST_INFO	questInfo[MAX_QUEST_COUNT] ;

	//sQUEST_INFO quickInfo[MAX_QUICKVIEW_QUEST] ;
} ;

// 071028 LYW --- UserInfoManager : Add struct to write error of give item function of quest group.
struct sUSERINFO_ERR_GIVEITEM												// ����Ʈ �׷쿡�� GiveItem ������ ����� ����ü.
{
	char UserName[32] ;														// ���� �̸��� ��� ����.
	BYTE bCaption ;															// ���� �߻� �Լ� ĸ���� ���� ����.
	BYTE bErrNum ;															// ���� ��ȣ�� ���� ����.
	DWORD dwQuestIdx ;														// ����Ʈ �ε����� ���� ����.
	DWORD dwItemIdx ;														// ������ �ε����� ���� ����.
} ;


#define USERINFOMGR USINGTON(CUserInfoManager)

class CUserInfoManager  
{
protected:
	char m_strUserID[MAX_NAME_LENGTH+1];
	DWORD m_dwSaveFolderName;

//for interface
//	cPtrList m_listInterface;
	BOOL m_bMapChange;

public:
	CUserInfoManager();
	virtual ~CUserInfoManager();

	void LoadUserInfo( DWORD dwFlag );
	void SaveUserInfo( DWORD dwFlag );
	void LoadUserState( char* strFilePath );
	void SaveUserState( char* strFilePath );
	// ������ ����Ʈ �˸��� ������ �����ϴ� �Լ�.
	void SaveUserQuestQuickView( char* strFilePath );
	// ������ ����Ʈ �˸��� ������ �ε��ϴ� �Լ�.
	void LoadUserQuestQuickView( char* strFilePath );
	void DeleteUserQuestInfo(DWORD playerIndex);
	void SetUserID( char* strID );
	char* GetUserID() { return m_strUserID; }

//for Interface
	BOOL IsValidInterfacePos( RECT* prcCaption, LONG lX, LONG lY );
//	void AddSaveDialog( cDialog* pDlg );

	void SetMapChage( BOOL bMapChange ) { m_bMapChange = bMapChange; }

	void SetSaveFolderName( DWORD dwUserID );
};


EXTERNGLOBALTON(CUserInfoManager)

#endif // !defined(AFX_USERINFOMANAGER_H__D1F0B1D0_16F6_4DA1_ABA7_278233F2BEB1__INCLUDED_)
