#pragma once


#define ITEMMGR CItemManager::GetInstance()


#include "item.h"


class CHero;
class CPlayer;
class cImage;
class cWindow;
class CVirtualItem;
class CItemShow;
class cQuickItem;
class CExchangeItem;
class CBuyItem;

// 090114 LYW --- ItemManager : ��� ���� ���� ���� ����� �з��Ѵ�.
enum RESULT_CANEQUIP
{
	eEquip_Disable = 0,				// ����Ұ�
	eEquip_Able,					// ���밡��
	eEquip_RaceFail,				// ���� �������� ����Ұ�
	eEquip_GenderFail,				// ���� �������� ����Ұ�
	eEquip_LevelFail,				// ���� �������� ����Ұ�
	eEquip_FishingLevelFail,		// ���� ���õ� �������� ����Ұ�
	eEquip_CookingLevelFail,		// �丮 ��� �������� ����Ұ�
};

struct EnchantEffect
{
	DWORD	ItemIdx;

	WORD	Effect[13][3]; //change for new effect
};

struct EVENTINFO
{
	DWORD	Index;
	DWORD	Delay;
	DWORD	Next;
	DWORD	Item;
	DWORD	Count;
	DWORD	Window;
	char*	Msg;
};

// 090812 ShinJS --- ����: ��� Item���� �⺻��� Item Index�� ���ϱ� ���� ����
struct stMixResultItemInfo
{
	DWORD dwResultItemIdx;		// ���� ����� Item Index
	DWORD dwBaseItemIdx;		// ��� Item Index �� ��Ī�Ǵ� ��� Item Index
	BOOL bOverlappedResult;		// ������� �ߺ��� ���(üũ �� ��� ī�װ� ����â Update�� ǥ������ �ʴ´�)
};

class CItemManager  
{

	//Alemuri ChangeItem List Obtain----------------------------------------------------------------------------------------------
	enum CHANGEITEM_RESULT_KIND										// ü���� ������ ��� ����
	{
		TRK_ITEM = 0,												// ..������
		TRK_MONEY,													// ..��
	};

	struct stCHANGEITEM_RESULT_BASE
	{
		CHANGEITEM_RESULT_KIND	eResultKind;						// ..��� ����
		DWORD					nValue01;							// ..��ġ1
		DWORD					nValue02;							// ..��ġ2
		DWORD					nValue03;							// ..��ġ3
		DWORD					nProbaValue;						// ..Ȯ��
	};

	struct stCHANGEITEM_RESULT
	{
		int						  nResultBaseNum;
		stCHANGEITEM_RESULT_BASE* stResultBase;
	};

	struct stCHANGEITEM												// ü���� ������ ����
	{
		int						nChangeItemID;						// ..ü���� ������ ID
		int						nResultNum;							// ..��� ����
		stCHANGEITEM_RESULT*	stResult;							// ..���
	};

	stCHANGEITEM*			m_stChangeItemInfo;
	int						m_nChangeItemInfoNum;
	//----------------------------------------------------------------------------------------------------------------------------

	DWORD m_GiftEventTime;
	CYHHashTable<EVENTINFO>	mEventInfoTable;
	CYHHashTable<CItem> m_ItemHash;

	CYHHashTable<ITEM_INFO> m_ItemInfoList;

	CYHHashTable<EnchantEffect> mEffectList;
	// 100511 ShinJS --- ��ü ����
	std::map< DWORD, std::string > m_ToolTipTextMap;
	// 100511 ShinJS --- Ÿ��Ʋ ����(������� Ư�� ����) ��ŭ �߷��� ����
	std::map< DWORD, std::vector< std::string > > m_CutToolTipText;

	// Ű: ������ DB �ε���
	typedef stdext::hash_map< DWORD, ITEM_OPTION* >	OptionMap;
	OptionMap										mOptionMap;
	//CYHHashTable<SHOPITEMBASE>		m_UsedItemList;

	CIndexGenerator m_IconIndexCreator;
	BOOL			m_bAddPrice;
	ITEMBASE		m_TempDeleteItem;

	//---KES ItemDivide Fix 071020
	POSTYPE			m_DividePos;
	int				m_DivideDur;
	//----------------------------

