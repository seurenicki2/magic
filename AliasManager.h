/*************************************************************************************************************
 *
 *	File Name	::	AliasManager.h
 *
 *	Owner		::	Death
 *
 *	Purpose Application	::	Alias Manager
 *
 *	Change Log	::
 *
 *
 *************************************************************************************************************/
#define ALIASMGR CAliasManager::GetInstance()

class cImageSelf;
class CAliasMark;
class CAliasMarkImg;

class CAliasManager
{	
private:	
   CAliasMarkImg* m_pAliasMark;
   const char * const mDirectory;

public:
	static CAliasManager* GetInstance();
	CAliasManager(void);
	virtual ~CAliasManager(void);

	void NetworkMsgParse(BYTE Protocol,void* pMsg);
	CAliasMark* GetAliasMark(eALIASTYPE type);
	CAliasMarkImg* LoadMarkImg(DWORD Image_Hard_Idx,eALIASTYPE type);

	void ItemKindAliasSyn(DWORD ItemSupplyType, DWORD itemIdx, POSTYPE itemPosition); //Alemuri Alias block Cheat
	void ItemKindAliasAck(void* pMsg);
	
};