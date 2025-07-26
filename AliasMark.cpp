/*************************************************************************************************************
 *
 *	File Name	::	AliasMark.cpp
 *
 *	Owner		::	Death
 *
 *	Purpose Application	::	Alias Manager
 *
 *	Change Log	::
 *
 *
 *************************************************************************************************************/
#include "stdafx.h"
#include "AliasMark.h"
#include "AliasMarkImg.h"



CAliasMark::CAliasMark()
{
	m_pMarkImg = NULL;
}

CAliasMark::~CAliasMark()
{
	Clear();
}

void CAliasMark::Clear()
{
	if(m_pMarkImg)
	{
		if(m_pMarkImg->GetMarkName() != 0)
		{
			m_pMarkImg->Release();
			delete m_pMarkImg;
			m_pMarkImg = NULL;
		}
	}
}

void CAliasMark::SetAliasMarkImg(CAliasMarkImg* pMarkImg)
{
	Clear();

	m_pMarkImg = pMarkImg;
}

void CAliasMark::Render(VECTOR2* pos, DWORD dwColor)
{
	if(m_pMarkImg == NULL)
		return;
	
	m_pMarkImg->Render(pos,dwColor);
}