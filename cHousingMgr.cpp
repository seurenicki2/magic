#include "stdafx.h"
#include "cHousingMgr.h"

#include "MouseCursor.h"
#include "MHMap.h"
#include "npc.h"
#include "GameResourceManager.h"
#include "ObjectManager.h"
#include "MoveManager.h"
#include "tilemanager.h"
#include "ObjectStateManager.h"
#include "AppearanceManager.h"
#include "Furniture.h"
#include "MouseEventReceiver.h"
#include "Item.h"
#include "mhfile.h"
#include "GameIn.h"
#include "MHCamera.h"
#include "ChatManager.h"
#include "MainTitle.h"
#include "ItemManager.h"
#include "cScrollIconGridDialog.h"
#include "cWindowManager.h"
#include "WindowIDEnum.h"
#include "cMsgBox.h"
#include "MiniMapDlg.h"
#include "../hseos/Monstermeter/SHMonstermeterDlg.h"

#include "cHouseNameDlg.h"
#include "InventoryExDialog.h"
#include "HousingRevolDlg.h"
#include "cHouseSearchDlg.h"
#include "cHousingDecoPointDlg.h"
#include "cHousingActionPopupMenuDlg.h"
#include "cHousingMainPointDlg.h"
#include "cHousingWarehouseDlg.h"
#include "cHousingWarehouseButtonDlg.h"
#include "cHousingStoredIcon.h"

//090708 pdy ��ȹ�� ��û���� UI ���� 
#include "cHousingDecoModeBtDlg.h"
#include "cHousingWebDlg.h"
#include "../[cc]skill/Client/Info//ActiveSkillInfo.h"
#include "../[cc]skill/client/manager/skillmanager.h"

#include "../input/UserInput.h"

GLOBALTON(cHousingMgr)
cHousingMgr::cHousingMgr(void)
{
	HousingInfoClear();
}

cHousingMgr::~cHousingMgr(void)
{
	Release();
}

void cHousingMgr::HousingInfoClear()				// �Ͽ�¡�������� �ѹ濡 Ŭ����.
{
	m_bDecorationMode = FALSE;	
	m_bDecoMove = FALSE;		
	m_bDecoAngle = FALSE;
	m_pDecoObj = NULL;
	m_PickAnotherObjHandle = NULL;
	m_bDoDecoration = FALSE;
	m_bHouseOwner = FALSE;
	m_bShowTest = FALSE;
	m_dwCameraFilterIdxOnDecoMode = 0 ;

	//091016 pdy �ʵ� ��ġ �ѵ� ī��Ʈ�� �ʱ�ȭ �ȵ��ִ� ���� ����
	m_dwFieldFurnitureNum = 0;

	m_CurActionInfo.Clear();
	m_CurHouse.Clear();
}

void cHousingMgr::Init()
{
	HousingInfoClear();
	Release();

	//�׽�Ʈ
	LoadHousing_Setting();

	if( !IsHousingMap() ) 
		return;

	m_stFurniturePool.Init(MAX_FURNITURE_STORAGE_NUM, 50 ,"stFurniture");
	m_CurHouse.pFurnitureList.Initialize(MAX_FURNITURE_STORAGE_NUM);
	m_CurHouse.pNotDeleteFurnitureList.Initialize(100);
	m_FieldFurnitureTable.Initialize(MAX_FURNITURE_STORAGE_NUM);	//090325 pdy �ʵ忡 �ִ��ϼ��� ī�װ� * ����  

}

void cHousingMgr::LoadHousing_Setting()
{
	m_HousingSettingInfo.Clear();

	CMHFile file;

	char StrBuf[256] = {0,};

	bool bPasing = FALSE;
	int	 iGroupCount = -1;
	DWORD dwCount = 0;
	const float f_1Radian = 0.01745f;
	const float fPI	  = 3.14159f;
	DWORD dwCurMapNum = 0;

	if( ! file.Init("System/Resource/Housing_Setting.bin", "rb" ) )
		return;

	while( ! file.IsEOF() )
	{
		file.GetString(StrBuf);

		if(! bPasing)		
		{
			//Todo OneLine Passing 
			if( StrBuf[0] != '#' )
			{
				file.GetLine(StrBuf,256);
				continue;
			}
			else if( strcmp( StrBuf , "#RANKPOINT_WEIGHT" ) == 0 )				//�Ͽ�¡ �������� ����ġ 
			{
				m_HousingSettingInfo.fDecoPoint_Weight	= file.GetFloat();
				m_HousingSettingInfo.fRecommend_Weight	= file.GetFloat();
				m_HousingSettingInfo.fVisitor_Weight	= file.GetFloat();
				continue;
			}
			else if( strcmp( StrBuf , "#STARPOINT_NOMAL" ) == 0 )				//������Ʈ		(������Ʈ = �ٹ̱⺸�ʽ� ���� �����Ǵ� ����Ʈ)
			{
				m_HousingSettingInfo.dwStarPoint_Nomal = file.GetDword();
			}
			else if( strcmp( StrBuf , "#STARPOINT_RANKER" ) == 0 )				//��Ŀ�� ������Ʈ 
			{
				m_HousingSettingInfo.dwStarPoint_Ranker = file.GetDword();

			}
			else if(strcmp( StrBuf , "#RANKINGDAY" ) == 0 )						//��ŷ�� ����ϴ� ���� 
			{
				m_HousingSettingInfo.dwRankingDay = file.GetDword();
			}
			else if( strcmp( StrBuf , "#GENERALFUNITURE_NUM" ) == 0 )			//�⺻�������� ���� ( �ܰ躰�� ���� ) 
			{
				iGroupCount++;
				dwCount=0;

				DWORD dwNum = file.GetFloat();
				m_HousingSettingInfo.dwGeneralFunitureNumArr[iGroupCount] = dwNum;
				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ] = new stGeneralFunitureInfo[ dwNum ];
				continue;
			}
			else if( strcmp( StrBuf , "#GENERALFUNITURE" ) == 0 )				//�⺻���� ���� ���� 
			{
				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ][ dwCount ].dwItemIndex = file.GetDword();
				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ][ dwCount ].vWorldPos.x = file.GetFloat();
				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ][ dwCount ].vWorldPos.y = file.GetFloat();
				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ][ dwCount ].vWorldPos.z = file.GetFloat();

				float fAngle = 0.0f;
				fAngle = file.GetFloat();
				if( fAngle != 0.0f )
				{
					//��ũ��Ʈ�� �������� -180~180���� �ְ� �ȴ�. �̰��� ���� ������ �ٲ���.
					fAngle *= f_1Radian;

					//180������ 
					if( fAngle > fPI )
						fAngle = fPI ; 

					if( fAngle < -fPI )
						fAngle = -fPI ;

				}

				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ][ dwCount ].fAngle	= fAngle;
				dwCount++;
				continue;
			}
			else if( strcmp( StrBuf , "#DynamicRankHouseNpcList" ) == 0 )				//�������� ���� �Ͽ콺 Npc List
			{
				stDynamicHouseNpcMapInfo* pMapInfo = new stDynamicHouseNpcMapInfo();
				dwCurMapNum = pMapInfo->MapIndex = file.GetDword();
				m_HousingSettingInfo.m_DynamicHouseNpcMapInfoList.Add( pMapInfo , pMapInfo->MapIndex );
			}
			else if( strcmp( StrBuf , "#DynamicRankHouseNpc" ) == 0 )					//�������� ���� �Ͽ콺 Npc ���� 
			{
				stDynamicHouseNpcMapInfo* pMapInfo = m_HousingSettingInfo.m_DynamicHouseNpcMapInfoList.GetData( dwCurMapNum ) ;

				stDynamicHouseNpcInfo* pNpcInfo = new stDynamicHouseNpcInfo();
				pNpcInfo->dwRankTypeIndex = file.GetDword();
				pNpcInfo->dwNpcKind = file.GetDword();
				pNpcInfo->fWorldPosX = file.GetFloat();
				pNpcInfo->fWorldPosZ = file.GetFloat();

				float fAngle = 0.0f;
				fAngle = file.GetFloat();
				pNpcInfo->fAnlge = fAngle;

				pNpcInfo->fDistance  = file.GetFloat();

				pMapInfo->pDynamicHouseNpcList.Add(pNpcInfo , pNpcInfo->dwRankTypeIndex );
			}
			else if( strcmp( StrBuf , "#DynamicRankHouseNpcEnd" ) == 0 )
			{
				dwCurMapNum = 0 ;
			}
			//Ŭ���̾�Ʈ������ ��� 
			else if(  strcmp( StrBuf , "#CameraFilterIndexOnDecoMode" ) == 0 )
			{
				m_dwCameraFilterIdxOnDecoMode = file.GetDword();
			}
		}
	}
}

void cHousingMgr::Release()
{
	m_HousingSettingInfo.Clear();
	ReleaseAllMarkingTest();

	m_stFurniturePool.Release();			
	m_CurHouse.pFurnitureList.RemoveAll();		
	m_CurHouse.pNotDeleteFurnitureList.RemoveAll();
	m_FieldFurnitureTable.RemoveAll();
	m_WaitForMakingObjList.RemoveAll();
	m_AlphaHandleList.RemoveAll();
}

// 091016 pdy �ε��� ��� ��Ŷ����Ʈ�� Add�� AddHead�� AddTail�� ����
BOOL cHousingMgr::BackupNetworkMsg(BYTE Protocol,void* pMsg)
{
	//�ε��߿� ����� �ʿ��� �������ݸ� ���� 
	switch(Protocol)
	{
		case MP_HOUSE_STORED_NACK:
		case MP_HOUSE_RESTORED_ACK:
		case MP_HOUSE_RESTORED_NACK:
		case MP_HOUSE_DECOMODE_ACK:
		case MP_HOUSE_DECOMODE_NACK:
		case MP_HOUSE_INSTALL_NACK:
		case MP_HOUSE_UNINSTALL_NACK:
		case MP_HOUSE_ACTION_NACK:
		case MP_HOUSE_UPDATE_NACK:	
		case MP_HOUSE_DESTROY_ACK:
		case MP_HOUSE_DESTROY_NACK:		
		case MP_HOUSE_VOTE_ACK:
		case MP_HOUSE_VOTE_NACK:
		case MP_HOUSE_EXIT_NACK:
		case MP_HOUSE_NOTIFY_VISIT:
		case MP_HOUSE_NOTIFY_EXIT:
		case MP_HOUSE_NOTIFY_ACTION:
			{
				MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
				MSG_DWORD* pBackupMsg = new MSG_DWORD;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}
			break;
		case MP_HOUSE_BONUS_ACK :
		case MP_HOUSE_BONUS_NACK :
			{
				MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
				MSG_DWORD2* pBackupMsg = new MSG_DWORD2;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}	
			break;
		case MP_HOUSE_UNINSTALL_ACK:
			{
				MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
				MSG_DWORD3* pBackupMsg = new MSG_DWORD3;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}
			break;
		case MP_HOUSE_USEITEM_NACK:	
			{
				MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
				MSG_DWORD4* pBackupMsg = new MSG_DWORD4;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}
			break;
		case MP_HOUSE_ACTION_FORCE_GETOFF_ACK:
			{
				MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
				MSG_DWORD3* pBackupMsg = new MSG_DWORD3;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}
			break;
		case MP_HOUSE_ACTION_ACK:
			{
				MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;
				MSG_DWORD5* pBackupMsg = new MSG_DWORD5;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}
			break;
		case MP_HOUSE_STORED_ACK:
		case MP_HOUSE_INSTALL_ACK:
		case MP_HOUSE_UPDATE_ACK:
			{
				MSG_HOUSE_FURNITURE* pmsg = (MSG_HOUSE_FURNITURE*)pMsg;
				MSG_HOUSE_FURNITURE* pBackupMsg = new MSG_HOUSE_FURNITURE;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}
			break;
		case MP_HOUSE_EXTEND:
			{
				MSG_HOUSE_EXTEND* pmsg = (MSG_HOUSE_EXTEND*)pMsg;
				MSG_HOUSE_EXTEND* pBackupMsg = new MSG_HOUSE_EXTEND;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}
			break;
	default:
		{
			return FALSE;
		}
	}

	return TRUE;
}

void cHousingMgr::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	if( m_dwHouseLoadState & eHSLoad_NOWLOADING )		//eHSLoad_NOWLOADING �÷��װ� �����Ǿ��ִٸ� �Ͽ콺 �ε����̴�. 
	{
		if ( BackupNetworkMsg( Protocol, pMsg) )		//�ε����� �̿��� ��Ŷ�� ����Ʈ�� ���� 
		{
			return;	//�ε��� ���þ��� ��Ŷ�̶�� ����Ǿ������� ó������ ���� 
		}
	}

	switch(Protocol)
	{
	case MP_HOUSE_CREATE_ACK:			House_Create_Ack(pMsg);break;				//�Ͽ콺 ����	
	case MP_HOUSE_CREATE_NACK:			House_Create_Nack(pMsg);break;				//�Ͽ콺 ��������
	case MP_HOUSE_ENTRANCE_NACK:		House_Entrance_Nack(pMsg);break;			//�Ͽ콺 ������� 
	case MP_HOUSE_INFO:					House_Info(pMsg);break;						//�Ͽ콺 ����			
	case MP_HOUSE_FURNITURELIST:		House_FurnitureList(pMsg);break;			//�������� ����Ʈ 
	case MP_HOUSE_STORED_ACK:			House_Stored_Ack(pMsg);break;				//����â����		(�κ� ������ -> â�� ����) 
	case MP_HOUSE_STORED_NACK:			House_Stored_Nack(pMsg);break;				//����â��������
	case MP_HOUSE_RESTORED_ACK:			House_Restored_Ack(pMsg);break;				//����â��������	(â�� ���� -> �κ������� )
	case MP_HOUSE_RESTORED_NACK:		House_Restored_Nack(pMsg);break;			//����â�������� ���� 
	case MP_HOUSE_DECOMODE_ACK:			House_Decomode_Ack(pMsg);break;				//�ٹ̱��� On/Off   
	case MP_HOUSE_DECOMODE_NACK:		House_Decomode_Nack(pMsg);break;			//�ٹ̱��� On/Off ���� 
	case MP_HOUSE_INSTALL_ACK:			House_Install_Ack(pMsg);break;				//�ʵ忡 ������ġ
	case MP_HOUSE_INSTALL_NACK:			House_Install_Nack(pMsg);break;				//�ʵ忡 ������ġ ���� 
	case MP_HOUSE_UNINSTALL_ACK:		House_UnInstall_Ack(pMsg);break;			//�ʵ忡�� ������ġ ���� 
	case MP_HOUSE_UNINSTALL_NACK:		House_UnInstall_Nack(pMsg);break;			//�ʵ忡�� ������ġ ���� ����
	case MP_HOUSE_ACTION_FORCE_GETOFF_ACK:	House_Action_Force_GetOff_Ack(pMsg);	break;	// �������� ������ ������
	case MP_HOUSE_ACTION_FORCE_GETOFF_NACK:	House_Action_Force_GetOff_Nack(pMsg);	break;	// �������� ������ ������ ����
	case MP_HOUSE_ACTION_ACK:			House_Action_Ack(pMsg);break;				//������ ���� �׼� �ߵ� 
	case MP_HOUSE_ACTION_NACK:			House_Action_Nack(pMsg);break;				//������ ���� �׼� �ߵ� ���� 
	case MP_HOUSE_BONUS_ACK :			House_Bonus_Ack(pMsg);break;				//�ٹ̱� ���ʽ� ���
	case MP_HOUSE_BONUS_NACK :			House_Bonus_Nack(pMsg);break;				//�ٹ̱� ���ʽ� ��� ���� 
	case MP_HOUSE_UPDATE_ACK:			House_Update_Ack(pMsg);break;				//���� ���� ���� (��ġ,ȸ��,���͸���,�Ⱓ��������)
	case MP_HOUSE_UPDATE_NACK:			House_Update_Nack(pMsg);break;				//���� ���� ���� ���� 
	case MP_HOUSE_UPDATE_MATERIAL_ACK:  House_Update_Material_Ack(pMsg);break;		//���� ���͸��� ���� ����
	case MP_HOUSE_DESTROY_ACK:			House_Destroy_Ack(pMsg);break;				//���� ���� 
	case MP_HOUSE_DESTROY_NACK:			House_Destroy_Nack(pMsg);break;				//���� ���� ���� 
	case MP_HOUSE_VOTE_ACK:				House_Vote_Ack(pMsg);break;					//�Ͽ콺 ��õ 
	case MP_HOUSE_VOTE_NACK:			House_Vote_Nack(pMsg);break;				//�Ͽ콺 ��õ ����
	case MP_HOUSE_EXIT_NACK:			House_Exit_Nack(pMsg);break;				//�Ͽ콺 ������ ���� 
	case MP_HOUSE_NOTIFY_VISIT:			House_Notify_Visit(pMsg);break;				//Player �Ͽ콺 �湮 �˸�
	case MP_HOUSE_NOTIFY_EXIT:			House_Notify_Exit(pMsg);break;				//Player �Ͽ콺 ������ �˸�
	case MP_HOUSE_NOTIFY_ACTION:		House_Notify_Action(pMsg);break;			//Player �׼� �˸� 		
	case MP_HOUSE_EXTEND:				House_Extend(pMsg);break;					//�Ͽ콺 Ȯ�� ó�� (�� & ����ġ�� & �⺻���������� ��ġ����or��ü �ȴ�)
	case MP_HOUSE_USEITEM_NACK:			House_UseItem_Nack(pMsg);break;				//�Ͽ콺 ���þ����� ������ 
	case MP_HOUSE_CHEAT_GETINFOALL_ACK:	House_Cheat_GetInfoAll_Ack(pMsg);break;		//��� �Ͽ콺 ���� ��� ġƮ
	case MP_HOUSE_CHEAT_GETINFOONE_ACK:	House_Cheat_GetInfoOne_Ack(pMsg);break;		//Ư�� �Ͽ콺 ���� ��� ġƮ 
	case MP_HOUSE_CHEAT_DELETE_ACK:		House_Cheat_Delete_Ack(pMsg);break;			//Ư�� �Ͽ콺 ���� 
	case MP_HOUSE_CHEAT_DELETE_NACK:	House_Cheat_Delete_Nack(pMsg);break;		//Ư�� �Ͽ콺 ���� ���� 
	}

	//-----------------------------------------------------------------------------------------------
	//	�Ͽ콺 �ε��� �����ٸ� ����Ʈ�� ����� ��Ŷ�� �������Ӿȿ� �ϰ������� ó�������� 
	//-----------------------------------------------------------------------------------------------

	if( m_dwHouseLoadState & eHSLoad_NOWLOADING )						//eHSLoad_NOWLOADING �÷��װ� �����Ǿ��ִٸ� �ε尡 �Ϸ�Ǿ����� �����Ѵ�  
	{
		if( (m_dwHouseLoadState - eHSLoad_NOWLOADING) == eHSLoad_OK )	//�������ӿ� �ε尡 �Ϸ�Ǿ��ٸ� �ʿ��� ó���� ����. 
		{
			m_dwHouseLoadState -= eHSLoad_NOWLOADING;					//eHSLoad_NOWLOADING �÷������� �ε�ó�� OK 

			//���� ������ ���μ����� �������ð��� ������������ 
			m_dwLastRemainProcessTime = gCurTime;

#ifdef _GMTOOL_
			//091016 pdy ���� �׽�Ʈ�� ���� �Ͽ�¡ ���� �α��߰�
			SYSTEMTIME systemTime ;
			GetLocalTime( &systemTime ) ;
			FILE *fp = fopen("HouseInfoLog.log", "a+");
			if( fp)
			{
				fprintf(fp,"\n");
				fprintf(fp,"HouseLoad_OK [%02d:%02d:%02d] \n", systemTime.wHour , systemTime.wMinute , systemTime.wSecond);
				fprintf(fp,"Installed Furniture Num : %d\n" ,m_dwFieldFurnitureNum);
				fprintf(fp,"Stored Furnitrue Num By OwnerList: %d\n" ,m_CurHouse.pFurnitureList.GetDataNum() ) ;
				for(int i=0; i< MAX_HOUSING_CATEGORY_NUM ; i++)
				{
					fprintf(fp,"Stored Num By Category : Category %d = %d \n",i,m_CurHouse.m_dwCategoryNum[i]);
				}
				fprintf(fp,"BackupPacketList Num %d \n" ,m_BackupPacketList.GetCount() ) ;
				fprintf(fp,"\n");
				fclose(fp);
			}
#endif

			//����� ��Ŷ�� ó������ 
			PTRLISTPOS pos = m_BackupPacketList.GetHeadPosition();
			while( pos )
			{
				MSGBASE* pBackupMsg = (MSGBASE*)m_BackupPacketList.GetNext( pos ) ;
				if( pBackupMsg )
				{
					NetworkMsgParse(pBackupMsg->Protocol,pBackupMsg);	//eHSLoad_NOWLOADING �÷��װ� �������⶧���� ���� ���ȣ���� �ƴϴ�. 
					SAFE_DELETE( pBackupMsg );
				}
			}
			m_BackupPacketList.RemoveAll();

			//����â�� �׻��ִ� ������ �����Ǿ���. 
			//�Ͽ콺 �ε��� ������ �������. 
			if( GAMEIN->GetHousingWarehouseDlg() )
			{
				GAMEIN->GetHousingWarehouseDlg()->SetActive( TRUE ) ;							
			}
		}
	}
}

void cHousingMgr::House_Create_Ack(void* pMsg)	
{
	//090527 pdy �Ͽ�¡ �ý��۸޼��� [����������]  
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1880 ));	//�Ͽ콺 ���� ����

	GAMEIN->GetHouseNameDlg()->SetDisable(FALSE);
	GAMEIN->GetHouseNameDlg()->SetActive(FALSE);
}

void cHousingMgr::House_Create_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	//090527 pdy �Ͽ�¡ �ý��۸޼��� ����NACK [����������]

	//�̹� �Ͽ콺�� �������ִ�.
	if( eHOUSEERR_HAVEHOUSE == pmsg->dwData )		
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1881 ) ); 
	}
	//�ֹε���� ����
	else if( eHOUSEERR_NOTREGIST == pmsg->dwData )	
	{				
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1284 ) );
	}
	//�Ͽ콺 ���� ���� 
	else											
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1882 ) ); 
	}

	PrintDebugErrorMsg( "MP_HOUSE_CREATE_NACK" ,pmsg->dwData);

	GAMEIN->GetHouseNameDlg()->SetDisable(FALSE);
}

