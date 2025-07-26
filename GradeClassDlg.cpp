#include "stdafx.h"
#include "GradeClassDlg.h"
#include "WindowIDEnum.h"
#include "./Interface/cStatic.h"
#include "./Interface/cPushupButton.h"
#include "./Interface/cButton.h"
#include "./Interface/cResourceManager.h"
#include "ObjectManager.h"
#include "./Tutorialmanager.h"			


CGradeClassDlg::CGradeClassDlg()
{
}


CGradeClassDlg::~CGradeClassDlg()
{
}


void CGradeClassDlg::Add( cWindow* window )
{
	WORD wWindowType = window->GetType() ;																		// ������ Ÿ���� �޾ƿ´�.

	if( wWindowType == WT_PUSHUPBUTTON )																		// ������ Ÿ���� Ǫ���� ��ư�̸�,
	{
		AddTabBtn( curIdx1++, (cPushupButton*)window ) ;														// ���� �ε���1�� �� ��ư�� �߰��Ѵ�.
	}
	else if( wWindowType == WT_DIALOG )																			// ������ Ÿ���� ���̾�α��̸�,
	{
		AddTabSheet( curIdx2++, window ) ;																		// ���� �ε���2�� ��Ʈ�� �߰��Ѵ�.
	}
	else																										// �̵� ���� �ƴϸ�,
	{
		cTabDialog::Add( window ) ;																				// �����츦 �߰��Ѵ�.
	}
}

void CGradeClassDlg::Init()
{
	for(int i=0; i<MAX_FIGHT_CLASS_BTN; i++)
	{
		//m_pFighterClassBtn[i]->SetPush(TRUE);
		//m_pFighterClassBtn[i]->SetDisable(TRUE);
	}
	for(int i=0; i<MAX_ROGUE_CLASS_BTN; i++)
	{
		//m_pRogueClassBtn[i]->SetPush(TRUE);
		//m_pRogueClassBtn[i]->SetDisable(TRUE);
	}
	for(int i=0; i<MAX_MAGE_CLASS_BTN; i++)
	{
		//m_pMageClassBtn[i]->SetPush(TRUE);
		//m_pMageClassBtn[i]->SetDisable(TRUE);
	}

	for(int i=0; i<MAX_DEVIL_CLASS_BTN; i++)
	{
		//m_pDevilClassBtn[i]->SetPush(TRUE);
		//m_pDevilClassBtn[i]->SetDisable(TRUE);
	}

	SelectTab( eClass_FIGHTER );
}

void CGradeClassDlg::Linking()
{
	// ���� �ܰ� ��ư �ε�
	for(int i=0; i<6; i++)
	{
		m_pStaticGradeBtn[i] = (cButton*)GetWindowForID(GRADECLASS_1 + i);
	//	m_pStaticGradeBtn[i]->SetActive(TRUE);
	//	m_pStaticGradeBtn[i]->SetDisable(TRUE);
	}
	// ������ ���� ��ư �ε�
	for(int i=0; i<MAX_FIGHT_CLASS_BTN; i++)
	{
		m_pFighterClassBtn[i] = (cButton*)((cTabDialog*)GetTabSheet(eClass_FIGHTER))->GetWindowForID(CLASS_1 + i);
		//m_pFighterClassBtn[i]->SetActive(TRUE);
	}
	// �α� ���� ��ư �ε�
	for(int i=0; i<MAX_ROGUE_CLASS_BTN; i++)
	{
		m_pRogueClassBtn[i] = (cButton*)((cTabDialog*)GetTabSheet(eClass_ROGUE))->GetWindowForID(CLASS_2 + i);
	//	m_pRogueClassBtn[i]->SetActive(TRUE);
	}
	// ������ ���� ��ư �ε�
	for(int i=0; i<MAX_MAGE_CLASS_BTN; i++)
	{
		m_pMageClassBtn[i] = (cButton*)((cTabDialog*)GetTabSheet(eClass_MAGE))->GetWindowForID(CLASS_3 + i);
	//	m_pMageClassBtn[i]->SetActive(TRUE);
	}
	// ���� ���� ��ư �ε�
	for(int i=0; i<MAX_DEVIL_CLASS_BTN; i++)
	{
		m_pDevilClassBtn[i] = (cButton*)((cTabDialog*)GetTabSheet(eClass_DEVIL))->GetWindowForID(CLASS_4 + i);
	//	m_pDevilClassBtn[i]->SetActive(TRUE);
	}
}

void CGradeClassDlg::Render()
{
	cDialog::RenderWindow();
	cDialog::RenderComponent();
	cTabDialog::RenderTabComponent();
}


void CGradeClassDlg::SetActive( BOOL val )
{
	if(val)
	{
		Init();
		//if( !TUTORIALMGR->CheckTutorialByIndex(GLADECLASS_TUTORIAL_NO) )
		//{
			CHARACTER_TOTALINFO ChaInfo;
			OBJECTMGR->GetHero()->GetCharacterTotalInfo(&ChaInfo);

			SelectTab( ChaInfo.Job[0]-1 );	
			// ĳ���� ���� Ʈ�� ����
			switch(ChaInfo.Job[0]-1)
			{
				case eClass_FIGHTER:	
					SetClassTree(m_pFighterClassBtn, eClass_FIGHTER);
					break;
				case eClass_ROGUE: 
					SetClassTree(m_pRogueClassBtn, eClass_ROGUE);
					break;
				case eClass_MAGE: 
					SetClassTree(m_pMageClassBtn, eClass_MAGE);
					break;
				case eClass_DEVIL:
					SetClassTree(m_pDevilClassBtn, eClass_DEVIL);
					break;
			}
		//}
	}

	cTabDialog::SetActive( val );
	//TUTORIALMGR->Check_OpenDialog(this->GetID(), val) ;
}

// �� ���� ������ �Ǵ��Ͽ� ĳ������ ���������� ���� �ش� ������ư�� Ȱ��ȭ�Ѵ�.
void CGradeClassDlg::SetClassTree(cButton** pPushBtn, ENUM_CLASS eClass)
{
    if(pPushBtn == NULL) return;

	// �� Ŭ������ ���������� ���� �����Ѵ�.
	const BYTE ClassLevelArray[eClass_MAXCLASS][MAX_CLASS_LEVEL] = 
		{	{1,2,3,4,6,3}, 
			{1,2,3,4,5,3}, 
			{1,2,3,4,5,3}, 
			{1,1,1,1,1,1} //0//fix majin
		};

	// ���������� ������� ĳ���� ������ ��´�.
	CHARACTER_TOTALINFO ChaInfo;
	OBJECTMGR->GetHero()->GetCharacterTotalInfo(&ChaInfo);

    // 1������ - ������ ��ư Ȱ��ȭ
	//pPushBtn[0]->SetPush(FALSE);
	// �� ���������� ĳ������ ���� ������ ���Ͽ� �ش� ��ư�� Ȱ��ȭ��Ų��.
	int nIdx = 1;
	int nLevel = 1;
	while(nLevel < ChaInfo.JobGrade)//+1//bug
	{
		for(int i = 0; i < ClassLevelArray[eClass][nLevel]; i++, nIdx++)
		{
			if(ChaInfo.Job[nLevel] == i+1)
			{
				//pPushBtn[nIdx]->SetPush(TRUE);
				pPushBtn[nIdx]->SetDisable(FALSE);//TRUE
			}
		}
		nLevel++;
	};
}
