/*************************************************************************************************************
 *
 *	File Name	::	MarryManager.h
 *
 *	Owner		::	Death
 *
 *	Purpose Application	::	Marry Manager
 *
 *	Change Log	::
 *	
 *	- Follow CPP File
 *
 *************************************************************************************************************/
#define MARRYMGR CMarryManager::GetInstance()

class CMarryManager
{	
	
	

    DWORD m_RequestMarryPlayerID;
public:
	static CMarryManager* GetInstance();
	CMarryManager(void);
	virtual ~CMarryManager(void);


	const static DWORD MARRYEFFECT = 20; // 20 = Heart
	void NetworkMsgParse(BYTE Protocol,void* pMsg);
	void RequestMarrySyn(CObject* RequestMarryToPlayerobj, DWORD itemIdx, DWORD itemPosition); //Alemuri added pItem to check Item and Position
	void AbortMarrySyn(DWORD itemIdx, DWORD itemPosition);
	void MarryInviteAccept();
	void MarryInviteDeny();
	void MarrySkillUpgradeSyn();

	void SetRequestMarryPlayerID(DWORD id) { m_RequestMarryPlayerID = id; }
	DWORD GetRequestMarryPlayerID() const { return m_RequestMarryPlayerID; }

	
private:
	void Marry_Add_Invite( void* pMsg );
	void Marry_Invite_Accept_Ack( void* pMsg );
	void Marry_Invite_Accept_Nack(void* pMsg );
	void Marry_Create_Ack( void* pMsg );
	void Marry_Invite_Deny_Ack( void* pMsg );
	void Marry_Create_Notify( void* pMsg );
	void Marry_Abort_Ack(void* pMsg);
		
	

	

};