void cHousingMgr::House_Entrance_Nack(void* pMsg)
{
	MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
	DWORD dwErr			= pmsg->dwData1;
	DWORD dwLinkKind	= pmsg->dwData2;
	DWORD dwSlot		= pmsg->dwData4;

	//090527 pdy �Ͽ�¡ �ý��۸޼��� ����NACK [���湮 ����]  
	switch(dwErr)
	{
		case eHOUSEERR_NOHOUSE :	//�Ͽ콺 ������ 
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1883 ) ); 
			break;
		case eHOUSEERR_FULL :		//���� Open �Ͽ콺 ���� (1000��)
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1875 ) );
			break;
		case eHOUSEERR_HOME2HOME_FAIL:	//���� -> ���� �̵� ���� 
			CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1904) ); 
			break;
		case eHOUSEERR_ONVEHICLE:		//Ż�� -> ���̵� ���� 
			CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(2037) ); 
			break;
		default:					//���� ����
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1884 ) );
			break;
	}

	PrintDebugErrorMsg("MP_HOUSE_ENTRANCE_NACK",dwErr);

	if( dwLinkKind == eHouseVisitByItem )
	{
		CItem* pItem = GAMEIN->GetInventoryDialog()->GetItemForPos(dwSlot);
		if( pItem )
			pItem->SetLock(FALSE);
	}

	GAMEIN->GetHouseSearchDlg()->SetDisableState(FALSE);
}

void cHousingMgr::House_Info(void* pMsg)
{
	//�Ͽ콺 ���� ������ �޾� ���´�  

	MSG_HOUSE_HOUSEINFO* pmsg = (MSG_HOUSE_HOUSEINFO*)pMsg;
	m_CurHouse.dwChannelID = pmsg->dwChannel;
	m_CurHouse.dwDecoPoint = pmsg->dwDecoPoint;
	m_byRankHouse = pmsg->cRank;

	memcpy(&(m_CurHouse.HouseInfo),&(pmsg->HouseInfo),sizeof(stHouseInfo));					//�Ͽ콺 ������ �����Ѵ�
	
	DWORD dwUserIdx = TITLE->GetUserIdx();													
	m_bHouseOwner = (dwUserIdx ==  m_CurHouse.HouseInfo.dwOwnerUserIndex )? TRUE : FALSE;	//�������ΰ� Ȯ���� �����Ѵ�.

	cHousingMainPointDlg* pMainPointDlg = NULL;
	pMainPointDlg = GAMEIN->GetHousingMainPointDlg();

	if( pMainPointDlg )																		//���� �������� �������� �����Ѵ� 
	{
		pMainPointDlg->SetDecoPoint( m_CurHouse.dwDecoPoint );
		pMainPointDlg->SetVisitCount( m_CurHouse.HouseInfo.dwTotalVisitCount ) ;
	}

	if( GAMEIN->GetHousingDecoPointDlg() )
		GAMEIN->GetHousingDecoPointDlg()->RefreshBonusList(m_CurHouse.dwDecoPoint,m_CurHouse.HouseInfo.dwDecoUsePoint);

	// NYJ
	if(pmsg->bInit)																			//�Ͽ콺 ������ ���ʷε��� �°��̶�� 
	{
		m_dwHouseLoadState |= eHSLoad_HOUSEINFO;											//m_dwHouseLoadState�� eHSLoad_HOUSEINFO�� �߰� 

		if( pMainPointDlg )
			pMainPointDlg->SetHouseName( m_CurHouse.HouseInfo.szHouseName );				//���� �������� ���̸��� ���ʷε��� �ѹ� �����Ѵ� 

		if( GAMEIN->GetHousingMainPointDlg() )												//�������� ������ ���� �����̸� ����  
			GAMEIN->GetHousingMainPointDlg()->SetActive( TRUE ) ;

		if( GAMEIN->GetHousingWarehouseButtonDlg())											//â���ư�� ���� 
			GAMEIN->GetHousingWarehouseButtonDlg()->SetActive( TRUE ) ;

		//090708 pdy ��ȹ�� ��û���� UI ���� 
		if( GAMEIN->GetHousingDecoModeBtDlg())												//�ٹ̱� ��� ��ư�� ���� 
			GAMEIN->GetHousingDecoModeBtDlg()->SetActive( TRUE ) ;

	}

#ifdef _GMTOOL_
	//091016 pdy ���� �׽�Ʈ�� ���� �Ͽ�¡ ���� �α��߰�
	if( pmsg->bInit )
	{
		SYSTEMTIME systemTime ;
		GetLocalTime( &systemTime ) ;
		FILE *fp = fopen("HouseInfoLog.log", "a+");
		if( fp)
		{
			fprintf(fp,"MP_HOUSE_INFO HouseName : %s Chanel : %d [%02d:%02d:%02d] \n", pmsg->HouseInfo.szHouseName,pmsg->dwChannel, systemTime.wHour , systemTime.wMinute , systemTime.wSecond);
			fprintf(fp,"\n");
			fclose(fp);
		}
	}
#endif
}

void cHousingMgr::House_FurnitureList(void* pMsg)
{
	//�������� ���� ����Ʈ�� �޴´� ���� ī�װ� ���� �ѹ����̾� ����´�.

	static DWORD dwLoadCount = 0;

	MSG_HOUSE_FURNITURELIST* pmsg = (MSG_HOUSE_FURNITURELIST*)pMsg;

	if( pmsg->wNum )
	{
		DWORD dwUserIdx = TITLE->GetUserIdx();
	
		for( int i=0; i < pmsg->wNum ; i++)
		{  
			if( pmsg->Furniture[i].dwObjectIndex )
			{
				m_CurHouse.m_dwFurnitureList[pmsg->wCategory][pmsg->Furniture[i].wSlot] = pmsg->Furniture[i].dwObjectIndex;

				//���°��� �ν����̸� �ʵ忡 ��ġ 
				if( pmsg->Furniture[i].wState == eHOUSEFURNITURE_STATE_INSTALL )
				{
					InstallFunitureToField( &pmsg->Furniture[i] , TRUE );
				}

				if( pmsg->Furniture[i].dwOwnerUserIndex == dwUserIdx )
				{
					//�������̸� �۴��ĸ���Ʈ�� ���� (â�� ���� ��������Ʈ)
					stFurniture* pNewFuniture = m_stFurniturePool.Alloc();
					*pNewFuniture = pmsg->Furniture[i];
					m_CurHouse.pFurnitureList.Add( pNewFuniture, pNewFuniture->dwObjectIndex );

					if( pmsg->Furniture[i].bNotDelete  )
					{
						m_CurHouse.pNotDeleteFurnitureList.Add(pNewFuniture , pNewFuniture->dwObjectIndex);
					}

					cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();	
					pDlg->AddIcon( pNewFuniture );													//â��Dlg�� �������� Add 

					m_CurHouse.m_dwCategoryNum[pmsg->wCategory]++;									//ī�װ��� �������� ++ 
				}
				else if( pmsg->Furniture[i].bNotDelete )
				{
					CFurniture*  pFurniture = m_FieldFurnitureTable.GetData(pmsg->Furniture[i].dwObjectIndex);		
					if(! pFurniture )
						continue;

					//�⺻��ġ ǰ���� �������� �ƴϾ ������ ����. 
					m_CurHouse.pNotDeleteFurnitureList.Add(pFurniture->GetFurnitureInfo() , pFurniture->GetFurnitureInfo()->dwObjectIndex);
				}
			}
		}
	}

	dwLoadCount++;
	if( MAX_HOUSING_CATEGORY_NUM == dwLoadCount	)
	{
		m_dwHouseLoadState |= eHSLoad_FURNITURELIST;
		dwLoadCount = 0;

		//090608 pdy ��ŷ���� ���� ����
		CheckAllMarkingObjByHeroZone();		//��簡���� �޾Ҵٸ� ����ο� ��ġ�� ��ŷ�� �Ǿ��ִ��� äũ 
	}

#ifdef _GMTOOL_
	//091016 pdy ���� �׽�Ʈ�� ���� �Ͽ�¡ ���� �α��߰�
	SYSTEMTIME systemTime ;
	GetLocalTime( &systemTime ) ;
	FILE *fp = fopen("HouseInfoLog.log", "a+");
	if( fp)
	{
		fprintf(fp,"MP_HOUSE_FURNITURELIST Categoty : %d [%02d:%02d:%02d] \n", pmsg->wCategory,systemTime.wHour , systemTime.wMinute , systemTime.wSecond);
		fclose(fp);
	}
#endif
}

void cHousingMgr::House_Stored_Ack(void* pMsg)
{
	//���� â�� ���� 
	MSG_HOUSE_FURNITURE* pmsg = (MSG_HOUSE_FURNITURE*)pMsg;

	DWORD dwUserIdx = TITLE->GetUserIdx();
	if( pmsg->Furniture.dwOwnerUserIndex ==  dwUserIdx )							//���� �����ϰ�� 
	{
		stFurniture* pNewFuniture = m_stFurniturePool.Alloc();						//�޸� �Ҵ� 

#ifdef _GMTOOL_
		if(! pNewFuniture)
			CHATMGR->AddMsg( CTC_SYSMSG, "HousingErr(Cli) : Memory Alloc Failed" );

		if(! pNewFuniture->dwObjectIndex)
			CHATMGR->AddMsg( CTC_SYSMSG, "HousingErr(Srv) : ObjectIndex Alloc Failed" );
#endif

		*pNewFuniture = pmsg->Furniture;
		m_CurHouse.pFurnitureList.Add( pNewFuniture, pNewFuniture->dwObjectIndex );	//â�����̺� ��� 

		cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();				

		pDlg->AddIcon( pNewFuniture );												//â��Dlg�� �������� Add 
		pDlg->FocusOnByFurniture( pNewFuniture );									//��ũ��,�� ���� �������� �����ش�.

		m_CurHouse.m_dwFurnitureList[pNewFuniture->wCategory][pNewFuniture->wSlot] = pNewFuniture->dwObjectIndex;
		m_CurHouse.m_dwCategoryNum[pNewFuniture->wCategory]++;						//ī�װ��� �������� ++ 


		const ITEM_INFO* info = ITEMMGR->GetItemInfo( pNewFuniture->dwLinkItemIndex );

		if( info )
		{
			//090527 pdy �Ͽ�¡ �ý��۸޼��� [�κ�->â�� ��������]
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1879 ) , info->ItemName ); //1879	"%s�� �� �� â��� �̵� �Ͽ����ϴ�."
		}
	}
}

void cHousingMgr::House_Stored_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_STORED_NACK",pmsg->dwData);
}

void cHousingMgr::House_Restored_Ack(void* pMsg)
{
	//���� â���� �������� 
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	DWORD dwUserIdx = TITLE->GetUserIdx();
	if( m_CurHouse.HouseInfo.dwOwnerUserIndex == dwUserIdx)
	{
		stFurniture* pRestoredFn = m_CurHouse.pFurnitureList.GetData(pmsg->dwData);	//���� �����ϰ�찡�����ִ� 
		if( pRestoredFn )
		{
			//���������̹Ƿ� ������ ��������
			DestroyFuniture(pRestoredFn);
		}
	}
}

void cHousingMgr::House_Restored_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	stFurniture* pRestoredFn = m_CurHouse.pFurnitureList.GetData(pmsg->dwData);	//���� �����ϰ�찡�����ִ� 
	if( pRestoredFn )
	{
		cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();

		if( pDlg)
		{
			cHousingStoredIcon* pICon = pDlg->GetStoredIconIcon(pRestoredFn->wCategory-1 , pRestoredFn->wSlot);

			if( pICon )
				pICon->SetLock(FALSE);
		}
	}
	PrintDebugErrorMsg("MP_HOUSE_RESTORED_NACK",pmsg->dwData);
}

void cHousingMgr::House_Decomode_Ack(void* pMsg)
{
	//�ٹ̱� ��� On/Off
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	if( pmsg->dwObjectID == gHeroID)
	{
		//�������� �ڽ��϶� �ٹ̱� ��� ���� 
		SetDecorationMode(BOOL(pmsg->dwData));
	}
	else
	{
		//3���϶� 
		CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwObjectID);
		if(! pPlayer )
			return;

		if( pmsg->dwData ) 
		{
			OBJECTSTATEMGR->StartObjectState(pPlayer, eObjectState_Housing);
		}
		else
		{
			OBJECTSTATEMGR->EndObjectState(pPlayer, eObjectState_Housing);
		}
	}
}

void cHousingMgr::House_Decomode_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	if( pmsg->dwObjectID == gHeroID)
	{
		PrintDebugErrorMsg("MP_HOUSE_DECOMODE_NACK",pmsg->dwData);
	}
}

void cHousingMgr::House_Install_Ack(void* pMsg)
{
	//���� �ʵ忡 ��ġ 
	MSG_HOUSE_FURNITURE* pmsg = (MSG_HOUSE_FURNITURE*)pMsg;
	DWORD dwUserIdx = TITLE->GetUserIdx();

	if( pmsg->Furniture.dwOwnerUserIndex ==  dwUserIdx )						//���� �����ϰ�� ��������,������,�ٹ̱�����Ʈ,���ʽ�����Ʈ,�ʵ忡��ġ
	{
		stFurniture* pFuniture = m_CurHouse.pFurnitureList.GetData(pmsg->Furniture.dwObjectIndex);

		if( ! pFuniture )
			return;

		*pFuniture = pmsg->Furniture;

		InstallFunitureToField( &pmsg->Furniture );										
		cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();	

		//090708 pdy RefreshIcon ���� 
		pDlg->RefreshIcon( pFuniture );

		m_CurHouse.dwDecoPoint = pmsg->dwDecoPoint;

		if( IsHouseOwner() )
		{
			if( GAMEIN->GetHousingMainPointDlg() )
				GAMEIN->GetHousingMainPointDlg()->SetDecoPoint( m_CurHouse.dwDecoPoint );

			if( GAMEIN->GetHousingDecoPointDlg() )
				GAMEIN->GetHousingDecoPointDlg()->RefreshBonusList(m_CurHouse.dwDecoPoint,m_CurHouse.HouseInfo.dwDecoUsePoint);
		}
	}
	else		
	{	
		InstallFunitureToField( &pmsg->Furniture );								//���������� �ƴѰ�� �ʵ忡 �� ��ġ 
	}
}

void cHousingMgr::House_Install_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_INSTALL_NACK",pmsg->dwData);

	RemoveCurDeco();
	EndDecoration();
}

void cHousingMgr::House_UnInstall_Ack(void* pMsg)
{
	//���� �ʵ忡�� ��ġ ���� 

	//��ġ ���� �Ǵ� ���������� �ٳ������ ���� �ƴ϶� ������Ʈ �ε����� ����� 
	//Ŭ���̾�Ʈ���� ���� ���� ���°��� �ٲٰ� �Ǿ��ִ�

	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;

	DWORD dwUserIdx = TITLE->GetUserIdx();

	DWORD dwTargetIdx = pmsg->dwData1 ;
	BOOL  bDestroy = pmsg->dwData2 ;
	DWORD dwDecoPoint = pmsg->dwData3;

	if( m_CurHouse.HouseInfo.dwOwnerUserIndex ==  dwUserIdx )								//������ �����ϰ��  ��������,������,�ٹ̱�����Ʈ,���ʽ�����Ʈ,�ʵ忡�� ����
	{
		stFurniture* pFuniture = m_CurHouse.pFurnitureList.GetData(dwTargetIdx);
		if( ! pFuniture )
			return;

		pFuniture->wState = eHOUSEFURNITURE_STATE_UNINSTALL;								
		UnInstallFunitureFromField(dwTargetIdx);

		cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();	
		//090708 pdy RefreshIcon ���� 
		pDlg->RefreshIcon( pFuniture );	

		m_CurHouse.dwDecoPoint = dwDecoPoint;

		if( GAMEIN->GetHousingMainPointDlg() )
			GAMEIN->GetHousingMainPointDlg()->SetDecoPoint( m_CurHouse.dwDecoPoint );

		if( GAMEIN->GetHousingDecoPointDlg() )
			GAMEIN->GetHousingDecoPointDlg()->RefreshBonusList(m_CurHouse.dwDecoPoint,m_CurHouse.HouseInfo.dwDecoUsePoint);

		if( bDestroy )	//�����϶�� ������ ���̸� ������ �����Ѵ�
		{
			stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(pFuniture->dwFurnitureIndex);
			if( stFurnitureInfo )
			{
				//090527 pdy �Ͽ�¡ �ý��۸޼��� [�Ⱓ�Ϸᰡ������]
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1861 ) , stFurnitureInfo->szName ); //1861	"%s��(��) �Ⱓ�� ���� �����Ǿ����ϴ�"
			}

			DestroyFuniture(pFuniture);
		}
	}
	else				
	{
		UnInstallFunitureFromField(dwTargetIdx);						//���������� �ƴѰ�� �ʵ忡�� ����
	}
}

void cHousingMgr::House_UnInstall_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_UNINSTALL_NACK",pmsg->dwData);

	EndDecoration();
}

void cHousingMgr::House_Action_Force_GetOff_Ack(void* pMsg)
{
	// ���� ä�ο� �ִ� ���������뺸
	// dwData1(����Index), dwData2(Attach����), dwData3(�׼�Index)
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
	DWORD dwFunitureID = pmsg->dwData1;
	DWORD dwActionIndex = pmsg->dwData3;

	CPlayer* pActionPlayer = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwObjectID);
	if( ! pActionPlayer )
		return;

	stHouseActionInfo* pActionInfo = GAMERESRCMNGR->GetHouseActionInfo(dwActionIndex);
	if( !pActionInfo)
		return;

	CFurniture* pActionFuniture = m_FieldFurnitureTable.GetData(dwFunitureID);		
	if( ! pActionFuniture ) 
		return;

	stFurniture* pstFurniture = NULL;
	if( IsHouseOwner() )
	{
		pstFurniture = m_CurHouse.pFurnitureList.GetData(dwFunitureID);
	}
	else
	{
		pstFurniture = pActionFuniture->GetFurnitureInfo();
	}

	if( ! pstFurniture) 
			return;

	stFunitureInfo* pstFunitureInfo = GAMERESRCMNGR->GetFunitureInfo(pstFurniture->dwFurnitureIndex);
	if( !pstFunitureInfo)
		return;

	RideOffPlayerFromFuniture(pActionPlayer, TRUE);
	APPEARANCEMGR->ShowWeapon( pActionPlayer ) ;
}

void cHousingMgr::House_Action_Force_GetOff_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_ACTION_FORCE_GETOFF_NACK",pmsg->dwData);
}

void cHousingMgr::House_Action_Ack(void* pMsg)
{
	// ���� ä�ο� �ִ� ���������뺸
	// dwData1(����Index), dwData2(Attach����), dwData3(�׼�Index), dwData4(x��ġ), dwData5(z��ġ)
	MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;
	DWORD dwFunitureID = pmsg->dwData1;
	DWORD dwRideSlot = pmsg->dwData2;
	DWORD dwActionIndex = pmsg->dwData3;
	DWORD dwPosX = pmsg->dwData4;
	DWORD dwPosZ = pmsg->dwData5;

	CPlayer* pActionPlayer = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwObjectID);
	if( ! pActionPlayer )
		return;

	stHouseActionInfo* pActionInfo = GAMERESRCMNGR->GetHouseActionInfo(dwActionIndex);
	if( !pActionInfo)
		return;

	CFurniture* pActionFuniture = m_FieldFurnitureTable.GetData(dwFunitureID);		
	if( ! pActionFuniture ) 
		return;

	stFurniture* pstFurniture = NULL;
	if( IsHouseOwner() )
	{
		pstFurniture = m_CurHouse.pFurnitureList.GetData(dwFunitureID);
	}
	else
	{
		pstFurniture = pActionFuniture->GetFurnitureInfo();
	}

	if( ! pstFurniture) 
			return;

	stFunitureInfo* pstFunitureInfo = GAMERESRCMNGR->GetFunitureInfo(pstFurniture->dwFurnitureIndex);
	if( !pstFunitureInfo)
		return;

	if( pActionInfo->dwActionType != eHOUSE_ACTION_TYPE_RIDE && 
		pActionInfo->dwActionType != eHOUSE_ACTION_TYPE_GETOFF )
	{
		//ž�� ���� �׼��� �ƴϸ� ������ ��ǰ� ����Ʈ�� �����Ͽ��ش�. 
		if( pActionInfo->dwPlayer_MotionIndex )
			pActionPlayer->ChangeMotion( pActionInfo->dwPlayer_MotionIndex ,FALSE);

		if( pActionInfo->dwPlayer_EffectIndex )
		{
			OBJECTEFFECTDESC desc( pActionInfo->dwPlayer_EffectIndex );				//�÷��̾ ����Ʈ�� ������ ���� 
			pActionPlayer->AddObjectEffect( pActionInfo->dwPlayer_EffectIndex , &desc, 1 );
		}

		if( pActionInfo->dwFurniture_MotionIndex )
			pActionFuniture->ChangeMotion( pActionInfo->dwFurniture_MotionIndex ,FALSE);		//������ ����� �ִٸ� ���� 

		if( pActionInfo->dwFurniture_EffectIndex )
		{
			OBJECTEFFECTDESC desc( pActionInfo->dwFurniture_EffectIndex );			//������ ����Ʈ�� ������ ���� 
			pActionFuniture->AddObjectEffect( pActionInfo->dwFurniture_EffectIndex , &desc, 1 );
		}
	}

	switch(pActionInfo->dwActionType)
	{
		case eHOUSE_ACTION_TYPE_RIDE:
			{
				pActionPlayer->SetRideFurnitureID(dwFunitureID);
				pActionPlayer->SetRideFurnitureSeatPos(dwRideSlot);

				RideOnPlayerToFuniture(pActionPlayer);

				VECTOR3 Position = {0,};
				Position.x = dwPosX;
				Position.z = dwPosZ;
				pActionPlayer->SetPosition(&Position);

				APPEARANCEMGR->HideWeapon( pActionPlayer ) ;
			}
			break;
		case eHOUSE_ACTION_TYPE_GETOFF:
			{
				RideOffPlayerFromFuniture(pActionPlayer);

				VECTOR3 Position = {0,};
				Position.x = dwPosX;
				Position.z = dwPosZ;
				pActionPlayer->SetPosition(&Position);

				APPEARANCEMGR->ShowWeapon( pActionPlayer ) ;
			}
			break;
		case eHOUSE_ACTION_TYPE_BUFF:
			{
				if( gHeroID == pActionPlayer->GetID() )
				{
					cActiveSkillInfo* info = ( cActiveSkillInfo* )SKILLMGR->GetSkillInfo( pActionInfo->dwActionValue );
					if(info)
					{
						ACTIVE_SKILL_INFO* skillInfo = info->GetSkillInfo();
						if(skillInfo)
						{
							//SKILLMGR->AddBuffSkill( *pPlayer, *skillInfo );
							CHATMGR->AddMsg( CTC_SYSMSG, "%s ���� ȿ���� ������ϴ�.", skillInfo->Name);
						}
					}
				}
			}
			break;
		// 091105 pdy �Ͽ�¡ ���� �׼��߰� ( �̴�Ȩ�� �� ������ ��ũ ) 
		case eHOUSE_ACTION_TYPE_OPEN_HOMEPAGE:
			{
				if( gHeroID ==  pActionPlayer->GetID() )
				{
					// ���������� �����. 
					/*
					cHousingWebDlg* pDlg = GAMEIN->GetHousingWebDlg();

					if( pDlg )
					{
						pDlg->OpenMiniHomePage( GetCurHouseOwnerIndex() );
					}
					*/
				}
			}
			break;
	}
}