	// 071127 LYW --- ItemManager : ���� ������ ( ���� ���� ) ��ġ ���� ���� �߰�.
	int			m_nSealItemDelPos ;

	
	ITEM_OPTION linkItemOption;
	CItem* itemLocked;
	BOOL isItemChatLink;
	std::vector<std::string> TooltipArrayText;
	std::vector<DWORD> TooltipArrayColors;
	
public:

	
	void SetItemLocked(CItem* itLocked) { itemLocked = itLocked; }
	CItem* GetItemLocked() { return itemLocked; }
	void SetItemChatLinkOption(ITEM_OPTION itemChatOption) { linkItemOption = itemChatOption; }
	ITEM_OPTION GetItemChatLinkOption() { return linkItemOption;}
	void ResetItemChatLinkOption() { memset(&linkItemOption, 0, sizeof linkItemOption); }
	void SetIsItemChat(BOOL isItemChat) {isItemChatLink = isItemChat ;}
	BOOL GetIsItemChat() {return isItemChatLink ;}
	std::vector<std::string> GetItemChatTextArray() { return TooltipArrayText; }
	std::vector<DWORD>		 GetItemChatColorArray() { return TooltipArrayColors; }
	void ClearItemChatTextArray() {TooltipArrayText.clear();}
	void ClearItemChatColorArray() {TooltipArrayColors.clear();}
	

	DWORD	m_itdbg;
	static CItemManager* GetInstance();
	CItemManager();
	virtual ~CItemManager();

	//---KES ItemDivide Fix 071020
	void SetDivideItemInfo( POSTYPE pos, int dur )	{ m_DividePos = pos; m_DivideDur = dur; }
	POSTYPE GetDivideItemInfoPos()					{ return m_DividePos; }
	int		GetDivideItemInfoDur()					{ return m_DivideDur; }
	//----------------------------

	void				AddOption	( const ITEM_OPTION& );
	const ITEM_OPTION&	GetOption	( const ITEMBASE& ) const;
	const ITEM_OPTION&	GetOption	( DWORD itemDbIndex ) const;
	void				RemoveOption( DWORD itemDbIndex );


	void Release();

	CItem* MakeNewItem( const ITEMBASE*, const char* strKind);

	void ItemDelete(CItem* pItem);
	void FakeDeleteItem(CItem* pItem);
	void SendDeleteItem();

	cImage* GetIconImage(DWORD ItemIdx, cImage * pImageGet);
	CItem* GetItem(DWORD dwDBIdx);

	//Block Inspect With Request----------
	DWORD			m_InspectRequesterId;
	//------------------------------------

	//Alemuri ChangeItem----------------------------------------------------
	void ChangeItemList(CItem* pItem);
	void ListAllChangeItemsForItem(std::string itemNameStr);
	//----------------------------------------------------------------------
	
public:

	//Alemuri ChangeItem--------------------------------
	BOOL LoadScriptFileDataChangeItem();
	int itemGrade;
	int itemIdxPrevious;
	std::vector<std::string> resultChangeItemText;
	std::vector<DWORD> resultChangeItemColors;
	std::vector<std::string> resultChangeItemItemName;
	//-------------------------------------------------

	//Alemuri whatnpc command-------------
	std::vector<std::string> npcMapDealer;
	std::vector<std::string> npcNameDealer;
	std::vector<std::string> npcNameExcludedDealer;
	std::vector<DWORD> npcIndexeDealer;
	std::vector<DWORD> itemsIndexDealer;
	//void addNpcNameDealer(char* npcName) {npcNameDealer.push_back(npcName);} // include PCShop but also NPCs out of game
	void addNpcIndexDealer(DWORD npcIndex) {npcIndexeDealer.push_back(npcIndex);}
	void addItemIndexDealer(DWORD itemIndex) {itemsIndexDealer.push_back(itemIndex);}
	void addNpcMapDealer(std::string mapName) {npcMapDealer.push_back(mapName);}
	void addNpcNameExcluded(std::string nameExcluded) {npcNameExcludedDealer.push_back(nameExcluded);}
	//std::string GetNpcNameDealer(DWORD index) {return npcNameDealer[index];} // include PCShop but also NPCs out of game
	DWORD GetNpcIdxDealer(DWORD index) {return npcIndexeDealer[index];}
	DWORD GetItemIdxDealer(DWORD index) {return itemsIndexDealer[index];}
	std::string GetNpcMapDealer(DWORD index) {return npcMapDealer[index];}
	std::string GetNpcNameExcluded(DWORD index) {return npcNameExcludedDealer[index];}
	DWORD GetNpcNameExcludedSize() {return npcNameExcludedDealer.size();}
	void resetNpcExcludedArray() {npcNameExcludedDealer.clear();}
	//------------------------------------

