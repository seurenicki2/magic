#pragma once
#include "stdafx.h"
#include "../[cc]Header/CommonGameDefine.h"
#include "../[cc]Header/CommonStruct.h"
#include "input/Mouse.h"
#include "MovePoint.h"
#include "Object.h"

class CItem;
class CFurniture;
class cHousingStoredIcon;

#define HOUSINGMGR USINGTON(cHousingMgr)
#define HOUSINGMAP	8652

#define HOUSE_DUMMYID 999
#define HOUSE_ATTACHBONENAME "ride"

class cHousingMgr
{
private:
	enum eHouseLoadState				//�Ͽ콺 �ε���� 
	{
		eHsLoad_NONE = 0 ,				//����Ʈ ���� 
		eHSLoad_NOWLOADING	  = 1,		//�ε��� �����ѻ��� 
		eHSLoad_HOUSEINFO	  = 2,		//�Ͽ콺����
		eHSLoad_FURNITURELIST = 4,		//��������Ʈ 
		eHSLoad_OK	= eHSLoad_HOUSEINFO + eHSLoad_FURNITURELIST,	//�Ͽ콺������ ��������Ʈ�� �о����� �ε�Ϸ� 
	} ;
	DWORD m_dwHouseLoadState;			//�Ͽ콺 �ε���� ���� 

	cPtrList m_BackupPacketList;		//��� ��Ŷ����Ʈ 
	BOOL BackupNetworkMsg(BYTE Protocol,void* pMsg);				//�ε��߿� �ٸ� ��Ŷ�� ������ ����Ʈ�� ����� �ε尡 ������ �ϰ�ó�� 

	//������ �����Ǵ� ���� 
private:
	stHouse							m_CurHouse;						//�Ͽ콺�������� 
	BOOL							m_bHouseOwner;					//�������ΰ� ����
	BYTE							m_byRankHouse;					//�Ͽ콺 ��ŷ����
	BOOL							m_bDecorationMode;				//�ٹ̱��忩��

	//������ ���� Ŭ���̾�Ʈ ��ü���� ���� ���� 
private:
	stHousingSettingInfo			m_HousingSettingInfo;			//�Ͽ콺����
	CMemoryPoolTempl<stFurniture>	m_stFurniturePool;				//stFurniture �޸�Ǯ 
	CYHHashTable<CFurniture>		m_FieldFurnitureTable;			//�ʵ忡 ��ġ�� cFuniture����Ʈ  
	cPtrList						m_AlphaHandleList;				//�������μ��� ����Ʈ 
	cPtrList						m_WaitForMakingObjList;			//��ŷ�� ����ϰ��ִ� ������Ʈ ����Ʈ 
	DWORD							m_dwLastRemainProcessTime;		//ProcessRemainTime()
	DWORD							m_dwFieldFurnitureNum;			//�ʵ忡 ��ġ�� �������� 

	BOOL							m_bDoDecoration;		//������ �ʵ忡 ������ 
	BOOL							m_bDecoMove;			//�Ⱦ��̵�
	BOOL							m_bDecoAngle;			//��ġȸ��

	GXOBJECT_HANDLE					m_PickAnotherObjHandle;	//��ġ�� ��ŷ�� ���� ����� ���� ������Ʈ �ڵ� ( ��ġ���� Obj�� ���� ) 
	CFurniture*						m_pDecoObj;				//��ġobj
	VECTOR3							m_vDecoPos;				//������ġ 
	VECTOR3							m_vBackupDecoPos;		//������ ��ġ 
	float							m_fBackupDecoAngle;		//������ �ޱ� 

	struct stUseActionInfo			//�����׼����� 
	{
		stFurniture* pActionFuniture;	//���õ� �׼��� ���� ����  
		stHouseActionInfo* pActionInfo;	//�׼��� ���� 

		stUseActionInfo(){Clear();}
		void Clear(){ memset(this, 0, sizeof(stUseActionInfo)); }
	} ;
	stUseActionInfo m_CurActionInfo;	//���缱�õ� �׼� 

