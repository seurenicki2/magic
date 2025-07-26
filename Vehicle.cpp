// ----------------------------- 
// 090422 ShinJS --- Ż�� Class
// ----------------------------- 
#include "stdafx.h"
#include "Vehicle.h"

#include "GameResourceManager.h"
#include "ObjectManager.h"
#include "TileManager.h"
#include "MoveManager.h"
#include "input/UserInput.h"
#include "MHMap.h"
#include "AppearanceManager.h"
#include "MOTIONDESC.h"
#include "ChatManager.h"
#include "ObjectStateManager.h"
#include "MHCamera.h"
#include "WindowIDEnum.h"
#include "Interface/cWindowManager.h"
#include "VehicleManager.h"
#include "PetStateDialog.h"
#include "GameIn.h"
#include "../[cc]skill/client/manager/SkillManager.h"
#include "../[cc]skill/client/info/SkillInfo.h"
#include "CharacterDialog.h"

CVehicle::CVehicle()
{
	m_htRiderInfo.Initialize( VehicleScript::Seat::MaxSize );
}

CVehicle::~CVehicle()
{
	Release();
}

void CVehicle::InitVehicle()
{
	m_bInstalled = FALSE;
	m_dwLastDistCheckTime = 0;
	m_dwRequestGetOnPlayerID = 0;
	ZeroMemory( &mUsedItemToSummon, sizeof( mUsedItemToSummon ) );
	memset( m_MasterName, 0, MAX_NAME_LENGTH+1 );
	m_dwExecutedEffectHandle = 0;
	m_nExecutedEffectNum = -1;
	m_bFollow = TRUE;
}

void CVehicle::Release()
{
	if( HasRider() )
	{
		// Release�� ������ ��� �ڱ��ڽ��� ObjectManager�� ObjectTable�� �̹� ���ŵ� ���¿��� ������ ������
		// GetOff()�� �����ϴ� ��� ���ϴ� ����� ���� �� ����. ���� Detach �۾��� ��������� �Ѵ�
		m_htRiderInfo.SetPositionHead();

		for(DWORD dwPlayerID = DWORD(m_htRiderInfo.GetData());
			0 < dwPlayerID;
			dwPlayerID = DWORD(m_htRiderInfo.GetData()))
		{
			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( dwPlayerID );

			if( pPlayer && pPlayer->GetObjectKind() == eObjectKind_Player )
			{
				// ž������ ��Ų��
				m_EngineObject.DetachRider( pPlayer->GetEngineObject() );

				// Player�� Vehicle���� �ʱ�ȭ
				pPlayer->SetVehicleID( 0 );											// Ż�� ID �ʱ�ȭ
				pPlayer->SetVehicleSeatPos( 0 );									// ž�� ��ġ �ʱ�ȭ
				pPlayer->SetBaseMotion();											// �⺻��� �缳��
				MOVEMGR->MoveStop( pPlayer, NULL );									// ���� �ڸ��� ����

				APPEARANCEMGR->InitAppearance( pPlayer );
			}
		}
		m_htRiderInfo.RemoveAll();
	}

	// 091110 ShinJS --- ���� �������� Effect ����
	if( m_dwExecutedEffectHandle )
	{
		while( EFFECTMGR->ForcedEndEffect( m_dwExecutedEffectHandle ) == FALSE )
		{}
		m_dwExecutedEffectHandle = NULL;
	}
	
	CObject::Release();
}

