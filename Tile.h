// Tile.h: interface for the CTile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILE_H__7312558F_B883_49C4_8AAA_69CA0AE2D8F4__INCLUDED_)
#define AFX_TILE_H__7312558F_B883_49C4_8AAA_69CA0AE2D8F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPathNode.h"

typedef WORD TILEATTR;
#define TFA_COLLISON		0x00000001

struct TILE_ATTR
{
	TILEATTR uAttr : 8;			//  0 ~ 255
	TILEATTR uSkillObjectBlock : 8;		//	0 ~ 255
};

// 090907 ShinJS --- 5�� ��ã�� Source ����, Tile�� Node�� �����Ͽ� ����Ͽ����� ����� Node�� �������� �ʰ� �߰��κи� ����
class CTile : public IPathNode
{
	TILE_ATTR m_Attr;
public:
	CTile();
	virtual ~CTile();

	void AddTileAttrib(AREATILE areatile);
	void RemoveTileAttrib(AREATILE areatile);

	inline void SetTileAttrib(TILE_ATTR Attr)
	{
		m_Attr = Attr;
	}
	inline BOOL IsCollisonTile()
	{
		return m_Attr.uAttr == TFA_COLLISON || m_Attr.uSkillObjectBlock != 0;
	}

	/// IPathNode
	// Cost ����
	virtual void Calc_Cost(IPathNode* p_DestinationNode);

	// AStarStatus
	virtual void Astar_MakeOpened( IPathNode* p_NowSelectedNode, IPathNode* p_DestinationNode );// ���� ���� �������ش�
	virtual BOOL Astar_CompareParentCost( IPathNode* p_NowSelectedNode, int p_Index );			// G����� ���� �θ� ����ġ���


	/// ��� �⺻ ����
private:
	DWORD		m_PosX;
	DWORD		m_PosY;
	TILE_ATTR	m_NodeAttrib;

public:
	DWORD		GetPosX() const { return m_PosX; }
	void		SetPosX(DWORD val) { m_PosX = val; }
	DWORD		GetPosY() const { return m_PosY; }
	void		SetPosY(DWORD val) { m_PosY = val; }

};

#endif // !defined(AFX_TILE_H__7312558F_B883_49C4_8AAA_69CA0AE2D8F4__INCLUDED_)