	//091006 pdy �ٹ̱� ���� ī�޶� ���� �߰� HousingSetting.Bin���� �о�´�. 
	DWORD							m_dwCameraFilterIdxOnDecoMode;

public:
	cHousingMgr(void);
	virtual ~cHousingMgr(void);

	void HousingInfoClear();				//�Ͽ�¡���� Ŭ���� 
	void LoadHousing_Setting();				//�Ͽ콺 ���� ���ð� �ε� 
	void Init();							
	void Release();							
	void Process();			
	void NetworkMsgParse(BYTE Protocol,void* pMsg);	

	DWORD GetCurHouseOwnerIndex()		{return m_CurHouse.HouseInfo.dwOwnerUserIndex;}

public:
	void ProcessCulling();					//ī�޶� ������ ���߾� �ܺ� & ����� �ø� 

	void ProcessAlpha();					//������ ĳ���͸� ������ ���ĸ� ���δ�.

	void ProcessWaitForMaikingObjList();	//��ŷ�õ��� HERO�� �浹�Ǿ� ������� ������ ����Ʈó�� 	

	void ProcessRemainTime();				//������ �ð��� 1�п� �ѹ��� ��´� 

	void ProcessMouseEvent_DecorationMode(CMouse* Mouse,DWORD MouseEvent);	//�ٹ̱���� ���콺�̺�Ʈ ó�� 

	void MoveDecorationMousePoint(CMouse* Mouse);							//������ ���콺 ��ŷ �̵� ó�� 

public:
	BOOL RequestStoredItem(POSTYPE ToPos, CItem * pFromItem);					//������ ��û
	BOOL RequestRestoredICon(POSTYPE ToPos, cHousingStoredIcon * pFromIcon);	//���������� ��û 

	BOOL RequestInstallCurDeco();												//�ʵ忡 ��ġ ��û
	BOOL RequestUninstallDecoFromUseIcon(stFurniture* pstFurniture);			//������ ����Ŭ�� ��ġ���� ��û
	BOOL RequestUninstallCurDeco();												//���� �ٹ̱��� ��ġ���� ��û
	BOOL RequestUpdateCurDeco();												//������ ���� ���� ��û
	BOOL RequestUseDecoBonus(DWORD dwBonusIndex);								//�ٹ̱⺸�ʽ� ��� ��û
	BOOL FakeUseAction(stFurniture* pFuniture,stHouseActionInfo* ActionInfo);	//�׼��˾�â Ŭ���� ȣ�� 
	BOOL RequestUseCurAction();													//���� ���õ� �׼��� ��û 
	BOOL RequestAction_GetOff();

