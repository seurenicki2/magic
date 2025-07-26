// TileManager.h: interface for the CTileManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILEMANAGER_H__5FD3CE62_5211_46E8_9C7B_4B5CB5D006CA__INCLUDED_)
#define AFX_TILEMANAGER_H__5FD3CE62_5211_46E8_9C7B_4B5CB5D006CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Tile.h"
#include "IPathFinder.h"
#define fTILE_WIDTH 50
#define fTILE_HEIGHT 50
#define TILECOLLISON_DETAIL		50.f

// 090907 ShinJS --- 5�� ��ã�� Source ����, PathManager ���� �̸� ������� PathFinder�� ����. TileManager�� PathManager�� ���� �Ͽ�����, �������� �ʰ� �߰����� �κ� ����.

enum ePathFindLayer
{
	ePathFindLayer_1x,
	ePathFindLayer_3x,

	eDefaultTileRatio = 50,
};

/// ��ã��� �����Լ�.
enum eH2DLineSide
{
	eH2DLineSide_Up_Left,		// �����̳� ����
	eH2DLineSide_Down_Right,	// �Ʒ����̳� ������
	eH2DLineSide_Equal,			// �� ���� ���� ��
};

// 2D���� y = ax + b�� ����
class H2DLine
{
public:
	// �� ���� �����κ��� ������ �����
	H2DLine( VECTOR2 p_P1, VECTOR2 p_P2 );

	// �� ���� �� ������ ����� ���⿡ �ֳ��� �������ش�
	// �������� �����̸� FALSE ���ų� �Ʒ����̸� TRUE
	// ���� ���Ⱑ ���� ���( x = 3 ���� ���) �����̸� FALSE ���ų� �������̸� TRUE
	eH2DLineSide CheckSide( VECTOR2 p_Point );

	float GetA() const { return m_a; }
	BOOL GetFlag_x_equal_b() const { return m_Flag_x_equal_b; }

private:
	// ����� ������ �̸��� �ƴϴ� ���п��� �̷��� ���ϱ�..
	float	m_a;				// ����
	float	m_b;				// y����
	BOOL	m_Flag_x_equal_b;	// x = b�����϶� �÷���

};


// 2d���¿��� ���а� ������ �浹����
BOOL CollisionCheck_2D_Segment_Line(VECTOR2 p_SegA, VECTOR2 p_SegB, VECTOR2 p_LineA, VECTOR2 p_LineB);
BOOL CollisionCheck_2D_Segment_Line(VECTOR2 p_SegA, VECTOR2 p_SegB, H2DLine& p_Line);

class CTileManager : public IPathFinder
{
protected:
	DWORD m_dwTileWidth;
	DWORD m_dwTileHeight;
	
	CTile* m_pTile;
	
	CTile* GetTile(DWORD dwX, DWORD dwZ);
	CTile* GetTile(float fx, float fz);

public:
	CTileManager();
	~CTileManager();

	DWORD GetTileWidth() { return m_dwTileWidth; }
	BOOL IsInTile(int x, int y, MAPTYPE MapNum,CObject* pObject);
	BOOL CollisionTile(int x, int y, MAPTYPE MapNum,CObject* pObject);
	BOOL CollisionLine(VECTOR3* pStart,VECTOR3* pEnd, VECTOR3 * Target, MAPTYPE MapNum,CObject* pObject);
	BOOL NonCollisionLine(VECTOR3* pStart, VECTOR3* pEnd, VECTOR3* pRtNonCollisonPos, MAPTYPE MapNum, CObject* pObj);	// �浹�ƴϸ� TRUE��ȯ�ϰ� pRtNonCollisonPos�� ���浹 Ÿ�� ����

	BOOL LoadTileInfo(char* TileFile);
	BOOL CollisonCheck(VECTOR3* pStart,VECTOR3* pEnd,VECTOR3* pRtCollisonPos,CObject* pObj);		// �浹�ϸ� TRUE��ȯ�ϰ� pRtCollisonPos�� ���ο� Ÿ�� ����

	void AddTileAttrByAreaData(CObject* Object,VECTOR3* pPos,AREATILE areatile);
	void RemoveTileAttrByAreaData(CObject* Object,VECTOR3* pPos,AREATILE areatile);

	void			SetPathFindMode( ePathFindLayer p_PathFindMode );
	ePathFindLayer	GetPathFindMode();

