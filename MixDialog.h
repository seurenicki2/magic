/**********************************************************************

�̸�: MixDialog
�ۼ�: 2007/10/22 10:16:28, �̿���

����: ��Ḧ �����Ͽ� ���ο� �������� ������.
                                                                     
***********************************************************************/
#pragma once

#include "interface/cDialog.h"


class cStatic;
class cIconDialog;
class cIconGridDialog;
class CItem;
class CItemShow;
class cListDialog;
class cSpin;
class cComboBox;
class cEditBox;

struct ItemMixResult;

#define LESSSTRINGLEN	4

// 090812 ShinJS --- ���� �������̽� ����, Category Dialog ����
enum eMixDlgCategoryType
{
	eMixDlgCategoryType_1,							// 1��° ī�װ� - Item Category
	eMixDlgCategoryType_2,							// 2��° ī�װ� - Item Equip, Expand, Material
	eMixDlgCategoryType_3,							// 3��° ī�װ� - ���� ����
	eMixDlgCategoryType_4,							// 4��° ī�װ� - ���ս� ����
	eMixDlgCategoryType_Max,
};

// 090901 ShinJS --- ������ ���� ����
enum eMixItemSortType
{
	eMixItemSortType_ByName,
	eMixItemSortType_ByLevel,
};

// 091126 ShinJS --- Mix Dialog�� ����
enum eMixDialogState
{
	eMixDialogState_Normal,		// �⺻ ����
	eMixDialogState_Drag,		// Item�� Drag�� ����
	eMixDialogState_Search,		// �˻� ����
};

// 090812 ShinJS --- ���� �������̽� ����, ǥ���� Item�� �����ϱ� ���� Category ����
struct stCategoryInfo
{
	eMixDlgCategoryType		categoryType;			// Dialog�� Category ��ġ
	WORD				wID;					// ListItem ID
	WORD				wHighCategoryID;		// ���� ListItem ID
    char				szName[ 256 ];			// List ���� �̸�
	std::set<DWORD>		setItemType;			// Item Type
	std::set<DWORD>		setItemTypeDetail;		// Item Type Detail

	stCategoryInfo()
	{
		categoryType = eMixDlgCategoryType_Max;
		wID = 0;
		wHighCategoryID = 0;
		ZeroMemory( szName, 256 );
	}
};

class CMixDialog : public cDialog
{	
public:
	CMixDialog();
	virtual ~CMixDialog();
	
	virtual void SetActive( BOOL );
	virtual void SetActiveRecursive(BOOL val );
	virtual	DWORD ActionKeyboardEvent( CKeyboard* keyInfo );
	
	void Linking();	
	void OnActionEvent( LONG lId, void* p, DWORD we );

	void Fail( const MSG_ITEM_MIX_ACK& );
	void Succeed( const MSG_ITEM_MIX_ACK& );
	void Restore();
	void Send();
	void Refresh();
	void RefreshResult( const ITEMBASE& );


	// 080228 LUJ, ��ȣ ������ ����
public:	
	void SetProtectedActive( CItem& );

private:
	ITEMBASE mProtectionItem;


private:
	// ������ ��û�Ѵ�. ���ڴ� ���� ����. isCheck�� ���̸� ���� �´��� üũ�Ͽ� �ٷ� �����Ѵ�
	void Submit( DURTYPE mixSize, BOOL isCheck );
	
	void RemoveResult();																		// ���� ������ ���� ����

	BOOL FakeMoveIcon( LONG x, LONG y, cIcon* icon );

	// 071218 LUJ, ���� �⺻ �������� �κ��丮�� ������
	void PutDescription( const ItemMixResult* );
	
	void Release();

	// 090812 ShinJS --- ���� �������̽� ����
	void LoadMixDlgCategory();																	// ListDialog�� Category List ������ �о�´�

	stCategoryInfo* GetSelectedCategoryInfo( eMixDlgCategoryType eCategory );					// ListDlg���� ���� ���õ� ���� Category ������ ��ȯ
	ITEM* GetSelectedItemFromListDlg( eMixDlgCategoryType eCategory, BOOL bCancelOpt=FALSE );	// ListDlg���� ���� ���õ� ���� ITEM ������ ��ȯ
	ITEM* GetSelectedItemFromListDlg( LONG lId );												// ListDlg���� ���� ���õ� ���� ITEM ������ ��ȯ, Category�� ID�� �Ǵ��Ͽ� GetSelectedCategoryInfo ����

