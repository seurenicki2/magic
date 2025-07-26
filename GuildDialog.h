#pragma once


#include "interface/cTabDialog.h"


class cStatic;
class cListDialog;
class CGuild;

struct GUILDMEMBERINFO;


class CGuildDialog : public cTabDialog  
{
public:
	CGuildDialog();
	virtual ~CGuildDialog();
	virtual void SetActive( BOOL );
	virtual void OnActionEvent( LONG id, void* p, DWORD event );
	virtual void Render();
	
	void Linking();
	
	void DeleteMemberAll();
	
	void SetScore( DWORD score );
	void SetUnionName( const char* );
	const char* GetUnionName() const;
	
	void Add( cWindow* );

	const GUILDMEMBERINFO* GetSelectedMember() const;

	void SetData			( const MSG_GUILD_DATA& );
	// 081031 LUJ, �� ���� �߰�
	void LoginMember		( DWORD playerIndex, MAPTYPE );
	void LogoutMember		( DWORD playerIndex );

	void SetLevel			( DWORD level );
	DWORD GetLevel			() const;
	void AddMember			( const GUILDMEMBERINFO& );
	void RemoveMember		( DWORD playerIndex );
	void SetMemberLevel		( DWORD playerIndex, DWORD level );
	void SetRank			( DWORD playerIndex, BYTE rank );
	
	// 080225 LUJ, Ư�� �÷��̾ ���� ��� ȸ�� ������ ��ȯ�Ѵ�
	GUILDMEMBERINFO* GetMember( DWORD playerIndex );
	// 081031 LUJ, �÷��̾� �̸����� ��� ȸ�� ���� ��ȯ
	GUILDMEMBERINFO* GetMember( const TCHAR* playerName );
	// 081031 LUJ, �̸����� �÷��̾� ��ȣ�� ��ȯ�Ѵ�
	DWORD GetMemberIndex( const TCHAR* playerName ) const;
	// 080225 LUJ, ������ �����Ѵ�
	void SetMember( const GUILDMEMBERINFO& );

	const char* GetGuildName() const;
	const char* GetMasterName() const;
	DWORD		GetMasterIndex() const;
	DWORD GetScore() const;
	DWORD GetMemberSize() const;
	DWORD GetStudentSize() const;

	DWORD GetKickMemberIndex() const;

	std::list< GUILDMEMBERINFO >* GetGuildMemberList() {return &mMemberList;}
	
private:
	void SortByRank();
	void SortByLevel();
	void RefreshMemeber();
	
private:
	cStatic*	mGuildName;
	cStatic*	mGuildLevel;
	cStatic*	mGuildScore;
	cStatic*	mMasterName;
	cStatic*	mMemberSize;
	cStatic*	mUnionName;
	
	// 091208 ShinJS --- ���� ���� List Dialog ����
	cListDialog*	mMemberNameList;
	cListDialog*	mMemberRankList;
	cListDialog*	mMemberLvList;
	
	cWindow* mRetireButton;
	cWindow* mKickMemberButton;
	cWindow* mNameMemberButton;
	cWindow* mRankMemberButton;
	cWindow* mInviteMemberButton;
	cWindow* mLeaveMemberButton;
	cWindow* mNoticeButton;
	cWindow* mWarehouseRankButton;
	cWindow* mUnionInviteButton;
	cWindow* mLeaveUnionButton;
	cWindow* mKickUnionButton;
	cWindow* mDeclareWarButton;
	cWindow* mSuggestStopWarButton;
	cWindow* mSurrenderButton;
	cWindow* mAddTimeButton;
	cWindow* mManageApprenticeButton;
	cWindow* mUnionCreateButton;
	cWindow* mUnionDisbandButton;

	DWORD	mKickMemberIndex;

	int	m_CurGuildMode;

	//���ù������ �÷���
	BOOL mSortRankFlag;
	BOOL mSortLevelFlag;
	
	typedef std::list< GUILDMEMBERINFO >	MemberList;
	MemberList								mMemberList;

	typedef std::set< DWORD >	LoginPlayerSet;
	LoginPlayerSet				mLoginPlayerSet;

	DWORD mMasterIndex;
};