#include "stdafx.h"
#include "OBalloonName.h"
#include "./interface/cFont.h"
#include "./Interface/cWindowManager.h"
#include "./interface/cMultiLineText.h"
#include "./interface/cScriptManager.h"
#include "Object.h"
#include "../SiegeWarfareMgr.h"

#include "ObjectManager.h" //Alemuri
#include "MHMap.h" //Alemuri

COBalloonName::COBalloonName()
{
	m_wFontIdx	= 0;
	
	m_lTall		= 0;
	m_lPosX		= 0;

	m_lMasterTall	= 0;
	m_lMasterPosX	= 0;

	m_lNickTall = 0;
	mFamilyNickPosX	= 0;
	
	m_fgColor	= RGB_HALF(255,255,255);
	m_dwGTColor = RGB_HALF(255,255,255);

	m_bActive	= FALSE;
	m_bShow		= FALSE;

	ZeroMemory( mObjectName, sizeof(mObjectName) );
	ZeroMemory( mGuildNickName, sizeof(mGuildNickName) );
	ZeroMemory( mFamilyNickName, sizeof(mFamilyNickName) );
	
	m_pSiegeName = new cMultiLineText;
	m_bSiegeMap = FALSE;

	m_bGTournamentMap = FALSE;

	// 080827 LYW --- OBalloonName : �θ� CObjectBalloon�� �θ� Ÿ���� ����/��ȯ�ϴ� ���� �߰�.
	m_byPParentObjectKind = eObjectKind_None ;
}

COBalloonName::~COBalloonName()
{
	delete m_pSiegeName;
}

void COBalloonName::InitName( const char * name )
{
	if( !name ) return;
	if( *name == 0 ) return;
//	ASSERTMSG(strlen(name) < 17, "object name is very long");
	SafeStrCpy( mObjectName, name, sizeof( mObjectName ) );
	
	SetPosX(-CFONT_OBJ->GetTextExtentEx(m_wFontIdx, mObjectName, strlen(mObjectName))/2);
}

void COBalloonName::SetName( const char* name)
{
	if( name && *name )
	{
		SafeStrCpy( mObjectName, name, sizeof( mObjectName ) );

		SetPosX(-CFONT_OBJ->GetTextExtentEx(m_wFontIdx, mObjectName, strlen(mObjectName))/2);
	}	
}


void COBalloonName::SetNickName( const char* nickname)
{
	if( nickname )
	{
		SafeStrCpy( mGuildNickName, nickname, sizeof( mGuildNickName ) );

		const LONG nameSize = CFONT_OBJ->GetTextExtentEx( m_wFontIdx, mObjectName, strlen(mObjectName) );
		const LONG nickSize = CFONT_OBJ->GetTextExtentEx( m_wFontIdx, mGuildNickName, strlen(mGuildNickName) ) ;

		mNickData.mX		= m_lPosX + ( nameSize - nickSize ) / 2;
		mNickData.mLength	= nickSize;
	}
}


void COBalloonName::SetFamilyNickName( const char* nickname)
{
	if( nickname ) 
	{
		SafeStrCpy( mFamilyNickName, nickname, sizeof( mFamilyNickName ) );

		const LONG nameSize = CFONT_OBJ->GetTextExtentEx( m_wFontIdx, mObjectName, strlen(mObjectName) );
		const LONG nickSize = CFONT_OBJ->GetTextExtentEx( m_wFontIdx, mFamilyNickName, strlen(mFamilyNickName) ) ;

		mFamilyNickPosX = m_lPosX + ( nameSize - nickSize ) / 2;
	}
}

