#pragma once

#define TUTORIALMGR	USINGTON(cTutorialManager)

enum ENUM_TUTORIAL_TYPE
{
	e_TT_FirstGameIn = 0 ,
	e_TT_LevelUp,
	e_TT_Max,
} ;

class CItem;
class cTutorial;
class cTutorial_Msg;
class cTutorialDlg;
class cHelperDlg;
class cTutorialBtnDlg;
class CMHFile;

struct HELPLINK																// ���� ��ũ ����ü�� �����Ѵ�.
{	
	int nIndex ;															// ��� �� ����Ʈ �ε���.
	int nTutorial_Idx ;														// Ŭ���� ���� �� Ʃ�丮�� �ε���.
	char Title[128] ;														// ��� �� ����Ʈ Ÿ��Ʋ.
} ;

class cTutorialManager														// Ʃ�丮���� ������ �Ŵ��� Ŭ������ �����Ѵ�.
{
	cPtrList			m_Tutorial_Msg ;									// Ʃ�丮�� �޽����� �����ϴ� ����Ʈ.
	cPtrList			m_Tutorial_List ;									// Ʃ�丮�� ������ �����ϴ� ����Ʈ.
	cPtrList			m_Helper_List ;										// ���� Ŭ������ �����ϴ� Ʃ�丮�� ������ �����ϴ� ����Ʈ.

	BOOL				m_bOnOff_Tutorial ;									// Ʃ�丮���� ���̱� ���� ����.

	cTutorialDlg*		m_pTutorialDlg ;									// Ʃ�丮�� ������ ���� ���̾�α� ������.
	cHelperDlg*			m_pHelperDlg ;										// ���� ��� ������ ���� ���̾�α� ������.
	cTutorialBtnDlg*	m_pBtnDlg ;											// Ʃ�丮�� �˸� ��ư ���̾�α� ������.

	BOOL				m_bActiveTutorial ;									// Ʃ�丮���� ���̱� ����.
	BOOL				m_bUpdated ;										// Ʃ�丮�� �̺�Ʈ�� ������Ʈ �Ǿ����� üũ�ϴ� ����.

	int					m_nCurTutorialIdx ;									// ���� �������� Ʃ�丮�� �ε���.
	int					m_nCompletedIdx ;
	SNDIDX				m_nShow_BtnSndIdx;
	SNDIDX				m_nEndTutorial_SndIdx;

public:
	// BASE PART.
	cTutorialManager(void);													// ������ �Լ�.
	virtual ~cTutorialManager(void);										// �Ҹ��� �Լ�.

	void Init() ;															// �ʱ�ȭ �Լ�.
	void Release() ;														// ���� �Լ�.


	// TUTORIAL PART.
	BOOL LoadTutorial(char* strFileName) ;									// Ʃ�丮�� ������ �ε��ϴ� �Լ�.
	void ReleaseTutorial() ;												// Ʃ�丮�� ������ �����ϴ� �Լ�.

	cTutorial* LoadMainTutorial(CMHFile* pFile) ;							// ���� Ʃ�丮�� ������ �ε��ϴ� �Լ�.
	BOOL LoadSubTutorial(CMHFile* pFile, cTutorial* pTutorial) ;			// ���� Ʃ�丮�� ������ �ε��ϴ� �Լ�.

	cTutorial* Get_Tutorial(int nIndex) ;									// �ε����� �ش��ϴ� Ʃ�丮�� ������ �����ϴ� �Լ�.


	// TUTORIAL MESSAGE PART.
	BOOL LoadMsg(char* strFileName) ;										// Ʃ�丮�� �޽����� �ε��ϴ� �Լ�.
	void ReleaseMsg() ;														// Ʃ�丮�� �޽����� �����ϴ� �Լ�.

	BOOL ReadMsgLine(CMHFile* pFile, cTutorial_Msg* pMsg) ;					// Ʃ�丮�� �޽����� ������ �д� �Լ�.

	cTutorial_Msg* Get_TutorialMsg(int nIndex) ;							// �ε����� �ش��ϴ� Ʃ�丮�� �޽��� ������ �����ϴ� �Լ�.


	// HELPER PART.
	BOOL LoadHelperInfo(char* strFileName) ;								// ���� ������ �ε��ϴ� �Լ�.
	void ReleaseHelperInfo() ;												// ���� ������ �����ϴ� �Լ�.

	HELPLINK* Get_HelpLink(int nIndex) ;									// �ε����� �ش��ϴ� ���� ��ũ�� �����ϴ� �Լ�.

