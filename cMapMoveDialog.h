#pragma once
//=================================================================================================
//	FILE		: cMapMoveDialog.h
//	PURPOSE		: Dialog interface to move map.
//	PROGRAMMER	: Yongs Lee
//	DATE		: November 30, 2007
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================





//=========================================================================
//		�ʿ��� ��������� �ҷ��´�.
//=========================================================================
#include "./Interface/cDialog.h"





//=========================================================================
//		�ʿ��� ��Ʈ�� �������̽��� �����Ѵ�.
//=========================================================================
class cButton ;
class cListDialog ;
class CItem ;

enum eMapMoveType
{
	eNormalStyle,
	eCastle_Rushen,
	eCastle_Zevyn,
	eLimitDungeon,
	eDungeon,
	eMapMoveNone, //Alemuri it was first
};

class cMapMoveDialog : public cDialog
{
	cPtrList	 m_MapMoveList;
	cListDialog* m_pMapListDlg;
	cButton*	 m_pMoveBtn;
	cButton*	 m_pCloseBtn;
	cButton*	 m_pXCloseBtn;
	int			 m_nSelectedMapNum;
	CItem*		 m_pItem;
  	eMapMoveType m_byUseCase;
	DWORD		 m_dwSelectedNpcID;
	WORD		 m_TypePre;

public:
	cMapMoveDialog(void);
	virtual ~cMapMoveDialog(void);
	void Linking();
	virtual DWORD ActionEvent(CMouse*);
	virtual void OnActionEvent(LONG id, LPVOID, DWORD event );
	void MoveMapSyn();
	void SetItemInfo(CItem* pItem) { m_pItem = pItem; }
	CItem* GetItemInfo() const { return m_pItem; }
	cListDialog* GetMapListDlg() const { return m_pMapListDlg; }
	void SetUseCase(eMapMoveType type, DWORD dwSelectNpcID=0);
	virtual void SetActive(BOOL);
	DWORD GetMapListCount() const;
	void SetTypePre(WORD);

protected:
	void InitializeMapList();
	void MoveNotice();
	void ReleaseMapList();
  	void SetMapMoveInfo();
	void SetSiegeWarMoveInfo();
	void SetLimitDungeon();
	void SetDungeon();
};
