// Effect.cpp: implementation of the CEffect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Effect.h"

#include "EffectUnit.h"
#include "EffectTrigger.h"
#include "EffectDesc.h"
#include "..\Object.h"
#include "EffectUnitDesc.h"

#include "AnimationEffectUnitDesc.h"
#include "EffectTriggerUnitDesc.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


void EFFECTPARAM::SetTargetSetArray(TARGETSET* pTargetSetArray,int MaxTargetSet,MAINTARGET* pMainTarget)
{
	m_nTargetNum = MaxTargetSet;
	memcpy(m_pTargetSet,pTargetSetArray,sizeof(TARGETSET)*m_nTargetNum);
	
	m_MainTarget = *pMainTarget;
}


CEffect::CEffect()
{

}

CEffect::~CEffect()
{
}

void CEffect::Init(int maxEffectUnitNum,EFFECTPARAM* pParam,CEffectDesc* pEffectDesc,
				 int TriggerNum,CEffectTriggerUnitDesc** ppEffectTriggerUnitArray)
{
	m_EffectTrigger.Init(TriggerNum,ppEffectTriggerUnitArray);
	
	m_Param.Copy(pParam);

	m_pEffectDesc = pEffectDesc;
	m_MaxEffectUnitNum = min(
		MAX_EFFECT_UNITNUM,
		maxEffectUnitNum);
	ZeroMemory(
		m_ppEffectUnitArray,
		sizeof(m_ppEffectUnitArray));

	if(maxEffectUnitNum > MAX_EFFECT_UNITNUM)
	{
		_tprintf(
			_T("error: %s, unit size is over %u\n"),
			pEffectDesc->GetFileName(),
			MAX_EFFECT_UNITNUM);
	}

	m_EndFlag = FALSE;
	
	m_EffectRefCount = 1;

	m_bHide = FALSE ;
}

void CEffect::Release()
{
	m_EffectTrigger.Release();
	for(int n=0;n<m_MaxEffectUnitNum;++n)
	{
		if(m_ppEffectUnitArray[n])
			m_ppEffectUnitArray[n]->Release();
	}
}

void CEffect::SetEffectUnit(int num,CEffectUnit* pEffectUnit)
{
	m_ppEffectUnitArray[num] = pEffectUnit;
}

BOOL CEffect::Process(DWORD CurTime)
{
#ifndef _GMTOOL_
	//090901 pdy ����Ʈ Hideó��
	if( m_Param.m_pOperator && m_Param.m_pOperator->IsInited() ) 
	{
		//HideObjEffect�� Ȱ��ȭ �Ǿ� �ִٸ� 
		if( m_Param.m_pOperator->IsHideEffect() )
		{
			if( m_bHide == FALSE )
			{
				//�̹� ���ư����ִ� ����Ʈ ����ó�� 
				for(int n=0;n<m_MaxEffectUnitNum;++n)
				{
					CEffectUnit* pTest = m_ppEffectUnitArray[n];
					m_ppEffectUnitArray[n]->SetHide( TRUE ) ; 
				}

				//�ߺ�ó���� �������� �÷��׸� �ϳ�����
				m_bHide = TRUE ;
			}

			//���μ����� ���ư��� Hide�� ���ѵ� ������. ����Ʈ�� ���μ����� ���� �ʰ� ���� 
			return FALSE ;
		}
		else if( m_bHide == TRUE )
		{
			//�̹� ���ư����ִ� ����Ʈ ���� ó��  
			for(int n=0;n<m_MaxEffectUnitNum;++n)
			{
				m_ppEffectUnitArray[n]->SetHide( FALSE ) ; 
			}

			m_bHide = FALSE ;

			//���� ���μ����� ����.
			//�ð��� �̹� ����Ѱ�� �˾Ƽ� �����ǰ� ������ 
		}
	}
#endif

	DWORD ElapsedTime = CurTime - m_Param.m_StartTime;
	m_EffectTrigger.Process(ElapsedTime,this);
	
	for(int n=0;n<m_MaxEffectUnitNum;++n)
	{
		m_ppEffectUnitArray[n]->Process(gTickTime,&m_Param);
	}

	if(ElapsedTime >= m_pEffectDesc->GetEffectEndTime())
	{
		m_EffectTrigger.ExecuteAll(this);			// ���� �������� ���� ����Ʈ�� �ִٸ� �����Ų��.
		return TRUE;		// TRUE ��ȯ�ϸ� EffectManager�� ����Ʈ�� �����Ѵ�.
	}
	else
		return FALSE;
}

void CEffect::SetDamage(TARGETSET* TargetSetArray,int MaxTargetSet,MAINTARGET* pMainTarget)
{
	m_Param.SetTargetSetArray(TargetSetArray,MaxTargetSet,pMainTarget);
		
	for(int n=0;n<m_MaxEffectUnitNum;++n)
	{
		m_ppEffectUnitArray[n]->InitEffect(MaxTargetSet,TargetSetArray);
	}
}

DWORD CEffect::GetEffectEndTime()
{
	return m_pEffectDesc->GetEffectEndTime();
}

void CEffect::Reset()
{
	m_Param.m_StartTime = gCurTime;
	m_EffectTrigger.Reset();
	for(int n=0;n<m_MaxEffectUnitNum;++n)
	{
		m_ppEffectUnitArray[n]->Reset();
	}
}

BOOL CEffect::IsRepeatEffect()
{
	return m_pEffectDesc->IsRepeat();
}

DWORD CEffect::GetNextEffect()
{
	return m_pEffectDesc->GetNextEffect();
}

int CEffect::GetEffectKind()
{
	return m_pEffectDesc->GetEffectKind();
}

CEffectUnit* CEffect::GetEffectUnit(size_t index)
{
	if(index >= size_t(m_MaxEffectUnitNum))
	{
		_tprintf(
			_T("error: %s unit(%d) isn't existed\n"),
			m_pEffectDesc->GetFileName(),
			index);

		// 100211 LUJ, ������ ������� �ʵ��� �� ����Ʈ ������ ��ȯ�Ѵ�
		static CEffectUnit effectUnit;
		static CEffectUnitDesc effectUnitDesc(
			eEffectUnit_Sound,
			FALSE);
		effectUnit.Initialize(
			&effectUnitDesc,
			0);
		return &effectUnit;
	}

	return m_ppEffectUnitArray[index];
}