	void AddToolTip( cWindow*, DWORD toolTipIndex, DWORD color = TTTC_DESCRIPTION );
	void AddToolTip( cIcon* );
	void AddToolTip( CVirtualItem* );
	void AddToolTip( CItemShow* );
	void AddToolTip( CItem* );
	void AddToolTip( cQuickItem* );
	void AddToolTip( CExchangeItem* );
	void AddToolTip( CBuyItem* );

	// 071125 LYW --- ItemManager : �ð� ���� ���� �����Լ� �߰�.
	void AddTimeToolTip( cIcon* icon, const ITEM_OPTION&, BOOL isFirst ) ;

	
	void AddNametoolTipItemChat					( cIcon&, const ITEMBASE& );
	void AddFurnitureFunctionToolTipItemChat	( cIcon&, const ITEMBASE& );
	void AddPetToolTipItemChat					( cIcon&, const ITEMBASE& );
	void AddOptionToolTipItemChat				( cIcon&, const ITEMBASE&, ITEM_OPTION );
	void AddDescriptionToolTipItemChat			( cIcon&, const ITEMBASE&, DWORD color = TTTC_DESCRIPTION );
	void AddPetSkillToolTipItemChat				( cIcon&, const ITEMBASE& );
	void AddSetToolTipItemChat					( cIcon&, const ITEMBASE& );
	void AddFunctionToolTipItemChat				( cIcon&, const ITEMBASE& );
	void AddMakerToolTipItemChat				( cIcon&, const ITEMBASE& );
	void AddFurnitureOptionToolTipItemChat		( cIcon&, const ITEMBASE& );
	void CItemManager::CreateItemChatTooltip	(ITEM_OPTION, DWORD, DWORD);
	void AddRemainTimeToolTipItemChat( cIcon&, const ITEMBASE&, BOOL bFirst );
	

	//Alemuri Search Drops From Mobs-------------------------------------------------------
	std::vector<std::string> searchDropsLowercase;
	std::vector<std::string> searchDrops;
	std::vector<DWORD> searchDropsIndexes;
	void resetTabIndex() {m_tabIndex = 0;}
	//-------------------------------------------------------------------------------------
private:
	//Alemuri Search Drops From Mobs------------
	DWORD m_tabIndex;
	//------------------------------------------

	void AddNametoolTip			( cIcon&, const ITEMBASE& );
	void AddOptionToolTip		( cIcon&, const ITEMBASE& );
	void AddFunctionToolTip		( cIcon&, const ITEMBASE& );
	void AddMakerToolTip		( cIcon&, const ITEMBASE& );
	void AddSetToolTip			( cIcon&, const ITEMBASE& );
	void AddPetToolTip			( cIcon&, const ITEMBASE& );
	void AddPetSkillToolTip		( cIcon&, const ITEMBASE& );
	void AddDescriptionToolTip	( cIcon&, const ITEMBASE&, DWORD color = TTTC_DESCRIPTION );
	
	// 071115 LYW --- ItemManager : ���� ���ð� ���� �߰�.
	void AddRemainTimeToolTip( cIcon&, const ITEMBASE&, BOOL bFirst );

	// 071126 LYW --- ItemManager : ���� ���ð� ���� �߰�.
	void AddUseableTimeToolTip( cIcon&, const ITEMBASE& );

	// 090430 pdy �Ͽ�¡ ���� ���¿� ���� ���� �߰� 
	void AddFurnitureStateToolTip	( cIcon&, const ITEMBASE& );

