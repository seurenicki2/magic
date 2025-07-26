#include "stdafx.h"
#include "Furniture.h"
#include "GameResourceManager.h"
#include "cHousingMgr.h"
#include "./Input/UserInput.h"
#include "GameIn.h"
#include "cHousingActionPopupMenuDlg.h"

CFurniture::CFurniture(void)
{
}

CFurniture::~CFurniture(void)
{
}

void CFurniture::Release()
{
	CObject::Release();
}

void CFurniture::InitFurniture(stFurniture* pFurniture)
{
	m_FurnitureInfo = *pFurniture;
}

void CFurniture::InitMarkingInfo()
{
	m_MarkingInfo.Clear();

	if(! m_EngineObject.GetGXOHandle() )
		return;

	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo( m_FurnitureInfo.dwFurnitureIndex );

	if(! stFurnitureInfo )
		return ;

	m_MarkingInfo.bUseMaking = stFurnitureInfo->bTileDisable;

	COLLISION_MESH_OBJECT_DESC CollisionDesc = {0,};
	g_pExecutive->GXOGetCollisionMesh( m_EngineObject.GetGXOHandle() , &CollisionDesc);
	
	const float fMinSize	 = 70.8f;
	float fLengthX = CalcDistanceXZ( &CollisionDesc.boundingBox.v3Oct[5], &CollisionDesc.boundingBox.v3Oct[6] );
	float fLengthZ = CalcDistanceXZ( &CollisionDesc.boundingBox.v3Oct[5] ,&CollisionDesc.boundingBox.v3Oct[1] );

	for( int i=0; i<8;i++)
		m_MarkingInfo.stResizeBoudingBox.v3Oct[i] = CollisionDesc.boundingBox.v3Oct[i];

	if( fLengthX < fMinSize ||  fLengthZ < fMinSize )
	{
		//���� �������� ���̰� �ּұ��̺��� �����ϸ� ��ŷ�� ���� �ȵǹǷ� ����ġ��ŭ �ø����� �ʿ��ϴ�.
		m_MarkingInfo.bUseResize = TRUE;

		//��ŷ������ �������� 
		UpdateMarkingInfo();
	}

	//��ŷ�� ���� �ѹ� 2D�������� ���ؿ���ȴ�. 
	VECTOR3 vObjectPos={0,};
	GetPosition(&vObjectPos);

	/*��ü �ٿ�¡�ڽ����� ������Ʈ Ű�� ������*/
	//fHeightSize = ���� - ������ �Ʒ��� 
	m_MarkingInfo.fHeightSize = m_MarkingInfo.stResizeBoudingBox.v3Oct[0].y - m_MarkingInfo.stResizeBoudingBox.v3Oct[1].y;


	//������Ʈ�� �ڽ��� 4�������� ���Ÿ��� ����ū���� ������ 
	float fMaxLength = 0.0f;
	for(int i=0 ; i < 4 ; i ++ )
	{
		float fCalcLength = CalcDistanceXZ(&vObjectPos,&m_MarkingInfo.stResizeBoudingBox.v3Oct[i]);

		if ( fCalcLength > fMaxLength )
			fMaxLength = fCalcLength;
	}
	
	m_MarkingInfo.f2DRadius = fMaxLength;
}

