#pragma once
#include ".\interface\cdialog.h"

class cListDialog;
class cStatic;

class cSkillTrainingDlgJct :
	public cDialog
{
	cListDialog*	m_SkillListDlg;
	cListDialog*	m_SkillTipListDlg;
	
	cStatic*		m_SkillPoint;
	cStatic*		m_Gold;

	cStatic*		m_Icon;

	cStatic*		m_NeedSp;
	cStatic*		m_NeedGold;
	cStatic*		m_AddDamageOptTooltip;

	SkillData		m_SkillList[ 100 ];

	int				m_Index;

	// 080417 LUJ, ��� ��ų ǥ�� �������� ��Ÿ���� �÷���
	BOOL			mGuildMode;

public:
	cSkillTrainingDlgJct(void);
	virtual ~cSkillTrainingDlgJct(void);

	void Linking();

	// 080618 LYW : ��Ȳ�� �°� �μ� â�� ǥ���ϴ� ����� �ϴ� �Լ� �߰�.
	void DisplayGuildSkill() ;

	// 080618 LYW : HERO�� ���� ���� �� �߿� �ش��ϴ� ��ų�� �ִ��� Ȯ���ϰ�, �ִٸ�,
	//			  : �� ��ų�� ���� ������ �ִ� ������ ��ȯ�ϴ� �Լ� �߰�.
	int GetSkillLevelFromJobTree(DWORD dwSkillIdx, WORD wCurJobLevel) ;

	// 080618 LYW : ������ ���� ��ų���� / ��ų�� / ���� / �������� �����ϴ� �Լ� �߰�.
	void AddSkillToSkillListDlg(BYTE byJobType, WORD wJobLevel, WORD* pPos, LONG lId) ;

	// ��ų ���� â�� ���� �Լ�.
	void OpenDialog(LONG lId) ;//() JobTree

	// 080417 LUJ, ��� ��ų ǥ��
	void OpenGuildSkill();
	// 080417 LUJ, ��� ��ų �н� â�̸� ���� ��ȯ�Ѵ�
	BOOL IsGuildMode() const { return mGuildMode; }

	void OnActionEvent( LONG lId, void* p, DWORD we );

	void SetSkillInfo();
	void SetGold();
	void SetSkillPoint();

	void Training();
};