	// 090430 pdy �Ͽ�¡ ���� �ɼǿ� ���� ���� �߰�
	void AddFurnitureOptionToolTip	( cIcon&, const ITEMBASE& );

	// 090430 pdy �Ͽ�¡ ���� �Ӽ��� ���� ���� �߰� 
	void AddFurnitureFunctionToolTip( cIcon&, const ITEMBASE& );


public:
	void NetworkMsgParse(BYTE Protocol,void* pMsg);
	void MoveItem( MSG_ITEM_MOVE_ACK*);
	void PetMoveItem( MSG_ITEM_MOVE_ACK*);
	void DivideItem(ITEMOBTAINARRAY*);
	void MoveItemToGuild(MSG_ITEM_MOVE_ACK*);
	void MoveItemToStorage(MSG_ITEM_MOVE_ACK*);
	void MoveItemToShop(MSG_ITEM_MOVE_ACK*);
	void CombineItem(MSG_ITEM_COMBINE_ACK*);
	void DeleteItem(POSTYPE, CItem**);
	BOOL AddItem(CItem*);
	eITEMTABLE GetTableIdxForAbsPos(POSTYPE);
	CItem* GetItemofTable(eITEMTABLE, POSTYPE);
	const ITEMBASE* GetItemInfoAbsIn(CPlayer*, POSTYPE);
	void DeleteItemofTable(eITEMTABLE, POSTYPE, BOOL bSellDel);
	BOOL IsEqualTableIdxForPos(eITEMTABLE, POSTYPE);
	void RefreshAllItem();
	void RefreshItem( CItem* pItem );
	void RefreshItemToolTip(DWORD ItemDBIdx);
	RESULT_CANEQUIP CanEquip(DWORD wItemIdx);
	BOOL CanUse(DWORD wItemIdx);
	void LoadItemList();
	void LoadItemToolTipList();
	void LoadItemEffectList();
	void LoadGiftEvent();

	EnchantEffect* GetItemEffect(DWORD ItemIdx);
	ITEM_INFO* GetItemInfo(DWORD wItemIdx);
	const char* GetName(EWEARED_ITEM) const;
	const char* GetName(ITEM_OPTION::Drop::Key) const;
	void SetPreItemData(sPRELOAD_INFO*, int* Level, int Count);
	DWORD m_dwStateParam;

	//SW050920 Rare
	//BOOL IsRareOptionItem( DWORD wItemIdx, DWORD dwRareIdx );
	BOOL IsRare( const ITEM_INFO* ) const;
	BOOL IsDupItem( DWORD wItemIdx );
	
	void SetPriceToItem( BOOL bAddPrice );
	void SetDisableDialog(BOOL val, BYTE TableIdx);

	// Ÿ��Ʋ����(ª�� ��� Ư�� ����)��ŭ �߷��� ���� ��ȯ
	const std::vector< std::string >& GetToolTipMsg( DWORD dwIndex );
	// ��ü ���� ��ȯ
	const std::string& GetTotalToolTipMsg( DWORD dwIndex );
	// ��ü �������� Ư�� ���̸�ŭ �߶� ���޹��� �����̳ʿ� �����Ѵ�.
	void CutToolTipMsg( const std::string totalToolTip, std::vector< std::string >& cutToolTipList, const int nCutWidth );
	ITEM_INFO* FindItemInfoForName(char*);
	//Alemuri Find item for Partial Name-------------
	ITEM_INFO* FindItemInfoForPartialName(char*);
	ITEM_INFO* FindItemInfoForPartialNameFast(char*);
	//-----------------------------------------------

