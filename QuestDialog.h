#pragma once

#include "Interface/cIconDialog.h"
#include "..\[CC]Quest\QuestDefines.h"
#include "GameResourceStruct.h"

#define MAX_REQUITAL_ICON_CELL 6

class cButton ;																// ��ư Ŭ������ �����Ѵ�.
class cIconGridDialog;														// ������ �׸��� ���̾�α� Ŭ������ �����Ѵ�.
class cIcon;																// ������ Ŭ������ �����Ѵ�.
class cListDialog;															// ����Ʈ ���̾�α� Ŭ������ �����Ѵ�.
class cMsgBox;																// �޽��� �ڽ� Ŭ������ �����Ѵ�.
class cPtrList;																// ������ ����Ʈ Ŭ������ �����Ѵ�.
class cPushupButton;														// Ǫ���� ��ư Ŭ������ �����Ѵ�.
class CQuestString;															// ����Ʈ ��Ʈ�� Ŭ������ �����Ѵ�.
class cStatic;																// ����ƽ Ŭ������ �����Ѵ�.

struct RequitalItem
{
	ITEM_INFO itemInfo ;
	int		nItemCount ;
	WORD	wPos ;

	cIcon*				pIcon;

	RequitalItem::RequitalItem()
	{
		memset(&itemInfo, 0, sizeof(itemInfo)) ;
		nItemCount = 0 ;
		wPos = WORD(-1);
		pIcon = 0;
	}
} ;

struct QuestInfo															// ����Ʈ ���� ����ü.
{
	CQuestString*	pQString;												// ����Ʈ ��Ʈ�� ������.
	SUBQUEST		sSubQuest;												// ���� ����Ʈ ����ü.

	QuestInfo::QuestInfo()													// ������ �Լ�.
	{
		pQString = NULL;													// ����Ʈ ��Ʈ�� �����͸� NULLó�� �Ѵ�.
		memset(&sSubQuest, 0, sizeof(sSubQuest));							// ���� ����Ʈ ����ü�� �޸� ���Ѵ�.
	}
};

struct QuestTree															// ����Ʈ Ʈ�� ����ü.
{
	DWORD		State;														// ���¸� ��� ����.
	DWORD		Count;														// ī��Ʈ ����.
	DWORD		QuestIdx;													// ����Ʈ �ε���.
	DWORD		SubQuestIdx;												// ���� ����Ʈ �ε���.

	int			nMapNum ;													// ����Ʈ �� ��ȣ.

	cPtrList	list;														// ����Ʈ.

	QuestTree()																// ������ �Լ�.
	{
		State = eQTree_NotUse;												// ���¸� eQTree_NotUse�� �����Ѵ�.
		Count = 0;															// ī��Ʈ�� 0���� �����Ѵ�.
		QuestIdx = 0;														// ����Ʈ �ε����� 0���� �����Ѵ�.
	}
};

struct sQuestItem															// ����Ʈ ������ ����ü.
{
	cIcon*				pIcon;												// ����Ʈ ������ ������.

	QUEST_ITEM_INFO*	pQuestItemInfo;										// ����Ʈ ������ ���� ������.

	DWORD				Count;												// ī��Ʈ.
	WORD				Pos;												// ��ġ.

	sQuestItem()															// ������ �Լ�.
	{
		pIcon = NULL;														// ������ �����͸� NULL ó�� �Ѵ�.
		pQuestItemInfo = NULL;												// ����Ʈ ������ ���� �����͸� NULL ó�� �Ѵ�.
		Count = 0;															// ī��Ʈ�� 0���� �����Ѵ�.
		Pos = WORD(-1);
	}
};


class CQuestDialog : public cIconDialog										// ������ �׸��� ���̾�α׸� ��ӹ��� ����Ʈ ���̾�α� Ŭ����.
{
	cPushupButton*		m_pPageBtn[MAX_QUEST_PAGEBTN];						// �ִ� ������ ������ �� ��ŭ ������ ��ư �����͸� �����Ѵ�.