void CFurniture::UpdateMarkingInfo()
{
	if(! m_EngineObject.GetGXOHandle() )
		return;

	if( ! m_MarkingInfo.bUseMaking || ! m_MarkingInfo.bUseResize )
		return ;

	//��ŷ ���� ���� 
	COLLISION_MESH_OBJECT_DESC CollisionDesc = {0,};
	g_pExecutive->GXOGetCollisionMesh( m_EngineObject.GetGXOHandle() , &CollisionDesc);
	BOUNDING_BOX* pBoundingBox = &(CollisionDesc.boundingBox);

	const float fMinSize	 = 70.8f;
	float fLengthX = CalcDistanceXZ( &CollisionDesc.boundingBox.v3Oct[5], &CollisionDesc.boundingBox.v3Oct[6] );
	float fLengthZ = CalcDistanceXZ( &CollisionDesc.boundingBox.v3Oct[5] ,&CollisionDesc.boundingBox.v3Oct[1] );

	for( int i=0; i<8;i++)
		m_MarkingInfo.stResizeBoudingBox.v3Oct[i] = pBoundingBox->v3Oct[i];

	VECTOR3 DirX ={0,};
	VECTOR3 DirZ ={0,};

	if( fLengthX < fMinSize )
	{
		float fHaffDeltaSize =  (fMinSize - fLengthX) * 0.5f; 
		DirX = m_MarkingInfo.stResizeBoudingBox.v3Oct[6] - m_MarkingInfo.stResizeBoudingBox.v3Oct[5];
		Normalize(&DirX,&DirX);

		m_MarkingInfo.stResizeBoudingBox.v3Oct[1] =  m_MarkingInfo.stResizeBoudingBox.v3Oct[1] + (  DirX * (-fHaffDeltaSize) );
		m_MarkingInfo.stResizeBoudingBox.v3Oct[5] =  m_MarkingInfo.stResizeBoudingBox.v3Oct[5] + (  DirX * (-fHaffDeltaSize) );
		m_MarkingInfo.stResizeBoudingBox.v3Oct[2] =  m_MarkingInfo.stResizeBoudingBox.v3Oct[2] + (  DirX * fHaffDeltaSize);
		m_MarkingInfo.stResizeBoudingBox.v3Oct[6] =  m_MarkingInfo.stResizeBoudingBox.v3Oct[6] + (  DirX * fHaffDeltaSize);

		fLengthX = fMinSize;
	}

	if( fLengthZ < fMinSize )
	{
		float fHaffDeltaSize =  (fMinSize - fLengthZ) * 0.5f; 
		DirZ = m_MarkingInfo.stResizeBoudingBox.v3Oct[1] - m_MarkingInfo.stResizeBoudingBox.v3Oct[5];
		Normalize(&DirZ,&DirZ);

		m_MarkingInfo.stResizeBoudingBox.v3Oct[1] =  m_MarkingInfo.stResizeBoudingBox.v3Oct[1] + (  DirZ * fHaffDeltaSize);
		m_MarkingInfo.stResizeBoudingBox.v3Oct[2] =  m_MarkingInfo.stResizeBoudingBox.v3Oct[2] + (  DirZ * fHaffDeltaSize);
		m_MarkingInfo.stResizeBoudingBox.v3Oct[5] =  m_MarkingInfo.stResizeBoudingBox.v3Oct[5] + (  DirZ * (-fHaffDeltaSize) );
		m_MarkingInfo.stResizeBoudingBox.v3Oct[6] =  m_MarkingInfo.stResizeBoudingBox.v3Oct[6] + (  DirZ * (-fHaffDeltaSize) );

		fLengthZ = fMinSize;
	}

	m_MarkingInfo.stResizeBoudingBox.v3Oct[0].x =  m_MarkingInfo.stResizeBoudingBox.v3Oct[1].x;
	m_MarkingInfo.stResizeBoudingBox.v3Oct[0].z =  m_MarkingInfo.stResizeBoudingBox.v3Oct[1].z;
	m_MarkingInfo.stResizeBoudingBox.v3Oct[3].x =  m_MarkingInfo.stResizeBoudingBox.v3Oct[2].x;
	m_MarkingInfo.stResizeBoudingBox.v3Oct[3].z =  m_MarkingInfo.stResizeBoudingBox.v3Oct[2].z;
	m_MarkingInfo.stResizeBoudingBox.v3Oct[4].x =  m_MarkingInfo.stResizeBoudingBox.v3Oct[5].x;
	m_MarkingInfo.stResizeBoudingBox.v3Oct[4].z =  m_MarkingInfo.stResizeBoudingBox.v3Oct[5].z;
	m_MarkingInfo.stResizeBoudingBox.v3Oct[7].x =  m_MarkingInfo.stResizeBoudingBox.v3Oct[6].x;
	m_MarkingInfo.stResizeBoudingBox.v3Oct[7].z =  m_MarkingInfo.stResizeBoudingBox.v3Oct[6].z;
}