void CVehicle::Process()
{
	// ��ġ���� ���� ����
	if( !m_bInstalled )
		return;

	CMonster::Process();
	
	if( DoGetMoveSpeed() <= 0.0f ||
		GetOwnerIndex() != gHeroID ||
		m_bDieFlag ||
		IsShocked() )
		return;

	CPlayer* pMaster = (CPlayer*)OBJECTMGR->GetObject(
		GetOwnerIndex());

	if( !pMaster || pMaster->GetObjectKind() != eObjectKind_Player )
	{
		return;
	}

	// Master�� �¼���ġ ���ϱ�
	DWORD dwMasterSeatPos = pMaster->GetVehicleSeatPos();

	if( m_bFollow &&
		dwMasterSeatPos == 0 &&
		gCurTime - m_dwLastDistCheckTime > 1000 )
	{
		VECTOR3 vecMasterPos, vecVehiclePos;

		pMaster->GetPosition( &vecMasterPos );
		GetPosition( &vecVehiclePos );
		float fDist = CalcDistanceXZ( &vecMasterPos, &vecVehiclePos );

		// ������ �Ÿ� Ȯ�νð� ����
		m_dwLastDistCheckTime = gCurTime;

		BASE_MONSTER_LIST* pList = GAMERESRCMNGR->GetMonsterListInfo( GetMonsterKind() );
		ASSERT( pList );
		if( !pList )		return;

		const float fVehicleRadius = float( pList->MonsterRadius );			// Ż�� ������
		fDist -= fVehicleRadius;

		// ���󰡱Ⱑ �ʿ��� ���
		if( fDist > CVehicle::eFollowExecuteDist )
		{
			MOVE_INFO* pMasterMoveInfo = pMaster->GetBaseMoveInfo();

			if( !pMasterMoveInfo )	return;

			// ���󰡱� ��ġ ���ϱ�			
			VECTOR3 vecDir, vecFollowPos, vecUp, vecCross;
			float fRandDist;

			// Master�� �ٶ󺸴� �ݴ���⿡ ��ġ�Ҽ� �ֵ��� �Ѵ�
			vecDir = pMasterMoveInfo->Move_Direction * -1.0f;
			Normalize( &vecDir, &vecDir );

			fRandDist = float( rand() % CVehicle::eFollowRandomDist );

			vecUp.x = 0.0f;		vecUp.y = 1.0f;		vecUp.z = 0.0f;
			CrossProduct( &vecCross, &vecUp, &vecDir );

			// Master�� �ݴ����� Ż���� ��������ġ, 
			vecDir = vecDir * ( fVehicleRadius + fRandDist ) + vecCross * fRandDist * float( rand()%2 ? 1 : -1 );
			vecFollowPos = vecMasterPos + vecDir;

			if( vecFollowPos.x < 0 )				vecFollowPos.x = 0;
			else if( vecFollowPos.x > 51199 )		vecFollowPos.x = 51199;

			if( vecFollowPos.z < 0 )				vecFollowPos.z = 0;
			else if( vecFollowPos.z > 51199 )		vecFollowPos.z = 51199;

			vecFollowPos.x = ((DWORD)(vecFollowPos.x/TILECOLLISON_DETAIL)*TILECOLLISON_DETAIL);
			vecFollowPos.z = ((DWORD)(vecFollowPos.z/TILECOLLISON_DETAIL)*TILECOLLISON_DETAIL);
			vecFollowPos.y = 0.0f;

			// �����̵��� �ʿ��� ���(�ʹ� �־��� ���)
			if( fDist > CVehicle::eRecallExecuteDist )
			{
				MOVE_POS message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category = MP_MOVE;
				message.Protocol = MP_MOVE_VEHICLE_RECALL_SYN;
				message.dwObjectID = pMaster->GetID();
				message.dwMoverID = GetID();
				message.cpos.Compress( &vecFollowPos );
				NETWORK->Send( &message,sizeof( MOVE_POS ) );
			}
			// ���󰡱�
			else
			{
				MOVE_ONETARGETPOS message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category = MP_MOVE;
				message.Protocol = MP_MOVE_VEHICLE_BEGIN_SYN;
				message.dwObjectID = pMaster->GetID();
				message.dwMoverID = GetID();
				message.SetStartPos( &GetCurPosition() );
				message.SetTargetPos( &vecFollowPos );
				NETWORK->Send( &message, sizeof( message ) );
			}
		}
	}
}

void CVehicle::SetMotionInState(EObjectState State)
{
	// 091110 ShinJS --- ����� Beff ������ �̿��Ͽ� ����
	m_ObjectState.State_Start_Motion = -1;
	m_ObjectState.State_Ing_Motion = -1;
	m_ObjectState.State_End_Motion = -1;
	m_ObjectState.State_End_MotionTime = 0;
	m_ObjectState.MotionSpeedRate = 1.0f;

	// ��ġ���� ���� ����
	if( !m_bInstalled )
		return;

	switch(State)
	{
	case eObjectState_Move:
		{
			CObject* const pObject = OBJECTMGR->GetObject(
				GetOwnerIndex());

			if( !pObject )		break;

			switch( pObject->GetBaseMoveInfo()->MoveMode )
			{
			case eMoveMode_Walk:
				{
					ChangeBeff( VehicleScript::Motion::MoveObjectWalk );
				}
				break;

			case eMoveMode_Run:
				{
					ChangeBeff( VehicleScript::Motion::MoveObjectRun );
				}
				break;
			}
		}
		break;
	case eObjectState_None:
		{
			ChangeBeff( VehicleScript::Motion::MoveObjectIdle );
		}
		break;
	}
}

float CVehicle::DoGetMoveSpeed()
{
	const BASE_MONSTER_LIST* const pList = GAMERESRCMNGR->GetMonsterListInfo( GetMonsterKind() );

	if( 0 == pList )
	{
		return 0;
	}

	float moveSpeed = 0;

	switch( GetBaseMoveInfo()->MoveMode )
	{
	case eMoveMode_Walk:
		{
			moveSpeed = pList->WalkMove;
			break;
		}
	case eMoveMode_Run:
		{
			moveSpeed = pList->RunMove;
			break;
		}
	}

	return moveSpeed;
}