	cIconGridDialog*	m_pIconGridDlg;										// ������ �׸��� ���̾�α� ������.
	cListDialog*		m_pQuestListDlg;									// ����Ʈ ����Ʈ ���̾�α� ������.

	cStatic*			m_pQuestCount ;										// ����Ʈ �������� ����ϴ� ����ƽ.

	cButton*			m_pNoticeBtn ;										// ����Ʈ �˸��� ��ư.
	cButton*			m_pGiveupBtn ;										// ����Ʈ ���� ��ư.

	cListDialog*		m_pQuestDescList ;									// ����Ʈ ���� ����Ʈ ���̾�α� ������.

	cIconGridDialog*	m_pRequitalGrid ;									// ����Ʈ ���� ������ ���̾�α� ������.

	cStatic*			m_pRequitalGold ;									// ��� ���� ����ƽ ������.
	cStatic*			m_pExperience ;										// ����ġ ���� ����ƽ ������.

	int					m_SelectedQuestID;									// ���� �� ����Ʈ ���̵�.
	int					m_QuestItemPage;									// ����Ʈ ������ ������.
	int					m_TooltipIdx;										// ���� �ε���.
	int					m_RequitalItemTooltipIdx ;							// ���� ������ ���� �ε���.

	cPtrList			m_QuestItemList;									// ����Ʈ ������ ����Ʈ.
	cPtrList			m_QuestRequitalList ;								// ����Ʈ ���� ������ ����Ʈ.

	CIndexGenerator		m_IndexGenerator;									// �ε��� ���� Ŭ����.
	int				m_nQuestCount ;											// ����Ʈ ī��Ʈ.

	cPtrList		m_QuestTree;											// ����Ʈ Ʈ�� ����Ʈ.

	// ���õ� ������ ����  �ٲ��ִ°ǵ�.. �̳༮�� ������.. �������ϴ�.. 
	// ���� ����� ����� �ٲ���..
	int				m_QListCount;											// ����Ʈ ����Ʈ ī��Ʈ.
	int				m_ChangedRow;											// ���� �� �� ��ȣ�� ��� ����.
	CQuestString*	m_QuestList[MAX_PROCESSQUEST];
	DWORD			m_dwQuestList[MAX_PROCESSQUEST] ;

	DWORD m_dwNpcIdx ;															// ����Ʈ �������� npc �ε����� �޴´�. �޴´�.
	DWORD m_dwScriptPage ;
	DWORD m_dwDialogType ;


	// 100414 ONS ������� ���� �߰�
	cIconGridDialog*	m_pSelectRequitalGrid ;								// ���ú��� �׸���
	cPushupButton*		m_pRequitalBtn;										// �Ϲݺ��� �ܹ�ư		
	cPushupButton*		m_pSelectRequitalBtn;								// ���ú��� �ܹ�ư
	cStatic*			m_pItemSignalOn;									// �Ϲݺ����� ���� ON
	cStatic*			m_pItemSignalOff;									// �Ϲݺ����� ���� OFF
	cStatic*			m_pSelectItemSignalOn;								// ���ú����� ���� ON
	cStatic*			m_pSelectItemSignalOff;								// ���ú����� ���� OFF
	cPtrList			m_QuestSelectRequitalList ;							// ���ú������� ����Ʈ
	BYTE				m_bRequitalType;									// ����Ÿ������(�Ϲݺ���/���ú���)


public:
	CQuestDialog();															// ������ �Լ�.
	virtual ~CQuestDialog();												// �Ҹ��� �Լ�.

	virtual DWORD ActionEvent(CMouse * mouseInfo);							// �̺�Ʈ ó�� �Լ�.

	void	Linking();
	virtual void Render();
	void	QuestListAdd(CQuestString* pQuestString, int SubID=0);			// ����Ʈ ����Ʈ�� ����Ʈ ��Ʈ�� ������ �߰��ϴ� �Լ�.
	void	SetSelectedQuestIdx(DWORD index) {	m_SelectedQuestID = index ; }
	DWORD	GetSelectedQuestID()		{	return m_SelectedQuestID;	}	// ���� �� ����Ʈ ���̵� �����ϴ� �Լ�.
	DWORD	GetQuestIdx(int index) ;
	void	GiveupQuestDelete(DWORD QuestIdx);								// ���� ����Ʈ�� �����ϴ� �Լ�.