void cHousingMgr::House_Action_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_ACTION_NACK",pmsg->dwData);
}

void cHousingMgr::House_Bonus_Ack(void* pMsg)
{
	//�ٹ̱� ���ʽ� ���� 

	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	if( IsHouseOwner() )
	{
		m_CurHouse.HouseInfo.dwDecoUsePoint =  pmsg->dwData2;

		if( GAMEIN->GetHousingMainPointDlg() )
			GAMEIN->GetHousingMainPointDlg()->SetDecoPoint( m_CurHouse.dwDecoPoint );

		if( GAMEIN->GetHousingDecoPointDlg() )
			GAMEIN->GetHousingDecoPointDlg()->RefreshBonusList( m_CurHouse.dwDecoPoint ,m_CurHouse.HouseInfo.dwDecoUsePoint);
	}

	stHouseBonusInfo* pBonusInfo = GAMERESRCMNGR->GetHouseBonusInfo(pmsg->dwData1);

	if( ! pBonusInfo )
		return;

	//091020 pdy �ٹ̱� ���ʽ� ��� �޼��� �����ο��Ը� ��쵵�� ����
	if( IsHouseOwner() )
	{
		switch(pBonusInfo->dwBonusType)
		{
			case 0 :	//����Ÿ�� 
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1871 ) , pBonusInfo->szName); //%s�� ȿ���� ������ϴ�. 
				}
				break;
		}
	}
}

void cHousingMgr::House_Bonus_Nack(void* pMsg)
{
	//�ٹ̱� ���ʽ��� ������� ���Ͽ���.
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	char buf[128] = {0,};
	sprintf(buf,"MP_HOUSE_BONUS_NACK BonusIndex : %d",pmsg->dwData1);
	PrintDebugErrorMsg(buf,pmsg->dwData2);
}
void cHousingMgr::House_Update_Ack(void* pMsg)
{
	//������ ������ ������Ʈ�Ǿ���. (��ġ,ȸ��,���͸��� ��)
	MSG_HOUSE_FURNITURE* pmsg = (MSG_HOUSE_FURNITURE*)pMsg;
	DWORD dwUserIdx = TITLE->GetUserIdx();
	if( pmsg->Furniture.dwOwnerUserIndex ==  dwUserIdx )								
	{
		stFurniture* pFuniture = m_CurHouse.pFurnitureList.GetData(pmsg->Furniture.dwObjectIndex);
		if( pFuniture )
			*pFuniture = pmsg->Furniture;		//���������� ��� ���������� ������ �ش�.
	}

	UpdateFunitere( &pmsg->Furniture ) ;
}
void cHousingMgr::House_Update_Nack(void* pMsg)
{
	//���� ������Ʈ ����
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_UPDATE_NACK",pmsg->dwData);
}

void cHousingMgr::House_Update_Material_Ack(void* pMsg)
{
	MSG_DWORD2*	 pmsg = (MSG_DWORD2*)pMsg;
	DWORD	dwFnObjdx = pmsg->dwData1 ;
	int		nMeterial =	(int) pmsg->dwData2 ;

	CFurniture* pFuniture = m_FieldFurnitureTable.GetData(dwFnObjdx);	
	if(! pFuniture) 
		return ;

	pFuniture->GetFurnitureInfo()->nMaterialIndex = nMeterial ;

	pFuniture->GetEngineObject()->SetMaterialIndex(nMeterial);

	// �������� ��� ���������� �ٲ� ����
	if( IsHouseOwner( ) )
	{
		stFurniture* pFuniture = m_CurHouse.pFurnitureList.GetData(dwFnObjdx);
		if( pFuniture )
		{
			pFuniture->nMaterialIndex = nMeterial ;
		}
	}
}

void cHousingMgr::House_Destroy_Ack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	stFurniture* pFuniture = m_CurHouse.pFurnitureList.GetData(pmsg->dwData);

	if( !pFuniture )
		return;

	DestroyFuniture(pFuniture);
}
void cHousingMgr::House_Destroy_Nack(void* pMsg)
{
	//�������� ����
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_DESTROY_NACK",pmsg->dwData);
}

void cHousingMgr::House_Vote_Ack(void* pMsg)
{
	//��õ ����
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1885 )); //��õ�Ͽ����ϴ�.
}

void cHousingMgr::House_Vote_Nack(void* pMsg)
{
	//��õ ���� 
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	//090527 pdy �Ͽ�¡ �ý��۸޼��� ����NACK [��õ����]  
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1886 )); //1886	"��õ�������߽��ϴ�."
	PrintDebugErrorMsg("MP_HOUSE_VOTE_NACK",pmsg->dwData);
}

void cHousingMgr::House_Exit_Nack(void* pMsg)
{
	//������ ������ ���� 
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_EXIT_NACK",pmsg->dwData);

	//090618 pdy �Ͽ�¡ ������ �׼� ���� ���� 
	OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
}
// �Ͽ콺���� �뺸
void cHousingMgr::House_Notify_Visit(void* pMsg)
{
	//�÷��̾� �湮�� 
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	m_CurHouse.dwJoinPlayerNum = pmsg->dwData;

	if( GAMEIN->GetHousingMainPointDlg() )	
			GAMEIN->GetHousingMainPointDlg()->SetVisitCount( m_CurHouse.dwJoinPlayerNum );
}
void cHousingMgr::House_Notify_Exit(void* pMsg)
{
	//�÷��̾� ������
}
void cHousingMgr::House_Notify_Action(void* pMsg)
{
	//�÷��̾� �׼ǽ�
}

void cHousingMgr::House_Extend(void* pMsg)
{
	MSG_HOUSE_EXTEND* pmsg = (MSG_HOUSE_EXTEND*)pMsg;

	if( IsHouseOwner() )
	{
		m_CurHouse.dwDecoPoint =  pmsg->dwDecoPoint;
		m_CurHouse.HouseInfo.ExtendLevel = pmsg->wExtendLevel;

		if( GAMEIN->GetHousingMainPointDlg() )	
			GAMEIN->GetHousingMainPointDlg()->SetDecoPoint( m_CurHouse.dwDecoPoint );

		if( GAMEIN->GetHousingDecoPointDlg() )	
			GAMEIN->GetHousingDecoPointDlg()->RefreshBonusList( m_CurHouse.dwDecoPoint ,m_CurHouse.HouseInfo.dwDecoUsePoint);
	}

	//Ȯ���� ��������� �⺻��ġ ǰ���� �ϴ� �� Destroy��Ų��.
	m_CurHouse.pNotDeleteFurnitureList.SetPositionHead();
	while(stFurniture* pstFurniture= m_CurHouse.pNotDeleteFurnitureList.GetData())
	{
		UnInstallFunitureFromField(pstFurniture->dwObjectIndex);
		if( IsHouseOwner() )	
		{
			DestroyFuniture(pstFurniture);
		}
	}
	m_CurHouse.pNotDeleteFurnitureList.RemoveAll();

	//�������� ���ν����� ��û�Ѱ͵��� �� ���ν����Ų��.
	for(int i=0 ; i < pmsg->wUnInstallNum ; i++)
	{
		stFurniture* pstFurniture = NULL;
		if( IsHouseOwner() )					//���� �����ϰ�� 
		{
			pstFurniture = m_CurHouse.pFurnitureList.GetData(pmsg->UnInstall[i].dwObjectIndex);
			*pstFurniture = pmsg->UnInstall[i]; //���� ���� ī�� 
			pstFurniture->wState = eHOUSEFURNITURE_STATE_UNINSTALL; //���ν��� ���� 

			cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();				
			//090708 pdy RefreshIcon ���� 
			pDlg->RefreshIcon( pstFurniture );
		}
		else
		{
			CFurniture* pFuniture = m_FieldFurnitureTable.GetData(pmsg->UnInstall[i].dwObjectIndex);	
			if( pFuniture)
				pstFurniture = pFuniture ->GetFurnitureInfo();
		}

		if(!pstFurniture)
			continue;

		UnInstallFunitureFromField(pstFurniture->dwObjectIndex);
	}

	//�������� �ν����� ��û�� �͵��� �� �ν��� ��Ų��.
	for(int i=0 ; i < pmsg->wInstallNum ; i++)
	{
		m_CurHouse.m_dwFurnitureList[pmsg->Install[i].wCategory][pmsg->Install[i].wSlot] = pmsg->Install[i].dwObjectIndex;

		InstallFunitureToField( &pmsg->Install[i] , TRUE );

		if( IsHouseOwner() )	//�������̸� �۴��ĸ���Ʈ�� ���� (â�� ���� ��������Ʈ)
		{
			stFurniture* pNewFuniture = m_stFurniturePool.Alloc();
			*pNewFuniture = pmsg->Install[i];
			m_CurHouse.pFurnitureList.Add( pNewFuniture, pNewFuniture->dwObjectIndex );

			if( pmsg->Install[i].bNotDelete  )
			{
				m_CurHouse.pNotDeleteFurnitureList.Add(pNewFuniture , pNewFuniture->dwObjectIndex);
			}

			cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();				

			pDlg->AddIcon( pNewFuniture );													//â��Dlg�� �������� Add 
			m_CurHouse.m_dwCategoryNum[pmsg->Install[i].wCategory]++;						//ī�װ��� �������� ++ 
		}
		else if( pmsg->Install[i].bNotDelete )
		{
			CFurniture*  pFurniture = m_FieldFurnitureTable.GetData(pmsg->Install[i].dwObjectIndex);		
			if(! pFurniture )
				continue;

			//�⺻��ġ ǰ���� �������� �ƴϾ ������ ����. 
			m_CurHouse.pNotDeleteFurnitureList.Add(pFurniture->GetFurnitureInfo() , pFurniture->GetFurnitureInfo()->dwObjectIndex);
		}
	}	

	//090608 pdy ��ŷ���� ���� ����
	CheckAllMarkingObjByHeroZone();		//�ʵ忡 ��ġ�� ������Ʈ�� ����� ��ġ�� ��ŷ�� ������Ʈ�� �����Ѵ�.

	//090527 pdy �Ͽ�¡ �ý��۸޼��� [�Ͽ콺Ȯ��] 
	CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1865 ) , pmsg->wExtendLevel+1  );//1865	"���� %d �ܰ�� Ȯ��Ǿ����ϴ�."
}

void cHousingMgr::House_UseItem_Nack(void* pMsg)
{
	// �κ����� ��밡���� �Ͽ�¡������ ������ ó��  
	MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
	DWORD dwItemPos = pmsg->dwData2;
	DWORD dwSupplyType = pmsg->dwData3;

	switch(dwSupplyType)
	{
		case ITEM_KIND_FURNITURE_WALLPAPER:
		case ITEM_KIND_FURNITURE_FLOORPAPER:
		case ITEM_KIND_FURNITURE_CEILINGPAPER:
			{
				ChangeMaterialFurniture( pmsg->dwData1 , 0 ,/*RollBack*/ 1 );
			}
			break;
	}

	CItem* pItem = GAMEIN->GetInventoryDialog()->GetItemForPos(dwItemPos);
	if( pItem )
		pItem->SetLock(FALSE);

	PrintDebugErrorMsg("MP_HOUSE_USEITEM_NACK",pmsg->dwData4);
}
void cHousingMgr::House_Cheat_GetInfoAll_Ack(void* pMsg)
{
	//�ʼ����� ��� �Ͽ콺���� ����������� ������� 
	MSG_HOUSE_CHEATINFO_ALL* pmsg = (MSG_HOUSE_CHEATINFO_ALL*)pMsg;

	char Buf[256] = {0,};

	CHATMGR->AddMsg( CTC_SYSMSG, "---------CHEATINFO_ONEINFO--------");
	sprintf(Buf,"HouseNum: %d UserNum: %d ReservationNum: %d" ,pmsg->dwHouseNum ,pmsg->dwUserNum , pmsg->dwReservationNum );
	CHATMGR->AddMsg( CTC_SYSMSG, Buf);

	CHATMGR->AddMsg( CTC_SYSMSG, "---------CHEATINFO_INFOALL--------");
	for(DWORD i=0; i < pmsg->dwHouseNum ; i++ )
	{
		sprintf(Buf,"HouseName: %s" ,pmsg->HouseInfo[i].szHouseName);
		CHATMGR->AddMsg( CTC_SYSMSG, Buf);

		sprintf(Buf,"ChannelID: %d OwnerUserIdx: %d VisiterNum: %d" 
			,pmsg->HouseInfo[i].dwChannelID,pmsg->HouseInfo[i].dwOwnerUserIndex,pmsg->HouseInfo[i].dwVisiterNum);
			CHATMGR->AddMsg( CTC_SYSMSG, Buf);

	}
}
void cHousingMgr::House_Cheat_GetInfoOne_Ack(void* pMsg)
{
	//�ʼ����� �Ͽ콺���� ����������� ������� 
	MSG_HOUSE_CHEATINFO_ONE* pmsg = (MSG_HOUSE_CHEATINFO_ONE*)pMsg;

	char Buf[256] = {0,};

	CHATMGR->AddMsg( CTC_SYSMSG, "---------CHEATINFO_ONEINFO--------");
	sprintf(Buf,"Name: %s, Ch: %d OwnerUserIdx: %d Visiter: %d",
			pmsg->HouseInfo.szHouseName, pmsg->HouseInfo.dwChannelID, pmsg->HouseInfo.dwOwnerUserIndex, pmsg->HouseInfo.dwVisiterNum);
			CHATMGR->AddMsg( CTC_SYSMSG, Buf);

	if(strlen(pmsg->HouseInfo.szOwnerName) > 2)
	{
		sprintf(Buf, "HouseOwner [%s] in HousingMap", pmsg->HouseInfo.szOwnerName);
		CHATMGR->AddMsg( CTC_SYSMSG, Buf);
	}

	for(int i=1; i<MAX_HOUSING_CATEGORY_NUM; i++)
	{
		sprintf(Buf, "Slot[%d] : (%d / %d)", i, pmsg->HouseInfo.m_dwInstalledNum[i], pmsg->HouseInfo.m_dwFurnitureNum[i]);
		CHATMGR->AddMsg( CTC_SYSMSG, Buf);
	}
}

void cHousingMgr::House_Cheat_Delete_Ack(void* pMsg)
{
	CHATMGR->AddMsg( CTC_SYSMSG, "House_Cheat_Delete OK");
}

void cHousingMgr::House_Cheat_Delete_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_CHEAT_DELETE_NACK",pmsg->dwData);
}


void cHousingMgr::Process()
{
	if(! IsHouseInfoLoadOk() )			
		return ;


	// ������� ������ �ð��� 1�п� �ѹ�. 
	// 1�п� �ѹ��� �ð��Ҹ��ų��.
	ProcessRemainTime();

	if( m_bShowTest )
		ShowAllMarkingTest();

	//090323 pdy �Ͽ�¡ ��ġ UI�߰� 
	if( m_bDecoAngle )
	{
		CHousingRevolDlg* pRevolDlg = GAMEIN->GetHousingRevolDlg();

		if( ! pRevolDlg & ! m_pDecoObj )
			return;

		float fAngle = pRevolDlg->GetRotateAngleFromBarRate();

		m_pDecoObj->SetAngle( fAngle );
	}

	ProcessCulling();

	ProcessAlpha();

	ProcessWaitForMaikingObjList();
}

//090406 pdy ��,����� �ø� 
void cHousingMgr::ProcessCulling()
{
	static DWORD dwLastCheckTime = 0;
	DWORD ElapsedTime = gCurTime - dwLastCheckTime;

	if( ElapsedTime < 50 )		//�ǽð��� ���׷��� 
		return;

	dwLastCheckTime = gCurTime;

	if( m_dwFieldFurnitureNum != 0)
	{
		MHCAMERADESC* pCurCamera = CAMERA->GetCameraDesc() ;

		m_FieldFurnitureTable.SetPositionHead();
		
		for(CFurniture* pFieldObj = m_FieldFurnitureTable.GetData();
			0 < pFieldObj;
			pFieldObj = m_FieldFurnitureTable.GetData())
		{
			DWORD dwIdx = pFieldObj->GetFurnitureInfo()->dwFurnitureIndex;

			if( m_pDecoObj && pFieldObj == m_pDecoObj )
				continue;

			//������̳� �ܺ��� �ƴҰ�� �ʿ���� 
			if( ! ( Get_HighCategory(dwIdx) ==  eHOUSE_HighCategory_Door ||  IsExteriorWall(dwIdx) ) ) 
				continue;

			VECTOR3 vDirz = {0,};
			float fAngle = pFieldObj->GetFurnitureInfo()->fAngle;

			//Y��ȸ���� �����ϸ� ������Ʈ�� Z���� ���������� (�ٶ󺸴� ����)

			//Max�� Front�� ���缭 �𵨸��� �ߴٸ� Dirz(ȸ������ 0���϶�) = 0 , 0 . -1 �� �ȴ�.

			//�׷��Ƿ� x = Sinf ���ǰ� z = -Cosf�� �ȴ�. 

			vDirz.x = sinf(fAngle);
			vDirz.z = - cosf(fAngle);
			Normalize(&vDirz,&vDirz);

			VECTOR3 vObjPos;
			pFieldObj->GetPosition( &vObjPos );

			VECTOR3 vCamera2Obj = vObjPos - pCurCamera->m_CameraPos;

			float fDot = vDirz * vCamera2Obj ;

			if( fDot > 0.0f )
			{
				//������(ī�޶�->������Ʈ)�� Obj�� Z���� �������� ����ϰ�쿡 Hide�� �������� 
				pFieldObj->GetEngineObject()->HideWithScheduling() ;
			}
			else
			{
				//�����ϰ�� Show 
				pFieldObj->GetEngineObject()->Show();
			}
		}
	}
}

//090406 pdy �Ͽ�¡ ���� �������μ���
void cHousingMgr::ProcessAlpha()
{
	//�ʵ忡 ��ġ�� �����̿��� ������ ������Ʈ�� ������ 
	//ī�޶� ���⼺�� ������Ų �Ÿ� - ������ �� HERO�� ī�޶��� �Ÿ����� ������ 
	//����äũ�� �����ϰ� XZ���̷� �ٿ�� �ڽ��� �����ϴ��ķ� ���ĸ���Ʈ�� ADD �Ѵ�. 

	if( ! HERO )
		return;

	static DWORD dwLastCheckTime = 0;
	DWORD ElapsedTime = gCurTime - dwLastCheckTime;
	DWORD interval = ElapsedTime/5;

	if( ElapsedTime >= 50 )	//---�ʹ� ���� ȣ�� �Ǵ� ���� ����
	{
		//////////////////////////////////
		//���������� ������Ʈ ��ȸ ���� //
		//////////////////////////////////
		MHCAMERADESC* pCurCamera = CAMERA->GetCameraDesc() ;

		m_FieldFurnitureTable.SetPositionHead();

		for(CFurniture* pFieldObj = m_FieldFurnitureTable.GetData();
			0 < pFieldObj;
			pFieldObj = m_FieldFurnitureTable.GetData())
		{
			DWORD dwIdx = pFieldObj->GetFurnitureInfo()->dwFurnitureIndex;

			if( pFieldObj->GetID() == HERO->GetRideFurnitureID() )	//HERO�� ž���� ������ Alpha�� �ʿ����.
				continue;	

			//090615 pdy �Ͽ�¡ ���Ĺ��� ����
			if( m_bDoDecoration && pFieldObj == m_pDecoObj )	//�������� ������� 
				continue;	

			//������̳� �ܺ��� ��� ���� �ʿ���� 
			if( Get_HighCategory(dwIdx) ==  eHOUSE_HighCategory_Door ||  IsExteriorWall(dwIdx)  ) 
					continue;

			CEngineObject* pEngineObj = pFieldObj->GetEngineObject();

			if( ! pEngineObj)	//�̷���� ����ϴ�..
				continue;

			GXOBJECT_HANDLE GxObjHandle = pEngineObj->GetGXOHandle();

			if( !GxObjHandle )		//����..������ �ϴ� ����ó��
				continue;

			VECTOR3 vObjPos;													
			pFieldObj->GetPosition( &vObjPos );

			VECTOR3 vCamera2Obj = vObjPos - pCurCamera->m_CameraPos;			//ī�޶�->������Ʈ ������ 
			float fCamera2FieldObj = VECTOR3Length(&vCamera2Obj);				//ī�޶�->������Ʈ �Ÿ�

			VECTOR3 vHeroPos;				
			HERO->GetPosition( &vHeroPos );										//�������ġ�� �����´�.
			vHeroPos.y += 100.0f;												//�㸮���������� ���̸� �÷��ش�.

			VECTOR3 vCamera2Hero = vHeroPos - pCurCamera->m_CameraPos;			//ī�޶�->����� ������ 
			float fCamera2HeroLength = VECTOR3Length(&vCamera2Hero);			//ī�޶�->����α����� �Ÿ� 

			COLLISION_MESH_OBJECT_DESC FieldObjDesc;				
			g_pExecutive->GXOGetCollisionMesh(pFieldObj->GetEngineObject()->GetGXOHandle(), &FieldObjDesc);	

			float fFieldObjRadius = FieldObjDesc.boundingSphere.fRs ;			//������Ʈ�� ������ 

			if( fCamera2HeroLength <  ( fCamera2FieldObj - fFieldObjRadius ) )	// (ī�޶�->�����)�Ÿ��� (ī�޶�->FieldObj)-������ ���� ũ�� continue;
				continue;														

			VECTOR3 vDir = vCamera2Hero;										
			Normalize(&vDir,&vDir);

			VECTOR3 vPos = pCurCamera->m_CameraPos;								
			float fDist = 0.0f;
			VECTOR3 vIntetSectPoint = {0};


			if(! IsCollisionMeshAndRay(&vIntetSectPoint,&fDist,&FieldObjDesc,&vPos,&vDir) )	
			{
				continue;														// Ray(ī�޶�->�����)�� ������Ʈ�� �ٿ���ڽ��� ��Ƽ� ������ �ȵȴٸ� continue 
			}
			else if(fDist > fCamera2HeroLength )
			{
				continue;														// �����µǾ����� ����κ��� �ִٸ� continue 
			}


			//���ĸ���Ʈ�� ����

			int setAlpha = g_pExecutive->GetAlphaFlag(GxObjHandle) - (interval*2);
			if( setAlpha < 100 )
			{
				setAlpha = 100;	//--- �ּ� ���ļ�ġ = 0
			}

			g_pExecutive->SetAlphaFlag(GxObjHandle,setAlpha);

			if( m_AlphaHandleList.Find(GxObjHandle) == NULL )	//---����Ʈ�� ���ٸ� ����Ʈ�� �߰�.
			{
				//090615 pdy �Ͽ�¡ ���Ĺ��� ����
				stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo( pFieldObj->GetFurnitureInfo()->dwFurnitureIndex );
				if( stFurnitureInfo )
					g_pExecutive->GXOSetZOrder( pFieldObj->GetEngineObject()->GetGXOHandle() , 8 );			//���ļ��ù����� ������ ���İ� ���� �ڿ��� �׷�����Ѵ�.

				m_AlphaHandleList.AddTail(GxObjHandle);
			}
		}
		/////////////////////////////////////////////////////////////////////////

		//---����Ʈ�� ��� ������Ʈ�� ���ĸ� �����־� ���� ���·� ���ư��� ���ش�.
		//---������ interval*2 �� ���ĸ� �ι�� ���ִ� ������, ���⼭ interval��ŭ �����ֱ� �����̴�.
		PTRLISTPOS pos = m_AlphaHandleList.GetHeadPosition();
		while(pos)
		{
			PTRLISTPOS oldPos = pos;
			GXOBJECT_HANDLE gxHandle = (GXOBJECT_HANDLE)m_AlphaHandleList.GetNext(pos);

			if( gxHandle == NULL )
			{
				m_AlphaHandleList.RemoveAt( oldPos );
				continue;
			}

			if(g_pExecutive->IsValidHandle(gxHandle) == GX_MAP_OBJECT_TYPE_INVALID)
			{
				m_AlphaHandleList.RemoveAt( oldPos );
				continue;
			}

			DWORD curAlpha = g_pExecutive->GetAlphaFlag(gxHandle);
			DWORD setAlpha = curAlpha + interval;
			if( setAlpha >= 255 )
			{
				CEngineObject* pEngineObj = (CEngineObject*)g_pExecutive->GetData( gxHandle );
				CObjectBase* pBaseObj = NULL;

				//090615 pdy �Ͽ�¡ ���Ĺ��� ����
				if( pEngineObj && pEngineObj->GetBaseObject() )
				{
					pBaseObj = pEngineObj->GetBaseObject();
					DWORD dwFunuitureIdx = ((CFurniture*)pBaseObj)->GetFurnitureInfo()->dwFurnitureIndex;

					stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(dwFunuitureIdx);
					if( stFurnitureInfo && stFurnitureInfo->bHasAlpha == FALSE )							//��ü���İ� ���� ������ 
						g_pExecutive->GXOSetZOrder( gxHandle , 0 );											//���İ� �����ٸ� Zorder�� 0����  
				}

				setAlpha = 255;
				m_AlphaHandleList.RemoveAt( oldPos );
			}
			g_pExecutive->SetAlphaFlag( gxHandle, setAlpha );			
		}

		dwLastCheckTime = gCurTime;
	}
}