void CVehicle::SetPosToMousePos()
{
	// ��ġ���� ���� ����
	if( !m_bInstalled )
	{
		CObject* pOverObject = (CObject*)GetSelectedObject( MOUSE->GetMouseX(), MOUSE->GetMouseY() );

		// ���õ� ������Ʈ�� �����ϰ� ���� ��ġ�� Ż��Object�� �ƴѰ�� �׸��� �ʴ´�
		if( pOverObject && pOverObject != this )
		{
			if( m_EngineObject.IsShow() )
				m_EngineObject.Hide();
			return;
		}

		VECTOR3 *vecPickPos;
		vecPickPos = GetPickedPosition( MOUSE->GetMouseX(), MOUSE->GetMouseY() );
		if( vecPickPos )
		{	
			if( !m_EngineObject.IsShow() )
				m_EngineObject.Show();

			MOVEMGR->SetPosition( this, vecPickPos );		// �̵������� ��ġ�� �����Ѵ�.
		}

		return;
	}
}

BOOL CVehicle::IsValidPosForInstall()
{
	// Ż�� Object�� �� Ÿ�� �˻�
	COLLISION_MESH_OBJECT_DESC ColMeshDescVehicle;
	g_pExecutive->GXOGetCollisionMesh( GetEngineObject()->GetGXOHandle(), &ColMeshDescVehicle);
	
	VECTOR3 vecVehicleMin, vecVehicleMax;
	vecVehicleMin.x = vecVehicleMin.z = 3.402823466e+38F;	// FLT_MAX
	vecVehicleMax.x = vecVehicleMax.z = 1.175494351e-38F;	// FLT_MIN
	vecVehicleMin.y = vecVehicleMax.y = 0.0f;

	for( int i=0 ; i<8 ; ++i )
	{
		if( ColMeshDescVehicle.boundingBox.v3Oct[i].x < vecVehicleMin.x )		vecVehicleMin.x = ColMeshDescVehicle.boundingBox.v3Oct[i].x;
		if( ColMeshDescVehicle.boundingBox.v3Oct[i].x > vecVehicleMax.x )		vecVehicleMax.x = ColMeshDescVehicle.boundingBox.v3Oct[i].x;

		if( ColMeshDescVehicle.boundingBox.v3Oct[i].z < vecVehicleMin.z )		vecVehicleMin.z = ColMeshDescVehicle.boundingBox.v3Oct[i].z;
		if( ColMeshDescVehicle.boundingBox.v3Oct[i].z > vecVehicleMax.z )		vecVehicleMax.z = ColMeshDescVehicle.boundingBox.v3Oct[i].z;
	}

	BOUNDING_SPHERE sphere;
	sphere.fRs = TILECOLLISON_DETAIL;
	sphere.v3Point.y = ColMeshDescVehicle.boundingSphere.v3Point.y;
	for( float x=vecVehicleMin.x ; x < vecVehicleMax.x ; x += TILECOLLISON_DETAIL )
	{
		for( float z=vecVehicleMin.z ; z < vecVehicleMax.z ; z += TILECOLLISON_DETAIL )
		{
			sphere.v3Point.x = x;
			sphere.v3Point.z = z;

			// üũ�ؾ��ϴ� Tile �� ���
			if( IsCollisionSphereAndBox( &sphere, &ColMeshDescVehicle.boundingBox ) )
			{
				// Tile �˻�
				if( MAP->CollisionTilePos( int(sphere.v3Point.x), int(sphere.v3Point.z), MAP->GetMapNum(), this ) )
					return FALSE;
			}
		}
	}

	// NPC�� �浹 �˻�
	VECTOR3 vecVehiclePos, vecObjPos;
	CYHHashTable<CObject>& htObjectTable = OBJECTMGR->GetObjectTable();

	htObjectTable.SetPositionHead();

	for(CObject* pObject = htObjectTable.GetData();
		0 < pObject;
		pObject = htObjectTable.GetData())
	{
		if( pObject->GetObjectKind() == eObjectKind_Npc )
		{
			// 090604 LUJ, �浹�ڽ��� �����Ǿ� ���� ���� NPC�� ���� ��찡 �����Ƿ� �˻��Ѵ�
			COLLISION_MESH_OBJECT_DESC ColMeshDesc = { 0 };
			CEngineObject* const engineObject = pObject->GetEngineObject();

			if( 0 == engineObject )
			{
				continue;
			}
			else if( 0 == engineObject->GetGXOHandle() )
			{
				continue;
			}

			g_pExecutive->GXOGetCollisionMesh( engineObject->GetGXOHandle(), &ColMeshDesc);

			VECTOR3 vecObjMin, vecObjMax;
			vecObjMin.x = vecObjMin.z = 3.402823466e+38F;	// FLT_MAX
			vecObjMax.x = vecObjMax.z = 1.175494351e-38F;	// FLT_MIN
			vecObjMin.y = vecObjMax.y = 0.0f;

			for( int i=0 ; i<8 ; ++i )
			{
				if( ColMeshDesc.boundingBox.v3Oct[i].x < vecObjMin.x )		vecObjMin.x = ColMeshDesc.boundingBox.v3Oct[i].x;
				if( ColMeshDesc.boundingBox.v3Oct[i].x > vecObjMax.x )		vecObjMax.x = ColMeshDesc.boundingBox.v3Oct[i].x;

				if( ColMeshDesc.boundingBox.v3Oct[i].z < vecObjMin.z )		vecObjMin.z = ColMeshDesc.boundingBox.v3Oct[i].z;
				if( ColMeshDesc.boundingBox.v3Oct[i].z > vecObjMax.z )		vecObjMax.z = ColMeshDesc.boundingBox.v3Oct[i].z;
			}
			
			VECTOR3 vecTmp;
			vecObjPos = ColMeshDesc.boundingSphere.v3Point;
			vecVehiclePos = ColMeshDescVehicle.boundingSphere.v3Point;
			vecObjPos.y = vecVehiclePos.y = 0.0f;
			vecTmp = vecObjPos - vecVehiclePos;
			float fObjDist = VECTOR3Length( &vecTmp );					// Object�� Ż�Ͱ��� �Ÿ�

			vecTmp = vecObjMax - vecObjMin;
			float fObjRadius = VECTOR3Length( &vecTmp ) / 2.0f;			// Object�� ������

			vecTmp = vecVehicleMax - vecVehicleMin;
			float fVehicleRadius = VECTOR3Length( &vecTmp ) / 2.0f;		// Vehicle�� ������

			if( fObjDist < fVehicleRadius + fObjRadius )
				return FALSE;
		}
	}
	return TRUE;
}