BOOL COBalloonName::Render(LONG absX, LONG absY, CObject* obj) //Alemuri added CObject* in the function
{
#ifdef _GMTOOL_
	if( WINDOWMGR->IsInterfaceHided() )	return FALSE;
#endif

	if(	! m_bActive ||
		! m_bShow)
	{
		return FALSE;
	}

	// 080827 LYW --- OBalloonName : ĳ������ ��µ��� �ʵ��� ó���Ѵ�.
   	if( m_byPParentObjectKind == eObjectKind_Player ) 
   	{
   		//if(SIEGEWARFAREMGR->Get_IsSiegeMap() && SIEGEWARFAREMGR->Is_SiegeState()) return TRUE ;

   		//if(SIEGEWARFAREMGR->IsSiegeWarfareZone()) return TRUE ; //Alemuri deactivated
		//Alemuri Shows Name in CS------------------------------------------------------------------------------------------------------------------------------------------------
		if(SIEGEWARFAREMGR->IsSiegeWarfareZone())
		{
			RECT rect = {(long)(absX + m_lPosX), (long)(absY + m_lTall), 1, 1};
			m_pSiegeName->SetXY( rect.left,  rect.top );
			m_pSiegeName->Render();
			rect.left += 15;
			CFONT_OBJ->RenderFont(m_wFontIdx, mObjectName, strlen(mObjectName), &rect, RGBA_MERGE( RGB_HALF(70,70,70), 180 ));
			rect.left -= 1;
			rect.top -= 1;
			if (HERO->GetGuildIdx() == ((CPlayer*)obj)->GetGuildIdx() || (HERO->GetGuildUnionIdx() == ((CPlayer*)obj)->GetGuildUnionIdx() && HERO->GetGuildUnionIdx() != 0))
			{
				CFONT_OBJ->RenderFont(m_wFontIdx, mObjectName, strlen(mObjectName), &rect, RGBA_MERGE( m_fgColor, 255 ));
				return TRUE;
			}
			else
			{
				CFONT_OBJ->RenderFont(m_wFontIdx, mObjectName, strlen(mObjectName), &rect, NAMECOLOR_PKMODE);
				return TRUE;				
			}
		}
		//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Alemuri Shows Name in Guild Tournament------------------------------------------------------------------------------------------------------------------------------------
		if(MAP->GetMapNum() == GTMAPNUM)
		{
			RECT rect = {(long)(absX + m_lPosX), (long)(absY + m_lTall), 1, 1};
			m_pSiegeName->SetXY( rect.left,  rect.top );
			m_pSiegeName->Render();
			rect.left += 15;
			CFONT_OBJ->RenderFont(m_wFontIdx, mObjectName, strlen(mObjectName), &rect, RGBA_MERGE( RGB_HALF(70,70,70), 180 ));
			rect.left -= 1;
			rect.top -= 1;
			if (HERO->GetGuildIdx() == ((CPlayer*)obj)->GetGuildIdx())
			{
				CFONT_OBJ->RenderFont(m_wFontIdx, mObjectName, strlen(mObjectName), &rect, RGBA_MERGE( m_fgColor, 255 ));
				return TRUE;
			}
			else
			{
				CFONT_OBJ->RenderFont(m_wFontIdx, mObjectName, strlen(mObjectName), &rect, NAMECOLOR_PKMODE);
				return TRUE;				
			}
		}
		//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
   	}
	
	RECT rect = {(long)(absX + m_lPosX), (long)(absY + m_lTall), 1, 1};
	int line = 0;

	if(m_bGTournamentMap)
	{
		if( *mObjectName != 0 )
		{
			CFONT_OBJ->RenderFont(m_wFontIdx, mObjectName, strlen(mObjectName), &rect, RGBA_MERGE( RGB_HALF(70,70,70), 180 ));
			rect.left -= 1;
			rect.top -= 1;
			CFONT_OBJ->RenderFont(m_wFontIdx, mObjectName, strlen(mObjectName), &rect, RGBA_MERGE( m_dwGTColor, 255 ));
		}
		return TRUE;
	}

	if( m_bSiegeMap )
	{
		m_pSiegeName->SetXY( rect.left,  rect.top );
		m_pSiegeName->Render();
		CFONT_OBJ->RenderFont(m_wFontIdx, mObjectName, strlen(mObjectName), &rect, RGBA_MERGE( RGB_HALF(70,70,70), 180 ));
		rect.left -= 1;
		rect.top -= 1;
		CFONT_OBJ->RenderFont(m_wFontIdx, mObjectName, strlen(mObjectName), &rect, RGBA_MERGE( m_fgColor, 255 ));
	}
	else if( *mObjectName != 0 )
	{
		CFONT_OBJ->RenderFont(m_wFontIdx, mObjectName, strlen(mObjectName), &rect, RGBA_MERGE( RGB_HALF(70,70,70), 180 ));
		rect.left -= 1;
		rect.top -= 1;
		CFONT_OBJ->RenderFont(m_wFontIdx, mObjectName, strlen(mObjectName), &rect, RGBA_MERGE( m_fgColor, 255 ));
	}

	// �йи��� ���� ��쿡�� �̸��� ""����, ó�� ������ ��쿡�� " " ���·� ���õȴ�. �Ѵ� ȣĪ�� ���� ���̸�, ���� �� ���¸� üũ����� �Ѵ�.
	if(	*mFamilyNickName	&&
		strcmp( " ", mFamilyNickName ) )
	{
		RECT nickrect2 = {(long)(absX + mFamilyNickPosX), (long)(absY + m_lNickTall - ++line * 15.0f), 1, 1};

		CFONT_OBJ->RenderFont(m_wFontIdx, mFamilyNickName, strlen(mFamilyNickName), &nickrect2, RGBA_MERGE( RGB_HALF(70,70,70), 180));
		nickrect2.left -= 1;
		nickrect2.top -= 1;
		CFONT_OBJ->RenderFont(m_wFontIdx, mFamilyNickName, strlen(mFamilyNickName), &nickrect2, RGBA_MERGE( RGB_HALF(234, 0, 255), 255 ));
	}

	if( *mGuildNickName != 0 )
	{
		RECT nickrect = {(long)(absX + mNickData.mX ), (long)(absY + m_lNickTall - ++line * 15.0f), 1, 1};

		CFONT_OBJ->RenderFont(m_wFontIdx, mGuildNickName, strlen(mGuildNickName), &nickrect, RGBA_MERGE( RGB_HALF(70,70,70), 180 ));
		nickrect.left -= 1;
		nickrect.top -= 1;
		CFONT_OBJ->RenderFont(m_wFontIdx, mGuildNickName, strlen(mGuildNickName), &nickrect, RGBA_MERGE( RGB_HALF(255,255,0), 180 ));
	}
	
	return TRUE;
}


void COBalloonName::SetPosX(LONG	pos)
{ 
	m_lPosX = pos;	
}

void COBalloonName::SetGTName(DWORD NameType, char* pGuildName)
{
	switch(NameType)
	{
	case eGTNameType_OurGuild:
		m_dwGTColor = TTCLR_SIEGEGUILD;
		break;
	case eGTNameType_Enemy:
		SetName(pGuildName);
		m_dwGTColor = TTCLR_ENERMY;
		break;
	case eGTNameType_Max:
		m_bActive = false;
		break;
	}

	m_bGTournamentMap = TRUE;
}


const COBalloonName::Data& COBalloonName::GetNickData() const
{
	return mNickData;
}


DWORD COBalloonName::GetNameLength()
{
	return CFONT_OBJ->GetTextExtentEx( m_wFontIdx, mObjectName, strlen( mObjectName ) );
}