	void RequestDecoRationMode();							//�ٹ̱��� On/Off ��û 
	void RequestDestroyFuniture(stFurniture* pFuniture);	//���� ������ ��û 

public:
	void House_Create_Ack(void* pMsg);						//�Ͽ콺 ����	
	void House_Create_Nack(void* pMsg);						//�Ͽ콺 ��������
	void House_Entrance_Nack(void* pMsg);					//�Ͽ콺 ������� 
	void House_Info(void* pMsg);							//�Ͽ콺 ����			
	void House_FurnitureList(void* pMsg);					//�������� ����Ʈ 
	void House_Stored_Ack(void* pMsg);						//����â����		(�κ� ������ -> â�� ����) 
	void House_Stored_Nack(void* pMsg);						//����â��������
	void House_Restored_Ack(void* pMsg);					//����â��������	(â�� ���� -> �κ������� )
	void House_Restored_Nack(void* pMsg);					//����â�������� ���� 
	void House_Decomode_Ack(void* pMsg);					//�ٹ̱��� On/Off   
	void House_Decomode_Nack(void* pMsg);					//�ٹ̱��� On/Off ���� 
	void House_Install_Ack(void* pMsg);						//�ʵ忡 ������ġ
	void House_Install_Nack(void* pMsg);					//�ʵ忡 ������ġ ���� 
	void House_UnInstall_Ack(void* pMsg);					//�ʵ忡�� ������ġ ���� 
	void House_UnInstall_Nack(void* pMsg);					//�ʵ忡�� ������ġ ���� ����
	void House_Action_Force_GetOff_Ack(void* pMsg);			//������ ���� ���� ������ 
	void House_Action_Force_GetOff_Nack(void* pMsg);		//������ ���� ���� ������ ���� 
	void House_Action_Ack(void* pMsg);						//������ ���� �׼� �ߵ� 
	void House_Action_Nack(void* pMsg);						//������ ���� �׼� �ߵ� ���� 
	void House_Bonus_Ack(void* pMsg);						//�ٹ̱� ���ʽ� ���
	void House_Bonus_Nack(void* pMsg);						//�ٹ̱� ���ʽ� ��� ���� 
	void House_Update_Ack(void* pMsg);						//���� ���� ���� (��ġ,ȸ��,�Ⱓ��������)
	void House_Update_Nack(void* pMsg);						//���� ���� ���� ���� 
	void House_Update_Material_Ack(void* pMsg);				//���� ���͸��� ���� ����
	void House_Destroy_Ack(void* pMsg);						//���� ���� 
	void House_Destroy_Nack(void* pMsg);					//���� ���� ���� 
	void House_Vote_Ack(void* pMsg);						//�Ͽ콺 ��õ 
	void House_Vote_Nack(void* pMsg);						//�Ͽ콺 ��õ ����
	void House_Exit_Nack(void* pMsg);						//�Ͽ콺 ������ ���� 
	void House_Notify_Visit(void* pMsg);					//Player �Ͽ콺 �湮 �˸�
	void House_Notify_Exit(void* pMsg);						//Player �Ͽ콺 ������ �˸�
	void House_Notify_Action(void* pMsg);					//Player �׼� �˸� 					
	void House_Extend(void* pMsg);							//�Ͽ콺 Ȯ�� ó�� (�� & ����ġ�� & �⺻���������� ��ġ����or��ü �ȴ�)
	void House_UseItem_Nack(void* pMsg);					//�Ͽ콺 ���þ����� ������ 
	void House_Cheat_GetInfoAll_Ack(void* pMsg);			//��� �Ͽ콺 ���� ��� ġƮ
	void House_Cheat_GetInfoOne_Ack(void* pMsg);			//Ư�� �Ͽ콺 ���� ��� ġƮ 
	void House_Cheat_Delete_Ack(void* pMsg);				//Ư�� �Ͽ콺 ���� 
	void House_Cheat_Delete_Nack(void* pMsg);				//Ư�� �Ͽ콺 ���� ���� 

public:
	BOOL		IsHouseInfoLoadOk(){return (m_dwHouseLoadState == eHSLoad_OK)? TRUE : FALSE;  }	//���� �ε尡 �Ϸ�Ǿ��°� 
	void		StartHouseInfoLoading();														//�Ͽ콺 �ε���� 
	void		PrintDebugErrorMsg(char* pstrMsg,WORD dwError);									//ifdef _GM_TOOL_ �϶� �޼���â�� ����ش� 

public:	
	CObjectBase*				 GetSelectedObjectFromHouseMap(int MouseX,int MouseY,int PickOption , BOOL bSort);	//�Ͽ�¡���� GetSelectedObject()
	stDynamicHouseNpcMapInfo*	 GetDynimiHouseNpcMapInfo(DWORD dwMapIndex);										//�ش���� �������� ��ŷ�Ͽ콺NPC ��������Ʈ�� �����´�
	stDynamicHouseNpcInfo*		 GetDynimiHouseNpcInfo(DWORD dwMapIndex, BYTE byRank, BYTE byExterioKind);			//�������� ��ŷ�Ͽ콺NPC ������ �����´�.
	stDynamicHouseNpcInfo*		 FindDynimiHouseNpcInfoByNpcKind(DWORD dwMapIndex , DWORD dwNpcKind);				//�������� ��ŷ�Ͽ콺NPC ������ dwNpcKind�� �˻��Ͽ� �����´�.
	WORD						 GetStoredFunitureNumByCategory(WORD dwCategoryIdx);								//�ش�ī�װ��� ������ � ���������� �����´�.
	WORD						 GetBlankSlotIndexFromStoredFunitureListByCategory(WORD dwCategoryIdx);				//�ش�ī�װ��� �󽽷� �ε����� �����´�.
	DWORD						 GetCameraFilterIdxOnDecoMode(){ return m_dwCameraFilterIdxOnDecoMode; }			//�ٹ̱� ���� ��� ī�޶����� �ε����� �����´�.

public:
	BOOL		IsHousingMap();										//�Ͽ�¡ ���ΰ�
	BOOL		IsRankHouse()		{return m_byRankHouse;}			//��Ŀ�Ͽ콺 �ΰ�
	BOOL		IsDecorationMode()	{return m_bDecorationMode;}		//�ٹ̱� ����ΰ�
	BOOL		IsDecoMove()		{return m_bDecoMove;}			//���� �̵����� ��ȿ�Ѱ�
	BOOL		IsDecoAngle()		{return m_bDecoAngle;}			//���� ȸ�������� ��ȿ�Ѱ�
	BOOL		IsHouseOwner()		{return m_bHouseOwner;}			//���� �Ͽ콺�� �����ΰ�
	BOOL		IsActionableFurniture(CFurniture* pFurniture);		//�׼��� �ִ� �����ΰ� 
	bool		IsFieldHeightObject(CObject* pObject);				//�ʵ��� ���̿� ���� ������Ʈ�ΰ�
	BOOL		IsDoDecoration()	{return m_bDoDecoration;}		//������ �ʵ忡 ��ġ���ΰ�
	BOOL		IsHouseOwnerID(DWORD dwUserID)	{ return ( dwUserID == m_CurHouse.HouseInfo.dwOwnerUserIndex ) ? TRUE : FALSE; }