void CVehicle::GetOn( CPlayer* pPlayer, DWORD dwSeatPos )
{
	if( 0 == pPlayer )
	{
		return;
	}

	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() );

	if( dwSeatPos == 0 ||
		dwSeatPos > script.mSeatSize )
	{
		return;
	}
	// ž���ο� �ʰ�
	else if( m_htRiderInfo.GetDataNum() >= script.mSeatSize )
	{
		return;
	}

	// 090316 LUJ, ������Ʈ�� �����Ǿ� �ʱ�ȭ�� �� �ִ�. ���Ŀ��� ž�� ó���� �ǵ��� ���� �����Ѵ�.
	pPlayer->SetVehicleID( GetID() );
	pPlayer->SetVehicleSeatPos( dwSeatPos );

	// 090424 ShinJS --- ž���� �߰�(�ߺ����� �ʵ��� ������ ��ġ�Ҷ� Attach �Ѵ�)
	if( 0 == m_htRiderInfo.GetData( dwSeatPos ) &&			// �ش� �¼��� ž���ڰ� ����
		pPlayer->GetEngineObject()->IsInited() &&			// ž���ڿ�
		TRUE == GetEngineObject()->IsInited() )				// Ż���� CEngineObject�� Init�Ǿ� �ִ°��(Attach����)
	{
		// 090422 ShinJS --- Ż���� �������̰� ž���ڰ� ���ٰ� ž���� ��� �ִϸ��̼��� �������ش�
		if( DoGetMoveSpeed() <= 0.0f )
		{
			// ž���ڸ� �߰����� ž���ڰ� �������� �Ǵ�
			if( !HasRider() )
			{
				// ������ ��� �ִϸ��̼� ����
				// 091110 ShinJS --- ����� Beff ������ �̿��Ͽ� ����
				ChangeBeff( VehicleScript::Motion::FixObjectPlay );
			}

			// 091215 ShinJS --- �������� ��� Effect�� �����
			pPlayer->HideEffect( TRUE );

			// 091215 ShinJS --- �������� ��� ���� AI ���¸� Stand�� �����Ѵ�.
			if( pPlayer->GetID() == HEROID && HEROPET )
			{
				HEROPET->SetAI( ePetAI_Stand );
			}
		}
		// 100326 ShinJS --- ž�� �Ϸ��� ��������� ��� ����
		else
		{	
			SetMotionInState( m_BaseObjectInfo.ObjectState );
		}

		m_htRiderInfo.Add( ( DWORD* )pPlayer->GetID(), dwSeatPos );			// ž���� �߰�
		
		m_EngineObject.AttachRider(
			pPlayer->GetEngineObject(),
			const_cast< char* >( script.mSeat[ dwSeatPos - 1 ].mAttachedName ) );

		pPlayer->SetBaseMotion();
		OBJECTSTATEMGR->InitObjectState(pPlayer);
		// 090428 ShinJS --- Ż���� �������� ž������ ������ �����Ѵ�
		pPlayer->SetAngle( GetAngle() );

		// 091105 ShinJS --- ���� ����
		APPEARANCEMGR->HideWeapon( pPlayer );
	}

	if( pPlayer->GetID() == HEROID )
	{
		GAMEIN->GetCharacterDialog()->RefreshInfo();
	}
}