	void ReLinkQuickPosition(CItem* pItem);	
	void Process();
	void ItemDropEffect( DWORD wItemIdx );

#ifdef _GMTOOL_
	void SetItemIfoPositionHead()	{ m_ItemInfoList.SetPositionHead(); }
	ITEM_INFO* GetItemInfoData()	{ return m_ItemInfoList.GetData(); }
#elif _TESTCLIENT_
	void SetItemIfoPositionHead()	{ m_ItemInfoList.SetPositionHead(); }
	ITEM_INFO* GetItemInfoData()	{ return m_ItemInfoList.GetData(); }
#endif

private:
	void Item_TotalInfo_Local( void* pMsg ) ;
	void Item_Storage_Item_Info_Ack( void* pMsg ) ;
	//void Item_Monster_RareObtain_Notify( void* pMsg ) ;
	void Item_ReInForce_Ack( void* pMsg ) ;
	void Item_Reinforce_Option_Ack( void* );
	void Item_ReInForce_Nack( void* pMsg ) ;
	void Item_Move_Ack( void* pMsg ) ;
	void Item_Pet_Move_Ack( void* pMsg ) ;
	void Item_Combine_Ack( void* pMsg ) ;
	void Item_Divide_Ack( void* pMsg ) ;
	void Item_Monster_Obtain_Notify( void* pMsg ) ;	
	void Item_HouseRestored( void* pMsg ) ;//090529 pdy �Ͽ�¡ �������� �߰� 
	void Item_HouseBonus( void* pMsg ) ; //090618 pdy �Ͽ�¡ �ٹ̱� ���ʽ� �����ۺ��� �߰� 
	void Item_Move_Nack( void* pMsg ) ;
	void Item_Divite_Nack( void* pMsg ) ;
	void Item_Combine_Nack( void* pMsg ) ;
	void Item_Discard_Nack( void* pMsg ) ;
	void Item_Discard_Ack( void* pMsg ) ;
	void Item_Use_Notify( void* pMsg ) ;
	void Item_Use_Ack( void* pMsg ) ;
	//sort test
	void Item_Move_Sort(void* pMsg);


private:
	void Item_Cook_Ack( void* pMsg ) ;
	void Item_Cook_GetItem(void*);

	void Item_Mix_Ack( void* pMsg ) ;
	void Item_Mix_Nack( void* pMsg );
	void Item_Mix_GetItem( void* );
	void Item_Working_Success( void* pMsg ) ;
	void Item_Working_Fail( void* pMsg ) ;
	void Item_Working_Start( void* pMsg ) ;
	void Item_Working_StartEx( void* pMsg ) ;
	void Item_Working_Stop( void* pMsg ) ;
	void Item_TimeLimit_Item_OneMinute( void* pMsg ) ;
	void Item_ShopItem_Use_Ack( void* pMsg ) ;
	void Item_Enchant_Success_Ack( void* pMsg ) ;
	void Item_Enchant_Failed_Ack( void* pMsg );
	void Item_Enchant_Nack( void* pMsg ) ;
	void Item_Use_Nack( void* pMsg ) ;
	void Item_Sell_Ack( void* pMsg ) ;
	void Item_Sell_Nack( void* pMsg ) ;
	void Item_Buy_Ack( void* pMsg ) ;
	void Item_Buy_Nack( void* pMsg ) ;
	void Item_Buy_By_Coin( void* pMsg );				// 090227 ShinJS --- �ϻ��� �߰��۾� : Coin Item ���Խ� Coin ���� �۾�
	void Item_Dealer_Ack( void* pMsg ) ;
	//void Item_Appearance_Change( void* pMsg ) ;
	void Item_Appearance_Add( void* );
	void Item_Appearance_Remove( void* );
	void Item_Money( void* pMsg ) ;
	void Item_Obtain_Money( void* pMsg ) ;
	void Item_Divide_NewItem_Notify( void* pMsg ) ;
	void Item_Guild_Move_Ack( void* pMsg ) ;
	void Item_Guild_Move_Nack( void* pMsg ) ;
	//void Item_Guild_WareHouse_Info_Ack( void* pMsg ) ;
	//void Item_Guild_WareHouse_Info_Nack( void* pMsg ) ;
	void Item_DeleteFrom_GuildWare_Notify( void* pMsg ) ;
	void Item_AddTo_GuildWare_Notify( void* pMsg ) ;
	void Item_Storage_Move_Ack( void* pMsg ) ;
	void Item_Dissolve_GetItem( void* pMsg ) ;
	void Item_Dissolve_Ack( void* pMsg ) ;
	void Item_Dissolve_Nack( void* pMsg ) ;
	void Item_Error_Nack( void* pMsg ) ;
	void Item_ChangeItem_Nack( void* pMsg ) ;
	void Item_Use_ChangeItem_Ack( void* pMsg ) ;
	void Item_EventItem_Use( void* pMsg ) ;
	void Item_ShopItem_Info_Ack( void* pMsg ) ;
	// 071204 LYW --- ItemManager :
	void Item_Reset_Stats_Ack( void* pMsg ) ;
	// 071208 LYW --- ItemManager : ���� �ʱ�ȭ ���� ó��.
	void Item_Reset_Stats_Nack( void* pMsg ) ;
	//aziz Reborn in Game 29 Sep
	void Item_Reset_Level_Ack( void* pMsg ) ;
	void Item_Reset_Level_Nack( void* pMsg ) ;