	void Start_HelpTutorial(cTutorial* pTutorial) ;							// ���� ��ũ���� ���� �� Ʃ�丮���� �����ϴ� �Լ�.


	// NETWORK PART.
	void NetworkMsgParse(BYTE Protocol,void* pMsg) ;						// ��Ʈ��ũ �޽��� �Ľ� �Լ�.


	// PROCESS PART.
	//void Process() ;														// ���μ��� �Լ�.
	BOOL Check_StartCondition(cTutorial* pTutorial) ;						// Ʃ�丮�� �ߵ� ������ üũ�ϴ� �Լ�.

	BOOL Check_NpcTalk(void* pMsg) ;										// npc�� ��ȭ�� �ϴ� ������ üũ�ϴ� �Լ�.
	BOOL Check_NpcTalk_StartCondition(DWORD dwNpcIdx) ;						// npc�� ��ȭ ���� ��, ���� ������ üũ�ϴ� �Լ�.
	BOOL Check_NpcTalk_EndCondition(DWORD dwNpcIdx) ;						// npc�� ��ȭ ���� ��, �Ϸ� ������ üũ�ϴ� �Լ�.
	// 080506 LYW --- TutorialManager : ������ ��, ���� ������ tutorial�� �ִ��� Ȯ�� ��, ������ ����.
	void Check_LevelUp() ;

	void Check_GetItem(CItem* pItem) ;										// �������� �������� �� Ʃ�丮�� ���� ���θ� üũ�ϴ� �Լ�.

	void Check_OpenDialog(DWORD dwWindowID, BOOL bVal) ;					// â�� ���� �Ϸ� ������ üũ�ϴ� �Լ�.


	// ETC
	void Set_ActiveTutorial(BOOL bActive) { m_bActiveTutorial = bActive ; }	// Ʃ�丮���� Ȱ��ȭ ���θ� �����ϴ� �Լ�.
	BOOL Is_ActiveTutorial() { return m_bActiveTutorial ; }					// Ʃ�丮���� Ȱ��ȭ ���θ� ��ȯ�ϴ� �Լ�.

	void Set_CurTutorialIdx(int nIndex) { m_nCurTutorialIdx = nIndex ; }	// ���� Ʃ�丮�� �ε����� �����ϴ� �Լ�.
	int  Get_CurTutorialIdx() { return m_nCurTutorialIdx ; }				// ���� Ʃ�丮�� �ε����� ��ȯ�ϴ� �Լ�.

	void Set_TutorialDlg(cTutorialDlg* pDlg) { m_pTutorialDlg = pDlg ; }	// ��� Ʃ�丮�� ���̾�α׸� �����ϴ� �Լ�.
	void Set_HelperDlg(cHelperDlg* pDlg) { m_pHelperDlg = pDlg ; }			// ��� ���� ���̾�α׸� �����ϴ� �Լ�.
	void Set_TutorialBtnDlg(cTutorialBtnDlg* pDlg) { m_pBtnDlg = pDlg ; }	// ��� Ʃ�丮�� �˸� ��ư ���̾�α׸� �����ϴ� �Լ�.
	cTutorialBtnDlg* Get_TutorialBtnDlg() { return m_pBtnDlg ; }			// Ʃ�丮�� �˸� ��ư ���̾�α׸� ��ȯ�ϴ� �Լ�.
	cPtrList& GetHelperList() { return m_Helper_List; }
	void UpdateTutorial(int nCompleteIndex);
	void Start_CurTutorial(cTutorial* pTutorial);
	void Reset_MemberDlg();
	SNDIDX Get_ShowBtnSndIdx() const { return m_nShow_BtnSndIdx ; }
	SNDIDX Get_EndTutorialSndIdx() const { return m_nEndTutorial_SndIdx; }
	BYTE IsCompleteTutorial(BYTE byTutorialType);
	// 090417 ONS Ʃ�丮�� ���̾�α� On/Off ���� ����/��ȯ 
	BOOL CheckTutorialByIndex(int TutorialIndex);
	void SetOnOffTutorial(BOOL val) { m_bOnOff_Tutorial = val; }
	BOOL GetOnOffTutorial() { return m_bOnOff_Tutorial; }

	// 080502 LYW --- TutorialManager : ���� �� Ÿ���� Ʃ�丮���� �����ϴ� �Լ� �߰�.
	void Start_Specification_Tutorial(BYTE byTutorialType) ;
};





EXTERNGLOBALTON(cTutorialManager)