void CVehicle::GetOffAll()
{
	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() );

	for( DWORD seatIndex = 1; seatIndex <= script.mSeatSize; ++seatIndex )
	{
		CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( DWORD(m_htRiderInfo.GetData( seatIndex )) );
		GetOff( pPlayer, seatIndex );
	}
}

BOOL CVehicle::GetOff( CPlayer* pPlayer, DWORD dwSeatPos )
{
	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() );

	// ���� Ȯ�� (������ �¼���ȣ Ȯ��)
	if( dwSeatPos == 0 || dwSeatPos > script.mSeatSize )		
		return FALSE;

	// 090427 ShinJS --- Player Ȯ��
	DWORD dwPlayerID = (DWORD)m_htRiderInfo.GetData( dwSeatPos );
	if( !pPlayer || 
		pPlayer->GetObjectKind() != eObjectKind_Player || 
		pPlayer->GetID() != dwPlayerID )
		return FALSE;

	m_EngineObject.DetachRider( pPlayer->GetEngineObject() );
	
	// ž���� ����
	m_htRiderInfo.Remove( dwSeatPos );

	// Master�� �ƴѰ�� Ż��ID���� �ʱ�ȭ
	if(dwPlayerID != GetOwnerIndex())
		pPlayer->SetVehicleID( 0 );										// Ż�� ID �ʱ�ȭ
	pPlayer->SetVehicleSeatPos( 0 );									// ž�� ��ġ �ʱ�ȭ
	pPlayer->SetBaseMotion();											// �⺻��� �缳��

	MOVEMGR->MoveStop( pPlayer, NULL );									// ���� �ڸ��� ����

	// 091105 ShinJS --- ���� ���̱�
	APPEARANCEMGR->ShowWeapon( pPlayer );

	APPEARANCEMGR->InitAppearance( pPlayer );							// ž������ �� �ٸ� Player�� ��� ������ �ʾ� ���̰� ��

	// 090422 ShinJS --- ž���ڰ� HERO�� ��� ī�޶� FollowView�� �ѹ� �����Ѵ�
    if( pPlayer->GetID() == HEROID )
	{
		CAMERA->ExecuteOnceFollowView();
		GAMEIN->GetCharacterDialog()->RefreshInfo();

		// ����ȿ���� ������ �޽������.
		DWORD dwSkillIndex = 0;
		for(int i=0; i<VehicleScript::Condition::BuffMaxSize; i++)
		{
			dwSkillIndex = script.mCondition.mBuffSkillIndex[i];
			cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo(dwSkillIndex);
			if(pSkillInfo)
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2004 ), pSkillInfo->GetName());
		}

		GAMEIN->GetCharacterDialog()->RefreshInfo();
	}

	// 090422 ShinJS --- Ż���� �������̰� ž���ڰ� �������� �ʴ� ���°� �� ��� �ִϸ��̼��� ������Ų��
	if( DoGetMoveSpeed() <= 0.0f )
	{
		// ž���ڸ� �����Ŀ� ž���ڰ� ������ �Ǵ�
		if( !HasRider() )
		{
			// ���� ��� �ִϸ��̼� Stop�� ����
			// 091110 ShinJS --- ����� Beff ������ �̿��Ͽ� ����
			ChangeBeff( VehicleScript::Motion::FixObjectStop );
		}

		// 091215 ShinJS --- �������� ��� Effect ������¸� �����Ѵ�.(ĳ���Ͱ� Hide ���°� �ƴѰ�� �����Ѵ�)
		if( pPlayer->GetCharacterTotalInfo()->bVisible != FALSE )
			pPlayer->HideEffect( FALSE );

		// 091215 ShinJS --- �������� ��� HEROPET�� AI���¸� ������� �ǵ�����.
		if( pPlayer->GetID() == HEROID && HEROPET )
		{
			if(CPetStateDialog* pPetStateDialog = GAMEIN->GetPetStateDlg())
			{
				HEROPET->SetAI(
					ePetAI(pPetStateDialog->GetAISelectedIdx()));
			}
		}
	}

	return TRUE;
}