void cHousingMgr::ProcessWaitForMaikingObjList()
{
	static DWORD dwLastCheckTime = 0;
	DWORD ElapsedTime = gCurTime - dwLastCheckTime;

	if( ElapsedTime < 1000 )	//1�ʿ� �ѹ��� ���� 
		return;

	dwLastCheckTime = gCurTime;

	PTRLISTPOS pos = m_WaitForMakingObjList.GetHeadPosition();
	while( pos )
	{
		CObject* pCurObject= (CObject*)m_WaitForMakingObjList.GetNext( pos ) ;

		if( CanMakingToFiled(pCurObject) )			//�ʵ忡 ��ŷ�� ���� �����ϸ� 
		{
			MarkingObjZone( pCurObject , TRUE ) ;	//�ʵ忡 ��ŷ�� �ϰ�
			RemoveWaitForMakingObj(pCurObject);		//����Ʈ���� �����ش� 
		}								
	}
}

void cHousingMgr::ProcessRemainTime()
{
	//���� ���� �ð� ���� ��Ű��

	if( m_dwLastRemainProcessTime > gCurTime )
		m_dwLastRemainProcessTime = gCurTime;

	DWORD dwDeltaSecond = (gCurTime - m_dwLastRemainProcessTime) /1000;

	if( dwDeltaSecond < 60 )
		return;

	m_dwLastRemainProcessTime = gCurTime;

	for(int i = 0 ; i<MAX_HOUSING_CATEGORY_NUM; i++)			//��� ������������Ʈ�� ���鼭 �ð����� 60�ʾ� ���� ��Ų�� 
	{
		if( m_CurHouse.m_dwCategoryNum[i] == 0)
			continue;

		for( int j = 0 ; j < MAX_HOUSING_SLOT_NUM ; j++ )
		{
			DWORD dwObjectIndex = m_CurHouse.m_dwFurnitureList[i][j];

			if( dwObjectIndex )
			{
				stFurniture* pstFurniture =  m_CurHouse.pFurnitureList.GetData(dwObjectIndex);

				if( !pstFurniture )
					continue;

				if( pstFurniture->wState == eHOUSEFURNITURE_STATE_KEEP )	//�������ΰ����� �ð��� ���� ���� �ʴ´�. ( �ѹ��� �ν��� ���� �������� )
					continue;

				if( pstFurniture->dwRemainTime < dwDeltaSecond )
				{
					pstFurniture->dwRemainTime = 0;
				}
				else 
				{
					pstFurniture->dwRemainTime -= dwDeltaSecond;
				}
			}
		}
	}
}

//�ٹ̱� ����϶��� ���콺 �̺�Ʈ�� �Ŵ������� ó������
void cHousingMgr::ProcessMouseEvent_DecorationMode(CMouse* Mouse,DWORD MouseEvent)
{
	if( ! m_bDecorationMode )
		return;

	//��ġ ���� ���϶�  
	if( m_bDecoMove )
	{
		if( MouseEvent == MOUSEEVENT_NONE )
		{
			//���콺�̺�Ʈ�� ������ ��ŷ���������� 
			MoveDecorationMousePoint(Mouse);
			CURSOR->SetCursor( eCURSOR_DEFAULT );
		}
		else if(  MouseEvent & MOUSEEVENT_LCLICK )
		{
			if ( CanInstallFunitureToField() )
			{
				//��ġ������ ��Ȳ ó��
				EndDecoMove();
				StartDecoAngle();
			}
			else
			{
				//��ġ�Ұ����� ��Ȳó��
				CURSOR->SetCursor(eCURSOR_HOUSING_CANNOT_INSTALL);

				//090527 pdy �Ͽ�¡ �ý��۸޼��� [��ġ �Ұ���]
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1860 ) );	//1860	"�̰��� �ش� ������ ��ġ �� �� �����ϴ�."
			}
		}
	}
	else if( MouseEvent & MOUSEEVENT_LCLICK ) 	//��ġ ���� ���� �ƴҶ� 
	{
		if( ! m_bDoDecoration )					//������Ʈ �������� �ƴ϶�� 
		{
			PICK_GXOBJECT_DESC gxoDesc[5] = {0,};
			GetSelectedObject( gxoDesc, Mouse->GetMouseX(), Mouse->GetMouseY(), PICK_TYPE_PER_FACE , TRUE );

			CObject* pPickObject	= NULL ;
			m_PickAnotherObjHandle  = NULL ;

			for( int i=0; i < 5 ; i++ )
			{
				if(! gxoDesc[i].gxo || ! (g_pExecutive->GetData(gxoDesc[i].gxo) ) )
					continue;

				CObjectBase* pBaseObj = ( (CEngineObject*)g_pExecutive->GetData(gxoDesc[i].gxo) )->GetBaseObject();
				if( ! pBaseObj || pBaseObj->GetEngineObjectType() != eEngineObjectType_Furniture )	
				{
					//������ �ƴϸ� ��ŷ���� �ʰ�..
					gxoDesc[i].gxo = NULL;
					continue;
				}

				if( ! pPickObject )					
				{
					pPickObject = (CObject*) pBaseObj;
				}
				else if( ! m_PickAnotherObjHandle )
				{
					m_PickAnotherObjHandle = gxoDesc[i].gxo;
					break;
				}
			}

			//���� ����� ������Ʈ�� �����´� 
			//pPickObject = (CObject*)GetSelectedObject( Mouse->GetMouseX(), Mouse->GetMouseY(), PICK_TYPE_PER_FACE , /*bSort = TRUE */ TRUE );
			if( pPickObject )
			{
				//091214 pdy �ٹ̱� ���� �ٴڿ� Ŭ������ ��쿡 �̵��Ұ� �޼��� ����.
				if( IsFloor ( ((CFurniture*) pPickObject)->GetFurnitureInfo()->dwFurnitureIndex ) )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2007 ) );
				}

				//�ƴϸ� ���������� ���� 
				if( FALSE == StartDecoReset(pPickObject) )
				{
					//������ �ȵǴ� ���� ó�� 
				}
			}
			else
			{
				// ��ŷ�� �ȵǾ��ٸ� �ܺο� Ŭ���� ���� �̵��Ұ� �޼��� ����.
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2007 ) );
			}
		}
	}
}

//�Ⱦ��� Obj �̵�ó�� 
void cHousingMgr::MoveDecorationMousePoint(CMouse* Mouse)
{
	if( !m_pDecoObj )	//�Ⱦ��ε� ���ڰ� ����?
			return;

	PICK_GXOBJECT_DESC gxoDesc[5] = {0,};
	GetSelectedObject( gxoDesc, Mouse->GetMouseX(), Mouse->GetMouseY(), PICK_TYPE_PER_FACE );

	//���� ����� �ﰢ���� ���Ѵ�.
	float fDist = 100000.0f;	//�ӽ÷� ���� �Ÿ�100000.0f�� �������� ������..
	int	  nIndex = 0;
	m_PickAnotherObjHandle = NULL ;
	DWORD Count = 0;

	for(int i=0;i<5;i++)
	{
		if(! gxoDesc[i].gxo || 
			gxoDesc[i].gxo == m_pDecoObj->GetEngineObject()->GetGXOHandle() ||		//��ġ Obj�� �����Ѵ�
			! (g_pExecutive->GetData(gxoDesc[i].gxo) ) )
		{
			//�ʿ�����Ʈ�� ����������Ʈ�� ���� �ɷ�����..
			gxoDesc[i].gxo = NULL;
			continue;
		}

		CObjectBase* pBaseObj = ( (CEngineObject*)g_pExecutive->GetData(gxoDesc[i].gxo) )->GetBaseObject();
		if( ! pBaseObj || pBaseObj->GetEngineObjectType() != eEngineObjectType_Furniture )	
		{
			//������ �ƴϸ� ��ŷ���� �ʰ�..
			gxoDesc[i].gxo = NULL;
			continue;
		}

		if(fDist > gxoDesc[i].fDist)
		{
			fDist = gxoDesc[i].fDist;
			m_PickAnotherObjHandle = gxoDesc[i].gxo;	
			nIndex = i;
			Count++;
		}
	}

	DWORD dwDecoIdx = m_pDecoObj->GetFurnitureInfo()->dwFurnitureIndex;

	if(  Get_HighCategory(dwDecoIdx) ==  eHOUSE_HighCategory_Door ) 
	{
		//����Ŀ��ϰ�� 
		if( m_PickAnotherObjHandle )
		{
			float fDist = 100000.0f;
			int	  nIndex = 0;

			CEngineObject* pPickWallObj = NULL;
			m_PickAnotherObjHandle = NULL;
			for(int i=0;i<5;i++)
			{
				if(! gxoDesc[i].gxo  ) 
					continue;

				//���� ��ŷó�� �������� �̹� ����ó�� ������ ����ó���ʿ����.. 
				CEngineObject* pCurEngineObj = (CEngineObject*)g_pExecutive->GetData(gxoDesc[i].gxo);
				CObjectBase* pBaseObj = pCurEngineObj->GetBaseObject();

				DWORD dwIdx = ( (CFurniture*)pBaseObj )->GetFurnitureInfo()->dwFurnitureIndex;

				if( IsExteriorWall(dwIdx) )
				{
					if(fDist > gxoDesc[i].fDist)
					{
						fDist = gxoDesc[i].fDist;
						m_PickAnotherObjHandle = gxoDesc[i].gxo;	
						pPickWallObj = pCurEngineObj;
						nIndex = i;
					}
				}
			}

			//������ ��ŷ�Ǿ��ٸ� ���� ȸ������ ����������� 
			if( pPickWallObj )
			{
				VECTOR3 vIntersectPos;
				VECTOR3 vColTri[3];
				POINT pt = {Mouse->GetMouseX(), Mouse->GetMouseY()};

				if(g_pExecutive->GXOIsCollisionBoungingBoxWithScreenCoord(m_PickAnotherObjHandle, &vIntersectPos, &vColTri[0], &pt, 0))
				{
					VECTOR3 edge1,edge2;
					VECTOR3_SUB_VECTOR3(&edge1, &vColTri[1], &vColTri[0]);
					VECTOR3_SUB_VECTOR3(&edge2, &vColTri[2], &vColTri[0]);

					VECTOR3 normal;
					CrossProduct(&normal, &edge1, &edge2 );
					Normalize(&normal, &normal);

					if( normal.y == 0 )
					{
						float fAngle = pPickWallObj->GetEngObjAngle();
						m_vDecoPos = gxoDesc[nIndex].v3IntersectPoint;
						m_pDecoObj->SetAngle(fAngle);
						m_pDecoObj->GetEngineObject()->ApplyHeightField(FALSE);
						m_pDecoObj->SetPosition(&m_vDecoPos);
						m_pDecoObj->GetEngineObject()->ApplyHeightField(TRUE);
					}

				}
			}
		}
	}
	else if( m_PickAnotherObjHandle )	
	{
		//������� �ƴ� �⺻ ������Ʈ 
		//���� ��ŷó�� �������� �̹� ����ó�� ������ ���⼱ ����ó���ʿ����.. 
		CEngineObject* pCurEngineObj = (CEngineObject*)g_pExecutive->GetData(m_PickAnotherObjHandle);
		CObjectBase* pBaseObj = pCurEngineObj->GetBaseObject();

		DWORD dwIdx = ( (CFurniture*)pBaseObj )->GetFurnitureInfo()->dwFurnitureIndex;

		//090603 pdy �Ͽ�¡ �⺻������Ʈ ��ŷ�̵��� ��ŷ�� ������Ʈ�� ����� or �ܺ��� �ƴҶ��� �̵�����
		if(! IsExteriorWall(dwIdx) &&  Get_HighCategory(dwIdx) !=  eHOUSE_HighCategory_Door )		
		{
			m_vDecoPos = gxoDesc[nIndex].v3IntersectPoint;

			if( IsFloor(dwIdx) )			//�ٴ��ϰ�� �ʵ��� ���̷� ������ 
			{
				float fFieldHeight = 0.0f;
				g_pExecutive->GXMGetHFieldHeight(&fFieldHeight,m_vDecoPos.x,m_vDecoPos.z);
				m_vDecoPos.y = fFieldHeight;
			}
			else
			{
				m_vDecoPos.y+= 0.1f;			//�ٴ��� �ƴҰ�� 0.1 �����÷����� 
			}

			m_pDecoObj->GetEngineObject()->ApplyHeightField(FALSE);
			m_pDecoObj->SetPosition(&m_vDecoPos);
			m_pDecoObj->GetEngineObject()->ApplyHeightField(TRUE);
		}
	}
	else
	{
		//��ŷ�� ���� �ڵ��� ���ٸ� �Ͽ콺 ���̹Ƿ� �̵���Ű�� �ʴ´�.
	}
}

BOOL cHousingMgr::RequestInstallCurDeco()
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	//�ʵ忡�� ���� ���õ� ������ �߰��ϵ��� ��û
	if( !m_pDecoObj )
		return FALSE;

	if(! CanInstallFunitureToField() )
	{
		//��ġ �Ұ����� ��Ȳ ó��
		//090527 pdy �Ͽ�¡ �ý��۸޼��� [��ġ �Ұ���]
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1860 ) );	//1860	"�̰��� �ش� ������ ��ġ �� �� �����ϴ�."
		return FALSE;
	}

	stFurniture* pstFuniture = m_pDecoObj->GetFurnitureInfo();


	WORD wCategory = Get_HighCategory(pstFuniture->dwFurnitureIndex);
	WORD wSlot = pstFuniture->wSlot;
	DWORD dwObjectIdx = m_CurHouse.m_dwFurnitureList[wCategory][wSlot];

	MSG_HOUSE_FURNITURE_INSTALL msg;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category	    = MP_HOUSE;
	msg.Protocol		= MP_HOUSE_INSTALL_SYN;
	msg.dwObjectID		= HEROID;
	msg.dwChannel		= m_CurHouse.dwChannelID;
	msg.dwFurnitureObjectIndex = dwObjectIdx;
	msg.fAngle			= m_pDecoObj->GetAngle();
	msg.vPos			= m_vDecoPos;
	msg.wSlot			= wSlot;

	NETWORK->Send(&msg,sizeof(msg));

	EndDecoAngle();
	return TRUE;
}

BOOL cHousingMgr::RequestUninstallCurDeco()
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	//�ʵ忡�� ���� ���õ� ������ �����ϵ��� ��û
	if( !m_pDecoObj )
		return FALSE;

	//090611 pdy �Ͽ�¡ �⺻ǰ��� ������ ��� ��ġ���� �Ұ��� 
	if( m_pDecoObj->GetFurnitureInfo()->bNotDelete || IsDoor( m_pDecoObj->GetFurnitureInfo()->dwFurnitureIndex ) )	
	{
		//090527 pdy �Ͽ�¡ �ý��۸޼��� �⺻�������� [��ġ����]
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1888 ) );	//1888	"�⺻ ��ǰ�� ��ġ���� �� �� �����ϴ�."
		return FALSE;
	}

	if( m_pDecoObj->GetID() == HOUSE_DUMMYID  )
	{
		CancelDecoration();
		return TRUE;
	}

	//dwData1(ä��) dwData2(����ObjectIndex)
	MSG_DWORD2 msg ;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category	    = MP_HOUSE;
	msg.Protocol		= MP_HOUSE_UNINSTALL_SYN;
	msg.dwObjectID		= HEROID;
	msg.dwData1			= m_CurHouse.dwChannelID;
	msg.dwData2			= m_pDecoObj->GetFurnitureInfo()->dwObjectIndex;

	NETWORK->Send(&msg,sizeof(msg));

	EndDecoAngle();
	return TRUE;
}

//�ʵ���� ������Ʈ�� �缳���Ҷ� ��û�Ѵ�.
BOOL cHousingMgr::RequestUpdateCurDeco()
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	if( !m_pDecoObj )
		return FALSE;

	//�ٹ̱��� ���ð� ���缳���� ������ ���� ������ ������Ʈ��û ����   
	if( fabs(m_fBackupDecoAngle - m_pDecoObj->GetAngle() ) < 0.001f && 
		fabs(m_vDecoPos.x - m_vBackupDecoPos.x ) < 0.001f &&
		fabs(m_vDecoPos.y - m_vBackupDecoPos.y ) < 0.001f &&
		fabs(m_vDecoPos.z - m_vBackupDecoPos.z ) < 0.001f		)
	{
		//090605 pdy ��ŷ���� ���� 
		//���������� �������.
		CancelDecoration();
		return FALSE;
	}

	if(! CanInstallFunitureToField() )
	{
		//��ġ �Ұ����� �����̹Ƿ� ������Ʈ ��û���� 
		//090527 pdy �Ͽ�¡ �ý��۸޼��� [��ġ �Ұ���]
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1860 ) );	//1860	"�̰��� �ش� ������ ��ġ �� �� �����ϴ�."
		return FALSE;
	}

	stFurniture* pstFuniture = m_pDecoObj->GetFurnitureInfo();
	WORD wCategory = Get_HighCategory(pstFuniture->dwFurnitureIndex);
	WORD wSlot = pstFuniture->wSlot;
	DWORD dwObjectIdx = m_CurHouse.m_dwFurnitureList[wCategory][wSlot];

	MSG_HOUSE_FURNITURE_INSTALL msg;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category	    = MP_HOUSE;
	msg.Protocol		= MP_HOUSE_UPDATE_SYN;
	msg.dwObjectID		= HEROID;
	msg.dwChannel		= m_CurHouse.dwChannelID;
	msg.dwFurnitureObjectIndex = dwObjectIdx;
	msg.fAngle			= m_pDecoObj->GetAngle();
	msg.vPos			= m_vDecoPos;
	msg.wSlot			= wSlot;

	NETWORK->Send(&msg,sizeof(msg));

	EndDecoAngle();

	return TRUE;
}


//������Ʈ�ٹ̱����� �ƴϸ鼭 ������Ŭ������ 
//��ġ ���� ��û���Ҷ� �۴��������� �ٷ� ��û����
BOOL cHousingMgr::RequestUninstallDecoFromUseIcon(stFurniture* pstFurniture)
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	if( !pstFurniture || m_bDoDecoration ) 	//���� ������Ʈ ��ġ ���̸� �ȵȴ�.
		return FALSE;

	if( pstFurniture->wState != eHOUSEFURNITURE_STATE_INSTALL )	//��ġ���� �ƴ� ������ �ȵȴ�.
		return FALSE;

	
	//dwData1(ä��) dwData2(����ObjectIndex)
	MSG_DWORD2 msg ;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category	    = MP_HOUSE;
	msg.Protocol		= MP_HOUSE_UNINSTALL_SYN;
	msg.dwObjectID		= HEROID;
	msg.dwData1			= m_CurHouse.dwChannelID;
	msg.dwData2			= pstFurniture->dwObjectIndex;

	NETWORK->Send(&msg,sizeof(msg));

	return TRUE;
}

