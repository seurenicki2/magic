//--------------------------------------------------
//	CWayPointManager.h
//	[7/19/2009 LeHide]
//
//	��������Ʈ���� ����
//
//	Version		:	1.0.2009719
//--------------------------------------------------

#pragma once

enum eWMStatus
{
	eWMStatus_Inactive,
	eWMStatus_Active,
	eWMStatus_Success,
	eWMStatus_Fail,
};


class CWayPointManager
{
public:
	CWayPointManager();
	virtual ~CWayPointManager();


private:
	std::deque<VECTOR3>		m_deq_WayPoint;			// ��������Ʈ
	VECTOR3					m_FlagDestination;		// ���� ������(���콺 Ŭ���� ��)

	eWMStatus				m_WMStatus;				// �Ŵ����� ����


public:
	// ��������Ʈ �ֱ�
	void AddWayPoint(const VECTOR3& p_WayPoint);

	// ��������Ʈ ������
	BOOL PopWayPoint(VECTOR3* p_WayPoint);

	// ��� ��������Ʈ ����(��ȿȭ)
	void ClearAllWayPoint();

	// ��������Ʈ�� �����?
	BOOL IsWayPointEmpty();

	// Ư�� �����ε� ��������Ʈ���� �м��ؼ�, ������ ���� ��������Ʈ�� ����


	/// ��������Ʈ �Ŵ����� ����
	// �� ��������Ʈ �Ŵ����� ����
	// 1. Ȱ��ȭ(�����Ͱ� �ְ�, ����� ���� �ϴ� �ܰ�)
	// 2. ��Ȱ��ȭ(�����Ͱ� ����, �������� ����. �̵��� �ƴ�)
	// 3. ������(��ã�Ⱑ ���������� ������ �����̶�,
	//			 ��������Ʈ�� �ӽ÷� �ֱ�� ������, ������� ����)
	eWMStatus GetWMStatus() const { return m_WMStatus; }
	void SetWMStatus(eWMStatus val) { m_WMStatus = val; }


	/// ���� ��ǥ�� - ��߲ű�
	VECTOR3 GetFlagDestination() const { return m_FlagDestination; }
	void SetFlagDestination(VECTOR3 val) { m_FlagDestination = val; }

	// ���� ��ü�� �ڵ鸵
	std::deque<VECTOR3>& GetVec_WayPoint() { return m_deq_WayPoint; }
	void SetVec_WayPoint(std::deque<VECTOR3>& p_vec_WayPoint) { m_deq_WayPoint = p_vec_WayPoint; }
	
	// ù ��° ����Ʈ�� ü�����Ѵ�(������ ��ǥ���� �ٲ۴�)
	void ChangeDestination( const VECTOR3& p_Dest );


	/// ����׿�
	// ��������Ʈ�� TRACE
	void Debug_TRACEWayPoint();

};

