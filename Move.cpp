#include "StdAfx.h"
#include "Move.h"

CMoveTarget::CMoveTarget(void)
	: m_bMove(TRUE)
	, m_bMoving(FALSE)
	, m_dwStartTime(0)
	, m_dwEndPosTime(0)
	, m_dwEndAngTime(0)
	, m_dwInterpolPosTime(0)
	, m_dwStopTime(0)
	, m_dwPauseDelayTime(0)
{
}

CMoveTarget::~CMoveTarget(void)
{
}

BOOL CMoveTarget::SetPos(VECTOR3* p_pv3CurPos)
{
	if(p_pv3CurPos)
	{
		m_v3StartPos.x = p_pv3CurPos->x;
		m_v3StartPos.y = p_pv3CurPos->y;
		m_v3StartPos.z = p_pv3CurPos->z;

		m_v3TargetPos.x = p_pv3CurPos->x;
		m_v3TargetPos.y = p_pv3CurPos->y;
		m_v3TargetPos.z = p_pv3CurPos->z;

		m_v3InterpolStartPos.x = p_pv3CurPos->x;
		m_v3InterpolStartPos.y = p_pv3CurPos->y;
		m_v3InterpolStartPos.z = p_pv3CurPos->z;

		m_v3InterpolTargetPos.x = p_pv3CurPos->x;
		m_v3InterpolTargetPos.y = p_pv3CurPos->y;
		m_v3InterpolTargetPos.z = p_pv3CurPos->z;

		m_v3CurPos.x = p_pv3CurPos->x;
		m_v3CurPos.y = p_pv3CurPos->y;
		m_v3CurPos.z = p_pv3CurPos->z;
	}
	else
	{
		m_v3StartPos.x = m_v3CurPos.x;
		m_v3StartPos.y = m_v3CurPos.y;
		m_v3StartPos.z = m_v3CurPos.z;

		m_v3TargetPos.x = m_v3CurPos.x;
		m_v3TargetPos.y = m_v3CurPos.y;
		m_v3TargetPos.z = m_v3CurPos.z;

		m_v3InterpolStartPos.x = m_v3CurPos.x;
		m_v3InterpolStartPos.y = m_v3CurPos.y;
		m_v3InterpolStartPos.z = m_v3CurPos.z;

		m_v3InterpolTargetPos.x = m_v3CurPos.x;
		m_v3InterpolTargetPos.y = m_v3CurPos.y;
		m_v3InterpolTargetPos.z = m_v3CurPos.z;
	}

	return TRUE;
}

BOOL CMoveTarget::SetAngDeg(float p_fCurAng)
{
	m_Angle.SetAngleDeg(p_fCurAng);

	return TRUE;
}

BOOL CMoveTarget::SetAngRad(float p_fCurAng)
{
	m_Angle.SetAngleRad(p_fCurAng);

	return TRUE;
}

BOOL CMoveTarget::SetMove(VECTOR3* p_pv3StartPos, VECTOR3* p_pv3TargetPos, DWORD p_dwCurTime, float p_fPosSpd, float p_fInterpolRate)
{
	m_dwStartTime = p_dwCurTime;
	m_dwStopTime = p_dwCurTime;

	if(p_fPosSpd <= 0.0f)
	{
		return FALSE;
	}

	//// 위치 설정
	m_v3TargetPos.x = p_pv3TargetPos->x;
	m_v3TargetPos.y = p_pv3TargetPos->y;
	m_v3TargetPos.z = p_pv3TargetPos->z;
	m_v3InterpolTargetPos.x = p_pv3StartPos->x;
	m_v3InterpolTargetPos.y = p_pv3StartPos->y;
	m_v3InterpolTargetPos.z = p_pv3StartPos->z;
	m_v3InterpolStartPos = m_v3CurPos;
	//////////////////////////////////////////////////////////////////////////

	//// 위치 시간 계산
	float l_fPosGap = CalcDistanceXZ(&m_v3TargetPos, &m_v3InterpolTargetPos);
	const float l_fPosTimeleft = l_fPosGap / p_fPosSpd;
	//////////////////////////////////////////////////////////////////////////

	m_dwEndPosTime = p_dwCurTime + static_cast<DWORD>(l_fPosTimeleft*1000.0f);
	m_dwInterpolPosTime = p_dwCurTime + static_cast<DWORD>(l_fPosTimeleft*p_fInterpolRate*1000.0f);

	return TRUE;
}

BOOL CMoveTarget::SetMoveRotDeg(VECTOR3* p_pv3StartPos, float p_fStartAng, VECTOR3* p_pv3TargetPos, float p_fTargetAng, DWORD p_dwCurTime, float p_fPosSpd, float p_fAngSpd, float p_fInterpolRate)
{
	SetMove(p_pv3StartPos, p_pv3TargetPos, p_dwCurTime, p_fPosSpd, p_fInterpolRate);

	if(p_fAngSpd <= 0.0f)
	{
		return FALSE;
	}

	//// 각 설정
	m_Angle.SetAngleDeg(p_fStartAng);
	m_Angle.SetTargetDeg(p_fTargetAng);
	//////////////////////////////////////////////////////////////////////////

	//// 각 시간 계산
	float l_fAngGap = fabs(m_Angle.GetChangeAmountDeg());
	const float l_fAngTimeleft = l_fAngGap/p_fAngSpd;
	//////////////////////////////////////////////////////////////////////////

	m_dwEndAngTime = p_dwCurTime + static_cast<DWORD>(l_fAngTimeleft*1000.0f);

    return TRUE;
}