//090331 pdy Box To Box äũ 
BOOL cHousingMgr::TestCollisionBox2Box(BOUNDING_BOX* pSrcBoundingBox, BOUNDING_BOX* pDestBoundingBox , BOOL bCheckHeight)
{
	// ������ ���̱����� ���� OBB�� �ƴ� ������ ���̿� �ڽ��� 2D������ äũ�� �ϴ� ������ �浹äũ   
	// �䱸����. BOUNDINGBOX�� ȸ������ Y�ุ�ִٰ� �����Ѵ�. ( ����üũ�� �����ϰ� �ϱ�����..)

	//�ٿ���ڽ� 8�� �ε��� 
	//������
	//	[0] [3]   
	//	[4] [7]

	//�Ʒ��� 
	//	[1]	[2]	  
	//	[5] [6]

	if( bCheckHeight )
	{
		//������ ����äũ
		//Src�� ���� ������ Dest�� ���� ������ ���Ե��� �ʴ°�� �浹 = FALSE 
		if( pSrcBoundingBox->v3Oct[4].y < pDestBoundingBox->v3Oct[5].y	||		// Src.MAXY < Dest.MINY || 
			pSrcBoundingBox->v3Oct[5].y > pDestBoundingBox->v3Oct[4].y	 )		// Src.MINY > Dest.MAXY
			return FALSE;
	}

	//2D ������ äũ 
	//2D �簢���� ������ X�� Z���� �������� �Ǹ�  
	//�� �簢���� �� ������ �����࿡ ������������ 
	//Min Max������ ���� ��ġ�� ������ �������� �����Ͽ� 
	//�� �簢���� �������� �ʴ�.

	VECTOR3 vAxis[4] = {0,};

	vAxis[0] = pSrcBoundingBox->v3Oct[1] - pSrcBoundingBox->v3Oct[5];
	vAxis[1] = pSrcBoundingBox->v3Oct[6] - pSrcBoundingBox->v3Oct[5];

	vAxis[2] = pDestBoundingBox->v3Oct[1] - pDestBoundingBox->v3Oct[5];
	vAxis[3] = pDestBoundingBox->v3Oct[6] - pDestBoundingBox->v3Oct[5];

	for( int i=0; i<4 ; i++)
		Normalize( &vAxis[i] , &vAxis[i] );


	DWORD dwVertexIndexArr[4] = {1,2,5,6};	//2D�̹Ƿ� �Ʒ����� ���ý��� �ε����� ��Ƶ���

	for( int i=0 ; i<4; i++)
	{
		float fSrcMin,fDestMin; 
		float fSrcMax,fDestMax;

		fSrcMin = fDestMin =  1000000.0f;
		fSrcMax = fDestMax = -1000000.0f;

		for( int j=0; j<4; j++)
		{
			float fSrcDotLength = 0 , fDestDotLength = 0;

			fSrcDotLength  =  vAxis[i] * pSrcBoundingBox->v3Oct[dwVertexIndexArr[j]];
			fDestDotLength =  vAxis[i] * pDestBoundingBox->v3Oct[dwVertexIndexArr[j]]; 

			if( fSrcDotLength < fSrcMin )
				fSrcMin = fSrcDotLength;

			if( fSrcDotLength > fSrcMax )
				fSrcMax = fSrcDotLength;

			if( fDestDotLength < fDestMin )
				fDestMin = fDestDotLength;

			if( fDestDotLength > fDestMax )
				fDestMax = fDestDotLength;
		}

		float fabsMinValue = (fSrcMin < fDestMin )? fabs(fSrcMin)+1.0f : fabs(fDestMin)+1.0f;

		//Src �� Dest�� �������� Min Max���� ���� ��ġ�� �ʴٸ� ������ ���� 
		if( (fSrcMax + fabsMinValue)  <  (fDestMin + fabsMinValue ) || (fSrcMin + fabsMinValue) > ( fDestMax + fabsMinValue) )
		{
			//�������� �����ϹǷ� �浹���� �ʾҴ�.
			return FALSE;	
		}
	}
	//�������� �������� �ʴٸ� �ιڽ��� �����ִ�. 
	return TRUE;
}

//�κ�Item->â��Icon
BOOL cHousingMgr::RequestStoredItem(POSTYPE ToPos, CItem * pFromItem)
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	pFromItem->SetLock(TRUE);

	//dwData1(ä��) dwData2(������Index) dwData3(������Slot) dwData4(����â��Slot)
	MSG_DWORD4 msg ;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category = MP_HOUSE;
	msg.Protocol = MP_HOUSE_STORED_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1		= m_CurHouse.dwChannelID;
	msg.dwData2		= pFromItem->GetItemIdx();
	msg.dwData3		= pFromItem->GetPosition();
	msg.dwData4		= ToPos;

	NETWORK->Send(&msg,sizeof(msg));


	return TRUE;
}

//â��Icon->�κ�Item
BOOL cHousingMgr::RequestRestoredICon(POSTYPE ToPos, cHousingStoredIcon * pFromIcon)
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	pFromIcon->SetLock(TRUE);

	//dwData1(ä��) dwData2(����ObjectIndex) dwData3(����â��Slot)
	MSG_DWORD3 msg;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category = MP_HOUSE;
	msg.Protocol = MP_HOUSE_RESTORED_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1		= m_CurHouse.dwChannelID; 
	msg.dwData2		= pFromIcon->GetLinkFurniture()->dwObjectIndex;
	msg.dwData3		= pFromIcon->GetLinkFurniture()->wSlot;

	NETWORK->Send(&msg,sizeof(msg));

	return TRUE;
}

void cHousingMgr::RequestDecoRationMode()
{
	if(! IsHouseInfoLoadOk() )
		return;

	//�ٹ̱� ��� ���ױ�� �ٷ� ��û 
	//dwData1(ä��), dwData2(On/Off)
	MSG_DWORD2 msg;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category	    = MP_HOUSE;
	msg.Protocol		= MP_HOUSE_DECOMODE_SYN;
	msg.dwData1			= m_CurHouse.dwChannelID;
	msg.dwData2			= (!m_bDecorationMode);

	NETWORK->Send(&msg,sizeof(msg));
}

void cHousingMgr::RequestDestroyFuniture(stFurniture* pFuniture)
{
	if(! IsHouseInfoLoadOk() )
		return;

	if( ! pFuniture )
		return;

	// dwData1(ä��), dwData2(����Index)
	MSG_DWORD2 msg;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category	    = MP_HOUSE;
	msg.Protocol		= MP_HOUSE_DESTROY_SYN;
	msg.dwData1			= m_CurHouse.dwChannelID;
	msg.dwData2			= pFuniture->dwObjectIndex;

	NETWORK->Send(&msg,sizeof(msg));
}

BOOL cHousingMgr::FakeUseAction(stFurniture* pFuniture,stHouseActionInfo* pActionInfo)
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	if( ! pFuniture || ! pActionInfo )
		return FALSE;

	//090618 pdy �׼� ���ѻ��� ���� ����� ���°� NONE�̾ƴҶ� ������,â���⸦ ������ �׼� ���Ұ�.
	if( HERO->GetState() != eObjectState_None &&
		pActionInfo->dwActionType != eHOUSE_ACTION_TYPE_GETOFF &&
		pActionInfo->dwActionType != eHOUSE_ACTION_TYPE_STORAGE	)		
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(789) ) ;
		return FALSE;
	}

	//���� ������ �׼��� �Ÿ�üũ�� �Ͽ��� �Ѵ�.
	//090618 pdy �Ͽ�¡ ����߰� â���� �׼ǵ� �Ÿ�üũ���� ����
	if( ! IsDoor( pFuniture->dwFurnitureIndex ) &&
		pActionInfo->dwActionType != eHOUSE_ACTION_TYPE_STORAGE &&
		pActionInfo->dwActionType != eHOUSE_ACTION_TYPE_OPEN_HOMEPAGE )
	{
		VECTOR3 vHeroPos = {0,};
		HERO->GetPosition(&vHeroPos);

		float fLength = CalcDistanceXZ(&vHeroPos, &pFuniture->vPosition ) ;

		//090527 pdy �Ͽ�¡ �ý��۸޼��� �׼� �Ÿ��� �ֶ� ���ѻ��� [�׼�]
		if( fLength > MAX_HOUSE_ACTION_DISTANCE )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1900 ) );	//1900	"����Ͻ÷��� ��ó�� ���ž��մϴ�."
			return FALSE;
		}
	}

	// ��ġ����Ʈ�� ������ ����
	if(!m_FieldFurnitureTable.GetData(pFuniture->dwObjectIndex))			
		return FALSE;

	m_CurActionInfo.Clear();

	m_CurActionInfo.pActionFuniture = pFuniture;
	m_CurActionInfo.pActionInfo = pActionInfo;

	switch(pActionInfo->dwActionType)
	{
	case eHOUSE_ACTION_TYPE_DOOR_EXITHOUSE :
		{
			//090527 pdy �Ͽ�¡ �˾�â [�Ͽ콺������]
			WINDOWMGR->MsgBox( MBI_HOUSE_EXIT_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1894)) ; //1894	"������ �����ðڽ��ϱ�?"
			GAMEIN->GetHousingActionPopupMenuDlg()->SetDisable(TRUE);

			//090618 pdy �Ͽ�¡ ������ �׼� ���� ���� 
			OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal) ;
			return FALSE; 
		}
		break;
	case eHOUSE_ACTION_TYPE_DOOR_VOTEHOUSE :
		{
			//090527 pdy �Ͽ�¡ �˾�â [�Ͽ콺��õ�ϱ�]
			WINDOWMGR->MsgBox( MBI_HOUSE_VOTEHOUSE_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1895)) ; //1895	"�� ���� ��õ �Ͻðڽ��ϱ�?"
			GAMEIN->GetHousingActionPopupMenuDlg()->SetDisable(TRUE);
			return FALSE; 
		}
		break;
	}
	//��Ŷ�� ������ 

	return RequestUseCurAction();
}

BOOL cHousingMgr::RequestUseCurAction()
{
	if( ! m_CurActionInfo.pActionFuniture || !m_CurActionInfo.pActionInfo )
		return FALSE;

	switch(m_CurActionInfo.pActionInfo->dwActionType)
	{
		case eHOUSE_ACTION_TYPE_RIDE:
			{
				DWORD* pSortedNearBoneNumArr = GAMEIN->GetHousingActionPopupMenuDlg()->GetSortedNearBoneNumArr();
				stFunitureInfo* pstFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(m_CurActionInfo.pActionFuniture->dwFurnitureIndex);
				if( ! pstFurnitureInfo )
					return FALSE;
				//dwData1(ä��), dwData2(����Index), dwData3(Attach����), dwData4(�׼�Type)

				int nAttachSlot = -1;
				for( int i=0; i < pstFurnitureInfo->byAttachBoneNum ; i++)
				{
					if( m_CurActionInfo.pActionFuniture->dwRidingPlayer[i] == 0 )
					{
						nAttachSlot = pSortedNearBoneNumArr[i] -1;
						break;
					}
				}

				//090527 pdy �Ͽ�¡ �ý��۸޼��� ž�½� ���ѻ��� [���ڸ��� ������]
				if( -1 == nAttachSlot )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1877 ) ); //1877	"�ο��� ������ ž�� �Ͻ� �� �����ϴ�."
					return FALSE;
				}

				MSG_DWORD4 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ACTION_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData1 = m_CurHouse.dwChannelID;
				msg.dwData2 = m_CurActionInfo.pActionFuniture->dwObjectIndex;  
				msg.dwData3 = nAttachSlot;
				msg.dwData4 = m_CurActionInfo.pActionInfo->dwActionIndex;//m_CurActionInfo.pActionInfo->dwActionType;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
		case eHOUSE_ACTION_TYPE_GETOFF : 
			{
				stFunitureInfo* pstFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(m_CurActionInfo.pActionFuniture->dwFurnitureIndex);

				if( ! pstFurnitureInfo )
					return FALSE;

				int nAttachSlot = -1;
				for( int i=0; i < pstFurnitureInfo->byAttachBoneNum ; i++)
				{
					if( m_CurActionInfo.pActionFuniture->dwRidingPlayer[i] ==  gHeroID )
					{
						nAttachSlot = i;
						break;
					}
				}

				if( -1 == nAttachSlot )
				{
					return FALSE;
				}

				//dwData1(ä��), dwData2(����Index), dwData3(Attach����), dwData4(�׼�Index)
				MSG_DWORD4 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ACTION_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData1 = m_CurHouse.dwChannelID;
				msg.dwData2 = m_CurActionInfo.pActionFuniture->dwObjectIndex;  
				msg.dwData3 = nAttachSlot;
				msg.dwData4 = m_CurActionInfo.pActionInfo->dwActionIndex;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
		case eHOUSE_ACTION_TYPE_NORMAL : 
			{
				MSG_DWORD4 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ACTION_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData1 = m_CurHouse.dwChannelID;
				msg.dwData2 = m_CurActionInfo.pActionFuniture->dwObjectIndex;  
				msg.dwData3 = 0;
				msg.dwData4 = m_CurActionInfo.pActionInfo->dwActionIndex;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
		case eHOUSE_ACTION_TYPE_BUFF :
			{
				MSG_DWORD4 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ACTION_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData1 = m_CurHouse.dwChannelID;
				msg.dwData2 = m_CurActionInfo.pActionFuniture->dwObjectIndex;  
				msg.dwData3 = 0;
				msg.dwData4 = m_CurActionInfo.pActionInfo->dwActionIndex;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
		case eHOUSE_ACTION_TYPE_DOOR_EXITHOUSE :	
			{
				MSG_DWORD msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_EXIT_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData = m_CurHouse.dwChannelID;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
		case eHOUSE_ACTION_TYPE_DOOR_VOTEHOUSE :
			{
				MSG_DWORD msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_VOTE_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData = m_CurHouse.dwChannelID;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
		//090616 pdy �Ͽ�¡ ����߰� [â���� �����׼� �߰�]
		case eHOUSE_ACTION_TYPE_STORAGE:
			{
				if( ! HOUSINGMGR->IsHouseOwner() )
				{
					//090616 pdy �Ͽ�¡ �ý��۸޼��� �������� �ƴҽ� ���ѻ��� [â���� �׼�]
					CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1887) ); //1887	"�������� �ƴҽ� ����Ͻ� �� �����ϴ�"
					return FALSE;
				}

				//â���� ��� 
				cDialog* storageDialog = WINDOWMGR->GetWindowForID( PYO_STORAGEDLG );
	
				if( ! storageDialog )
				{
					return FALSE;
				}
				else if( storageDialog->IsActive() )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1552 ) );
					return FALSE;
				}
				else if( ! HERO->GetStorageNum() )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1551 ) );
					return FALSE;
				}
				// 090403 ONS ����,����,��ȭ,��æƮ ���̾�αװ� Ȱ��ȭ �Ǿ� �ִ� ���¿��� â���ȯ ������ ��� �Ұ�
				else
				{
					cDialog* pMixDialog				= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );			// ����
					cDialog* pDissolutioniDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );	// ����
					cDialog* pReinforceDialog		= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );	// ��ȭ
					cDialog* pUpgradeDialog			= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );		//��þƮ

					if( !pMixDialog				|| 
						!pDissolutioniDialog	|| 
						!pReinforceDialog		|| 
						!pUpgradeDialog	) 
					{
							return FALSE;
					}

					if(	pMixDialog->IsActive()			|| 
						pDissolutioniDialog->IsActive() || 
						pReinforceDialog->IsActive()	|| 
						pUpgradeDialog->IsActive()		)
					{
						CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1080 ) );
						return FALSE;
					}
				}

				//��Ŷ�� ������.
				MSG_DWORD4 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ACTION_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData1 = m_CurHouse.dwChannelID;
				msg.dwData2 = m_CurActionInfo.pActionFuniture->dwObjectIndex;  
				msg.dwData3 = 0;
				msg.dwData4 = m_CurActionInfo.pActionInfo->dwActionIndex;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
		// 091105 pdy �Ͽ�¡ ���� �׼��߰� ( �̴�Ȩ�� �� ������ ��ũ ) 
		case eHOUSE_ACTION_TYPE_OPEN_HOMEPAGE:
			{
				MSG_DWORD4 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ACTION_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData1 = m_CurHouse.dwChannelID;
				msg.dwData2 = m_CurActionInfo.pActionFuniture->dwObjectIndex;  
				msg.dwData3 = 0;
				msg.dwData4 = m_CurActionInfo.pActionInfo->dwActionIndex;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
	}

	return TRUE;
}

BOOL cHousingMgr::RequestAction_GetOff()
{
	stFunitureInfo* pstFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(m_CurActionInfo.pActionFuniture->dwFurnitureIndex);

	if( ! pstFurnitureInfo )
		return FALSE;

	int nAttachSlot = -1;
	for( int i=0; i < pstFurnitureInfo->byAttachBoneNum ; i++)
	{
		if( m_CurActionInfo.pActionFuniture->dwRidingPlayer[i] ==  gHeroID )
		{
			nAttachSlot = i;
			break;
		}
	}

	if( -1 == nAttachSlot )
	{
		return FALSE;
	}

	// ����̵��� �ؾ��ϴ� ����ޱ����� ���� ���� Ǯ������.
	CheckAllMarkingObjByHeroZone();

	//dwData1(ä��), dwData2(����Index), dwData3(Attach����), dwData4(�׼�Index)
	MSG_DWORD4 msg;
	msg.Category = MP_HOUSE;
	msg.Protocol = MP_HOUSE_ACTION_FORCE_GETOFF_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1 = m_CurHouse.dwChannelID;
	msg.dwData2 = m_CurActionInfo.pActionFuniture->dwObjectIndex;  
	msg.dwData3 = nAttachSlot;
	msg.dwData4 = HOUSE_ACTIONINDEX_GETOFF;
	NETWORK->Send(&msg,sizeof(msg));

	return TRUE;
}

BOOL cHousingMgr::RequestUseDecoBonus(DWORD dwBonusIndex)
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	stHouseBonusInfo* pBonusInfo = GAMERESRCMNGR->GetHouseBonusInfo(dwBonusIndex);

	if( !pBonusInfo )
		return FALSE;

	if(! HOUSINGMGR->CanUseDecoBonus(pBonusInfo) )
		return FALSE;

	//dwData1(ä��), dwData2(���ʽ�index)
	MSG_DWORD2 msg;
	msg.Category = MP_HOUSE;
	msg.Protocol = MP_HOUSE_BONUS_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1 = m_CurHouse.dwChannelID;
	msg.dwData2 = dwBonusIndex;
	NETWORK->Send(&msg,sizeof(msg));

	return TRUE;
}

void cHousingMgr::StartHouseInfoLoading()
{
	m_dwHouseLoadState = eHSLoad_NOWLOADING;

	// ���� ���� â ��Ȱ��ȭ.
	if( GAMEIN->GetMonstermeterDlg() )
		GAMEIN->GetMonstermeterDlg()->SetActive(FALSE) ;
}

//�������� NACK �޼����� ������� ȣ��ȴ�
void cHousingMgr::PrintDebugErrorMsg(char* pstrMsg,WORD dwError)
{
	//GMTOOL ������ �������� ������ ��������� ǥ�����ش�.
#if defined(_GMTOOL_)	
	if( pstrMsg == NULL )
		return ;

	char MsgBuf[256] ={0,};
	CHATMGR->AddMsg( CTC_SYSMSG,"HousingMgr::PrintDebugErrorMsg [ONLY GMTOOL]");

	switch(dwError)
	{
		case eHOUSEERR_NOOWNER:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NOOWNER");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_NOHOUSE:	
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NOHOUSE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_HAVEHOUSE:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_HAVEHOUSE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_ENTRANCEFAIL:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_ENTRANCEFAIL");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_LOADING:	
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_LOADING");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_FULL:	
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_FULL");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_DONOT_HOUSE:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_DONOT_HOUSE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_DO_HOUSE:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_DO_HOUSE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_NOTOWNER:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NOTOWNER");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_MAX_EXTEND:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_MAX_EXTEND");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_DECOMODE:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_DECOMODE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_NOTENOUGHPOINT:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NOTENOUGHPOINT");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_INVAILDSTATE:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_INVAILDSTATE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_NOFURNITURE:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NOFURNITURE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_HASRIDER:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_HASRIDER");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_NOTRIDING:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NOTRIDING");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_RIDING:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_RIDING");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_RESERVATING:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_RESERVATING");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_DISTANCE_FAR:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_DISTANCE_FAR");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_NOTREGIST:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NOTREGIST");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_HOME2HOME_FAIL:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_HOME2HOME_FAIL");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_ERROR:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_ERROR");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_ONVEHICLE:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_ONVEHICLE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_NORANKER:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NORANKER");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case 777:
			{
				sprintf(MsgBuf,"CLIENT_DEBUG_MSG : %s",pstrMsg);
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		default:
			{
				sprintf(MsgBuf,"NOT DEFINED ERROR NUMBER %s %d",pstrMsg,dwError);
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
	}
#endif
}

CObjectBase* cHousingMgr::GetSelectedObjectFromHouseMap(int MouseX,int MouseY,int PickOption , BOOL bSort)
{
	//�Ͽ콺 �ʿ��� �ٹ̱� ��尡 �ƴҶ��� ���Լ��� �̿��Ͽ� ��ŷ�� ������Ʈ�� �������� 
	CEngineObject* pEngineObject = NULL;
	PICK_GXOBJECT_DESC gxoDesc[5] = {0,};
	GetSelectedObject( gxoDesc, MouseX, MouseY, PickOption , /*bSort = TRUE*/ bSort);

	for( DWORD i = 0 ; i < 5 ; ++i )
	{
		if(gxoDesc[i].gxo)
		{
			pEngineObject = (CEngineObject*)g_pExecutive->GetData(gxoDesc[i].gxo);
			if( pEngineObject != NULL )
			{
				if( HERO )
				if( HERO->GetEngineObject() != pEngineObject )
				{
					if( pEngineObject->GetBaseObject() )
					{
						if( pEngineObject->GetBaseObject()->GetEngineObjectType() == eEngineObjectType_Monster )
						{
							if( ((CMonster*)(pEngineObject->GetBaseObject()))->GetSInfo()->SpecialType == 2 ) //������ Ŭ�� �ȵǵ���
							{
								pEngineObject = NULL;
								continue;
							}
						}
						//�Ͽ�¡�ʿ����� ������ �׼��� ������ Ŭ���� �ȵȴ�
						else if(	HERO->GetState() != eObjectState_Housing &&
									pEngineObject->GetBaseObject()->GetEngineObjectType() == eEngineObjectType_Furniture	)
						{
							if( ! IsActionableFurniture( (CFurniture *)pEngineObject->GetBaseObject()) )
							{
								pEngineObject = NULL;
								continue;
							}
						}
					}
					break;	//---pick first one
				}
			}
		}
	}

	if(pEngineObject == NULL)
		return NULL;
	
	return pEngineObject->GetBaseObject();
}

stDynamicHouseNpcMapInfo* cHousingMgr::GetDynimiHouseNpcMapInfo(DWORD dwMapIndex)
{
	return m_HousingSettingInfo.m_DynamicHouseNpcMapInfoList.GetData(dwMapIndex);
}

