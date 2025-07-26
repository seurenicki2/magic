// CharacterDialog.h: interface for the CCharacterDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHARACTERDIALOG_H__EF27ABB3_EA18_4466_88E9_4035E3807912__INCLUDED_)
#define AFX_CHARACTERDIALOG_H__EF27ABB3_EA18_4466_88E9_4035E3807912__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Interface/cDialog.h"
#include "./Interface/cStatic.h"
#include "./Interface/cGuagen.h"

class CGuildMarkDialog;
class cStatic;

// 070111 LYW --- Define max count for sub item of tree dialog.
#define MAX_SUBITEM			5
//#define MAX_CLASS_STATIC	9

// 070112 LYW --- Define max count for character image.
#define MAX_CHARIMAGE		4

// 070113 LYW --- Define max count for static of background image.
#define MAX_BGSTATIC			6
#define MAX_CIRCLE_IMAGE		9
// 090423 ONS 신규종족 추가에 따른 캐릭터 이미지 수 추가
#define	MAX_CHAR_IMAGE			6
#define MAX_RESISTANCE_IMAGE	6
#define MAX_LOCKED_TEXT			33
//aziz MallShop in Game Method 1
#define MAX_CHANGE_TEXT			37//+reborn point+crit damage
#define MAX_STATUS_BTN			5
#define MAX_SUB_ITEMS			5
#define MAX_CLASS_STATIC		6

struct CHARSTATICCTRL
{
	cStatic* munpa;
	cStatic* jikwe;
	cStatic* badfame;
	cStatic* name;
//	cStatic* stage;		// 경지
//	cStatic* job;		// 직업
//	cStatic* ideology;	// 가치관

	cStatic* Str;
	cStatic* Wis;
	cStatic* Dex;
	cStatic* Vit;

	// 070111 LYW --- Add static control for intelligence of character.
	cStatic* Int ;

	cStatic* level;
	cStatic* expPercent;		// 게이지로 표시
	cStatic* point;		// 잔여포인트

	cStatic* meleeattack;	// 공격력
	cStatic* rangeattack;	// 공격력
	cStatic* defense;
//	cStatic* kyeongkong;
//	cStatic* move;
	cStatic* life;
	cStatic* Mana;
//	cStatic* dok;

	cStatic* spname;
	cStatic* critical;
	cStatic* attackdistance;	
	cStatic* marryname;
	//aziz MallShop in Game Method 1
	cStatic* vipinfo;
	//aziz Reborn 24 Sep
	cStatic* reborninfo;
	//aziz Kill Shop 30 Sep
	cStatic* killinfo;
};

class cGuagen;

class CCharacterDialog  : public cDialog
{
	CHARSTATICCTRL		m_ppStatic;			// 값이 출력될 변수들
	cButton *			m_pPointBtn[MAX_BTN_POINT];

	// 070111 LYW --- Add button control for call class tree dialog.
	//cButton*			m_pClassTreeBtn ;

	// 070111 LYW --- Add new controls for class tree.
	/*
	cStatic*			m_pTreeBgImg ;
	cStatic*			m_pTreeMark ;
	cStatic*			m_pTreeTitle ;
	cStatic*			m_pSubItem[ MAX_SUBITEM ] ;
	cStatic*			m_pClassStatic[ MAX_CLASS_STATIC ] ;
	*/

	// 070112 LYW --- Add static controls for character image.
	//cStatic*			m_pCharImage[ MAX_CHARIMAGE ] ;

	// 070111 LYW --- Delete this line.
	//cButton*			m_pPointMinusBtn[MAX_BTN_POINT];
	int					m_nocoriPoint;
	int					m_addedStatPoints; //ALemuri adding stat option
	BYTE				m_whatStat;

	CGuildMarkDialog*	m_pGuildMarkDlg;
	// 070111 LYW --- Add variable for show tree dialogl.
	BOOL				m_bShowTreeDlg ;
	BOOL				m_bCheckedTreeInfo ;

	// 070112 LYW --- Add variable to store up character image number.
	BYTE				m_byCharImgNum ;