	float		GetStartAngle();									//������ġ�� ��������.

public:
	BOOL CanUseItemFromHousingMap(CItem* pItem);								//�Ͽ�¡�ʿ��� ��밡���� �������ΰ� 

	BOOL CanUseItemFromDecorationMode(CItem* pItem);							//�ٹ̱����߿� ��밡���� �������ΰ�

	BOOL CanExtendHouse(){return (m_CurHouse.HouseInfo.ExtendLevel < MAX_HOUSE_EXTEND_LEVEL-1 ) ? TRUE : FALSE; };	//Ȯ���� ���Ҽ��ִ°� 

	BOOL CanUseDecoBonus(stHouseBonusInfo* pBonusInfo,DWORD* pdwErr = NULL );	//�ش� ���ʽ��� ����Ҽ� �ִ°� 

	BOOL CanInstallFunitureToField();											//���� �������� ������ �ʵ忡 ��ġ�� �����Ѱ�

	BOOL CanMakingToFiled(CObject* pMarkingObj);								//�ʵ忡 ��ŷ�� �� �� �ִ°� (HERO BOX vs Marking Box)

	BOOL CanResetDecorationFuniture(CFurniture* pFurniture);					//������ �Ҽ��ִ� �����ΰ� ?

	BOOL CheckPercentShellToBoundingBox( BOUNDING_BOX* pSrcBoundingBox, VECTOR3* pMakingShellCenter);	//�ش缿�� ��ŷ�� �Ҽ��ִ°� äũ 

	BOOL TestCollisionBox2Box(BOUNDING_BOX* pSrcBoundingBox, BOUNDING_BOX* pDestBoundingBox , BOOL bCheckHeight = TRUE);	//�ڽ� vs �ڽ� �浹äũ 

public:
	void		SetDecorationModeTemp();					//�ٹ̱� ��� �ӽü���(���������ޱ� �� �ӽ÷� Ŭ�󿡼����� ó��)
	void		SetDecorationMode(BOOL bMode);				//�ٹ̱� ��� ����

	void		StartDecoration(stFurniture* pstFuniture);	//���� ���ý��� ( Start -> Install)
	void		EndDecoration();							//���� ���ó� 

	void		CancelDecoration();							//���� ���� ���
	BOOL		DecideSettingCurDeco();						//���� ���� Ȯ�� 