stDynamicHouseNpcInfo* cHousingMgr::GetDynimiHouseNpcInfo(DWORD dwMapIndex, BYTE byRank, BYTE byExterioKind)
{
	stDynamicHouseNpcMapInfo* pMapInfo = GetDynimiHouseNpcMapInfo( dwMapIndex );

	if( ! pMapInfo ) 
		return NULL;

	DWORD dwRankTypeIndex = byRank*100 + byExterioKind;
	return pMapInfo->pDynamicHouseNpcList.GetData( dwRankTypeIndex );
}

stDynamicHouseNpcInfo*	cHousingMgr::FindDynimiHouseNpcInfoByNpcKind(DWORD dwMapIndex , DWORD dwNpcKind)
{
	stDynamicHouseNpcMapInfo* pMapInfo = GetDynimiHouseNpcMapInfo( dwMapIndex );

	if( ! pMapInfo ) 
		return NULL;

	pMapInfo->pDynamicHouseNpcList.SetPositionHead();

	for(stDynamicHouseNpcInfo* pNpcInfo = pMapInfo->pDynamicHouseNpcList.GetData();
		0 < pNpcInfo;
		pNpcInfo = pMapInfo->pDynamicHouseNpcList.GetData())
	{
		if( pNpcInfo->dwNpcKind == dwNpcKind )
		{
			return pNpcInfo;
		}
	}

	return 0;
}

WORD cHousingMgr::GetStoredFunitureNumByCategory(WORD dwCategoryIdx)
{
	return m_CurHouse.m_dwCategoryNum[dwCategoryIdx];
}

WORD cHousingMgr::GetBlankSlotIndexFromStoredFunitureListByCategory(WORD dwCategoryIdx)
{
	for(int i=0; i<MAX_HOUSING_SLOT_NUM; i++ )
	{
		if( m_CurHouse.m_dwFurnitureList[dwCategoryIdx][i] == 0 )
		{
			return i;
		}
	}

	return 0;
}

BOOL cHousingMgr::IsHousingMap()
{
	return MAP->GetMapNum() == HOUSINGMAP;
}

BOOL cHousingMgr::IsActionableFurniture(CFurniture* pFurniture)
{
	if(! pFurniture )
		return FALSE;

	DWORD dwIdx =  pFurniture->GetFurnitureInfo()->dwFurnitureIndex;

	if( ! IsActionable(dwIdx) && ! IsDoor(dwIdx) ) 
	{
		//��ɼ��� �ƴҶ�
		return FALSE;
	}

	return TRUE;
}

bool cHousingMgr::IsFieldHeightObject(CObject* pObject)
{
	VECTOR3 ObjPos;
	pObject->GetPosition(&ObjPos);

	float fFieldHeight = 0.0f;

	g_pExecutive->GXMGetHFieldHeight(&fFieldHeight,ObjPos.x,ObjPos.z);

	if( fabs(fFieldHeight - ObjPos.y) < 0.0001f )
		return true;

	return false;
}

float cHousingMgr::GetStartAngle()
{
	stFurniture* pFurniture = NULL;
	stFurniture* pStart = NULL;
	m_CurHouse.pNotDeleteFurnitureList.SetPositionHead();
	while((pFurniture = m_CurHouse.pNotDeleteFurnitureList.GetData()) != NULL)
	{
		if(IsStart(pFurniture->dwFurnitureIndex))
			pStart = pFurniture;
	}

	if(pStart)
		return pStart->fAngle;

	return 0.0f;
}

BOOL cHousingMgr::CanUseItemFromHousingMap(CItem* pItem)
{
	//�Ͽ�¡ �ʿ��� ��� �Ұ����� �������� return FALSE

	if(	pItem->GetItemInfo()->SupplyType == ITEM_KIND_CHANGESIZE_UPWEIGHT ||	// ĳ���� Ű���� - Ŀ��Ȯ�� ����ġ
		pItem->GetItemInfo()->SupplyType == ITEM_KIND_CHANGESIZE_DNWEIGHT ||	// ĳ���� Ű���� - �۾���Ȯ�� ����ġ
		pItem->GetItemInfo()->SupplyType == ITEM_KIND_CHANGESIZE_DEFALUT ||		// ĳ���� Ű���� - �������
		pItem->GetItemInfo()->SupplyType == ITEM_KIND_SUMMON_MONSTER	||		// ���� ��ȯ
		pItem->GetItemInfo()->SupplyType == ITEM_KIND_SUMMON_NPC )				// NPC ��ȯ 
	{
			return FALSE;
	}

	return TRUE;
}

BOOL cHousingMgr::CanUseItemFromDecorationMode(CItem* pItem)
{
	//�ٹ̱� ����߿� ��� ������ �������� return TRUE
	if(	pItem->GetItemInfo()->SupplyType == ITEM_KIND_FURNITURE_WALLPAPER ||		//���� ��ü ������
		pItem->GetItemInfo()->SupplyType == ITEM_KIND_FURNITURE_FLOORPAPER ||		//�ٴ��� ��ü ������
		pItem->GetItemInfo()->SupplyType == ITEM_KIND_FURNITURE_CEILINGPAPER )		//õ���� ��ü ������ 
	{
			return TRUE;
	}

	return FALSE;
}

BOOL cHousingMgr::CanUseDecoBonus(stHouseBonusInfo* pBonusInfo,DWORD* pdwErr )
{
	if( !IsHouseOwner() )
	{
		//�������� �ƴҶ�
		if( pdwErr )
			*pdwErr = 1;
		return FALSE;
	}

	if( pBonusInfo->byKind == 1 && ! IsRankHouse() )
	{
		//��Ŀ ���ʽ��ε� ��Ŀ�� �ƴҶ�
		if( pdwErr )
			*pdwErr = 4;
		return FALSE;
	}

	if( m_CurHouse.dwDecoPoint < pBonusInfo->dwDecoPoint  )
	{
		//�ٹ̱� ����Ʈ �䱸 ������ �����Ҷ�
		if( pdwErr )
			*pdwErr = 2;
		return FALSE;
	}

	if( m_CurHouse.HouseInfo.dwDecoUsePoint < pBonusInfo->dwUsePoint )
	{
		//�� ����Ʈ �䱸 ������ �����Ҷ�
		if( pdwErr )
			*pdwErr = 3;
		return FALSE;
	}

	return TRUE;
}

BOOL cHousingMgr::CanInstallFunitureToField()
{
	//���� �ٹ̱� ���� ������ ��ġ�� �����������θ� ����
	if(!m_pDecoObj)
		return FALSE;

	if( ! m_pDecoObj->GetEngineObject()->GetGXOHandle() )
		return FALSE;

	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo( m_pDecoObj->GetFurnitureInfo()->dwFurnitureIndex );
	if( ! stFurnitureInfo )	
		return FALSE;

	if(! m_PickAnotherObjHandle )		//��ŷ�� �ٸ� ������Ʈ �ڵ��� ������� ���� (�ٴ� , �� , ������Ʈ ��) 
		return FALSE;

	CObjectBase* pBaseObj = ( (CEngineObject*)g_pExecutive->GetData(m_PickAnotherObjHandle) )->GetBaseObject();
	DWORD dwPickAnotherObjIdx =  ((CFurniture*)pBaseObj)->GetFurnitureInfo()->dwFurnitureIndex;

	stFunitureInfo* pstPickAnotherInfo = GAMERESRCMNGR->GetFunitureInfo( dwPickAnotherObjIdx );
	if( !pstPickAnotherInfo )
		return FALSE;

	if( m_dwFieldFurnitureNum > MAX_FURNITURE_STORAGE_NUM )			//�ʵ忡 ������ �Ѱ谹������ ũ�� ��ġ �Ұ��� 
		return FALSE;

	//------�浹 äũ���� ���� Ÿ��äũ-----// 
	if( Get_HighCategory(stFurnitureInfo->dwFurnitureIndex) ==  eHOUSE_HighCategory_Door)		//�ڽ��� ������ϰ�� 
	{
		if(! IsExteriorWall(dwPickAnotherObjIdx) )					//��ŷ�� ������Ʈ�� �ܺ��̾�� �Ѵ�. 
			return FALSE;
	}
	else if( IsStart(stFurnitureInfo->dwFurnitureIndex) )			//������ġ�ϰ��  
	{
		//�ڽ��� ������ġ�ϰ�� 
		if( Get_HighCategory(dwPickAnotherObjIdx) != eHOUSE_HighCategory_Carpet )	//��ŷ�� �ٸ�������Ʈ�� �ٴڷ��� �ƴϸ� ��ġ �Ұ��� 
			return FALSE;

		VECTOR3 Pos;
		m_pDecoObj->GetPosition(&Pos);

		if( Pos.y >= MAX_START_FURNITURE_HEIGHT ) 
			return FALSE;

		if( ! pstPickAnotherInfo->bStackable )
			return FALSE;
	}
	else // �Ϲݿ�����Ʈ�ϰ�� 
	{
		//m_PickAnotherObjHandle�� ������Ʈ�� bStackable�� �����ؾ� ��ġ�Ҽ��ִ�.
		if( ! pstPickAnotherInfo->bStackable )
			return FALSE;
	}
	//���� Ÿ��äũ ��  

	if( m_dwFieldFurnitureNum != 0)									
	{
		m_FieldFurnitureTable.SetPositionHead();

		for(CFurniture* pDestObj = m_FieldFurnitureTable.GetData();
			0 < pDestObj;
			pDestObj = m_FieldFurnitureTable.GetData())
		{
			if( m_pDecoObj->GetID() == pDestObj->GetFurnitureInfo()->dwObjectIndex )
				continue;

			if( ! pDestObj->GetEngineObject()->GetGXOHandle() )
				continue;


			stFunitureInfo* stDestInfo = GAMERESRCMNGR->GetFunitureInfo( ((CFurniture*)pDestObj)->GetFurnitureInfo()->dwFurnitureIndex );

			if( IsFloor ( stDestInfo->dwFurnitureIndex ) )		//�ٴڰ��� �浹äũ ���Ѵ�.
				continue;

			COLLISION_MESH_OBJECT_DESC DecoObjDesc,DestObjDesc ;
			float fDecoRadius,fDestRadius;
			VECTOR3 vDecoPos , vDestPos ; 

			g_pExecutive->GXOGetCollisionMesh(m_pDecoObj->GetEngineObject()->GetGXOHandle(), &DecoObjDesc);
			g_pExecutive->GXOGetCollisionMesh(pDestObj->GetEngineObject()->GetGXOHandle(), &DestObjDesc);

			fDecoRadius = DecoObjDesc.boundingSphere.fRs ;
			fDestRadius = DestObjDesc.boundingSphere.fRs ;

			m_pDecoObj->GetPosition( &vDecoPos );
			pDestObj->GetPosition( &vDestPos );

			VECTOR3 vDeco2DestPos = vDestPos - vDecoPos;
			float fDeltaLength = VECTOR3Length(&vDeco2DestPos);

			if( fDeltaLength > (fDecoRadius + fDestRadius) ) //���� ��ġ�� ������ �˻簡 �ʿ���� ����Obj�� �н�  	
				continue;


			// NYJ - ������Ʈ�� �ٿ���ڽ����� �浹üũ
			const int MAX_COLLISION_DESC_NUM = 10;
			COLLISION_MESH_OBJECT_DESC DecoObjectDesc[MAX_COLLISION_DESC_NUM];
			ZeroMemory(DecoObjectDesc, sizeof(DecoObjectDesc));
			COLLISION_MESH_OBJECT_DESC DestObjectDesc[MAX_COLLISION_DESC_NUM];
			ZeroMemory(DestObjectDesc, sizeof(DestObjectDesc));

			DWORD dwDecoColDescNum, dwDestColDescNum;
			DWORD dwDecoModelNum, dwDestModelNum;
			DWORD dwDecoObjectNum, dwDestObjectNum;

			dwDecoModelNum = g_pExecutive->GXOGetModelNum(m_pDecoObj->GetEngineObject()->GetGXOHandle());
			dwDestModelNum = g_pExecutive->GXOGetModelNum(pDestObj->GetEngineObject()->GetGXOHandle());

			DWORD i,j;
			dwDecoColDescNum = 0;
			for(i=0; i<dwDecoModelNum; i++)
			{
				dwDecoObjectNum = g_pExecutive->GXOGetObjectNum(m_pDecoObj->GetEngineObject()->GetGXOHandle(), i);

				for(j=0; j<dwDecoObjectNum; j++)
				{
					if(dwDecoColDescNum >= MAX_COLLISION_DESC_NUM)
					{
						__asm int 3;
						break;
					}

					if(!g_pExecutive->GXOGetCollisononObjectDesc(m_pDecoObj->GetEngineObject()->GetGXOHandle(), &DecoObjectDesc[dwDecoColDescNum], i, j))
						continue;

					dwDecoColDescNum++;
				}
			}

			// Dest�浹���� ��������
			dwDestColDescNum = 0;
			for(i=0; i<dwDestModelNum; i++)
			{
				dwDestObjectNum = g_pExecutive->GXOGetObjectNum(pDestObj->GetEngineObject()->GetGXOHandle(), i);

				for(j=0; j<dwDestObjectNum; j++)
				{
					if(dwDestColDescNum >= MAX_COLLISION_DESC_NUM)
					{
						__asm int 3;
						break;
					}

					if(!g_pExecutive->GXOGetCollisononObjectDesc(pDestObj->GetEngineObject()->GetGXOHandle(), &DestObjectDesc[dwDestColDescNum], i, j))
						continue;

					dwDestColDescNum++;
				}
			}

			// �浹�˻�
			BOOL bBoxTest = FALSE;
			for(i=0; i<dwDecoColDescNum; i++)
			{
				for(j=0; j<dwDestColDescNum; j++)
				{
					if(TestCollisionBox2Box(&DecoObjectDesc[i].boundingBox, &DestObjectDesc[j].boundingBox))
						bBoxTest = TRUE;//return FALSE;
				}
			}

			if(!bBoxTest)
				continue;

			//�ﰢ���˻簡 �Ѵ� �ʿ������ ��ġ ���� 
			if( !stFurnitureInfo->bMeshTest && !stDestInfo->bMeshTest  )
				return FALSE;
			
			VECTOR3 pos = {0};
			float dist = 0;
			DWORD modelindex,objectindex;

			// NYJ - �ﰢ���浹�� ��� �ϱ�� (��������� vs ���ڿ�����Ʈ�ٿ���ڽ�)
			if( g_pExecutive->GXOIsCollisionMeshWithObjectBoundingBox(pDestObj->GetEngineObject()->GetGXOHandle(), 
								&pos, &modelindex, &objectindex, &dist, m_pDecoObj->GetEngineObject()->GetGXOHandle()) )
								return FALSE;
		}
	}

	return TRUE;
}

//��ŷ�� �ʵ忡 �Ҽ��ִ°� 
BOOL cHousingMgr::CanMakingToFiled(CObject* pObject)
{
	//�ȿ��� ����μ� vs ��ŷBB �浹äũ�� ����.

	//�ϴ� ���浹äũ 
	COLLISION_MESH_OBJECT_DESC ObjDesc,HeroDesc ;
	float fObjRadius,fHeroRadius;
	VECTOR3 vObjPos , vHeroPos ; 

	stMarkingInfo* pMarkinginfo = ((CFurniture*)pObject)->GetMarkingInfo();

	if(! pMarkinginfo->bUseMaking )
		return FALSE;

	g_pExecutive->GXOGetCollisionMesh(pObject->GetEngineObject()->GetGXOHandle(), &ObjDesc);
	g_pExecutive->GXOGetCollisionMesh(HERO->GetEngineObject()->GetGXOHandle(), &HeroDesc);

	fObjRadius	= ObjDesc.boundingSphere.fRs ;
	fHeroRadius	= HeroDesc.boundingSphere.fRs ;

	pObject->GetPosition( &vObjPos );
	HERO->GetPosition( &vHeroPos );

	VECTOR3 vObj2HeroPos = vHeroPos - vObjPos;
	float fDeltaLength = VECTOR3Length(&vObj2HeroPos);

	if( fDeltaLength > (fObjRadius + fHeroRadius) )		//���� ��ġ�� ������ �ڽ� �˻簡 �ʿ����   	
		return TRUE;

	//090608 ��ŷ���� ���׼���	����ΰ� ��ġ�� �� VS ��ŷ�ڽ� 
	DWORD dwCurTile_x =  DWORD((vHeroPos.x) / 50);
	DWORD dwCurTile_z =  DWORD((vHeroPos.z) / 50);

	VECTOR3 vHeroShellCenter = {0,};
	vHeroShellCenter.x =  float( (dwCurTile_x * 50.0f) + 25.0f );
	vHeroShellCenter.z =  float( (dwCurTile_z * 50.0f) + 25.0f );

	if( pMarkinginfo->bUseResize )						//��ŷ������ ���������� �ٿ�� �ڽ��� �������� �ڽ��� äũ  
	{
		((CFurniture*)pObject)->UpdateMarkingInfo();
		return (! CheckPercentShellToBoundingBox( &pMarkinginfo->stResizeBoudingBox , &vHeroShellCenter ) );		//������� ���� �浹���� ������ return TRUE 
	}
	else												//�ƴ϶�� �⺻ �ڽ��� äũ 
	{
		return (! CheckPercentShellToBoundingBox( &ObjDesc.boundingBox , &vHeroShellCenter ) );						//������� ���� �浹���� ������ return TRUE 
	}
}

//�缳���� ������ �����ΰ� 
BOOL cHousingMgr::CanResetDecorationFuniture(CFurniture* pFurniture)
{
	if(! pFurniture )
		return FALSE;

	DWORD dwIdx = pFurniture->GetFurnitureInfo()->dwFurnitureIndex;

	if( pFurniture->GetFurnitureInfo()->bNotDelete )
	{
		//�⺻���� �����ϰ�� ���� ������ġ�� �����ϰ� ������ �Ұ��� 
		if( ! IsDoor(dwIdx) && ! IsStart(dwIdx) ) 
			return FALSE;
	}

	return TRUE;
}

//���� ���ԵǴ� ������ äũ�Ͽ� �ش� ���� ��ŷ�� �ؾ��ϴ��� ���θ� ����
BOOL cHousingMgr::CheckPercentShellToBoundingBox( BOUNDING_BOX* pSrcBoundingBox, VECTOR3* pMakingShellCenter)
{
	//�ٿ���ڽ� 8�� �ε��� 
	//������
	//	[0] [3]   
	//	[4] [7]

	//�Ʒ��� 
	//	[1]	[2]	  
	//	[5] [6]

	BOUNDING_BOX MakingBox = {0,};
	MakingBox.v3Oct[1].x = pMakingShellCenter->x - 25.0f;
	MakingBox.v3Oct[1].z = pMakingShellCenter->z + 25.0f;

	MakingBox.v3Oct[2].x = pMakingShellCenter->x + 25.0f;
	MakingBox.v3Oct[2].z = pMakingShellCenter->z + 25.0f;

	MakingBox.v3Oct[5].x = pMakingShellCenter->x - 25.0f;
	MakingBox.v3Oct[5].z = pMakingShellCenter->z - 25.0f;

	MakingBox.v3Oct[6].x = pMakingShellCenter->x + 25.0f;
	MakingBox.v3Oct[6].z = pMakingShellCenter->z - 25.0f;

	if( !TestCollisionBox2Box(pSrcBoundingBox ,&MakingBox,FALSE) )
		return FALSE;


	BOUNDING_BOX ShellCenterBox = {0,};
	ShellCenterBox.v3Oct[1].x = pMakingShellCenter->x - 6.25f;
	ShellCenterBox.v3Oct[1].z = pMakingShellCenter->z + 6.25f;

	ShellCenterBox.v3Oct[2].x = pMakingShellCenter->x + 6.25f;
	ShellCenterBox.v3Oct[2].z = pMakingShellCenter->z + 6.25f;

	ShellCenterBox.v3Oct[5].x = pMakingShellCenter->x - 6.25f;
	ShellCenterBox.v3Oct[5].z = pMakingShellCenter->z - 6.25f;

	ShellCenterBox.v3Oct[6].x = pMakingShellCenter->x + 6.25f;
	ShellCenterBox.v3Oct[6].z = pMakingShellCenter->z - 6.25f;

	if( ! TestCollisionBox2Box(pSrcBoundingBox ,&ShellCenterBox,FALSE) )
		return FALSE;

	return TRUE;
}

void cHousingMgr::SetDecorationModeTemp()
{
	if(! IsHouseInfoLoadOk() )
		return;

	// �������� ��츸 ó��
	if(! IsHouseOwner() )
		return;

	m_bDecorationMode = TRUE;

	//090603 pdy �Ͽ�¡ �ٹ̱� ���ÿ� ���õǾ��� Object�� ����Ѵ� 
	CObject* pObject = OBJECTMGR->GetSelectedObject() ;								// ���õ� ������Ʈ ������ �޾ƿ´�.

	if( pObject )																	// ������Ʈ ������ ��ȿ�ϴٸ�,
	{
		pObject->ShowObjectName( FALSE );											// ������Ʈ �̸��� ��Ȱ��ȭ�Ѵ�.

		OBJECTMGR->SetSelectedObject(NULL) ;										// ������Ʈ ������ ���� �Ѵ�.
	}
	OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Housing);
}

void cHousingMgr::SetDecorationMode(BOOL bMode)
{
	if(! IsHouseInfoLoadOk() )
		return;

	// �������� ��츸 ó��
	if(! IsHouseOwner() )
		return;

	if( ! bMode )
	{
		if( m_bDoDecoration ) 
			CancelDecoration();
	}

	m_bDecorationMode = bMode;

	if( bMode )
	{
		//090603 pdy �Ͽ�¡ �ٹ̱� ���ÿ� ���õǾ��� Object�� ����Ѵ� 
		CObject* pObject = OBJECTMGR->GetSelectedObject() ;								// ���õ� ������Ʈ ������ �޾ƿ´�.

		if( pObject )																	// ������Ʈ ������ ��ȿ�ϴٸ�,
		{
			pObject->ShowObjectName( FALSE );											// ������Ʈ �̸��� ��Ȱ��ȭ�Ѵ�.

			OBJECTMGR->SetSelectedObject(NULL) ;										// ������Ʈ ������ ���� �Ѵ�.
		}

		//090527 pdy �Ͽ�¡ �ý��۸޼��� [�ٹ̱��� ����]
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1868 ) );	//1868	"�ٹ̱� ��带 �����մϴ�."
		OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Housing);
	}
	else
	{
		//090527 pdy �Ͽ�¡ �ý��۸޼��� [�ٹ̱��� ����]
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1869 ) );
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Housing); //1869	"�ٹ̱� ��带 �����մϴ�."
	}

	cHousingDecoModeBtDlg* pDecoModeBtDlg = GAMEIN->GetHousingDecoModeBtDlg();

	if( pDecoModeBtDlg )
		pDecoModeBtDlg->OnChangeDecoMode( m_bDecorationMode );
}

