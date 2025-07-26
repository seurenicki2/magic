/**********************************************************************

�̸�: ProgressDialog
�ۼ�: 2007/09/18 17:35:59, �̿���

����:	����/����/��ȭ/��æƮ �� ���α׷����ٸ� ǥ���Ѵ�.
		������ �۾��� ��û ������ �����صξ��ٰ�, ������ �ð� ���Ŀ�
		�ڵ����� ��û�ȴ�.
                                                                     
***********************************************************************/
#pragma once


#include "interface/cDialog.h"

// 071213 KTH -- ������ 2��
#define WAITMILISECOND	2000
#define WAITMILISECOND_FOR_COOK	2800

class cStatic;
class CObjectGuagen;
class cTextArea;

class CProgressDialog : public cDialog
{
public:
	CProgressDialog();
	virtual ~CProgressDialog();

	virtual void Render();
	virtual void SetActive( BOOL );
	virtual void OnActionEvent( LONG id, void* p, DWORD event );

	void Linking();
	void SetText( const char* );

	enum eAction
	{
		eActionNone,
		eActionEnchant,		// ��æƮ
		eActionMix,			// ����
		eActionReinforce,	// ��ȭ
		eActionApplyOption,	// �ɼ� ����
		eActionPetSummon,
		eActionPetGradeUp,
		eActionPetResurrection,
		// 080916 LUJ, ������ �ռ� �߰�
		eActionCompose,

		// 080826 LYW --- ProgressDialog : ���ͽõ� ���.
		eActionUseWaterSeed,

		eActionChangeSize,	// Ű����
		eActionSummonMonster, // ���ͼ�ȯ
		
		eActionExSummonEffect,	// ����Ʈ��ȯ
		eActionExCooking,	// �丮
		// 090422 ShinJS --- Ż�� ��ȯ/����
		eActionVehicleSummon,
		eActionVehicleSeal,
	};

	void Wait(eAction);
	void Wait(DWORD second, LPCTSTR, cbFUNC);
	void Cancel();
	eAction	GetActionType() const { return	mAction.mType; }

private:
	void Restore();

	cWindow* mTextImage;
   	cTextArea* mTextArea;
   	cWindow* mCancelBtn;
	cStatic* mText;
	CObjectGuagen* mGauge;

	struct Action
	{
		DWORD mBeginTick;
		DWORD mEndTick;
		eAction	mType;
		BOOL mIsWaiting;
		cbFUNC mFunctionPointer;
	}
	mAction;

	BOOL  m_bHideDlg;
	DWORD m_dwExtra;
	// 090422 ShinJS --- Ż�� ��ȯ ��ġ�� ����, Progress ������ ��ȯ��Ŷ���۽� ���
	VECTOR3	m_vecVehicleSummonPos;

public:
	void SetVehicleSummonPos( const VECTOR3& vecPos ) { m_vecVehicleSummonPos = vecPos; }
};
