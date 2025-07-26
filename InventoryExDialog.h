#pragma once

#include "Interface/cTabDialog.h"

#define MAX_BACKGROUND	6
#define EXTENDED_INVENTORY_END		(TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() ))

class CWearedExDialog;
class CItem;
class cSkillBase;
class CDealItem;
class cStatic;
class cMsgBox;
class cPushupButton;
class cHousingStoredIcon;

class CInventoryExDialog : public cTabDialog  
{
public:
	CInventoryExDialog();
	virtual ~CInventoryExDialog();
	virtual void Render();
	virtual void Add(cWindow * window);
	virtual DWORD ActionEvent(CMouse * mouseInfo);

	BOOL	AddItem(ITEMBASE * itemBase);
	BOOL	AddItem(CItem* pItem);
	BOOL	DeleteItem(POSTYPE Pos,CItem** ppItem);
	void	MoveItem(POSTYPE FromPos, POSTYPE ToPos);
	virtual BOOL FakeMoveIcon(LONG x, LONG y, cIcon*);
	BOOL	GetBlankPositionRestrictRef(POSTYPE&);
	void	Linking();
	BOOL	GetPositionForXYRef(BYTE param, LONG x, LONG y, POSTYPE&);
	void	SetMoney(MONEYTYPE);
	BOOL	IsExist(POSTYPE);
	BOOL	CanBeMoved(CItem*, POSTYPE);
	void	OnActionEvnet(LONG lId, LPVOID, DWORD we);
	void	UseItem(CItem*);
	CItem* 	GetItemForPos(POSTYPE);
	CItem* 	GetCurSelectedItem(eITEMTABLE);
	BOOL	UpdateItemDurabilityAdd(POSTYPE, DURTYPE);
	WORD	GetTabIndex(POSTYPE);
	void	RefreshInvenItem();
	int		GetBlankNum();
	virtual void SetActive(BOOL val);


	DURTYPE	GetTotalItemDurability(DWORD wItemIdx);
	// 100614 ShinJS --- Seal Option�� ����, ����, �����ۿɼ�(��æ/��ȭ/����)���θ� �����Ҽ� �ֵ��� ����
	CItem * GetItemForIdx(DWORD wItemIdx, WORD wExceptSealOpt=eITEM_PARAM_UNSEAL, DURTYPE quantity=0, BOOL bExceptItemOpt=FALSE);
	void ItemUnSealingSync();
	void ItemUnSealing(POSTYPE);
	//auto process sort
	void	SortWait();
	//sort test
	BOOL	ClearInventoryForSort();
	void	SortStackItems(); //fix stack item
private:
	//sort test
	int		NextSortTime;
	BOOL	m_bWaitProcess;
	int		m_Count;

	void	FakeBuyItem(LONG mouseX, LONG mouseY, CDealItem * icon);
	BOOL	IsDivideFunctionKey();
	BOOL	FakeMoveItem(LONG mouseX, LONG mouseY, CItem * icon);				// network syn msg send
	BOOL	FakeGeneralItemMove( POSTYPE ToPos, CItem * pFromItem, CItem * pToItem );
	BOOL	FakeItemCombine( POSTYPE ToPos, CItem * pFromItem, CItem * pToItem );
	BOOL	FakeItemDivide( POSTYPE ToPos, CItem * pFromItem, CItem * pToItem );
#ifdef _FORMOON_
	static void OnFakeItemDivideOk( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 );
	static void OnFakeItemDivideCancel( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 );
#else
	static void OnFakeItemDivideOk( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 );
	static void OnFakeItemDivideCancel( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 );
#endif
	//090423 �Ͽ�¡â�� ����������->�κ��丮 ȸ��
	BOOL	FakeMoveHousingStoredIcon(LONG mouseX, LONG mouseY, cHousingStoredIcon* pFromStoredIcon);

	CWearedExDialog* m_pWearedDlg;
	cStatic* m_pMoneyEdit;
	cWindow* m_pBackground[MAX_BACKGROUND];
	MSG_ITEM_DIVIDE_SYN	m_divideMsg;

//��ưǪ�� ó��
	DWORD	m_BtnPushstartTime;	
	DWORD	m_BtnPushDelayTime;
	BOOL	m_bPushTabWithMouseOver;
	
// quest
public:
	//Alemuri Link Item To Chat---
	BOOL itemChatLocked; 
	//----------------------------

	cMsgBox*			m_pQuestMsgBox;
	CItem*				m_pTempQuestItem;
	void	ItemUseForQuestStart();
	VECTOR2	m_PrevPos;
	VECTOR2* GetPrevPos() { return &m_PrevPos ; }
	void CheckBackground();
	void SetCoolTime(DWORD itemIndex, DWORD miliSecond);
	void SetCoolTimeAbs(DWORD itemIndex, DWORD dwBegin, DWORD dwEnd, DWORD miliSecond);
	void Send();
	void Restore();
	void RefreshArmorItem();

private:
	MSG_ITEM_USE_SYN mMessage;
	// 071204 LYW --- InventoryExDialog : �� ���� ����ϰ�, �κ��� ���� �� ���� �� ��� �� �ӽ� ���� ����.
	CItem* m_pQuickSelectedItem ;
public : 
	void Set_QuickSelectedItem(CItem* pItem) { m_pQuickSelectedItem = pItem ; }
	CItem* Get_QuickSelectedItem() { return m_pQuickSelectedItem ; }

	// 071210 LYW --- CInventoryExDialog : Ȯ�� �κ��丮�� Ȱ��ȭ ���θ� �����ϴ� �Լ� �߰�.
	void InitExtended_Inventory( CHARACTER_TOTALINFO* pInfo ) ;

	// 080213 KTH -- ClearInventory
	BOOL ClearInventory();

	void SetMultipleItemLock(DWORD dwItemIdx, BOOL bVal);	// 080429 NYJ --- ������ ������ Lockó��

	void PetGradeUp();

	// 090422 ShinJS --- Ż�� ��ȯ/���ν� ��Ÿ�Ӽ����� Progress ������ ���� �Լ� �߰�(��ġ��ġ ����)
	void StartVehicleSummon( const VECTOR3& vecPos );
	void StartVehicleSeal( DWORD dwVehicleID );
};