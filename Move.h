#pragma once
#include "Angle.h"

class IMove
{	
public:
	IMove(void) {}
	virtual ~IMove(void) {}

public:
	virtual BOOL SetMove(VECTOR3* p_pv3StartPos, VECTOR3* p_pv3TargetPos, DWORD p_dwCurTime, float p_fPosSpd, float p_fInterpolRate) = 0;
	virtual BOOL SetMoveRotDeg(VECTOR3* p_pv3StartPos, float p_fStartAng, VECTOR3* p_pv3TargetPos, float p_fTargetAng, DWORD p_dwCurTime, float p_fPosSpd, float p_fAngSpd, float p_fInterpolRate) = 0;
	virtual BOOL SetMoveRotRad(VECTOR3* p_pv3StartPos, float p_fStartAng, VECTOR3* p_pv3TargetPos, float p_fTargetAng, DWORD p_dwCurTime, float p_fPosSpd, float p_fAngSpd, float p_fInterpolRate) = 0;

	virtual BOOL ProcessMove(DWORD p_dwCurTime) = 0;

	virtual BOOL StopMove(DWORD p_dwStopTime) = 0;
	virtual BOOL ResumeMove(DWORD p_dwResumeTime) = 0;
	virtual BOOL PauseMove(DWORD p_dwPauseTime, DWORD p_dwDelay) = 0;

	virtual BOOL SetPos(VECTOR3* p_pv3CurPos) = 0;
	virtual BOOL SetAngDeg(float p_fCurAng) = 0;
	virtual BOOL SetAngRad(float p_fCurAng) = 0;

	virtual VECTOR3* GetCurPos(void) = 0;
	virtual float GetCurAngDeg(void) = 0;
	virtual float GetCurAngRad(void) = 0;

	virtual VECTOR3* GetTargetPos(void) = 0;
	virtual float GetTargetAngDeg(void) = 0;
	virtual float GetTargetAngRad(void) = 0;

	virtual BOOL IsMoving(void) = 0;
	virtual BOOL CanMove(void) = 0;
	virtual void SetbMove(BOOL p_bMove) = 0;

	virtual DWORD GetEndPosTime(void) = 0;
	virtual DWORD GetEstimatedPosTime(void) = 0;
};

class CMoveTarget
	: virtual public IMove
{	
public:
	CMoveTarget(void);
	virtual ~CMoveTarget(void);

public:
	virtual BOOL SetMove(VECTOR3* p_pv3StartPos, VECTOR3* p_pv3TargetPos, DWORD p_dwCurTime, float p_fPosSpd, float p_fInterpolRate);
	virtual BOOL SetMoveRotDeg(VECTOR3* p_pv3StartPos, float p_fStartAng, VECTOR3* p_pv3TargetPos, float p_fTargetAng, DWORD p_dwCurTime, float p_fPosSpd, float p_fAngSpd, float p_fInterpolRate);
	virtual BOOL SetMoveRotRad(VECTOR3* p_pv3StartPos, float p_fStartAng, VECTOR3* p_pv3TargetPos, float p_fTargetAng, DWORD p_dwCurTime, float p_fPosSpd, float p_fAngSpd, float p_fInterpolRate);

	virtual BOOL ProcessMove(DWORD p_dwCurTime);

	virtual BOOL StopMove(DWORD p_dwStopTime);
	virtual BOOL ResumeMove(DWORD p_dwResumeTime);
	virtual BOOL PauseMove(DWORD p_dwPauseTime, DWORD p_dwDelay);

	virtual BOOL SetPos(VECTOR3* p_pv3CurPos);
	virtual BOOL SetAngDeg(float p_fCurAng);
	virtual BOOL SetAngRad(float p_fCurAng);

	virtual VECTOR3* GetCurPos(void) { return &m_v3CurPos; }
	//virtual float GetCurAngDeg(void) { return m_Angle.GetAngleDeg(); }
	//virtual float GetCurAngRad(void) { return m_Angle.GetAngleRad(); }

	virtual VECTOR3* GetTargetPos(void) { return &m_v3TargetPos; }
	virtual float GetTargetAngDeg(void) { return m_Angle.GetTargetAngleDeg(); }
	virtual float GetTargetAngRad(void) { return m_Angle.GetTargetAngleRad(); }

	virtual BOOL IsMoving(void) { return m_bMoving; }
	virtual BOOL CanMove(void) { return m_bMove; }
	virtual void SetbMove(BOOL p_bMove) { m_bMove = p_bMove; }

	virtual DWORD GetEndPosTime(void) { return m_dwEndPosTime; }
	virtual DWORD GetEstimatedPosTime(void) { return m_dwEndPosTime - m_dwStartTime; }

private:
	BOOL Interpolate(DWORD p_dwCurTime);

private:
	BOOL m_bMove;
	BOOL m_bMoving;

	VECTOR3 m_v3StartPos;
	VECTOR3 m_v3TargetPos;
	VECTOR3 m_v3InterpolStartPos;
	VECTOR3 m_v3InterpolTargetPos;
	VECTOR3 m_v3CurPos;

	CAngle m_Angle;

	DWORD m_dwStartTime;
	DWORD m_dwEndPosTime;
	DWORD m_dwEndAngTime;
	DWORD m_dwInterpolPosTime;
	DWORD m_dwStopTime;
	DWORD m_dwPauseDelayTime;
};