	// 070113 LYW --- Add static controls for background image.
	// 070118 LYW --- Delete this control.
	//cStatic*			m_pBGStatic[ MAX_BGSTATIC ] ;
	// 070118 LYW --- Delete this control.
	//cStatic*			m_pCircleImage[ MAX_CIRCLE_IMAGE ] ;
	cStatic*			m_pCharImage[ MAX_CHAR_IMAGE ] ;
	// 070118 LYW --- Delete this control.
	//cStatic*			m_pResistanceImage[ MAX_RESISTANCE_IMAGE ] ;
	// 070118 LYW --- Delete this control.
	//cStatic*			m_pLockedText[ MAX_LOCKED_TEXT ] ;
	cStatic*			m_pChangeText[ MAX_CHANGE_TEXT ] ;

	cButton*			m_pStatusBtn[ MAX_STATUS_BTN ] ;
	cButton*			m_pCloseBtn ;
	cButton*			m_pClassTreeBtn ;

	cStatic*			m_pTreeBgImg ;
	cStatic*			m_pTreeMark ;
	cStatic*			m_pTreeTitle ;
	cStatic*			m_pSubItem[ MAX_SUB_ITEMS ] ;
	cStatic*			m_pClassStatic[ MAX_CLASS_STATIC ] ;
	//Alemuri Couple and Pvp Score------------------------------------------------
	cStatic*			m_MarryLabel; 
	cStatic*			m_MarryName;
	cStatic*			m_KillLabel; 
	cStatic*			m_KillPoints; 
	cStatic*			m_KillLabelFighter; 
	cStatic*			m_KillPointsFighter; 
	cStatic*			m_KillLabelRogue;
	cStatic*			m_KillPointsRogue; 
	cStatic*			m_KillLabelMage; 
	cStatic*			m_KillPointsMage;
	cStatic*			m_KillLabelDevil;
	cStatic*			m_KillPointsDevil;
	//----------------------------------------------------------------------------
	//Alemuri Job Icons-----------------------------------------------------------
	cStatic*			m_JobFighterIcon; 
	cStatic*			m_JobGuardIcon; 
	cStatic*			m_JobWarriorIcon; 
	cStatic*			m_JobInfantrymanIcon; 
	cStatic*			m_JobSwordsmanIcon; 
	cStatic*			m_JobMercenaryIcon; 
	cStatic*			m_JobPhalanxIcon; 
	cStatic*			m_JobKnightIcon; 
	cStatic*			m_JobGladiatorIcon; 
	cStatic*			m_JobRuneknightIcon; 
	cStatic*			m_JobPaladinIcon; 
	cStatic*			m_JobPanzerIcon; 
	cStatic*			m_JobCrusaderIcon; 
	cStatic*			m_JobDestroyerIcon; 
	cStatic*			m_JobSwordmasterIcon; 
	cStatic*			m_JobMagnusIcon;

	cStatic*			m_JobMageIcon; 
	cStatic*			m_JobClericIcon; 
	cStatic*			m_JobWizardIcon; 
	cStatic*			m_JobPriestIcon; 
	cStatic*			m_JobSorcererIcon; 
	cStatic*			m_JobMonkIcon; 
	cStatic*			m_JobBishopIcon; 
	cStatic*			m_JobWarlockIcon; 
	cStatic*			m_JobInquirerIcon; 
	cStatic*			m_JobElementalmasterIcon; 
	cStatic*			m_JobCardinalIcon; 
	cStatic*			m_JobSoularbiterIcon; 
	cStatic*			m_JobGrandmasterIcon; 
	cStatic*			m_JobNecromancerIcon; 
	cStatic*			m_JobRunemasterIcon; 

