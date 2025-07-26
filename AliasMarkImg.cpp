#include "stdafx.h"
#include "AliasMarkImg.h"
#include "cImage.h"
#include "cImageSelf.h"


CAliasMarkImg::CAliasMarkImg(eALIASTYPE aliastype,cImageSelf* pImg) :
m_MarkType( aliastype ),
m_pImgSelf( pImg )
{
	
	cImageRect rect;
	rect.left	= 0;
	rect.top	= 0;
	rect.right	= GUILDMARKWIDTH - 1;
	rect.bottom = GUILDMARKHEIGHT - 1;

	if( m_pImgSelf )
	{
		m_pImgSelf->SetImageSrcRect(&rect);
	}
}

CAliasMarkImg::CAliasMarkImg(eALIASTYPE aliastype,cImage* pImg) :
m_MarkType( aliastype ),
m_pImg( pImg )
{
	cImageRect rect;
	rect.left	= 0;
	rect.top	= 0;
	rect.right	= GUILDMARKWIDTH - 1;
	rect.bottom = GUILDMARKHEIGHT - 1;

	if( m_pImg )
	{
		m_pImg->SetImageSrcRect(pImg->GetImageRect());
	}
}


CAliasMarkImg::~CAliasMarkImg()
{}


void CAliasMarkImg::Render(VECTOR2* pos, DWORD dwColor )
{
	VECTOR2 scale;
	scale.x = 1.0f;
	scale.y = 1.0f;

	m_pImg->RenderSprite( &scale, 0, 0, pos, dwColor );

}


void CAliasMarkImg::Release()
{
	SAFE_DELETE( m_pImg );
}