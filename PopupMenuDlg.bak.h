#pragma once
#include "./interface/cdialog.h"

class cStatic;
class cButton;

class CPopupMenuDlg :
	public cDialog
{
	
	cStatic* mFace;
	cStatic* mName;

	cButton* mWhisper;
	cButton* mTrade;
	cButton* mFallow;
	cButton* mFriend;
	cButton* mParty;
	cButton* mGuild;
	cButton* mFamily;
	cButton* mVersus;

	//090615 pdy �Ͽ�¡ ĳ���� ��Ŭ�� ��ũ �Ͽ콺���� ��� �߰�
	cButton* mVisitHouse;
	cButton* mQuickDate;

	// 100121 ONS PC�渮��Ʈ���� �˾��޴��� Ȱ��ȭ���״��� ����.
	BOOL mPCRoomDlgOpen;			
	char mSelectedName[MAX_NAME_LENGTH + 1];
	DWORD mSelectedIndex;

public:
	CPopupMenuDlg(void);
	virtual ~CPopupMenuDlg(void);

	void Linking();
	virtual void OnActionEvent(LONG lId, LPVOID, DWORD we) ;
	virtual void SetActive(BOOL);
	virtual void SetActiveByChat(BOOL, DWORD);
	virtual DWORD ActionEvent(CMouse*);
	void SetActivePCRoomDlg(DWORD dwPlayerIdx, const char* pObjectName, BOOL val);
};