//�ٹ̱� ���� (������->�ʵ�)
void cHousingMgr::StartDecoration(stFurniture* pstFuniture)
{
	if(! pstFuniture )
		return;

	if( ! m_bDecorationMode || m_bDoDecoration )			//�ٹ̱� ��尡 �ƴҽÿ� �ٹ̱⸦ �Ҽ�����. || �ٹ̱����̸� �Ҽ�����.
		return;

	//�Ϲ� ���� �ٹ̱� 
	m_pDecoObj = HOUSINGMGR->MakeDummyFuniture(pstFuniture);

	if(! m_pDecoObj->GetEngineObject()->GetGXOHandle() )
	{
		#if defined(_GMTOOL_)
			char Buf[256];
			stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo( m_pDecoObj->GetFurnitureInfo()->dwFurnitureIndex );

			if( ! stFurnitureInfo )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, "�� �б� ����");
				CHATMGR->AddMsg(CTC_SYSMSG, "������� ������ ������Ͽ� �����ϴ�.");
				::sprintf(Buf,"�����ε��� : %d ",m_pDecoObj->GetFurnitureInfo()->dwFurnitureIndex);
				CHATMGR->AddMsg(CTC_SYSMSG, Buf);
			}
			else
			{
				CHATMGR->AddMsg(CTC_SYSMSG, "�� �б� ����" );
				CHATMGR->AddMsg(CTC_SYSMSG, "���� ��Ͽ� ������ �ش� ���ϸ��� Data�����ȿ� �ִ��� Ȯ���Ͽ� �ּ���" );
				sprintf(Buf,"�����̸� : %s �����ε��� : %d " ,stFurnitureInfo->szName , stFurnitureInfo->dwFurnitureIndex );
				CHATMGR->AddMsg(CTC_SYSMSG, Buf);
				sprintf(Buf,"�� ���� : %s " ,stFurnitureInfo->szModelFileName);
				CHATMGR->AddMsg(CTC_SYSMSG, Buf);
			}
		#endif
		RemoveCurDeco();
		return;
	}
	StartDecoMove();

	m_bDoDecoration = TRUE;

	//090615 pdy �Ͽ�¡ ���Ĺ��� ����
	m_pDecoObj->GetEngineObject()->SetAlpha(0.5f);
	g_pExecutive->GXOSetZOrder( m_pDecoObj->GetEngineObject()->GetGXOHandle() , 8 );  // ������ ���İ� ���� ���� ���ù����� Zorder�� 8�� �������ش�. 
}

//�ٹ̱� ���� 
void cHousingMgr::EndDecoration()
{
	m_pDecoObj = NULL;
	m_bDoDecoration = FALSE;

	EndDecoAngle();										// ȸ����ġ�� ���� 
	EndDecoMove();										// �̵���ġ�� ���� 

	CHousingRevolDlg* pRevolDlg = GAMEIN->GetHousingRevolDlg();

	if( pRevolDlg ) 
		pRevolDlg->SetActive(FALSE);					// ��ġUI�� ���� 
}

//��ġ�� ĵ���ɶ��� ó���� �Ѵ� 
void cHousingMgr::CancelDecoration()
{
	if(! m_bDoDecoration )								// ��ġ���� �ƴ϶�� return
		return;

	if( m_pDecoObj )
	{
		if( m_pDecoObj->GetID() == HOUSE_DUMMYID )			// ��ġ���ΰ����� ���̿�����Ʈ�� ���� ó��  
		{
			RemoveCurDeco();
		}
		else												// ��ġ���ΰ����� ���̰� �ƴϸ� ���� ��ġ,ȸ�� ������ ������ �ٽ� ��ŷ���� 
		{
			m_pDecoObj->GetEngineObject()->ApplyHeightField(FALSE);
			m_pDecoObj->SetPosition(&m_vBackupDecoPos);
			m_pDecoObj->GetEngineObject()->ApplyHeightField(TRUE);
			m_pDecoObj->SetAngle(m_fBackupDecoAngle);
			m_pDecoObj->GetEngineObject()->SetAlpha(1.0f);
			MarkingObjZone(m_pDecoObj,TRUE);

			//090615 pdy �Ͽ�¡ ���Ĺ��� ����
			stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo( m_pDecoObj->GetFurnitureInfo()->dwFurnitureIndex );
			if( stFurnitureInfo && stFurnitureInfo->bHasAlpha == FALSE )							//��ü ���İ� ���� ������
				g_pExecutive->GXOSetZOrder( m_pDecoObj->GetEngineObject()->GetGXOHandle() , 0 );	//Zorder�� 0���� ���� 
		}
	}

	EndDecoration();
}

//������Ʈ�ٹ̱��� Ȯ���� �������� ȣ�� 
BOOL cHousingMgr::DecideSettingCurDeco()
{
	//�Լ������� ��û�� �Ͽ����� return TRUE�̴�.
	stFurniture* pstFuniture = m_pDecoObj->GetFurnitureInfo();

	if( pstFuniture->wState == eHOUSEFURNITURE_STATE_KEEP )
	{
		//ó����ġ�� �õ��ϴ� ������ �˾�â�� ������� OK�� ��ġ ��û

		//090527 pdy �Ͽ�¡ �˾�â [���ʼ�ġ ��û]
		WINDOWMGR->MsgBox( MBI_DECOINSTALL_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1892) ) ; //1892	"����� �������� ���� �ð��� ���ҵ˴ϴ�. ����Ͻðڽ��ϱ�?"
		return FALSE;
	}

	if( pstFuniture->wState == eHOUSEFURNITURE_STATE_INSTALL	) 
	{
		//�ν���� ��Ȳ�̸� ������Ʈ�� ��û���� 
		if( ! RequestUpdateCurDeco() )
			return FALSE;
			
	}
	else if( pstFuniture->wState == eHOUSEFURNITURE_STATE_UNINSTALL	)
	{
		//ó����ġ�� �ƴ� ��ġ�õ��� �˾�â���� �ٷ� ��ġ�õ�   
		if( ! RequestInstallCurDeco() )
			return FALSE;
	}

	return TRUE;
}

//��ġ���� 
void cHousingMgr::StartDecoMove()
{
	if(! m_pDecoObj ) 
		return;

	m_bDecoMove = TRUE;							
	m_PickAnotherObjHandle = NULL;
}

void cHousingMgr::EndDecoMove()
{
	//g_pExecutive->GetGeometry()->SetDrawDebugFlag(0);

	m_bDecoMove = FALSE;
	//m_PickAnotherObjHandle = NULL;
	//m_vDecoPos.x = m_vDecoPos.y = m_vDecoPos.z = 0.0f;
}

//ȸ������
void cHousingMgr::StartDecoAngle()
{
	CHousingRevolDlg* pRevolDlg = GAMEIN->GetHousingRevolDlg();

	if( !pRevolDlg ) 
		return;

	m_bDecoAngle = TRUE;

	LONG x = MOUSE->GetMouseX() - 100;
	LONG y = MOUSE->GetMouseY() + 15;

	pRevolDlg->SetAbsXY( x, y );

	pRevolDlg->SetActive(TRUE);
	pRevolDlg->SetBarRateFromAngle( m_pDecoObj->GetAngle() );

	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo( m_pDecoObj->GetFurnitureInfo()->dwFurnitureIndex );

	if( ! stFurnitureInfo )
		return ;

	if(stFurnitureInfo->bRotatable)
		pRevolDlg->StartRotate();
}

void cHousingMgr::EndDecoAngle()
{
	//ȸ���� ������
	m_bDecoAngle = FALSE;
}

//�ʵ忡 ��ġ�� �������� ���� ����
BOOL cHousingMgr::StartDecoReset(CObject* pPickObject)
{  
	//�ٹ̱� ���ÿ� ��Ŀ� ������ ��Ŭ���� ȣ��ȴ�. 
	if( !pPickObject )
		return FALSE;

	if( pPickObject->GetEngineObjectType() != eEngineObjectType_Furniture )					// Pick ������Ʈ�� ������ �ƴϸ� return 
		return FALSE;

	//090708 pdy 
	cHousingWarehouseDlg* pWarehouseDlg = GAMEIN->GetHousingWarehouseDlg();
	pWarehouseDlg->FocusOnByFurniture( ((CFurniture*) pPickObject)->GetFurnitureInfo() );	//��ũ��,�� ���� �������� �����ش�.

	if( ! CanResetDecorationFuniture((CFurniture*)pPickObject) )
	{
		return FALSE;
	}

	m_pDecoObj = (CFurniture*)pPickObject;

	m_pDecoObj->GetPosition(&m_vDecoPos);			// ó��������ġ�� m_vDecoPos���� ����						
	m_pDecoObj->GetPosition(&m_vBackupDecoPos);		// Reset�Ǳ����� ��ġ�� ����Ѵ�.
	m_fBackupDecoAngle = m_pDecoObj->GetAngle();	// Reset�Ǳ����� ȸ������ ����Ѵ�.
	m_bDoDecoration = TRUE;

	RemoveWaitForMakingObj(m_pDecoObj);
	MarkingObjZone(m_pDecoObj, FALSE);

	m_pDecoObj->GetEngineObject()->ApplyHeightField(FALSE);
	m_pDecoObj->SetPosition(&m_vDecoPos);
	m_pDecoObj->GetEngineObject()->ApplyHeightField(TRUE);

	//090615 pdy �Ͽ�¡ ���Ĺ��� ����
	m_pDecoObj->GetEngineObject()->SetAlpha(0.5f);
	g_pExecutive->GXOSetZOrder( m_pDecoObj->GetEngineObject()->GetGXOHandle() , 8 );			// ������ ���İ� ���� ���� ���ù����� Zorder�� 8�� �������ش�. 

	PTRLISTPOS pos = m_AlphaHandleList.Find( m_pDecoObj->GetEngineObject()->GetGXOHandle() );	//���ĸ���Ʈ���� �ִٸ� ����Ʈ���� �������� 
	if( pos )
		m_AlphaHandleList.RemoveAt( pos );

	StartDecoAngle();

	return TRUE;
}

CFurniture* cHousingMgr::MakeDummyFuniture(stFurniture* pstFuniture)
{
	// ������ġ�� ������ ������ �ޱ����� �����Ǵ� ���� ���� 

	if(! pstFuniture )
		return NULL;

	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(pstFuniture->dwFurnitureIndex);

	if( stFurnitureInfo == 0)
		return NULL;

	BASEOBJECT_INFO Baseinfo;
	Baseinfo.dwObjectID =  HOUSE_DUMMYID;
	SafeStrCpy(Baseinfo.ObjectName, stFurnitureInfo->szName, MAX_NAME_LENGTH+1);

	BASEMOVE_INFO moveInfo;
	moveInfo.bMoving = FALSE;
	moveInfo.CurPosition.x = 0;
	moveInfo.CurPosition.y = 0;
	moveInfo.CurPosition.z = 0;

	stFurniture Furniture = *pstFuniture;
	Furniture.dwObjectIndex = HOUSE_DUMMYID;

	CFurniture* pDummy = MakeFuniture(&Baseinfo,&moveInfo,&Furniture);

	return pDummy;
}

CFurniture* cHousingMgr::MakeFuniture(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,stFurniture* pstFuniture)
{
	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(pstFuniture->dwFurnitureIndex);

	if( stFurnitureInfo == 0)
		return NULL;

	CFurniture* pObject = OBJECTMGR->AddFuniture(pBaseObjectInfo,pMoveInfo,pstFuniture);

	//090611 pdy �Ͽ�¡ ���Ĺ��� ���� 
	if( pObject && pObject->GetEngineObject()->GetGXOHandle() ) 
	{
		if(	stFurnitureInfo->bHasAlpha )		//��ü ���ĸ� ������ �ִ� ������� ������ �켱������ �ڷ� ������.
		{
			g_pExecutive->GXOSetZOrder( pObject->GetEngineObject()->GetGXOHandle() , 8 ); // ����Ʈ�� 9�ϱ� �׺��� �Ѵܰ� ���� 8�� �����Ѵ�.
		}
	}

	return pObject;
}

//�������� 
void cHousingMgr::RemoveDeco(DWORD dwObjectID)
{
	CObject* pObject = OBJECTMGR->GetObject(dwObjectID);
	if(pObject)
	{
		if(OBJECTMGR->GetSelectedObjectID() == dwObjectID)
		{
			OBJECTMGR->SetSelectedObjectID(0);
		}

		OBJECTMGR->AddGarbageObject(pObject);
	}

}

//���� �ٹ̱� ���� ���� ���� 
void cHousingMgr::RemoveCurDeco()
{
	if( m_pDecoObj)
		RemoveDeco(m_pDecoObj->GetID());
}

//�������� ���� 
void cHousingMgr::DestroyFuniture(stFurniture* pDestroyFn)
{
	if( pDestroyFn )
	{
		if( pDestroyFn->wState == eHOUSEFURNITURE_STATE_INSTALL	)
		{
			//Ȥ�ö� �����ɰ����� �ν�����ִٸ� UnInstall����
			UnInstallFunitureFromField(pDestroyFn->dwObjectIndex); 
		}

		m_CurHouse.m_dwFurnitureList[pDestroyFn->wCategory][pDestroyFn->wSlot] = 0;
		m_CurHouse.m_dwCategoryNum[pDestroyFn->wCategory]--;					//ī�װ��� �������� --

		cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();		

		//090708 pdy DeleteIcon ���� 
		pDlg->DeleteIcon( pDestroyFn );

		m_CurHouse.pFurnitureList.Remove(pDestroyFn->dwObjectIndex);							//â����Ʈ���� �����ϰ�
		m_stFurniturePool.Free(pDestroyFn);									//�޸� ��������
	}
}


//�ʵ忡 �ν��� 
BOOL cHousingMgr::InstallFunitureToField(stFurniture* pstFuniture , BOOL bFirstInstall)
{
	if (! pstFuniture )
		return FALSE;

	//090325 �ʵ����̺� �߰� 
	//�ʵ����̺� �����ϸ� ���ġ, �������� ������ ���λ��� 
	CFurniture* pFuniture = m_FieldFurnitureTable.GetData(pstFuniture->dwObjectIndex);	

	//������ ������ �޾����� ���� ���̴� ����� ���θ�����.
	RemoveCurDeco();

	//�ӽ��׽�Ʈ BASEOBJECT_INFO , BASEMOVE_INFO
	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(pstFuniture->dwFurnitureIndex);
	if( ! stFurnitureInfo )
	{
		return FALSE;
	}

	BASEOBJECT_INFO Baseinfo;
	Baseinfo.dwObjectID =  pstFuniture->dwObjectIndex;	
	SafeStrCpy(Baseinfo.ObjectName, stFurnitureInfo->szName, MAX_NAME_LENGTH+1);

	BASEMOVE_INFO moveInfo;
	moveInfo.bMoving = FALSE;
	moveInfo.CurPosition.x = pstFuniture->vPosition.x;
	moveInfo.CurPosition.y = pstFuniture->vPosition.y;
	moveInfo.CurPosition.z = pstFuniture->vPosition.z;

	pFuniture = (CFurniture*)MakeFuniture( &Baseinfo , &moveInfo ,  pstFuniture );

	if( ! pFuniture )
		return FALSE;

	//�ʵ帮��Ʈ�� Obj�� �߰��Ѵ�
	m_FieldFurnitureTable.Add(pFuniture , pFuniture->GetID());
	m_dwFieldFurnitureNum++;

	//�ε��� �ν����̸� ��ŷ�ϰ� ����.
	if( bFirstInstall )
	{
		MarkingObjZone(pFuniture, TRUE);
		return TRUE;
	}

	RemoveWaitForMakingObj(pFuniture);

	if( CanMakingToFiled(pFuniture) )			//HERO�� ��ġ�� ��ŷ�� �ϴ��� �˻��� 
	{
		MarkingObjZone(pFuniture, TRUE);		//��ŷ�����ϸ� ��ŷ
	}
	else
	{
		AddWaitForMakingObj(pFuniture);			//�Ұ����ϸ� ��⸮��Ʈ�� ADD
	}

	m_pDecoObj = NULL;							

	EndDecoration();							//��ġ�� �Ϸ�Ǹ� ���� �ٹ̱⸦ �������� 

	return TRUE;
}

BOOL cHousingMgr::UpdateFunitere(stFurniture* pstFuniture)
{
	if (! pstFuniture )
		return FALSE;

	CFurniture* pFuniture = m_FieldFurnitureTable.GetData(pstFuniture->dwObjectIndex);	
	if(! pFuniture) 
		return FALSE;

	//090608 pdy ��ŷ���� ���� ����
	if( IsHouseOwner() == FALSE )
	{
		//�������ϰ�� ������Ʈ ��û�� �ϱ����� ��ŷ�� �����ϰ� ���ٴ�.
		//�׷��� 3���ϰ�쿣 ��ŷ�� �Ǿ������Ƿ� ������Ʈ�� ������ ��������ġ�� ��ŷ�� ������ �־�� �Ѵ�.
		RemoveWaitForMakingObj(pFuniture);
		MarkingObjZone(pFuniture, FALSE);		//��ŷ�� �����ϰ� 
	}

	pFuniture->InitFurniture(pstFuniture);

	pFuniture->GetEngineObject()->ApplyHeightField(FALSE);
	pFuniture->SetPosition(&pstFuniture->vPosition);
	pFuniture->GetEngineObject()->ApplyHeightField(TRUE);
	pFuniture->SetAngle(pstFuniture->fAngle);
	pFuniture->GetEngineObject()->SetAlpha(1.0f);
	pFuniture->GetEngineObject()->SetMaterialIndex(pstFuniture->nMaterialIndex);

	//090615 pdy �Ͽ�¡ ���Ĺ��� ����
	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(pstFuniture->dwFurnitureIndex);
	if( stFurnitureInfo && stFurnitureInfo->bHasAlpha == FALSE )									//��ü ���İ� ���� ������
		g_pExecutive->GXOSetZOrder( pFuniture->GetEngineObject()->GetGXOHandle() , 0 );				//Zorder�� 0���� ���� 


	if( CanMakingToFiled(pFuniture) )		//HERO�� ��ġ�� ��ŷ�� �ϴ��� �˻��� 
	{
		MarkingObjZone(pFuniture, TRUE);	//��ŷ�����ϸ� ��ŷ
	}
	else
	{
		AddWaitForMakingObj(pFuniture);		//�Ұ����ϸ� ��⸮��Ʈ�� ADD
	}

	m_pDecoObj = NULL;				

	//��ġ�� �Ϸ�Ǹ� �ٹ̱⸦ �������� 
	EndDecoration();

	return TRUE;
}

BOOL cHousingMgr::UnInstallFunitureFromField(DWORD dwObjectIdx)
{
	CFurniture* pFuniture = m_FieldFurnitureTable.GetData(dwObjectIdx);		//�ʵ� ���̺��� ����Obj�� �����´�

	if(!pFuniture)
		return FALSE;

	RemoveWaitForMakingObj(pFuniture);				//��ŷ ����� ��Ͽ� �ִٸ� Remove

	MarkingObjZone(pFuniture, FALSE);				//��ŷ���� 

	RemoveDeco(dwObjectIdx);						//������Ʈ�� ����		
	m_FieldFurnitureTable.Remove( dwObjectIdx );	//�ʵ����̺��� ����
	m_dwFieldFurnitureNum--;						//�ʵ忡 �������� ���� 

	m_pDecoObj = NULL;

	if(m_CurHouse.HouseInfo.dwOwnerUserIndex == TITLE->GetUserIdx()) // NYJ
		EndDecoration();

	return TRUE;
}