	cStatic*			m_JobRogueIcon; 
	cStatic*			m_JobVoyagerIcon; 
	cStatic*			m_JobRuffianIcon; 
	cStatic*			m_JobArcherIcon; 
	cStatic*			m_JobThiefIcon; 
	cStatic*			m_JobScoutIcon; 
	cStatic*			m_JobRangerIcon; 
	cStatic*			m_JobTreasurehunterIcon; 
	cStatic*			m_JobAssassinIcon; 
	cStatic*			m_JobRunewalkerIcon; 
	cStatic*			m_JobSniperIcon; 
	cStatic*			m_JobEntrapperIcon; 
	cStatic*			m_JobBladetakerIcon; 
	cStatic*			m_JobTempermasterIcon; 
	cStatic*			m_JobArchrangerIcon;
	//----------------------------------------------------------------------------
	// 070416 LYW --- CharacterDialog : Add static to print racial.
	cStatic*			m_pRacial ;

	// 070503 LYW --- CharacterDialog : Add static to setting family name.
	cStatic*			m_pFamilyName ;
public:
	CCharacterDialog();
	virtual ~CCharacterDialog();
	virtual void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	void Linking();
	virtual void SetActive( BOOL val );

	//Alemuri Add Function to add Multi Stat Box
	static void OnInputStatsDivideOk(LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2);
	static void OnInputStatsDivideCancel(LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2);
	void SetAddedStats();

	void UpdateData();				// 거의 변하지 않는것
	CHARSTATICCTRL * GetCharacterData(){return &m_ppStatic;}
	void RefreshGuildInfo();
	void RefreshFamilyInfo();
	void SetLevel(LEVELTYPE);
	void SetStrength();
	void SetDexterity();
	void SetVitality();
	void SetWisdom();

	// 070411 LYW --- CharacterDialog : Add function to print values of physical part.
	void PrintAttackRate() ;
	void PrintDefenseRate() ;
	void PrintMagAttackRate() ;
	void PrintMagDefenseRate() ;
	void PrintAccuracyRate() ;
	void PrintEvasionRate() ;
	void PrintCriticalRate() ;
	void PrintCriticalDamage();
	void PrintMagicCriticalRate() ;
	void PrintMoveSpeedRate() ;
	void PrintAttackSpeedRate() ;
	void PrintCastingSpeedRate() ;
	void PrintBlockRate();
	void PrintRange();

	// 070113 LYW --- Add function to setting intelligence.
	void SetIntelligence() ;

	void SetExpPointPercent(float perc);
	void SetAttackRate();
	void SetDefenseRate();
	
	void SetCritical();
	void SetAttackRange();
	void SetBadFame(FAMETYPE);
	void SetPointLeveling(BOOL val, WORD point=0);	
	void OnAddPoint(BYTE whatsPoint);
	void SetPointLevelingHide();
	void RefreshInfo();

	// 070111 LYW --- Add function to process events.
	virtual DWORD ActionEvent( CMouse* mouseInfo ) ;
	void OnActionEvent( LONG lId, void* p, DWORD we ) ;

	// 070111 LYW --- Add function to check and show tree information.
	void CheckTreeInfo() ;
	void ShowTreeDlg() ;

	// 070112 LYW --- Add function to check character image.
	void CheckHeroImage() ;

	// 070115 LYW --- Add function to setting view tree dialog.
	void SetViewTreeDlg( BOOL val ) { m_bShowTreeDlg = val ; ShowTreeDlg() ; }

	// 070416 LYW --- CharacterDialog : Add function to change class name.
	//void SetClassName( char* className ) { m_pChangeText[ 5]->SetStaticText( className ) ; }
	//void SetMarryName( char* MarryName ) { m_pChangeText[ 29]->SetStaticText( MarryName ) ; }
	//aziz MallShop in Game Method 1
	//void SetVipInfo();
	//void PrintVipInfo();
	//aziz Fish 24 Sep
	//void SetFishInfo();
	//aziz Reborn 24 Sep
	//void SetRebornInfo();
	//aziz Kill Shop 30 Sep
	void SetKillInfo();
	//aziz Reborn Point 13 Oct
	//void SetRBPointInfo();
};

#endif // !defined(AFX_CHARACTERDIALOG_H__EF27ABB3_EA18_4466_88E9_4035E3807912__INCLUDED_)