	void		StartDecoMove();							//���� �̵� ���� ���� 
	void		EndDecoMove();								//���� �̵� ���� ��

	void		StartDecoAngle();							//���� ȸ�� ���� ����
	void		EndDecoAngle();								//���� ȸ�� ���� ��

	BOOL		StartDecoReset(CObject* pPickObject);		//���� ������ ���� (Start -> Update ) 
	
	CFurniture*	MakeDummyFuniture(stFurniture* pstFuniture);	//���� ���ý� �ʵ忡 ���°����� Dummy ���� 
	CFurniture*	MakeFuniture(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,stFurniture* pstFuniture);	//����������Ʈ ����� 

	void		RemoveDeco(DWORD dwObjectID);				//��������
	void		RemoveCurDeco();							//���� �������� ���� ����
	void		DestroyFuniture(stFurniture* pDestroyFn);	//�������� ���� 

	BOOL		InstallFunitureToField(stFurniture* pstFuniture , BOOL bFirstInstall = FALSE);	//���������� ������ ����� �ʵ忡 ��ġ�� �Ѵ�
	BOOL		UpdateFunitere(stFurniture* pstFuniture);										//���������� �����Ͽ� ���� ������Ʈ 
	BOOL		UnInstallFunitureFromField(DWORD dwObjectIdx);									//�ش� ���̵��� ���� ���ν��� 

	void		MarkingObjZone(CObject* pObject, bool bMakingObjZone);							//TRUE : �ʵ忡 ��ŷ , FALSE : �ʵ忡�� ��ŷ���� 
	void		AddWaitForMakingObj(CObject* pWaitObj);											//��ŷ��� ����Ʈ�� ��� (HERO�� ��ġ�� �浹��) 
	void		RemoveWaitForMakingObj(CObject* pWaitObj);										//��ŷ��� ����Ʈ���� �Ҹ�
	void		FindOverapMarkingToHeroObj(cPtrList* pResultList);								//HERO�� ������ �浹�Ǵ� ���� ��ŷ�� ������Ʈ����Ʈ�� ����
	void		CheckAllMarkingObjByHeroZone();													//HERO�� ������ �浹�Ǵ� ���� ��ŷ�� ������Ʈ���� ��ŷ������ ��ŷ��⸮��Ʈ�� ���

	void		ChangeMaterialFurniture(DWORD dwFurnitureKind,DWORD dwMaterialIndex,BYTE byFlag);	//��,õ��,�ٴ� ���͸��� ���� 
	void		RideOnPlayerToFuniture(CPlayer* pPlayer);										//������ ž�¾׼� ó��
	void		RideOffPlayerFromFuniture(CPlayer* pPlayer, BOOL bForceRideOff=FALSE);			//������ ���⸮�׼� ó��  


//////////////////////////////////////////////////////////////////////////////////////////////////
//���⼭���ʹ� �׽�Ʈ������ �ڵ� 
//////////////////////////////////////////////////////////////////////////////////////////////////
public:
	// ��ŷ�׽�Ʈ�� �ڵ� 
	struct cMarkingTest
	{
		CObject*	m_pMarkinObj;
		VECTOR3*	m_pvPos;
		CMovePoint*	m_pTestShowObj;
	};
	cPtrList	m_MarkinTestList;		//��ġ�� ������ ��ŷ�Ǹ� �������� ADD�ȴ� 

	bool		m_bShowTest;			//TRUE : ��ŷ�� �� ǥ�� , FALSE : ��ŷ�� �� ǥ�þ��� 
	DWORD		m_dwLastShowTick;		

	void		ShowAllMarkingTest();	
	void		SetShowTestMaking(bool bShow);

	void		AddMarkingTest(CObject* pMarkingObj,VECTOR3* Pos);	
	void		ReleaseMarkingTest(CObject *pMarkingObj);		
	void		ReleaseAllMarkingTest();
	bool		IsShowTestMaking(){return m_bShowTest;}


};

EXTERNGLOBALTON(cHousingMgr)