	// 071210 LYW --- ItemManager : �κ��丮 Ȯ�� ����, ���� �Լ� �߰�.
	void Item_Increase_Inventory_Ack( void* pMsg ) ;
	void Item_Increase_Inventory_Nack( void* pMsg ) ;
	// 071226 LYW --- ItemManager : �̸� ���� ������ ��� ����, ���� �Լ� �߰�.
	void Item_Change_Character_Name_Ack( void* pMsg ) ;
	void Item_Change_Character_Name_Nack( void* pMsg ) ;
	void Item_Skill_Ack(LPVOID);
	void Item_Gift_Event_Notify(LPVOID);
	void CompositString(char* inputStr, char* str, ...);

public:
	int	GetSetItemSize(CPlayer*, const SetScript*);
	const MixScript* GetMixScript( DWORD itemIndex ) const;
	const ItemMixResult* GetMixResult( DWORD itemIndex, DWORD level ) const;
	CYHHashTable<stMixResultItemInfo>& GetMixInfoForScriptTable() { return m_htMixResultItemInfo; }
	void GetMixResultFromResultItemIdx( DWORD resultItemIdx, CYHHashTable<ItemMixResult>* pEqualMixScript );		// ��� ���������� ���հ����� ��� ��ũ��Ʈ�� ���Ѵ�
	const DWORD	GetMixResultPosInfo( DWORD BaseItemIdx, ItemMixResult* pMixResult );								// �⺻��� ���������� Mix Script List���� ���缱�õ� Script�� ��ġ�� ���Ѵ�.

	void itDebug();
	// 080228 LUJ, ���� ��ȣ ������ ����
	BOOL IsMixProtectionItem( const ITEMBASE& ) const;
	// 080925 LUJ, ���� ���� ������ �����´�
	const MixSupportScript* GetMixSupportScript( DWORD itemIndex ) const;
	void LoadMixScript();
private:
	// Ű: ������ �ε���
	typedef stdext::hash_map< DWORD, MixScript >	MixScriptMap;
	MixScriptMap									mMixScriptMap;

	// 080228 LUJ, ���� ��ȣ ������ ����
	typedef stdext::hash_set< DWORD >	MixProtectionItemSet;
	MixProtectionItemSet				mMixProtectionItemSet;
	// 080925 LUJ, ���� ���� ���� ����. Ű: ������ �ε���
	typedef stdext::hash_map< DWORD, MixSupportScript >	MixSupportScriptMap;
	MixSupportScriptMap									mMixSupportScriptMap;

	// 090812 ShinJS --- ����: ��� Item���� script�� ���ϱ� ���� ����
	CYHHashTable<stMixResultItemInfo>	m_htMixResultItemInfo;

	// 070709 ������ �̸� ������ ������
public:
	DWORD	GetItemNameColor( const ITEM_INFO& ) const;


	// 070710 ����, ���� ������ ����
public:
	const DissolveScript*	GetDissolveScript( DWORD itemIndex ) const;
	void LoadDissolveScript();
private:
	// Ű: ������ �ε���
	typedef stdext::hash_map< DWORD, DissolveScript >	DissolveScriptMap;
	DissolveScriptMap									mDissolveScriptMap;

public:
	DWORD GetCoolTimeGroupIndex( DWORD itemIndex ) const;