BOOL CVehicle::IsGetOn( CPlayer* pPlayer )
{	
	ASSERT( pPlayer );
	if( !pPlayer )		return FALSE;

	DWORD dwSeatPos = pPlayer->GetVehicleSeatPos();
	if( DWORD(m_htRiderInfo.GetData( dwSeatPos )) == pPlayer->GetID() )
		return TRUE;

	return FALSE;
}

DWORD CVehicle::CanGetOn( CPlayer* pPlayer )
{
	// Player ���� �˻�
	switch( pPlayer->GetState() )
	{
	case eObjectState_Rest:
	case eObjectState_Exchange:
	case eObjectState_StreetStall_Owner:
	case eObjectState_StreetStall_Guest:
	case eObjectState_PrivateWarehouse:
	case eObjectState_Munpa:

	case eObjectState_SkillStart:
	case eObjectState_SkillSyn:
	case eObjectState_SkillBinding:
	case eObjectState_SkillUsing:
	case eObjectState_SkillDelay:
	
	case eObjectState_TiedUp_CanMove:
	case eObjectState_TiedUp_CanSkill:
	case eObjectState_TiedUp:
	
	case eObjectState_Die:
	case eObjectState_Fishing:
	case eObjectState_FishingResult:

	case eObjectState_Cooking:
	case eObjectState_ItemUse:
	case eObjectState_Society:
	case eObjectState_Exit:
	case eObjectState_BattleReady:
	case eObjectState_Deal:
		return 0;
	}

	// 091106 ShinJS --- PK ������ ž�� �Ұ�
	if( pPlayer->IsPKMode() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1263 ) );
		return 0;
	}

	// Ż���� ž�°��� ������ �÷��̾� ���� �� �˻�
	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() );
	
	if( pPlayer->GetLevel() < script.mCondition.mPlayerMinimumLevel )
	{
		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			CHATMGR->GetChatMsg(2210),
			script.mCondition.mPlayerMinimumLevel,
			script.mCondition.mPlayerMaximumLevel);
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			text);
		return 0;
	}
	else if( pPlayer->GetLevel() > script.mCondition.mPlayerMaximumLevel )
	{
		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			CHATMGR->GetChatMsg(2210),
			script.mCondition.mPlayerMinimumLevel,
			script.mCondition.mPlayerMaximumLevel);
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			text);
		return 0;
	}
	else if( pPlayer->GetID() != GetOwnerIndex() &&
			pPlayer->GetVehicleID() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1834 ) );
		return 0;
	}

	// �̹� ž������ ��� (�˾��� ��ũ��Ű�� ���۽� ����)
	if( IsGetOn( pPlayer ) )
	{
		// �޼��� ���� ó�� ���� �ʵ��� �Ѵ�
		return 0;
	}

	// 090922 ShinJS --- HERO�� ��� ����/����/��æ/��ȭ ���̾�αװ� Ȱ��ȭ ���� ��� ž������ ���ϵ��� �Ѵ�.
	if( pPlayer->GetID() == HEROID )
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
			return 0;
		}

		if(	pMixDialog->IsActive()			|| 
			pDissolutioniDialog->IsActive() || 
			pReinforceDialog->IsActive()	|| 
			pUpgradeDialog->IsActive()		)
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1104 ) );
			return 0;
		}
	}

	// Ż�Ͱ� HERO�� �Ÿ� Ȯ��	
	VECTOR3 vecPlayerPos, vecVehiclePos;
	pPlayer->GetPosition( &vecPlayerPos );
	GetPosition( &vecVehiclePos );
	float fDist = CalcDistanceXZ( &vecPlayerPos, &vecVehiclePos );

	if( fDist > script.mCondition.mSummonRange )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1825 ) );
		return 0;
	}

	// 091013 ShinJS --- �ð� �˻� ���� Ȯ��
	if( VEHICLEMGR->IsCheckTime() )
	{
		// Ż�� ��ž�� ���� �ð� Ȯ��
		DWORD dwGetOffElapsedTime = gCurTime - pPlayer->GetLastVehicleGetOffTime();
		if( dwGetOffElapsedTime < CVehicle::eGetOnLimitTime )
		{
			float fGetOffElapsedTime = float( CVehicle::eGetOnLimitTime - dwGetOffElapsedTime ) / 1000.0f;
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1822 ), fGetOffElapsedTime );
			return 0;
		}
	}

	// 090428 ShinJS --- Ż���� ��ġ�� ��ȿ���� Ȯ��
	if( MAP->CollisionTilePos( int(vecVehiclePos.x), int(vecVehiclePos.z), MAP->GetMapNum(), this ) )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1840 ) );
		return 0;
	}

	// 090422 ShinJS --- ����ִ� �¼� ���ϱ�
	return GetEmptySeatPos( pPlayer->GetID() );
}