BOOL CMoveTarget::SetMoveRotRad(VECTOR3* p_pv3StartPos, float p_fStartAng, VECTOR3* p_pv3TargetPos, float p_fTargetAng, DWORD p_dwCurTime, float p_fPosSpd, float p_fAngSpd, float p_fInterpolRate)
{
	SetMove(p_pv3StartPos, p_pv3TargetPos, p_dwCurTime, p_fPosSpd, p_fInterpolRate);

	if(p_fAngSpd <= 0.0f)
	{
		return FALSE;
	}

	//// 각 설정
	m_Angle.SetAngleRad(p_fStartAng);
	m_Angle.SetTargetRad(p_fTargetAng);
	//////////////////////////////////////////////////////////////////////////

	//// 각 시간 계산
	float l_fAngGap = fabs(m_Angle.GetChangeAmountRad());
	const float l_fAngTimeleft = l_fAngGap/p_fAngSpd;
	//////////////////////////////////////////////////////////////////////////

	m_dwEndAngTime = p_dwCurTime + static_cast<DWORD>(l_fAngTimeleft*1000.0f);

	return TRUE;
}

BOOL CMoveTarget::ProcessMove(DWORD p_dwCurTime)
{
	m_bMoving = FALSE;

	//// Stop Check
	if(m_bMove == FALSE)
	{
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////

	//// Pause Check
	if(m_dwPauseDelayTime > 0)
	{
		if(m_dwPauseDelayTime > p_dwCurTime - m_dwStopTime)
		{
			return FALSE;
		}
		else
		{
			ResumeMove(p_dwCurTime);
			m_dwPauseDelayTime = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////


	//// 서버와 클라이언트의 시작이 다를것이므로 시작을 보간한다. (도착은 같음)
	Interpolate(p_dwCurTime);
	//////////////////////////////////////////////////////////////////////////

	//// 위치 계산
	if(p_dwCurTime < m_dwEndPosTime && m_dwStartTime < m_dwEndPosTime)
	{
		float l_fTimeRate = static_cast<float>(p_dwCurTime - m_dwStartTime) / static_cast<float>(m_dwEndPosTime - m_dwStartTime);
		m_v3CurPos = m_v3StartPos + (m_v3TargetPos - m_v3StartPos) * l_fTimeRate;
		m_bMoving = TRUE;
	}
	else
	{
		m_v3CurPos = m_v3TargetPos;
	}
	//////////////////////////////////////////////////////////////////////////

	//// 각 계산
	if(p_dwCurTime < m_dwEndAngTime && m_dwStartTime < m_dwEndAngTime)
	{
		float l_fTimeRate = static_cast<float>(p_dwCurTime - m_dwStartTime) / static_cast<float>(m_dwEndAngTime - m_dwStartTime);
		m_Angle.Process(l_fTimeRate);
		m_bMoving = TRUE;
	}
	else
	{
		m_Angle.Process(1.0f);
	}
	//////////////////////////////////////////////////////////////////////////

	return TRUE;
}

BOOL CMoveTarget::StopMove(DWORD p_dwStopTime)
{
	if(m_bMove == FALSE)
	{
		return FALSE;
	}

	if(m_dwPauseDelayTime == 0)
	{
		m_dwStopTime = p_dwStopTime;
	}

	m_bMove = FALSE;

	return TRUE;
}

BOOL CMoveTarget::ResumeMove(DWORD p_dwResumeTime)
{
	if(m_bMove == TRUE && m_dwPauseDelayTime == 0)
	{
		return FALSE;
	}

	DWORD l_dwTimeGap = p_dwResumeTime - m_dwStopTime;

	m_dwStartTime = m_dwStartTime + l_dwTimeGap;
	m_dwEndPosTime = m_dwEndPosTime + l_dwTimeGap;
	m_dwEndAngTime = m_dwEndAngTime + l_dwTimeGap;
	m_dwInterpolPosTime = m_dwInterpolPosTime + l_dwTimeGap;
	m_bMove = TRUE;

	return TRUE;
}

BOOL CMoveTarget::PauseMove(DWORD p_dwPauseTime, DWORD p_dwDelay)
{
	if(m_bMove == FALSE)
	{
		return FALSE;
	}

	m_dwStopTime = p_dwPauseTime;
	m_dwPauseDelayTime = p_dwDelay;

	return TRUE;
}

BOOL CMoveTarget::Interpolate(DWORD p_dwCurTime)
{
	//// 위치 보간
	if(p_dwCurTime < m_dwInterpolPosTime && m_dwStartTime < m_dwInterpolPosTime )
	{
		float l_fTimeRate = static_cast<float>(p_dwCurTime - m_dwStartTime) / static_cast<float>(m_dwInterpolPosTime - m_dwStartTime);
		m_v3StartPos = m_v3InterpolStartPos + (m_v3InterpolTargetPos - m_v3InterpolStartPos) * l_fTimeRate;
	}
	else
	{
		m_v3StartPos = m_v3InterpolTargetPos;
	}
	//////////////////////////////////////////////////////////////////////////

	return TRUE;
}