	// QuestItem
	void	QuestItemAdd(QUEST_ITEM_INFO* pQuestItemInfo, DWORD Count);		// ����Ʈ �������� �߰��ϴ� �Լ�.
	void	QuestItemDelete(DWORD ItemIdx);									// ����Ʈ �������� �����ϴ� �Լ�.
	DWORD	QuestItemUpdate(DWORD type, DWORD ItemIdx, DWORD data);			// ����Ʈ �������� ������Ʈ �ϴ� �Լ�.
	void	RefreshRequitalItem() ;
	void	RefreshQuestItem(DWORD Page);									// ����Ʈ ������ �������� ���ΰ�ħ�ϴ� �Լ�.
	cPtrList& GetTreeList() { return m_QuestTree; }
	void	UpdateQuestDescView(DWORD dwQuestIdx, DWORD dwSubQuestIdx) ;

	void	SetQuestCount() ;
	int		GetQuestCount() { return m_nQuestCount ; }

	void	SetGold(DWORD dwGold) ;
	void	SetExp(DWORD dwExp) ;

	//void	AddQuestTree(QuestTree* pData) ;
	void AddQuestTree(int nMapNum) ;
	void DeleteTree(QuestTree* pTree) ;
	void	AddRequitalItem(ITEM_INFO* pQuestItemInfo, DWORD Count, BYTE bReauitalType ) ;

	void	SetQuestPrevInfo(CQuestString* pQuestString) ;
	void	RequestInfo(CQuestString* pQuestString) ;
	void	RequitalInfo(CQuestString* pQuestString) ;
	void	QuestDescView(CQuestString* pQuestString);						// ����Ʈ ������ ���� �Լ�.
	void AddQuestTitle(CQuestString* pQuestString, int nMapNum) ;

protected:
	//void	MainQuestAdd(CQuestString* pQuestString, int nMapNum);			// ���� ����Ʈ�� �߰��ϴ� �Լ�.
	void	SubQuestAdd(CQuestString* pQuestString);						// ���� ����Ʈ�� �߰��ϴ� �Լ�.
	void	AddTooltipInfo(char* pString, cIcon* pIcon);					// ���� ������ �߰��ϴ� �Լ�.
	void	ShowItemToolTip(DWORD Index);									// ������ ������ �����ִ� �Լ�.
	void	ShowRequitalItemToolTip(cPtrList& RequitalList, DWORD Index) ;
	void	QuestTreeStateCheck(int RowIdx);								// ����Ʈ Ʈ�� ���¸� üũ�ϴ� �Լ�.

//======================================================================================//
public:

	void StartQuest(DWORD dwQuestIdx) ;
	void EndQuest(DWORD dwQuestIdx) ;

	void AddMapTree(int nMapNum, DWORD dwQuestIdx) ;
	void DeleteMapTree(int nMapNum) ;

	void RefreshQuestList() ;

//======================================================================================//
	// 100414 ONS ��������۰��� �Լ�
	void RefreshSelectRequitalItem();
	void SetActiveSelectRequital( BOOL bVal );
	void InitRequitalDlg();
	void SetRequitalType( BYTE bType ) { m_bRequitalType = bType; }

	// 100422 ShinJS --- ����Ʈ �䱸���� ���� ���
	void AddRemandQuestExecuteString( DWORD dwQuestIdx );
	// 100611 ONS ����Ʈ���̾�α� ����Ʈ�� ������ ����Ʈ�� �����ϴ��� �˻��Ѵ�.
	BOOL IsExistInQuestList( DWORD dwQuestIdx );
	// 100617 ONS ����Ʈ Ʈ���κ��� ������ �׸��� �ε����� ��ȯ�Ѵ�.
	DWORD GetSelectedQuestIDFromTree( DWORD dwSelectIndex );
};