// 090423 ShinJS --- ���� ���� ���� Ȯ��
BOOL CVehicle::CanGetOff( CPlayer* pPlayer )
{
	// Player ���� �˻�
	switch( pPlayer->GetState() )
	{
	case eObjectState_Rest:
	case eObjectState_Exchange:
	case eObjectState_StreetStall_Owner:
	case eObjectState_StreetStall_Guest:
	case eObjectState_PrivateWarehouse:
	case eObjectState_Munpa:

	case eObjectState_SkillStart:
	case eObjectState_SkillSyn:
	case eObjectState_SkillBinding:
	case eObjectState_SkillUsing:
	case eObjectState_SkillDelay:
	
	case eObjectState_TiedUp_CanMove:
	case eObjectState_TiedUp_CanSkill:
	case eObjectState_TiedUp:
	
	case eObjectState_Die:
	case eObjectState_Fishing:
	case eObjectState_FishingResult:

	case eObjectState_Cooking:
	case eObjectState_ItemUse:
	case eObjectState_Society:
	case eObjectState_Exit:
	case eObjectState_BattleReady:
	case eObjectState_Deal:
		return FALSE;
	}

	// ž�������� ���� ���
	if( !IsGetOn( pPlayer ) )
	{
		return FALSE;
	}

	return TRUE;
}

DWORD CVehicle::GetEmptySeatPos( DWORD dwPlayerID )
{
	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() );
	
	// Master�� ��� Control �¼� ����
	if( GetOwnerIndex() == dwPlayerID )
	{
		for( DWORD dwSeatPos=1 ; dwSeatPos <= script.mSeatSize ; ++dwSeatPos )
		{
			if( script.mSeat[ dwSeatPos - 1 ].mIsEnableControl )
				return dwSeatPos;
		}
	}
    
	// ž���ο� �ʰ�
	if( m_htRiderInfo.GetDataNum() >= script.mSeatSize )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1824 ) );		// ž���ο� �ʰ� �޼���
		return 0;
	}

	std::vector<DWORD> vEmptySeatPos;
	DWORD dwEmptyPos = 0;
	BYTE byPassengerCnt = 0;

	// ����ִ� �¼� Ȯ��
	for( dwEmptyPos=1 ; dwEmptyPos <= script.mSeatSize ; ++dwEmptyPos )
	{
		// Control �¼��� �ƴϰ� �¼���ġ�� ž���ڰ� ���°��
		if( !script.mSeat[ dwEmptyPos - 1 ].mIsEnableControl )
		{
			// 091214 ShinJS --- Control �¼��� �ƴ� �¼� Count
			++byPassengerCnt;

			if(	DWORD(m_htRiderInfo.GetData( dwEmptyPos )) == 0 )
				vEmptySeatPos.push_back( dwEmptyPos );
		}
	}

	// �߸��� ������ ����
	if( vEmptySeatPos.empty() )
	{
		if( byPassengerCnt > 0 )
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1824 ) );		// ž���ο� �ʰ� �޼���
		else
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2003 ) );		// "�����ָ� ž�� �Ͻ� �� �ֽ��ϴ�."

		return 0;
	}

	// ž����ġ�� �����ϰ� ���Ѵ�
	int nRandPos = rand() % vEmptySeatPos.size();
	dwEmptyPos = vEmptySeatPos[ nRandPos ];
	vEmptySeatPos.clear();
	
	return dwEmptyPos;
}

BOOL CVehicle::CanControl( CPlayer* pPlayer )
{
	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() );
	
	// �¼� ������ �ִ� ��� Script�� Control ������ ��ȯ�Ѵ�
	const DWORD dwSeatPos = pPlayer->GetVehicleSeatPos();
	if( dwSeatPos )
		return script.mSeat[ dwSeatPos - 1 ].mIsEnableControl;

	return FALSE;
}

void CVehicle::Move_OneTarget( MOVE_ONETARGETPOS_FROMSERVER* pMsg )
{
	// Vehicle �̵�ó��
	pMsg->dwObjectID = GetID();						// �̵� Object ����
	MOVEMGR->Move_OneTarget( (void*)pMsg );				// �̵�

	m_htRiderInfo.SetPositionHead();

	for(DWORD dwPlayerID = DWORD(m_htRiderInfo.GetData());
		0 < dwPlayerID;
		dwPlayerID = DWORD(m_htRiderInfo.GetData()))
	{
		// Master�� ��� �̵�ó���� �����Ѵ�
		if( dwPlayerID == GetOwnerIndex() )
			continue;

		CObject* pObject = OBJECTMGR->GetObject( dwPlayerID );
		ASSERT( pObject );
		if( !pObject || pObject->GetObjectKind() != eObjectKind_Player )
			continue;

		// ž���� �̵�ó��
		pMsg->dwObjectID = dwPlayerID;				// �̵� Object ����
		MOVEMGR->Move_OneTarget( (void*)pMsg );		// �̵�
	}
}

