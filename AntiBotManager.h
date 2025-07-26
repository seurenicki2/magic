/*************************************************************************************************************
 *
 *	File Name	::	AntiBotManager.h
 *
 *	Owner		::	Death
 *
 *	Purpose Application	::	AntiBot , Auto Joy By has dialog display if player atk more one hour
 *
 *	Change Log	::
 *
 *	Death , Sep 05		Intitial Version
 *
 *	Death , Sep 08		Add New 3 Condition to Not Let Anti bot Work
 *						- Map Alker , Alker Farm , Guild Tournament and Castle
 *						- Change to 2 Hour per time to AntiBot Work
 *						- Get GodStatus while Answer the antibot
 *
 *************************************************************************************************************/
#define ANTIBOTMGR CAntiBotManager::GetInstance()

class CAntiBotManager
{	
	__time64_t	CurrentServerTime;
	__time64_t	NextCheckBotTime;
	DWORD		AttackMonsterCount;

	const static int MONSTERHITACCEPTCHECKBOT = 60; //300
	const static int SECONDTORECHECKBOT	   = 300;	//7200

public:
	static CAntiBotManager* GetInstance();
	CAntiBotManager(void);
	virtual ~CAntiBotManager(void);
	
	void	DisplayCheckBotDialog();
	BOOL	RequireCheckBotDialog();

	void	SetAttackMonsterCount(DWORD AtkCount);	
	void	IncreseAttackMonsterCount();

	void	RequestAntiBotBuffToServer();

	void	Process();
};