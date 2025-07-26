// GuildMarkImg.h: interface for the CGuildMarkImg class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class cImage;
class cImageSelf;

class CAliasMarkImg  
{
	eALIASTYPE m_MarkType;
	cImageSelf* m_pImgSelf;
	cImage* m_pImg;

public:
	CAliasMarkImg(eALIASTYPE aliastype,cImageSelf* pImg);
	CAliasMarkImg(eALIASTYPE aliastype,cImage* pImg);
	virtual ~CAliasMarkImg();

	eALIASTYPE GetMarkName()	{	return m_MarkType;	}
	void Render(VECTOR2* pos, DWORD dwColor);
	void Release();
};