void CVehicle::StartMove( VECTOR3* pTargetPos )
{
	MOVE_INFO* pMoveInfo = GetBaseMoveInfo();
	pMoveInfo->InitTargetPosition();
	pMoveInfo->SetTargetPosition( 0, *pTargetPos );
	pMoveInfo->SetMaxTargetPosIdx( 1 );

	MOVEMGR->StartMoveEx( this, NULL, gCurTime );

	m_htRiderInfo.SetPositionHead();

	for(DWORD dwPlayerID = DWORD(m_htRiderInfo.GetData());
		0 < dwPlayerID;
		dwPlayerID = DWORD(m_htRiderInfo.GetData()))
	{
		// Master�� ��� �̵�ó���� �����Ѵ�
		if( dwPlayerID == GetOwnerIndex() )
			continue;

		CObject* pObject = OBJECTMGR->GetObject( dwPlayerID );
		if( !pObject || pObject->GetObjectKind() != eObjectKind_Player )
			continue;

		// ž���� �̵�ó��
		pMoveInfo = pObject->GetBaseMoveInfo();
		pMoveInfo->InitTargetPosition();
		pMoveInfo->SetTargetPosition( 0, *pTargetPos );
		pMoveInfo->SetMaxTargetPosIdx( 1 );

		MOVEMGR->StartMoveEx( pObject, NULL, gCurTime );
	}
}

void CVehicle::MoveStop( VECTOR3* pVecPos )
{
	// Vehicle Stop ó��
	MOVEMGR->MoveStop( this, pVecPos );

	m_htRiderInfo.SetPositionHead();

	for(DWORD dwPlayerID = DWORD(m_htRiderInfo.GetData());
		0 < dwPlayerID;
		dwPlayerID = DWORD(m_htRiderInfo.GetData()))
	{
		// Master�� ��� �̵�ó���� �����Ѵ�
		if( dwPlayerID == GetOwnerIndex() )
			continue;

		CObject* pObject = OBJECTMGR->GetObject( dwPlayerID );
		ASSERT( pObject );
		if( !pObject || pObject->GetObjectKind() != eObjectKind_Player )
			continue;

		// ž���� Stopó��
		MOVEMGR->MoveStop( pObject, pVecPos );
	}

}

MONEYTYPE CVehicle::GetTollMoney()
{
	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() );
	
	return script.mCondition.mTollMoney;
}

// 091109 ShinJS --- Ż�� ��������� ���� Beff�����̸��� ��ȯ
const char* CVehicle::GetBeffFileNameFromMotion( DWORD dwMotionType )
{
	VehicleScript& script = const_cast<VehicleScript&>( GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() ) );
	typedef std::multimap<DWORD, std::string>::iterator BeffFileIter;
	std::pair<BeffFileIter, BeffFileIter> EqualRange;

	// ���õ� MotionType (Stop, Play, Idle, Walk, Run)�� BeffFile �̸����� ���Ѵ�.
    EqualRange = script.mMotion.mBeffFileList.equal_range( dwMotionType );

	// BeffFile �̸��� ���� ���
	if( EqualRange.first == script.mMotion.mBeffFileList.end() &&
		EqualRange.second == script.mMotion.mBeffFileList.end() )
		return "";

	// BeffFile�� 1���� ��� �ش� BeffFile �̸� ��ȯ
	int nFileCnt = (int)std::distance( EqualRange.first, EqualRange.second );
	if( nFileCnt == 1 )
		return (EqualRange.first)->second.c_str();

	// 2�� �̻��� ��� �����ϰ� BeffFile �̸� ��ȯ
	int nRandPos = rand() % nFileCnt;
	std::advance( EqualRange.first, nRandPos );

	return (EqualRange.first)->second.c_str();
}

void CVehicle::ChangeBeff( DWORD dwMotionType )
{
	// MotionType�� ���� EffectNum�� ���Ѵ�.
	int nEffectNum = FindEffectNum( (char*)GetBeffFileNameFromMotion( dwMotionType ) );

	// ���� ����� Effect�� �ƴѰ��
	if( m_nExecutedEffectNum != nEffectNum )
	{
		// ���� �������� Effect ����
		if( m_dwExecutedEffectHandle )
			EFFECTMGR->ForcedEndEffect( m_dwExecutedEffectHandle );

		// Effect ����
		TARGETSET set;
		set.pTarget = this;
		m_dwExecutedEffectHandle = EFFECTMGR->StartEffectProcess( nEffectNum, this, &set, 1, GetID() );

		// ���� Effect ����
		m_nExecutedEffectNum = nEffectNum;
	}
}