	// ������ (��Ȯ��) ���ü� �Ǵ� �Լ�
	// Ư�� ������, Ÿ�� �ϳ�
	BOOL CollisionCheck_OneLine_New(DWORD p_StartX, DWORD p_StartY, DWORD p_EndX, DWORD p_EndY);
	BOOL CollisionCheck_OneLine_New(VECTOR3* p_pStart,VECTOR3* p_pEnd);

	

private:
	// ���̾ Ÿ���� ���μ��ΰ�
	DWORD	m_dwWidth1x;		// 1�� ����
	DWORD	m_dwHeight1x;

	DWORD	m_dwWidth3x;		// 3�� (�������κ��� ����)
	DWORD	m_dwHeight3x;
	
	DWORD	m_dwWidth4x;		// 4�� (�������κ��� ����)
	DWORD	m_dwHeight4x;

	float	m_NodeRatio;


	///--------------------------------------------------
	///	��ã�� ���� - IPathFinder ����
	///--------------------------------------------------
public:

	/// ���� - ��ã��� �߰� ������
	virtual void AStarDataReset();

	/// ���� ���� �� ��带 �Է�
	virtual BOOL InputPoint( IPathNode* p_StartNode, IPathNode* p_DestNode );
			BOOL InputPoint( const VECTOR3& p_StartPosition, const VECTOR3& p_DestPosition );

	/// ��ã��!
	virtual BOOL PathFind_AStar( UINT p_LoopCount );

	/// ��ã�� �Ϸ�Ǿ���, Path�� ������, ���� ����
	virtual BOOL MakeAStarPath();

	/// A-Star�� ���ؼ� ���� ��ġ������ ���ü� �Ǵ�, ����ȭ �ǽ�
	virtual BOOL Optimize();
		private:
			BOOL Optimize(std::deque<IPathNode*>& p_deq_AStarPath, std::deque<IPathNode*>& p_deq_AStarPath_Optimize);


public:
	DWORD GetWidth1x() const { return m_dwWidth1x; }
	DWORD GetHeight1x() const { return m_dwHeight1x; }

	// ��带 2���� �迭�� ���ڷ� ���´�
	IPathNode*	GetMovableNode(DWORD p_PosX, DWORD p_PosY);
	IPathNode*	GetMovableNode(float p_PosX, float p_PosY);
	CTile*	GetNode(DWORD p_PosX, DWORD p_PosY);
	CTile*	GetNode(float p_PosX, float p_PosY);
	
	CTile*	GetNode1x(DWORD p_PosX, DWORD p_PosY);
	CTile*	GetNode1x(float p_PosX, float p_PosY);

	std::vector<VECTOR3> GetPathResult(float p_Ratio);
	std::deque<VECTOR3> GetPathResult_Optimize(float p_Ratio);
	std::deque<VECTOR3> GetPathResult_Optimize_Debug(float p_Ratio);


	/// �� �󿡼��� ���� ��ã�⸦ ���� �߰����� �κ�
	
	// Ư�� ��ġ���� ���� ����� NxŸ���� ã�´� (3���)
	BOOL FindClosestTile(const VECTOR3& p_Position, IPathNode*& p_ClosestNode);
	// Ư�� Ÿ�� ������ ���� �ִ� Ÿ���� ã�´�
	BOOL FindClosest1xTile(VECTOR3& p_SafePosition, DWORD p_NowPosX, DWORD p_NowPosY);

	// Node�� Ratio�� ��ȯ�Ѵ�.
	float GetNodeRatio() const { return m_NodeRatio; }

	// ���� ��ġ�� 3�� Ÿ�������� �����Ѵ�.
	void UpdateNodeInfo( VECTOR3* pPos );


private:
	/// A-Star
	// ������ �Ǵ� ��ü ���. ����
	CTile*								m_pAllNode1x;		// 1�� ����
	CTile*								m_pAllNode3x;		// 3�� (�������κ��� ����)

	// �߰�����. �������� ���纻�ϻ��̴�
	std::multimap<float, IPathNode*>	m_map_OpenList;
	std::vector<IPathNode*>				m_vec_ClosedList;
	IPathNode*							m_StartNode;
	IPathNode*							m_DestNode;
	IPathNode*							m_NowNode;

	// ����� ã�� ��. ���纻
	std::deque<IPathNode*>				m_AStarPath;
	std::deque<IPathNode*>				m_AStarPath_Optimize;

	// ���� ��ġ ������ & ����׿�
	std::vector<VECTOR3>				m_PathResult;
	std::deque<VECTOR3>					m_PathResult_Optimize;
	std::deque<VECTOR3>					m_PathResult_Optimize_Debug;
};

#endif // !defined(AFX_TILEMANAGER_H__5FD3CE62_5211_46E8_9C7B_4B5CB5D006CA__INCLUDED_)