	void InitCategory();																		// Category ListDlg �ʱ�ȭ
	void UpdateCategory();																		// ���� ���õ� ������ Category ListDlg Update
	void UpdateCategoryByItemIdx( DWORD dwItemIdx );											// ���� Script�� �ش��ϴ� ������ Category Update
	void UpdateHighCategory( DWORD dwResultItemIdx );											// ��� �����ۿ� �ش��ϴ� ������ ���� Category Update
	void UpdateOwnStuff();																		// ���� ���õ� ���վ������� ������� ������Ȳ Update
	void UpdateCostInfo();																		// ������� Update

	void SetSortType( eMixItemSortType eSortType );												// Mix Item List ���� ��� ����
	void ClearSelectedCategoryInfo();															// Category ���ý� ������ �������� �����Ѵ�

	int GetCategoryIndexFromName( eMixDlgCategoryType eCategory, const char* szItemName );		// Category List�� �߰��� �̸����� List�� Index�� ���Ѵ�.
	int GetCategoryIndexFromItemIdx( eMixDlgCategoryType eCategory, DWORD dwItemIdx );			// Category List�� �߰��� Data(ItemIndex)�� List�� Index�� ���Ѵ�.

	void SearchMixResultItem();																	// ���� ��������� �˻�

	void UpdateMixInfo();																		// ���� ���õ� List���������� �������� Update
	void UpdateMixScript();																		// ���� ���õ� List�������� ���սĿ� ���� Update

public:
	void UpdateCategory( eMixDlgCategoryType eCategory );										// �ش� Category �� Update
	void PushCatagory1BTN();
	void PushCatagory2BTN();
	void CatagoryScrollEnd();

	// 080925 LUJ, ���� ���� ������
public:
	// 080925 LUJ, ���� ���� ������ ����
	void SetSupportItem( const ITEMBASE& );
private:
	ICONBASE mSupportItem;
	
private:
	cListDialog*		mDescription;															// ���� ����� ���� ListDialog
	cIconDialog*		mResultIconDialog;														// ���� ����� IconDialog
	cIconGridDialog*	mIconGridDialog;														// ���� ��� IconGridDialog
	
	CYHHashTable<stCategoryInfo>	m_htCategoryInfo;											// Category List ������ ���� Table

	int					m_SelectedCategory[ eMixDlgCategoryType_Max ];							// Category�� ListDlg���� ���õ� Item Index
	BOOL				m_bChangedCategory[ eMixDlgCategoryType_Max ];							// Category�� ���� ���� ����
	cListDialog*		m_CategoryDlg[ eMixDlgCategoryType_Max ];								// Category ListDialog
	cSpin*				m_pCntSpin;																// ���� ���� ������ ���� Spin
	cStatic*			m_pInvenMoney;															// ���� �ݾ� ��� static
	cStatic*			m_pMixCost;																// ���� ��� ��� static
	cComboBox*			m_pSortTypeCombo;														// ���� Type ���� ComboBox
	

	std::vector< CItemShow* >				mStuffIcon;											// ��� ������ ���� ������

	CItem*									mResultIcon;										// ���� ��� ������

	ItemMixResult*							m_pSelectedMixResult;								// ���õ� ���� �������� ��ũ��Ʈ ����
	DURTYPE									m_MixItemCnt;										// ���õ� ���� �������� ���� ����
	CYHHashTable<ItemMixResult>				m_htEqualSelectedMixResult;							// ���õ� ���� �������� ��ũ��Ʈ Table (�⺻ �������۰� ��� �������� ���� ������ �ٸ� ��ũ��Ʈ�� ����)
	eMixItemSortType						m_eMixItemSortType;									// ���� ������ List ���� ���
	eMixDialogState							m_eMixDialogState;									// ���� Dialog ����

	MSG_ITEM_MIX_SYN mMessage;

	// 080228 LUJ, �Ϲ�/��ȣ ��� ǥ�� ���� ��Ʈ��
	cWindow*	mTitle;
	cWindow*	mProtectTitle;
	cWindow*	mProtectSymbol;

	// 091124 ShinJS --- �˻� ��� �߰�
	cEditBox*								m_pSearchNameEdit;									// �˻��� �Է� Edit Box
	cEditBox*								m_pSearchMinLv;										// �˻� Lv �Է� Edit Box
	cEditBox*								m_pSearchMaxLv;
	cButton*								m_pSearchConditionInitBTN;

	DWORD									m_dwDragItemIdx;									// Drag �� Item Index ����
	DWORD									m_dwMixResultCnt;
	BOOL									m_bIsEnoughMoney;									
};