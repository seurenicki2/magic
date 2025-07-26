// CameraEffectUnit.cpp: implementation of the CCameraEffectUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CameraEffectUnit.h"
#include "CameraEffectUnitDesc.h"
#include "..\ObjectManager.h"
#include "..\MHCamera.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCameraEffectUnit::CCameraEffectUnit()
{
	
}

CCameraEffectUnit::~CCameraEffectUnit()
{
}

void CCameraEffectUnit::Initialize(CEffectUnitDesc* pEffectUnitDesc,EFFECTPARAM* pEffectParam)
{
	CCameraEffectUnitDesc* pDesc = (CCameraEffectUnitDesc*)pEffectUnitDesc;

	m_CameraEffectUnitType = pDesc->m_CameraEffectUnitType;

	switch( m_CameraEffectUnitType )
	{
	case eCameraEffectUnitType_AngleDist:
		{
			CAMERA->InitCamera(pDesc->m_CameraNum,pDesc->m_InitAngleX,pDesc->m_InitAngleY,pDesc->m_InitDistance,(CObject*)NULL);
		}
		break;
	case eCameraEffectUnitType_Filter:
		{
			m_dwFilterIndex = pDesc->m_dwFilterIndex;
			m_bFilterRepeat = pDesc->m_bFilterRepeat;
			m_fFilterFadeTime = pDesc->m_fFilterFadeTime / 1000.f;
			m_bFilterDetachAll = pDesc->m_bFilterDetachAll;
			m_bFilterOnlyOne = pDesc->m_bFilterOnlyOne;
			m_bFilterRelease = pDesc->m_bFilterRelease;
			m_dwFilterRemainTime = pDesc->m_dwFilterRemainTime;
		}
		break;
	}

	m_dwCameraFilterObjectIndex = 0;

	CEffectUnit::Initialize(pEffectUnitDesc,pEffectParam);
}
void CCameraEffectUnit::Release()
{
	switch( m_CameraEffectUnitType )
	{
	case eCameraEffectUnitType_Filter:
		{
			// 100608 ShinJS --- Release������ Filter�� ������ ��� ���Ž�Ų��.
			if( m_bFilterRelease &&
				m_dwCameraFilterObjectIndex )
			{
				const BOOL bFade = (m_fFilterFadeTime > 0.f);
				CAMERA->DetachCameraFilter(  m_dwFilterIndex, m_dwCameraFilterObjectIndex, bFade, m_fFilterFadeTime );
			}
		}
		break;
	}

	CCameraEffectUnitDesc::ReleaseUnit(this);
}

void CCameraEffectUnit::Reset()
{
	switch( m_CameraEffectUnitType )
	{
	case eCameraEffectUnitType_Filter:
		{
			if( m_dwCameraFilterObjectIndex )
			{
				const BOOL bFade = (m_fFilterFadeTime > 0.f);
				CAMERA->DetachCameraFilter(  m_dwFilterIndex, m_dwCameraFilterObjectIndex, bFade, m_fFilterFadeTime );
			}
		}
		break;
	}


	CEffectUnit::Reset();
}

