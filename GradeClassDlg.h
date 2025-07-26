#pragma once
#include "./Interface/ctabdialog.h"

#define MAX_FIGHT_CLASS_BTN	19
#define MAX_ROGUE_CLASS_BTN	18
#define MAX_MAGE_CLASS_BTN	18
#define MAX_DEVIL_CLASS_BTN 6


// ���� ���̵� Ʃ�丮�� ��ȣ
#define GLADECLASS_TUTORIAL_NO	20

class cPushupButton;
class cStatic;
class cButton;

// ���� Ʈ�� ���̵� Ŭ����
// 100302 ONS ���� Ʈ�� ���̵� ���̾�α׸� ������ ����, ���� ���� �߰�
class CGradeClassDlg : public cTabDialog
{
	// ���� ������ư
	cButton*		m_pStaticGradeBtn[6];

	// ������ ��ư�׷�
    cButton*		m_pFighterClassBtn[MAX_FIGHT_CLASS_BTN];
	cButton*		m_pRogueClassBtn[MAX_ROGUE_CLASS_BTN];
	cButton*		m_pMageClassBtn[MAX_MAGE_CLASS_BTN];
	cButton*		m_pDevilClassBtn[MAX_DEVIL_CLASS_BTN];

public:
	CGradeClassDlg();
	virtual ~CGradeClassDlg();

	void Linking();
	void Init();

	virtual void Add( cWindow* );
	virtual void Render();
	virtual void SetActive( BOOL val );
private:
	void SetClassTree(cButton** pPushBtn, ENUM_CLASS eClass);
};