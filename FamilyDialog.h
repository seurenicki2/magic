#pragma once

#define MAX_MEMBER_COUNT 5

#include "INTERFACE\cDialog.h"																		// ���̾�α� �ش� ������ �ҷ��´�.

class cButton;
class cListDialog;
class cStatic;
class cImage;
class CSHFamily;
class CSHFamilyMember;
class CFamily;

class CFamilyDialog : public cDialog  
{
	cStatic*	m_pFamilyName;																		// �йи� �̸�.
	cStatic*	m_pMasterName;																		// ������ �̸�.
	cStatic*	m_pHonorPoint ;																		// �� ����Ʈ.

	cListDialog*	m_pLvListDlg;																	// ��� Lv ���� ����Ʈ ���̾�α�.
	cListDialog*	m_pIDListDlg;																	// ��� ID ���� ����Ʈ ���̾�α�.

	cStatic*	m_pMemberID ;																		// ��� ���̵�.
	cStatic*	m_pMemberLevel ;																	// ��� ����.
	cStatic*	m_pMemberClass ;																	// ��� Ŭ����.
	cStatic*	m_pMemberLogin ;																	// ��� �α���.
	cStatic*	m_pMemberNickName ;																	// ��� ȣĪ.
	cStatic*	m_pMemberGuildName ;																// ��� ����.

	cButton*	m_pChangeNickName ;																	// ȣĪ���� ��ư.
	cButton*	m_pInvite ;																			// �ʴ��û ��ư.
	cButton*	m_pRegistMark ;																		// ������ ��ư.

	// desc_hseos_�йи�01
	// S �йи� �߰� added by hseos 2007.10.22
	cButton*	m_pLeaveBtn;
	cButton*	m_pExpelBtn;
	cButton*	m_pFarmManageBtn;
	// 091204 ONS �йи� ���̾�α׿� �йи��� �̾� �� ���弼�� ���� ��� ��ư �߰�
	cButton*	m_pPayTaxBtn;
	cButton*	m_pTransferBtn;
	cImage		m_pClassImg[MAX_MEMBER_COUNT];
	cImage		m_pLongInImg[MAX_MEMBER_COUNT];
	cImage		m_pLogOutImg[MAX_MEMBER_COUNT];
	int			m_nCurSelectIdx;

public:
	CFamilyDialog();
	virtual ~CFamilyDialog();
	void Linking();
	void UpdateAllInfo();
	void SetMemberList(CSHFamily*);
	void SetMemberInfo(CSHFamilyMember*);
	virtual void Render();
	virtual DWORD ActionEvent(CMouse* mouseInfo);
	void OnActionEvent( LONG lId, void* p, DWORD we );
	int GetSelectedMemberIdx() const { return m_nCurSelectIdx; }
	void SetHonorPoint(DWORD dwPoint);
	int GetClassIconNum(int nClass);
	void SelectMemberOfList(int nIndex);
};