BOOL CCameraEffectUnit::On(EFFECTPARAM* pParam)
{
	CCameraEffectUnitDesc* pDesc = (CCameraEffectUnitDesc*)m_pEffectUnitDesc;

	switch( m_CameraEffectUnitType )
	{
	case eCameraEffectUnitType_AngleDist:
		{
			CObject* pObject = NULL;
			if(pDesc->IsDangledToOperator())	// ���۷����� or Ÿ��
				pObject = pParam->m_pOperator;
			else
				pObject = pParam->m_pTargetSet[0].pTarget;

			if( pObject == HEROPETBACKUP )
				return TRUE;

			if(pObject->IsInited() == FALSE)	// �ɸ��Ͱ� �������� ������ return
				return TRUE;
			if((pParam->m_dwFlag & EFFECT_FLAG_HEROATTACK) == FALSE)
				return TRUE;

			CAMERA->SetCameraPivotObject(pDesc->m_CameraNum,pObject);

			if(pDesc->m_bRelatedCoordinate)		// ��밢���� ��ȯ
			{
				float angleDeg = pDesc->m_InitAngleY+RADTODEG(pObject->GetAngle());
				CAMERA->SetTargetAngleY(pDesc->m_CameraNum,angleDeg,0);
			}

			CAMERA->SetCurCamera(pDesc->m_CameraNum);
		}
		break;
	case eCameraEffectUnitType_Filter:
		{
			int objectnum;
			TARGETSET* pObject;
			GetObjectSet(pParam,&pObject,&objectnum);

			// ����� �ڱ��ڽ��� ��쿡�� ���͸� �����Ų��.
			for(int n=0;n<objectnum;++n)
			{
				if( pObject[n].pTarget->GetID() != HEROID )
					continue;

				const BOOL bFade = (m_fFilterFadeTime > 0.f);

				if( m_bFilterOnlyOne )
					CAMERA->DetachCameraFilter( m_dwFilterIndex, bFade, m_fFilterFadeTime );

				CCameraFilterObject& cameraFilterObject = CAMERA->AttachCameraFilter( m_dwFilterIndex, !m_bFilterRepeat, bFade, m_fFilterFadeTime, m_dwFilterRemainTime );
				m_dwCameraFilterObjectIndex = cameraFilterObject.GetObjectIndex();
			}
		}
		break;
	}

	return TRUE;
}
BOOL CCameraEffectUnit::Off(EFFECTPARAM* pParam)
{
	CCameraEffectUnitDesc* pDesc = (CCameraEffectUnitDesc*)m_pEffectUnitDesc;

	switch( m_CameraEffectUnitType )
	{
	case eCameraEffectUnitType_AngleDist:
		{
			CObject* pObject = NULL;
			if(pDesc->IsDangledToOperator())	// ���۷����� or Ÿ��
				pObject = pParam->m_pOperator;
			else
				pObject = pParam->m_pTargetSet[0].pTarget;

			if( pObject == HEROPETBACKUP )
				return TRUE;

			if(pObject->IsInited() == FALSE)	// �ɸ��Ͱ� �������� ������ return
				return TRUE;	

			if(pParam->m_pOperator->GetID() != HERO->GetID() &&
				pParam->m_pTargetSet[0].pTarget->GetID() != HERO->GetID() )		// if He is not Hero, just return.
				return TRUE;
			CAMERA->ChangeToDefaultCamera();
		}
		break;
	case eCameraEffectUnitType_Filter:
		{
			if( !m_bFilterDetachAll && m_dwCameraFilterObjectIndex == 0 )
				break;

			int objectnum;
			TARGETSET* pObject;
			GetObjectSet(pParam,&pObject,&objectnum);

			// ����� �ڱ��ڽ��� ��쿡�� ���͸� �����Ų��.
			for(int n=0;n<objectnum;++n)
			{
				if( pObject[n].pTarget->GetID() != HEROID )
					continue;

				const BOOL bFade = (m_fFilterFadeTime > 0.f);
				if( m_bFilterDetachAll )
					CAMERA->DetachCameraFilter( m_dwFilterIndex, bFade, m_fFilterFadeTime );
				else
					CAMERA->DetachCameraFilter(  m_dwFilterIndex, m_dwCameraFilterObjectIndex, bFade, m_fFilterFadeTime );
			}
		}
		break;
	}

	return TRUE;
}

BOOL CCameraEffectUnit::CameraRotate(EFFECTPARAM* pParam,float fRotateAngle,DWORD Duration)
{
	CCameraEffectUnitDesc* pDesc = (CCameraEffectUnitDesc*)m_pEffectUnitDesc;

	CAMERA->Rotate(pDesc->m_CameraNum,fRotateAngle,Duration);

	return TRUE;
}

BOOL CCameraEffectUnit::CameraAngle(EFFECTPARAM* pParam,float fTargetAngle,DWORD Duration)
{
	CCameraEffectUnitDesc* pDesc = (CCameraEffectUnitDesc*)m_pEffectUnitDesc;

	CAMERA->SetTargetAngleX(pDesc->m_CameraNum,fTargetAngle,Duration);

	return TRUE;
}

BOOL CCameraEffectUnit::CameraZoom(EFFECTPARAM* pParam,float fZoomDistance,DWORD Duration)
{
	CCameraEffectUnitDesc* pDesc = (CCameraEffectUnitDesc*)m_pEffectUnitDesc;

	CAMERA->ZoomDistance(pDesc->m_CameraNum,fZoomDistance,Duration);

	return TRUE;
}

BOOL CCameraEffectUnit::ChangeCamera(EFFECTPARAM* pParam,int Camera,DWORD Duration)
{
	CCameraEffectUnitDesc* pDesc = (CCameraEffectUnitDesc*)m_pEffectUnitDesc;

	CObject* pObject;
	if(pDesc->IsDangledToOperator())	// ���۷����� or Ÿ��
		pObject = pParam->m_pOperator;
	else
		pObject = pParam->m_pTargetSet[0].pTarget;

	if( pObject == HEROPETBACKUP )
		return TRUE;

	if(pObject->IsInited() == FALSE)	// �ɸ��Ͱ� �������� ������ return
		return TRUE;	
	
	if(pParam->m_pOperator->GetID() != HERO->GetID() &&
		pParam->m_pTargetSet[0].pTarget->GetID() != HERO->GetID() )		// if He is not Hero, just return.
		return TRUE;


	CAMERA->ChangeCameraSmooth(Camera,Duration);

	return TRUE;
}

void CCameraEffectUnit::DoProcess(DWORD tickTime,EFFECTPARAM* pParam)
{

}