	// �ش� �������� ��Ÿ�� ������ üũ�Ѵ�.
	BOOL IsCoolTime( DWORD playerIndex, DWORD itemIndex ) const;

	// �������� ��Ÿ�� ���·� ����ϰų� ����
	BOOL AddCoolTime( DWORD playerIndex, DWORD itemIndex );

	// �÷��̾��� ��Ÿ�� ������ �����.
	void RemoveCoolTime( DWORD playerIndex );

	// 080326 NYJ --- ��Ÿ�Ӱ���
	void UpdateDlgCoolTime(DWORD itemIndex);	//HERO���Ը� ����.
	void LoadCoolTimeScript();
private:
	void ProcessCoolTime();

	struct CoolTimeScript
	{
		struct Unit
		{
			DWORD mIndex;	// ��Ÿ�� �׷� �ε���
			DWORD mTime;	// ��Ÿ�� ���� �ð�: �и�������
		};

		// Ű: ������ �ε���
		typedef stdext::hash_map< DWORD, Unit >	ItemData;
		ItemData								mItemData;

		// ��: ������ �ε���
		typedef stdext::hash_set< DWORD >				ItemIndex;

		// Ű: �׷� �ε���
		typedef stdext::hash_map< DWORD, ItemIndex >	GroupData;
		GroupData										mGroupData;
	}
	mCoolTimeScript;	

	// ��Ÿ���� ���� �����Ǵ� ������ ���Ľ�Ű�µ� ����ϴ� ��ƿ��Ƽ Ŭ����.
	class CoolTimeSort
	{
	public:
		struct Tick
		{
			DWORD mPlayerIndex;		// �ش� ��Ÿ�� ������ ������ �÷��̾� �ε���
			DWORD mCoolTimeGroup;	// ��Ÿ�� �׷� �ε���
			DWORD mBegin;			// ��Ÿ�� ���� �ð�
			DWORD mEnd;				// ��Ÿ�� ���� �ð�
		};

		bool operator() ( const Tick& lhs, const Tick& rhs ) const
		{
			const DWORD	 maxTick		= UINT_MAX;
			const DWORD lhsRemainTime	= ( lhs.mBegin > lhs.mEnd ? maxTick - lhs.mBegin + lhs.mEnd : lhs.mEnd - lhs.mBegin );
			const DWORD rhsRemainTime	= ( rhs.mBegin > rhs.mEnd ? maxTick - rhs.mBegin + rhs.mEnd : rhs.mEnd - rhs.mBegin );

			return lhsRemainTime > rhsRemainTime;
		}
	};

	// ���� �ð��� ���� 
	// ���ǻ���: �ڷ�� �ݵ�� �� ����� �־���Ѵ�. �����ͷ� ���� ��� �����ϸ鼭 �� �ּҸ� �Ҿ������.
	std::priority_queue< CoolTimeSort::Tick, std::vector< CoolTimeSort::Tick >, CoolTimeSort > mCoolTimeQueue;

	// ��: ��Ÿ�� �׷�
	typedef stdext::hash_set< DWORD >	CoolTimeGroup;

	// Ű: �÷��̾� �ε���, ��: ��� ���� ��Ÿ�� ������ �׷� �ε���
	typedef stdext::hash_map< DWORD, CoolTimeGroup >	UsingCoolTimeGroup;
	UsingCoolTimeGroup									mUsingCoolTimeGroup;

	// 080326 NYJ --- ��Ÿ������ ����
	std::map<DWORD, CoolTimeSort::Tick> m_mapCoolTime;


	// 070927. ��ȯ ������ ���� ���α׷����� ǥ���ϵ��� ��
public:
	BOOL IsChangeItem( DWORD itemIndex ) const;

