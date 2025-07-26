#pragma once

#include "stdAfx.h"
#include "./Interface/cTabDialog.h"

class CItem;
class cHousingStoredIcon;
class cStatic;

//����â�� CItem�� �ƴ� cHousingStoredIcon���� ���� 
class cHousingWarehouseDlg : public cTabDialog  
{
public:
	cHousingWarehouseDlg();
	virtual ~cHousingWarehouseDlg();

protected:
	//cButton* m_BtDecorationMode;	090708 pdy cHousingDecoModeBtDlg���� �̵�
	//cButton* m_BtDecoPoint;		090708 pdy cHousingDecoPointBtDlg���� �̵�

	cButton* m_BtLeftScroll;
	cButton* m_BtRightScroll;
	cButton* m_BtUpScroll;
	cButton* m_BtDownScroll;

public:
	virtual void SetActive(BOOL val);
	virtual void Render();
	virtual DWORD ActionEvent(CMouse * mouseInfo);
	void	Linking();
	void	OnActionEvent(LONG lId, void* p, DWORD we);

	void	Add(cWindow * window);
	BOOL	AddIcon(stFurniture* pstFuniture);
	BOOL	AddIcon(cIcon* pIcon);

	virtual BOOL FakeMoveIcon(LONG x, LONG y, cIcon * icon);
	BOOL	CanBeMoved(CItem* pItem,POSTYPE pos);
	//BOOL	DeleteIcon(WORD wTapNum , WORD wSlot);
	virtual void SelectTab(BYTE idx);

	void	UseIcon(cIcon* pIcon);
	//void	RefreshIcon(WORD wTapNum , WORD wSlot);
	cHousingStoredIcon* GetStoredIconIcon(WORD wTapNum , WORD wSlot);

	void FakeDeleteIcon(cHousingStoredIcon* pStoredIcon);

	void Set_QuickSelectedIcon(cHousingStoredIcon* pIcon) { m_pQuickSelectedIcon = pIcon ; }
	cHousingStoredIcon* Get_QuickSelectedIcon() { return m_pQuickSelectedIcon ; }

private:
	cHousingStoredIcon* m_pQuickSelectedIcon ;
	BOOL	FakeMoveItem(LONG mouseX, LONG mouseY, CItem * pFromItem);	
	cHousingStoredIcon* MakeHousingStoredIcon(stFurniture* pstFuniture);

private:
	//090708 pdy ��ȹ�� ��û������ ���� ���¿����� ���ı�� �߰� 
	CYHHashTable<cHousingStoredIcon> m_StoredIconHash;

	cStatic*	   m_pStatic_Basic;
	cStatic*	   m_pStatic_Push;
public:
	void	RefreshIcon(stFurniture* pFurniture);
	void	SortIconByTapNum(WORD wTapNum);
	BOOL	DeleteIcon(stFurniture* pFurniture);
	cHousingStoredIcon* GetStoredIconByFurniture(stFurniture* pFurniture);
	cHousingStoredIcon* GetStoredIconByObjectIdx(DWORD dwObjectIdx);

	void FocusOnByFurniture(stFurniture* pFurniture);
};