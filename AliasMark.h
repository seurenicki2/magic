/*************************************************************************************************************
 *
 *	File Name	::	AliasMark.h
 *
 *	Owner		::	Death
 *
 *	Purpose Application	::	Alias Manager
 *
 *	Change Log	::
 *
 *
 *************************************************************************************************************/
class CAliasMarkImg;
class CAliasMark  
{
	CAliasMarkImg* m_pMarkImg;


public:
	CAliasMark();
	virtual ~CAliasMark();
	
	void Clear();

	void SetAliasMarkImg(CAliasMarkImg* pMarkImg);
	void Render(VECTOR2* pos, DWORD dwColor = 0xffffffff);
};
