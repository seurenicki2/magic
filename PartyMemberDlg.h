// PartyMemberDlg.h: interface for the CPartyMemberDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTYMEMBERDLG_H__7426A94E_644C_44BE_80FE_877AE4A137B7__INCLUDED_)
#define AFX_PARTYMEMBERDLG_H__7426A94E_644C_44BE_80FE_877AE4A137B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Interface/cDialog.h"

class CObjectGuagen;
class cPushupButton;
class cStatic;

// 080318 LUJ, ��Ƽ�� ���� ��ų ǥ������ Ŭ���� ����
class CStatusIconDlg;
class cCheckBox;

class CPartyMemberDlg  : public cDialog
{
	cPushupButton * m_pName;
	CObjectGuagen * m_pLife;
	CObjectGuagen * m_pMana;
	cStatic		  * m_pLevel;
	// 091127 LUJ, ��Ƽ ������ �������� ǥ���Ѵ�
	cImage			m_MasterMark;

public:

	DWORD m_MemberID;
	
	CPartyMemberDlg();
	virtual ~CPartyMemberDlg();
	
	void SetActive(BOOL val);
	void Linking(int i);
	void SetMemberData(PARTY_MEMBER*);
	void SetNameBtnPushUp(BOOL);
	virtual DWORD ActionEvent(CMouse*);
	virtual void Render();
	// 080318 LUJ, ��Ƽâ�� ǥ���ϰ� �ִ� �÷��̾� ��ȣ�� ��ȯ�Ѵ�
	inline DWORD GetPlayerIndex() const { return m_MemberID; }

	// 080318 LUJ,	���� ������ ǥ�� ��ġ ���� ���� �������̵�.
	//				����: SetAbsX(), SetAbsY() ���� �Լ��� �ִµ� ������� �ʾ� �������̵����� �ʾҴ�
	virtual void SetAbsXY( LONG x, LONG y);

	// 080318 LUJ, ���� ��ų�� ǥ�� ����
	void SetVisibleSkill( BOOL );
	BOOL IsVisibleSkill() const;
	void SetLife(float);
	void SetMana(float);
	void SetLevel(LONG);

private:
	void SetTip(const PARTY_MEMBER&);

	// 080318 LUJ, ��Ƽ�� ���� ��ų ǥ�� ����
private:	
	CStatusIconDlg* mStatusIconDialog;
public:
	inline CStatusIconDlg* GetStatusIconDialog() const { return mStatusIconDialog; }

	// 080318 LUJ, ���� ��ų ��� ���� ��Ʈ��
private:	
	cCheckBox* mCheckBox;
	VECTOR2 mMasterMarkPosition;
};

#endif // !defined(AFX_PARTYMEMBERDLG_H__7426A94E_644C_44BE_80FE_877AE4A137B7__INCLUDED_)
