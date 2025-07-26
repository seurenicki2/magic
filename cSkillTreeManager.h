#pragma once


#define SKILLTREEMGR cSkillTreeManager::GetInstance()

class cIcon;
class cSkillBase;
class cImage;



class cSkillTreeManager
{
	CIndexGenerator mIconIndexCreator;
	CYHHashTable< cSkillBase > mSkillBaseTable;
	cSkillTreeManager(void);
	virtual ~cSkillTreeManager(void);

public:
	static cSkillTreeManager* GetInstance();
	void Init();
	void Release();
	void NetworkMsgParse(BYTE Protocol, LPVOID);
	void SetToolTipIcon(cIcon*, DWORD skillIndex);
	void GetIconImage(DWORD Skilldx, cImage * pLowImage , cImage * pHighImage);
	cSkillBase* GetSkill(DWORD SkillIdx);
	const SKILL_BASE* GetSkillBase( DWORD skillIndex );
	void Update(const SKILL_BASE&);
	void AddSetSkill(DWORD skillIndex, LEVELTYPE);
	void RemoveSetSkill(DWORD skillIndex, LEVELTYPE);
	void ResetSetSkill();
	void AddJobSkill( DWORD skillIndex, BYTE level );
	void ClearJobSkill();

private:
	// ��Ʈ�� ���� �ο��� �ɷ��� ���ŵ� ����, �÷��̾ �������� �ο��� �ɷ��� �����ϱ� ���� �ʿ��� ����
	typedef std::map< DWORD, SKILL_BASE >	StoredSkill;
	StoredSkill								mStoredSkill;

	// ��Ʈ �������� ��ų�� �ο��ϰų� �����Ѵ�. �̶� ������ �ٸ� ���� ��ų(��: ���̾1, ���̾2)�� ���� ��� ������ ����� ����.
	// �̿� ���ÿ� �����Ͽ� ��Ʈ ��ų�� ������ ������ üũ�Ͽ� �������� �����Ѵ�.

	typedef std::stack< BYTE >			SetSkill;		// ��������� ����Ǵ� ��ų ������
	typedef std::map< DWORD, SetSkill >	StoredSetSkill;
	StoredSetSkill						mStoredSetSkill;

	// 081023 KTH -- 
	std::list<SKILL_BASE>			m_JobSkillList;
public :
	void SetPositionHead();
	SKILL_BASE* GetSkillBase();
};