	// 071203 LYW --- ItemManager : ���� �ʱ�ȭ ������ ó��.
	void UseItem_ResetStats(CItem* pItem) ;
	//aziz Reborn in Game 29 Sep
	void UseItem_ResetLevel(CItem* pItem) ;
	// 071203 LYW --- ItemManager : ��ų �ʱ�ȭ ������ ó��.
	void UseItem_ResetSkill(CItem* pItem) ;
	// 071203 LYW --- ItemManager : �� �̵� ��ũ�� ó��.
	void UseItem_MapMoveScroll(CItem* pItem) ;
	void Item_MapMoveScroll_Nack( void* pMsg ) ;
	// 071206 LYW --- ItemManager : â�� Ȯ�� ������ ó��.
	void UseItem_ExtendStorage(CItem* pItem) ;
	// 080611 LYW --- ��ų ����Ʈ �߰� �ֹ��� ó��.
	void GetSkillPointByItem(CItem* pItem) ;

	// 080826 LYW --- ItemManager : ���������� ����ϴ� ���ͽõ� ����Լ� �߰�.
	void UseItem_WarterSeed(CItem* pItem) ;

	// 071210 LYW --- ItemManager : �κ��丮 Ȯ�� ������ ó��.
	void UseItem_ExtendInventory(CItem* pItem) ;
	// 071206 LYW --- ItemManager : �̹� ����Ͽ� ������ Ȱ��ȭ ������ üũ�ϴ� �Լ� �߰�.
	BOOL IsUsingItem(CItem* pItem) ;
	// 071206 LYW --- ItemManager : �����۸� ������ ��� �Լ� �߰�.
	void UseItem_ItemMallItem(CItem* pItem, CObject* pTarget) ;
	// 071228 LYW --- ItemManager : �̸� ���� ������ ��� �Լ� �߰�.
	void UseItem_ChangeName(char* pName) ;

	// 080218 KTH -- Reset RegistResident
	void UseItem_ResetRegistResident(CItem* pItem);

	// 080312 NYJ --- ItemManager : ��ü��ġ�� ������ ó��.
	void UseItem_WorldShout(CItem* pItem) ;
	// 080326 NYJ --- ��Ÿ������ �ʱ�ȭ
	void ClearCoolTimeInfo() {m_mapCoolTime.clear();}
	// 080820 LUJ, ȹ�� �� ��Ÿ���� ���� �������� �̵� �ÿ� �ð��� ������Ʈ������Ѵ�. ������ �˾Ƽ� �����Ѵ�
	void UpdateCoolTimeItem( CItem& );
	void LoadChangeItem();
private:
	typedef stdext::hash_set< DWORD >	ChangeItemSet;
	ChangeItemSet						mChangeItemSet;

	// 080414 LUJ, �ܾ� ��ȯ ��ũ��Ʈ ó��
public:
	struct BodyChangeScript
	{
		DWORD		mGender;
		RaceType	mRace;
		PART_TYPE	mPart;
		
		// 080414 LUJ, �ؽ�Ʈ ��ȣ(InterfaceMsg.bin ����)
		DWORD		mText;
		DWORD		mValue;
	};

	typedef std::list< BodyChangeScript >					BodyChangeScriptList;
	typedef stdext::hash_map< DWORD, BodyChangeScriptList >	BodyChangeScriptListMap;
	
	const BodyChangeScriptListMap&	GetBodyChangeScriptListMap() const;
	void LoadBodyChangeScript();
private:	
	BodyChangeScriptListMap	mBodyChangeScriptListMap;

	// 080916 LUJ, �ռ� ��ũ��Ʈ
public:
	BOOL IsGiftEvent() const { return 0 < m_GiftEventTime; }
	WORD GetItemStackNum(DWORD dwItemIndex);
	void UseItem_ChangeClass(CItem*);

// 100609 ONS +10�̻� ��æƮ ������� �߰�
private:  //johan old enchant
	//struct EnchantNotifyInfo
	//{
	//	DWORD	ItemIndex;
	//	DWORD	EnchantLevel;
	//	BYTE	IsSuccessed;
	//	char	Name[MAX_NAME_LENGTH + 1];
	//};
	//std::queue<EnchantNotifyInfo>			m_EnchantNotifyQueue;
	//DWORD									m_ENchantNotifyCheckTime;
public:
	void EnchantNotifyAdd( void* pMsg );

public:
	WORD GetItemList(const char* pKeyWord, const char* pCategory1, const char* pCategory2, int nRareLevel, WORD wMinLevel, WORD wMaxLevel, WORD wNum, DWORD* pItemList);
};