void cHousingMgr::MarkingObjZone(CObject* pObject, bool bMakingObjZone)
{
	if( ! pObject)
		return;

	if( ! IsFieldHeightObject(pObject) )
		return;

	if(! pObject->GetEngineObject()->GetGXOHandle() )
		return;

	stMarkingInfo* pMarkingInfo = ((CFurniture*)pObject)->GetMarkingInfo();

	if( ! pMarkingInfo || ! pMarkingInfo->bUseMaking )
		return;

	GXOBJECT_HANDLE PickUpHANDLE = pObject->GetEngineObject()->GetGXOHandle();

	BOUNDING_BOX* pBoundingBox = NULL; 

	if( pMarkingInfo->bUseResize )
	{
		if( bMakingObjZone ) 
			((CFurniture*)pObject)->UpdateMarkingInfo();

		pBoundingBox = &pMarkingInfo->stResizeBoudingBox ;
	}
	else 
	{
		COLLISION_MESH_OBJECT_DESC CollisionInfo = {0,}; 
		g_pExecutive->GXOGetCollisionMesh( PickUpHANDLE , &CollisionInfo);
		pBoundingBox = &(CollisionInfo.boundingBox);
	}

	// 091016 ��ŷ���� ���׷� ��ħ äũ �ּ�ó��
	//cPtrList NearObjList; 
	//
	//if( ! bMakingObjZone )
	//{
	//	//��ŷ�� �����ҽÿ� ������ ��ġ�� ����Ʈ�� ��Ƶд�.
	//	m_FieldFurnitureTable.SetPositionHead();
	//	CFurniture* pDestObj= NULL;

	//	float fDecoRadius = pMarkingInfo->f2DRadius;
	//	VECTOR3 vDecoPos;
	//	pObject->GetPosition( &vDecoPos );

	//	while( pDestObj = m_FieldFurnitureTable.GetData() )
	//	{
	//		if( ! IsFieldHeightObject(pDestObj)  )
	//			continue;

	//		if( pObject->GetID() == pDestObj->GetFurnitureInfo()->dwObjectIndex )
	//			continue;

	//		stMarkingInfo* pDestMarkingInfo = pDestObj->GetMarkingInfo();

	//		if( ! pDestMarkingInfo || ! pDestMarkingInfo->bUseMaking )
	//			continue;			

	//		float fDestRadius = pDestMarkingInfo->f2DRadius;
	//		VECTOR3 vDestPos ;
	//		pDestObj->GetPosition( &vDestPos );

	//		float fDeltaLength = CalcDistanceXZ(&vDestPos ,&vDecoPos );

	//		if( fDeltaLength <= (fDecoRadius + fDestRadius) ) //���� ��ġ�� ����Ʈ�� ����  	
	//			NearObjList.AddTail(pDestObj);
	//	}
	//}


	VECTOR3 Temp[4] ={0,};
	Temp[0] = pBoundingBox->v3Oct[1];
	Temp[1] = pBoundingBox->v3Oct[2];
	Temp[2] = pBoundingBox->v3Oct[5];
	Temp[3] = pBoundingBox->v3Oct[6];

	// �ڽ��� �ִ� �ּ� ��ġ�� ���Ѵ�.
	float fXMin = 100000.0f;
	float fXMax = 0.0f;
	float fZMin = 100000.0f;
	float fZMax = 0.0f;

	for(int i=0;i<4;i++)
	{
		if( Temp[i].x < fXMin )
			fXMin = Temp[i].x;

		if( Temp[i].x > fXMax )
			fXMax = Temp[i].x;

		if( Temp[i].z < fZMin )
			fZMin = Temp[i].z;

		if( Temp[i].z > fZMax )
			fZMax = Temp[i].z;
	}

	//���κ��� 50������ �ݿø� ���� �Ѵ�.
	fXMin = (float)(DWORD(fXMin / 50) * 50)- 50;
	fXMax = (float)(DWORD(fXMax / 50) * 50)+ 50; 
	fZMin = (float)(DWORD(fZMin / 50) * 50)- 50;
	fZMax = (float)(DWORD(fZMax / 50) * 50)+ 50; 
	CTileManager* pTileManager = MAP->GetTileManager();
	DWORD dwTileWidth = pTileManager->GetTileWidth();

	for( float Curz = fZMax; Curz > fZMin;Curz -= 50.0f ) 
	{
		if( Curz < 0.0f )
			continue;

		for( float Curx = fXMin; Curx < fXMax ; Curx += 50.0f)
		{
			if( Curx < 0.0f )
				continue;

			DWORD CurTile_x =  DWORD((Curx) / 50);
			DWORD CurTile_z =  DWORD((Curz) / 50);

			if( CurTile_z > dwTileWidth || CurTile_x > dwTileWidth)
				continue;

			VECTOR3 MakingShellCenter = {0,};
			MakingShellCenter.x =  float( (CurTile_x * 50.0f) + 25.0f );
			MakingShellCenter.z =  float( (CurTile_z * 50.0f) + 25.0f );
			
			if( ! CheckPercentShellToBoundingBox( pBoundingBox ,&MakingShellCenter) )
				continue;
		
			if( bMakingObjZone ) 
			{
				pTileManager->AddTileAttrByAreaData( NULL, &MakingShellCenter, SKILLAREA_ATTR_BLOCK );

				if( m_bShowTest )
					AddMarkingTest(pObject,&MakingShellCenter);
			}
			else
			{
				// 091016 ��ŷ���� ���׷� ��ħ äũ �ּ�ó��
				//BOOL bOverlapShell = FALSE;

				//PTRLISTPOS pos = NearObjList.GetHeadPosition();
				//while( pos )
				//{
				//	CFurniture* pDestObj = (CFurniture*)NearObjList.GetNext( pos ) ;

				//	stMarkingInfo* pDestMarkingInfo = pDestObj->GetMarkingInfo();

				//	if( ! pDestMarkingInfo )
				//		continue;

				//	BOUNDING_BOX* pDestBoundingBox = NULL;

				//	if( pDestMarkingInfo->bUseResize )
				//	{
				//		pDestBoundingBox = &pDestMarkingInfo->stResizeBoudingBox;
				//	}
				//	else
				//	{
				//		COLLISION_MESH_OBJECT_DESC DestObjDesc ;
				//		g_pExecutive->GXOGetCollisionMesh(pDestObj->GetEngineObject()->GetGXOHandle(), &DestObjDesc);

				//		pDestBoundingBox = &DestObjDesc.boundingBox;
				//	}

				//	if( CheckPercentShellToBoundingBox( pDestBoundingBox ,&MakingShellCenter) )
				//	{
				//		bOverlapShell = TRUE;
				//		break;
				//	}
				//}

				//if( ! bOverlapShell )
				//{
					pTileManager->RemoveTileAttrByAreaData( NULL, &MakingShellCenter, SKILLAREA_ATTR_BLOCK );
				//}

				if( m_bShowTest )
					ReleaseMarkingTest(pObject);
			}
		}
	}
}

void cHousingMgr::AddWaitForMakingObj(CObject* pObject)
{
	//090608 pdy ��ŷ���� ���� ���� 
	if( pObject == NULL )
		return;

	if( pObject->GetObjectKind() == eObjectKind_Furniture )
	{
		if( ((CFurniture*)pObject)->GetMarkingInfo()->bUseMaking == FALSE )
			return;

		if( ! IsFieldHeightObject(pObject) )
			return;
	}

	//�ߺ��ȰŸ� ADD���� �ʴ´� 
	PTRLISTPOS pos = m_WaitForMakingObjList.GetHeadPosition();
	while( pos )
	{
		CObject* pCurObject = (CObject*)m_WaitForMakingObjList.GetNext( pos ) ;
		if( pCurObject == pObject)
			return ;
	}

	//����Ʈ�� ��� 
	m_WaitForMakingObjList.AddTail(pObject);
}

void cHousingMgr::RemoveWaitForMakingObj(CObject* pObject)
{
	//����Ʈ���� �Ҹ� 

	PTRLISTPOS pos = m_WaitForMakingObjList.GetHeadPosition();
	while( pos )
	{
		CObject* pCurObject = (CObject*)m_WaitForMakingObjList.GetNext( pos ) ;

		if( pCurObject == pObject)
		{
			m_WaitForMakingObjList.Remove( pCurObject );
			return ;
		}
	}
}

//090608 pdy ��ŷ���� ���� ����
void cHousingMgr::FindOverapMarkingToHeroObj(cPtrList* pResultList)
{
	//HERO�� ������ �浹�Ǵ� ���� ��ŷ�� ������Ʈ����Ʈ�� ����

	if(! pResultList) 
		return;

	m_FieldFurnitureTable.SetPositionHead();

	for(CFurniture* pFnObject = m_FieldFurnitureTable.GetData();
		0 < pFnObject;
		pFnObject = m_FieldFurnitureTable.GetData())
	{
		DWORD dwFunitureIndex = pFnObject->GetFurnitureInfo()->dwFurnitureIndex;

		if( IsExteriorWall( dwFunitureIndex ) )				// �ܺ��̶�� Contunue 
			continue;

		if(! CanMakingToFiled( pFnObject ) )				// HERO�� ������ �⵹�� �ȴٸ� 
		{
			pResultList->AddTail( pFnObject );				// ��� ����Ʈ�� ADD
		}
	}
}

//090608 pdy ��ŷ���� ���� ����
void cHousingMgr::CheckAllMarkingObjByHeroZone()
{
	//HERO�� ������ �浹�Ǵ� ���� ��ŷ�� ������Ʈ���� ��ŷ������ ��ŷ��⸮��Ʈ�� ���
	cPtrList OverrapMarkingList;
	FindOverapMarkingToHeroObj(&OverrapMarkingList);		//�ʵ忡 ��ġ�� ������Ʈ�� ����� ��ġ�� ��ŷ�� ������Ʈ�� �����Ѵ�.

	PTRLISTPOS pos = OverrapMarkingList.GetHeadPosition();
	while( pos )
	{
		CFurniture* pObject = (CFurniture*)OverrapMarkingList.GetNext( pos ) ;

		if( pObject )
		{
			MarkingObjZone( pObject , FALSE );		//��ŷ�� ���� 
			AddWaitForMakingObj(pObject);			//����Ͽ� ��� 
		}
	}
}

void cHousingMgr::ChangeMaterialFurniture(DWORD dwFurnitureKind,DWORD dwMaterialIndex,BYTE byFlag)
{
	//byFlag 0 FreeView , 1 RollBack , 2 RealChange 
	switch(dwFurnitureKind)
	{
		case ITEM_KIND_FURNITURE_WALLPAPER :
		{
			//���̸� NotDelte ����Ʈ���� �ܺ���ã�� �ؽ��ĸ� �ٲ�����
			m_CurHouse.pNotDeleteFurnitureList.SetPositionHead();

			for(stFurniture* pstFurniture = m_CurHouse.pNotDeleteFurnitureList.GetData();
				0 < pstFurniture;
				pstFurniture = m_CurHouse.pNotDeleteFurnitureList.GetData())
			{
				if(! IsExteriorWall(pstFurniture->dwFurnitureIndex) )
					continue;

				CFurniture* pFuniture = (CFurniture*)m_FieldFurnitureTable.GetData(pstFurniture->dwObjectIndex);	

				if(! pFuniture )
					continue;

				if( 0 == byFlag )
				{
					//byFlag 0 FreeView 
					pFuniture->GetEngineObject()->SetMaterialIndex(dwMaterialIndex);
				}
				else if( 1 == byFlag )
				{
					//byFlag 1 RollBack
					pFuniture->GetEngineObject()->SetMaterialIndex( pstFurniture->nMaterialIndex );
				}
				else if( 2 == byFlag )
				{
					//byFlag 1 RealChange
					pstFurniture->nMaterialIndex = dwMaterialIndex;
					pFuniture->GetFurnitureInfo()->nMaterialIndex = dwMaterialIndex;
					pFuniture->GetEngineObject()->SetMaterialIndex(dwMaterialIndex);
				}
			}
		}
		break;
	case ITEM_KIND_FURNITURE_FLOORPAPER:
		{
			//�ٴ��̸� NotDelte ����Ʈ���� �ܺ���ã�� �ؽ��ĸ� �ٲ����� 
			m_FieldFurnitureTable.SetPositionHead();

			for(stFurniture* pstFurniture = m_CurHouse.pNotDeleteFurnitureList.GetData();
				0 < pstFurniture;
				pstFurniture = m_CurHouse.pNotDeleteFurnitureList.GetData())
			{
				if(! IsFloor(pstFurniture->dwFurnitureIndex) )
					continue;

				CFurniture* pFuniture = (CFurniture*)m_FieldFurnitureTable.GetData(pstFurniture->dwObjectIndex);	

				if(! pFuniture )
					continue;

				if( 0 == byFlag )
				{
					//byFlag 0 FreeView 
					pFuniture->GetEngineObject()->SetMaterialIndex(dwMaterialIndex);
				}
				else if( 1 == byFlag )
				{
					//byFlag 1 RollBack
					pFuniture->GetEngineObject()->SetMaterialIndex( pstFurniture->nMaterialIndex );
				}
				else if( 2 == byFlag )
				{
					//byFlag 1 RealChange
					pstFurniture->nMaterialIndex = dwMaterialIndex;
					pFuniture->GetFurnitureInfo()->nMaterialIndex = dwMaterialIndex;
					pFuniture->GetEngineObject()->SetMaterialIndex(dwMaterialIndex);
				}
			}
		}
		break;
	case ITEM_KIND_FURNITURE_CEILINGPAPER:
		{
			//õ���̸� NotDelte ����Ʈ���� �ܺ���ã�� �ؽ��ĸ� �ٲ����� 
			m_CurHouse.pNotDeleteFurnitureList.SetPositionHead();

			for(stFurniture* pstFurniture = m_CurHouse.pNotDeleteFurnitureList.GetData();
				0 < pstFurniture;
				pstFurniture = m_CurHouse.pNotDeleteFurnitureList.GetData())
			{
				if( Get_HighCategory(pstFurniture->dwFurnitureIndex) != eHOUSE_HighCategory_Decoration || 
					Get_LowCategory(pstFurniture->dwFurnitureIndex)	 != eHOUSE_LowCategory_Decoration_CCover )
					continue;

				CFurniture* pFuniture = (CFurniture*)m_FieldFurnitureTable.GetData(pstFurniture->dwObjectIndex);	

				if(! pFuniture )
					continue;

				if( 0 == byFlag )
				{
					//byFlag 0 FreeView 
					pFuniture->GetEngineObject()->SetMaterialIndex(dwMaterialIndex);
				}
				else if( 1 == byFlag )
				{
					//byFlag 1 RollBack
					pFuniture->GetEngineObject()->SetMaterialIndex( pstFurniture->nMaterialIndex );
				}
				else if( 2 == byFlag )
				{
					//byFlag 1 RealChange
					pstFurniture->nMaterialIndex = dwMaterialIndex;
					pFuniture->GetFurnitureInfo()->nMaterialIndex = dwMaterialIndex;
					pFuniture->GetEngineObject()->SetMaterialIndex(dwMaterialIndex);
				}
			}
		}
		break;
	}

}

void cHousingMgr::RideOnPlayerToFuniture(CPlayer* pPlayer)
{
	//090603 pdy �Ͽ�¡ ž�¹��� ����
	if( pPlayer == NULL || pPlayer->IsRideFurniture() || ! pPlayer->GetRideFurnitureID() )
		return;

	const DWORD dwFurnitureID			= pPlayer->GetRideFurnitureID();
	const DWORD dwFurnitureRideSlot = pPlayer->GetRideFurnitureSeatPos();

	if( dwFurnitureID == 0)
		return;

	CFurniture* pFurniture = (CFurniture*)OBJECTMGR->GetObject( dwFurnitureID );
	if(! pFurniture )
		return;

	if(! pFurniture->GetEngineObject()->IsInited() &&
	   ! pPlayer->GetEngineObject()->IsInited()			) 
	   return;

	stFurniture* 	pstFurniture = NULL;
	if( IsHouseOwner() )
	{
		pstFurniture = m_CurHouse.pFurnitureList.GetData(dwFurnitureID);
	}
	else
	{
		pstFurniture = pFurniture->GetFurnitureInfo();
	}

	stFunitureInfo* pstFunitureInfo = GAMERESRCMNGR->GetFunitureInfo(pFurniture->GetFurnitureInfo()->dwFurnitureIndex);

	if(! pFurniture || !pstFunitureInfo )
	{
		return;
	}

	if( ! pstFunitureInfo->dwActionIndex )
	{
		return;
	}

	char RideNodeName[32] = {0,};
	sprintf(RideNodeName,"%s0%d",HOUSE_ATTACHBONENAME , dwFurnitureRideSlot+1  );

	stHouseActionGroupInfo* pGroupInfo = GAMERESRCMNGR->GetHouseActionGroupInfo(pstFunitureInfo->dwActionIndex);

	if(! pGroupInfo )
	{
		return;
	}

	stHouseActionInfo* pActionInfo = NULL;
	for( int j=0; j < pGroupInfo->byActionNum ; j++)
	{
		stHouseActionInfo* pCurActionInfo = GAMERESRCMNGR->GetHouseActionInfo( pGroupInfo->dwActionIndexList[j] );
		if( pCurActionInfo->dwActionType == eHOUSE_ACTION_TYPE_RIDE )
		{
			pActionInfo = pCurActionInfo;
			break;
		}
	}

	if( ! pActionInfo )
	{
		return;
	}

	pFurniture->GetEngineObject()->AttachRider(pPlayer->GetEngineObject() , RideNodeName);
	pstFurniture->dwRidingPlayer[dwFurnitureRideSlot] = pPlayer->GetID();
	pFurniture->InitFurniture(pstFurniture);

	//���ĸ���Ʈ�� �����ϴ� ������ ����Ʈ���� �������ش�.
	PTRLISTPOS FindPos = m_AlphaHandleList.Find( pFurniture->GetEngineObject()->GetGXOHandle() );
	if( FindPos )
	{
		m_AlphaHandleList.RemoveAt( FindPos );

		//090615 pdy �Ͽ�¡ ���Ĺ��� ����
		if( pstFunitureInfo->bHasAlpha == FALSE )											 //��ü ���İ� ���� ������
			g_pExecutive->GXOSetZOrder( pFurniture->GetEngineObject()->GetGXOHandle() , 0 ); //Zorder�� 0���� �����Ѵ�.

		pFurniture->GetEngineObject()->SetAlpha(1.0f);
	}

	OBJECTSTATEMGR->StartObjectState(pPlayer, eObjectState_HouseRiding);
	pPlayer->SetRideFurnitureMotion(pActionInfo->dwPlayer_MotionIndex);
	pPlayer->SetBaseMotion();										// �⺻��� �缳��
	OBJECTSTATEMGR->InitObjectState( pPlayer );						// ����� ��� ������ �� �ֵ��� ���� �ʱ�ȭ

	if( pActionInfo->dwPlayer_EffectIndex )
	{
		OBJECTEFFECTDESC desc( pActionInfo->dwPlayer_EffectIndex );				//�÷��̾ ����Ʈ�� ������ ���� 
		pPlayer->AddObjectEffect( pActionInfo->dwPlayer_EffectIndex , &desc, 1 );
	}

	if( pActionInfo->dwFurniture_MotionIndex )
	{
		pFurniture->ChangeMotion( pActionInfo->dwFurniture_MotionIndex ,FALSE);		//������ ����� �ִٸ� ���� 
	}

	if( pActionInfo->dwFurniture_EffectIndex )
	{
		OBJECTEFFECTDESC desc( pActionInfo->dwFurniture_EffectIndex );			//������ ����Ʈ�� ������ ���� 
		pFurniture->AddObjectEffect( pActionInfo->dwFurniture_EffectIndex , &desc, 1 );
	}

	pPlayer->SetAngle(pFurniture->GetAngle());
	pPlayer->SetRideFurniture(TRUE);

	//090603 pdy �Ͽ�¡ ž�½� �׸��� ���ֱ� �߰�
	OBJECTMGR->ApplyShadowOption( pPlayer );
}

void cHousingMgr::RideOffPlayerFromFuniture(CPlayer* pPlayer, BOOL bForceRideOff)
{
	//090603 pdy �Ͽ�¡ ž�¹��� ����
	if( pPlayer == NULL || ! pPlayer->IsRideFurniture()  || ! pPlayer->GetRideFurnitureID() )
		return;

	const DWORD dwFurnitureID		= pPlayer->GetRideFurnitureID();
	const DWORD dwFurnitureRideSlot = pPlayer->GetRideFurnitureSeatPos();

	CFurniture* pFurniture = (CFurniture*)OBJECTMGR->GetObject( dwFurnitureID );
	if(! pFurniture )
		return;

	if( pFurniture->GetFurnitureInfo()->dwRidingPlayer[dwFurnitureRideSlot] != pPlayer->GetID() )
		return;

	stFurniture* 	pstFurniture = NULL;
	if( IsHouseOwner() )
	{
		pstFurniture = m_CurHouse.pFurnitureList.GetData(dwFurnitureID);
	}
	else
	{
		pstFurniture = pFurniture->GetFurnitureInfo();
	}
	stFunitureInfo* pstFunitureInfo = GAMERESRCMNGR->GetFunitureInfo(pFurniture->GetFurnitureInfo()->dwFurnitureIndex);

	if(! pFurniture || !pstFunitureInfo )
	{
		return;
	}

	if( ! pstFunitureInfo->dwActionIndex )
	{
		return;
	}

	if( pstFurniture->dwRidingPlayer[dwFurnitureRideSlot] != pPlayer->GetID() )
	{
		return;
	}

	pFurniture->GetEngineObject()->DetachRider(pPlayer->GetEngineObject());
	pstFurniture->dwRidingPlayer[dwFurnitureRideSlot] = 0;
	pFurniture->InitFurniture(pstFurniture);

	if( pPlayer == HERO )					//HERO�� ��������
	{
		//090608 pdy ��ŷ���� ���� ����
		CheckAllMarkingObjByHeroZone();		//�ʵ忡 ��ġ�� ������Ʈ�� ����� ��ġ�� ��ŷ�� ������Ʈ�� �����Ͽ� ó���Ѵ�.
	}

	pPlayer->SetRideFurnitureID(0);
	pPlayer->SetRideFurnitureSeatPos(0);

	OBJECTSTATEMGR->EndObjectState(pPlayer, eObjectState_HouseRiding);

	// ����������� �̵����·� ���������~
	if(bForceRideOff)
		OBJECTSTATEMGR->StartObjectState(pPlayer, eObjectState_Move);

	pPlayer->SetBaseMotion();											// �⺻��� �缳��
	pPlayer->SetAngle(pFurniture->GetAngle());
	pPlayer->SetRideFurniture(FALSE);

	APPEARANCEMGR->InitAppearance( pPlayer );							// ž������ �� �ٸ� Player�� ��� ������ �ʾ� ���̰� ��
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//���⼭���ʹ� �׽�Ʈ������ �ڵ� 
//////////////////////////////////////////////////////////////////////////////////////////////////
void cHousingMgr::ShowAllMarkingTest()
{
	DWORD dwCurTick = GetTickCount();

	if( dwCurTick < ( m_dwLastShowTick + 2000 ) )
		return;

	m_dwLastShowTick = dwCurTick;

	PTRLISTPOS pos = m_MarkinTestList.GetHeadPosition();
	while( pos )
	{
		cHousingMgr::cMarkingTest* pMarkingTest = (cMarkingTest*)m_MarkinTestList.GetNext( pos ) ;
		pMarkingTest->m_pTestShowObj->SetPoint( pMarkingTest->m_pvPos ) ;
	}
}

void cHousingMgr::SetShowTestMaking(bool bShow)
{ 
	m_bShowTest =  bShow;

	if( !bShow ) 
		ReleaseAllMarkingTest();
}	

//�׽�Ʈ�� ���� ��ŷ���� ���
void cHousingMgr::AddMarkingTest(CObject* pMarkingObj,VECTOR3* Pos)
{
	if(! pMarkingObj )
		return;

	cMarkingTest* pMarkingTest = new cMarkingTest();

	pMarkingTest->m_pTestShowObj = new CMovePoint();
	pMarkingTest->m_pTestShowObj->Init();

	pMarkingTest->m_pvPos = new VECTOR3();
	*pMarkingTest->m_pvPos = *Pos;

	pMarkingTest->m_pMarkinObj = pMarkingObj;

	m_MarkinTestList.AddTail(pMarkingTest);
}

//�׽�Ʈ�� ���� ��ŷ���� ����
void cHousingMgr::ReleaseMarkingTest(CObject* pMarkingObj)
{
	PTRLISTPOS pos = m_MarkinTestList.GetHeadPosition();
	while( pos )
	{
		cMarkingTest* pMarkingTest = (cMarkingTest*)m_MarkinTestList.GetNext( pos ) ;

		if( pMarkingTest->m_pMarkinObj == pMarkingObj)
		{
			m_MarkinTestList.Remove( pMarkingTest );

			pMarkingTest->m_pTestShowObj->Release();
			SAFE_DELETE( pMarkingTest->m_pTestShowObj );
			SAFE_DELETE( pMarkingTest->m_pvPos );
			SAFE_DELETE( pMarkingTest );

			return;
		}
	}
}
void cHousingMgr::ReleaseAllMarkingTest()
{
	PTRLISTPOS pos = m_MarkinTestList.GetHeadPosition();
	while( pos )
	{
		cMarkingTest* pMarkingTest = (cMarkingTest*)m_MarkinTestList.GetNext( pos ) ;

		pMarkingTest->m_pTestShowObj->Release();
		SAFE_DELETE( pMarkingTest->m_pTestShowObj);
		SAFE_DELETE( pMarkingTest->m_pvPos);
		SAFE_DELETE( pMarkingTest);
	}
	m_MarkinTestList.RemoveAll();
}