BOOL CFurniture::GetRideNodeNumByMousePoint(LONG MouseX,LONG  MouseY , DWORD* pBoneNumArr)
{
	if( ! m_EngineObject.GetGXOHandle() )
		return FALSE;

	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo( m_FurnitureInfo.dwFurnitureIndex );

	if( ! stFurnitureInfo )
		return FALSE;

	PICK_GXOBJECT_DESC gxoDesc[5] = {0,};
	GetSelectedObject( gxoDesc, MOUSE->GetMouseX(), MOUSE->GetMouseY(), PICK_TYPE_PER_COLLISION_MESH );

	PICK_GXOBJECT_DESC* pgxoDesc = NULL;
	for(int i=0;i<5;i++)
	{
		if(! gxoDesc[i].gxo )
			continue;

		if( gxoDesc[i].gxo ==  m_EngineObject.GetGXOHandle() )
		{
			pgxoDesc = &gxoDesc[i];
			break;
		}
	}

	if( ! pgxoDesc )
		return FALSE;
	
	struct stSortInfo
	{
		float fLength;
		DWORD  dwBoneNumber;
		VECTOR3 vTest;
		stSortInfo(){fLength=100000.0f;dwBoneNumber=0;}
	};

	stSortInfo SortInfo[5];
	cPtrList SortList;
	
	//������� �����ͼ� ��ŷ�� �����ǰ� ���������� �����Ͽ� ����Ʈ�� ���� 
	for(int i=0; i <  stFurnitureInfo->byAttachBoneNum ; i++ )
	{
		char szBoneName[128] = {0,};
		sprintf(szBoneName,"%s0%d",HOUSE_ATTACHBONENAME,i+1);

		SortInfo[i].dwBoneNumber = i+1;

		MATRIX4 AttachBonemat;

		if( g_pExecutive->GXOGetWorldMatrixByName(m_EngineObject.GetGXOHandle(), &AttachBonemat,szBoneName)  )
		{
			VECTOR3 vBonePos = {0};
			vBonePos.x = AttachBonemat._41 ;
			vBonePos.y = AttachBonemat._42 ;
			vBonePos.z = AttachBonemat._43 ;

			//��ŷ��ǥ�� ���� ��ġ�� �����ǥ�� ���Ѵ� 
			VECTOR3 vDeltaPos = vBonePos - pgxoDesc->v3IntersectPoint;
			float fDeltaLength = VECTOR3Length(&vDeltaPos);

			SortInfo[i].fLength = fDeltaLength;
			SortInfo[i].dwBoneNumber = i+1;
			SortInfo[i].vTest = vBonePos;


			BOOL bInsert = FALSE;
			PTRLISTPOS pos = SortList.GetHeadPosition();
			//������ ū�� �ִٸ� �׳���� ������ ����  
			while(pos)
			{
				PTRLISTPOS OldPos = pos;
				stSortInfo* pSortInfo = (stSortInfo*)SortList.GetNext(pos);
				if( SortInfo[i].fLength < pSortInfo->fLength )
				{
					SortList.InsertBefore(OldPos,&SortInfo[i]);
					bInsert = TRUE;
					break;
				}
			}

			//�ڽź��� ū�� ���ٸ� ���� ����
			if( ! bInsert )
				SortList.AddTail(&SortInfo[i]);
		}
	}


	PTRLISTPOS pos = SortList.GetHeadPosition();
	if( !pos)
		return FALSE;

	int n=0;
	while(pos)
	{
		stSortInfo* pSortInfo = (stSortInfo*)SortList.GetNext(pos);
		pBoneNumArr[n] = pSortInfo->dwBoneNumber;
		n++;
	}

	return TRUE;
}