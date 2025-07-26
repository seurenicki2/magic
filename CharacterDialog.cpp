#include "stdafx.h"
#include "CharacterDialog.h"
#include "ObjectManager.h"
#include "Interface/CWindowManager.h"
#include "Interface/cStatic.h"
#include "Interface/cButton.h"
#include "WindowIDEnum.h"
#include "GuildManager.h"
#include "GuildMarkDialog.h"
#include "CheatMsgParser.h"
#include "CharacterCalcManager.h"

#include "GameIn.h"
#include "..\hseos\Family\SHFamily.h"
#include "ChatManager.h"

// 070111 LYW --- Include header file to need.
#include "cResourceManager.h"

// 071022 LYW --- CharacterDialog : Include header file of tutorial manager.
#include "TutorialManager.h"

#include "GameIn.h"
#include "MiniMapDlg.h"
//Alemuri add multi stat
#include "cDivideBox.h"
#include "./input/UserInput.h"

//aziz MallShop 24
#include "VipMgr.h"
//aziz Fish 24 Sep
#include "FishingManager.h"
//aziz Reborn 24 Sep
#include "UtilityMgr.h"
//aziz Kill Shop 30 Sep
#include "KillMgr.h"

CCharacterDialog::CCharacterDialog()
{
	// 070113 LYW --- Add variable for count.
	int count = 0 ;

	m_type = WT_CHARINFODIALOG;
	m_nocoriPoint = 0;
	// 070111 LYW --- Add button control for call class tree dialog.
	m_pClassTreeBtn = NULL ;

	m_bShowTreeDlg		= FALSE ;
	m_bCheckedTreeInfo  = FALSE ;

	// 070112 LYW --- Add variable to store up character image number.
	m_byCharImgNum = 0x01 ;

	for( count = 0 ; count < MAX_CHAR_IMAGE ; ++count )
	{
		m_pCharImage[ count ] = NULL ;
	}

	for( count = 0 ; count < MAX_CHANGE_TEXT ; ++count )
	{
		m_pChangeText[ count ] = NULL ;
	}

	for( count = 0 ; count < MAX_STATUS_BTN ; ++count )
	{
		m_pStatusBtn[ count ] = NULL ;
	}

	for( count = 0 ; count < MAX_SUB_ITEMS ; ++count )
	{
		m_pSubItem[ count ] = NULL ;
	}

	for( count = 0 ; count < MAX_CLASS_STATIC ; ++count )
	{
		m_pClassStatic[ count ] = NULL ;
	}

	m_pCloseBtn			= NULL ;
	m_pClassTreeBtn		= NULL ;

	m_pTreeBgImg		= NULL ;
	m_pTreeMark			= NULL ;
	m_pTreeTitle		= NULL ;

	// 070416 LYW --- CharacterDialog : Add static to print racial.
	m_pRacial = NULL ;

	// 070503 LYW --- CharacterDialog : Add static to setting family name.
	m_pFamilyName = NULL ;
}

CCharacterDialog::~CCharacterDialog()
{

}

void CCharacterDialog::Linking()
{
	// 070111 LYW --- Add variable for count.
	int count = 0 ;

	for( count = 0 ; count < MAX_CHAR_IMAGE ; ++count )
	{
		m_pCharImage[ count ] = ( cStatic* )GetWindowForID( CI_CHAR_IMAGE0 + count ) ;
	}

	for( count = 0 ; count < MAX_CHANGE_TEXT ; ++count )
	{
		m_pChangeText[ count ] = ( cStatic* )GetWindowForID( CI_CHANGED_TEXT0 + count ) ;
		m_pChangeText[ count ]->SetActive( TRUE ) ;
	}

	for( count = 0 ; count < MAX_STATUS_BTN ; ++count )
	{
		m_pStatusBtn[ count ] = ( cButton* )GetWindowForID( CI_STATUS_BTN_0 + count ) ;
		// 070122 LYW --- Setting to deactive.
		m_pStatusBtn[ count ]->SetActive( FALSE ) ;
	}

	for( count = 0 ; count < MAX_SUB_ITEMS ; ++count )
	{
		m_pSubItem[ count ] = ( cStatic* )GetWindowForID( CI_SUB_ITEMS1 + count ) ;
	}

	m_pCloseBtn			= ( cButton* )GetWindowForID( CI_CLOSEBTN ) ;
	m_pClassTreeBtn		= ( cButton* )GetWindowForID( CI_CLASSTREEBTN ) ;

	m_pTreeBgImg		= ( cStatic* )GetWindowForID( CI_TREEBGIMG ) ;
	m_pTreeMark			= ( cStatic* )GetWindowForID( CI_TREEMARK ) ;
	m_pTreeTitle		= ( cStatic* )GetWindowForID( CI_TREETITLE ) ;
	//Alemuri Couple and Pvp Score------------------------------------------------------------------------------------------
	m_MarryLabel		= ( cStatic* )GetWindowForID( CI_MARRYLABEL ) ; 
	m_MarryName			= ( cStatic* )GetWindowForID( CI_MarryName ) ; 
	m_KillLabel			= ( cStatic* )GetWindowForID( CI_KILL_LABEL ) ; 
	m_KillPoints		= ( cStatic* )GetWindowForID( CI_KILL_INFO ) ; 
	m_KillLabelFighter	= ( cStatic* )GetWindowForID( CI_KILL_LABEL_FIGHTER ) ; 
	m_KillPointsFighter	= ( cStatic* )GetWindowForID( CI_KILL_INFO_FIGHTER ) ;
	m_KillLabelRogue	= ( cStatic* )GetWindowForID( CI_KILL_LABEL_ROGUE ) ;
	m_KillPointsRogue	= ( cStatic* )GetWindowForID( CI_KILL_INFO_ROGUE ) ;
	m_KillLabelMage		= ( cStatic* )GetWindowForID( CI_KILL_LABEL_MAGE ) ;
	m_KillPointsMage	= ( cStatic* )GetWindowForID( CI_KILL_INFO_MAGE ) ; 
	m_KillLabelDevil	= ( cStatic* )GetWindowForID( CI_KILL_LABEL_DEVIL ) ;
	m_KillPointsDevil	= ( cStatic* )GetWindowForID( CI_KILL_INFO_DEVIL ) ;
	m_MarryLabel->SetActive( FALSE ); 
	m_MarryName->SetActive( FALSE ); 
	m_KillLabel->SetActive( FALSE );
	m_KillPoints->SetActive( FALSE );
	m_KillLabelFighter->SetActive( FALSE );
	m_KillPointsFighter->SetActive( FALSE );
	m_KillLabelRogue->SetActive( FALSE );
	m_KillPointsRogue->SetActive( FALSE );
	m_KillLabelMage->SetActive( FALSE );
	m_KillPointsMage->SetActive( FALSE );
	m_KillLabelDevil->SetActive( FALSE );
	m_KillPointsDevil->SetActive( FALSE );
	//-----------------------------------------------------------------------------------------------------------------------
	//Alemuri Job Icons------------------------------------------------------------------------------------------------------
	m_JobFighterIcon			= ( cStatic* )GetWindowForID( CI_ICON_FIGHTER ) ;
	m_JobGuardIcon				= ( cStatic* )GetWindowForID( CI_ICON_GUARD ) ;
	m_JobWarriorIcon			= ( cStatic* )GetWindowForID( CI_ICON_WARRIOR ) ;
	m_JobInfantrymanIcon		= ( cStatic* )GetWindowForID( CI_ICON_INFANTRYMAN ) ;
	m_JobSwordsmanIcon			= ( cStatic* )GetWindowForID( CI_ICON_SWORDSMAN ) ;
	m_JobMercenaryIcon			= ( cStatic* )GetWindowForID( CI_ICON_MERCENARY ) ;
	m_JobPhalanxIcon			= ( cStatic* )GetWindowForID( CI_ICON_PHALANX ) ;
	m_JobKnightIcon				= ( cStatic* )GetWindowForID( CI_ICON_KNIGHT ) ;
	m_JobGladiatorIcon			= ( cStatic* )GetWindowForID( CI_ICON_GLADIATOR ) ;
	m_JobRuneknightIcon			= ( cStatic* )GetWindowForID( CI_ICON_RUNEKNIGHT ) ;
	m_JobPaladinIcon			= ( cStatic* )GetWindowForID( CI_ICON_PALADIN ) ;
	m_JobPanzerIcon				= ( cStatic* )GetWindowForID( CI_ICON_PANZER ) ;
	m_JobCrusaderIcon			= ( cStatic* )GetWindowForID( CI_ICON_CRUSADER ) ;
	m_JobDestroyerIcon			= ( cStatic* )GetWindowForID( CI_ICON_DESTROYER ) ;
	m_JobSwordmasterIcon		= ( cStatic* )GetWindowForID( CI_ICON_SWORDMASTER ) ;
	m_JobMagnusIcon				= ( cStatic* )GetWindowForID( CI_ICON_MAGNUS ) ;

	m_JobMageIcon				= ( cStatic* )GetWindowForID( CI_ICON_MAGE ) ;
	m_JobClericIcon				= ( cStatic* )GetWindowForID( CI_ICON_CLERIC ) ;
	m_JobWizardIcon				= ( cStatic* )GetWindowForID( CI_ICON_WIZARD ) ;
	m_JobPriestIcon				= ( cStatic* )GetWindowForID( CI_ICON_PRIEST ) ;
	m_JobSorcererIcon			= ( cStatic* )GetWindowForID( CI_ICON_SORCERER ) ;
	m_JobMonkIcon				= ( cStatic* )GetWindowForID( CI_ICON_MONK ) ;
	m_JobBishopIcon				= ( cStatic* )GetWindowForID( CI_ICON_BISHOP ) ;
	m_JobWarlockIcon			= ( cStatic* )GetWindowForID( CI_ICON_WARLOCK ) ;
	m_JobInquirerIcon			= ( cStatic* )GetWindowForID( CI_ICON_INQUIRER ) ;
	m_JobElementalmasterIcon	= ( cStatic* )GetWindowForID( CI_ICON_ELEMENTALMASTER ) ;
	m_JobCardinalIcon			= ( cStatic* )GetWindowForID( CI_ICON_CARDINAL ) ;
	m_JobSoularbiterIcon		= ( cStatic* )GetWindowForID( CI_ICON_SOULARBITER ) ;
	m_JobGrandmasterIcon		= ( cStatic* )GetWindowForID( CI_ICON_GRANDMASTER ) ;
	m_JobNecromancerIcon		= ( cStatic* )GetWindowForID( CI_ICON_NECROMANCER ) ;
	m_JobRunemasterIcon			= ( cStatic* )GetWindowForID( CI_ICON_RUNEMASTER ) ;

	m_JobRogueIcon				= ( cStatic* )GetWindowForID( CI_ICON_ROGUE ) ;
	m_JobVoyagerIcon			= ( cStatic* )GetWindowForID( CI_ICON_VOYAGER ) ;
	m_JobRuffianIcon			= ( cStatic* )GetWindowForID( CI_ICON_RUFFIAN ) ;
	m_JobArcherIcon				= ( cStatic* )GetWindowForID( CI_ICON_ARCHER ) ;
	m_JobThiefIcon				= ( cStatic* )GetWindowForID( CI_ICON_THIEF ) ;
	m_JobScoutIcon				= ( cStatic* )GetWindowForID( CI_ICON_SCOUT ) ;
	m_JobRangerIcon				= ( cStatic* )GetWindowForID( CI_ICON_RANGER ) ;
	m_JobTreasurehunterIcon		= ( cStatic* )GetWindowForID( CI_ICON_TREASUREHUNTER ) ;
	m_JobAssassinIcon			= ( cStatic* )GetWindowForID( CI_ICON_ASSASSIN ) ;
	m_JobRunewalkerIcon			= ( cStatic* )GetWindowForID( CI_ICON_RUNEWALKER ) ;
	m_JobSniperIcon				= ( cStatic* )GetWindowForID( CI_ICON_SNIPER ) ;
	m_JobEntrapperIcon			= ( cStatic* )GetWindowForID( CI_ICON_ENTRAPPER ) ;
	m_JobBladetakerIcon			= ( cStatic* )GetWindowForID( CI_ICON_BLADETAKER ) ;
	m_JobTempermasterIcon		= ( cStatic* )GetWindowForID( CI_ICON_TEMPERMASTER ) ;
	m_JobArchrangerIcon			= ( cStatic* )GetWindowForID( CI_ICON_ARCHRANGER ) ;

	m_JobFighterIcon->SetActive(FALSE);
	m_JobGuardIcon->SetActive(FALSE);
	m_JobWarriorIcon->SetActive(FALSE);
	m_JobInfantrymanIcon->SetActive(FALSE);
	m_JobSwordsmanIcon->SetActive(FALSE);
	m_JobMercenaryIcon->SetActive(FALSE);
	m_JobPhalanxIcon->SetActive(FALSE);
	m_JobKnightIcon->SetActive(FALSE);
	m_JobGladiatorIcon->SetActive(FALSE);
	m_JobRuneknightIcon->SetActive(FALSE);
	m_JobPaladinIcon->SetActive(FALSE);
	m_JobPanzerIcon->SetActive(FALSE);
	m_JobCrusaderIcon->SetActive(FALSE);
	m_JobDestroyerIcon->SetActive(FALSE);
	m_JobSwordmasterIcon->SetActive(FALSE);
	m_JobMagnusIcon->SetActive(FALSE);

	m_JobMageIcon->SetActive(FALSE);
	m_JobClericIcon->SetActive(FALSE);
	m_JobWizardIcon->SetActive(FALSE);
	m_JobPriestIcon->SetActive(FALSE);
	m_JobSorcererIcon->SetActive(FALSE);
	m_JobMonkIcon->SetActive(FALSE);
	m_JobBishopIcon->SetActive(FALSE);
	m_JobWarlockIcon->SetActive(FALSE);
	m_JobInquirerIcon->SetActive(FALSE);
	m_JobElementalmasterIcon->SetActive(FALSE);
	m_JobCardinalIcon->SetActive(FALSE);
	m_JobSoularbiterIcon->SetActive(FALSE);
	m_JobGrandmasterIcon->SetActive(FALSE);
	m_JobNecromancerIcon->SetActive(FALSE);
	m_JobRunemasterIcon->SetActive(FALSE);

	m_JobRogueIcon->SetActive(FALSE);
	m_JobVoyagerIcon->SetActive(FALSE);
	m_JobRuffianIcon->SetActive(FALSE);
	m_JobArcherIcon->SetActive(FALSE);
	m_JobThiefIcon->SetActive(FALSE);
	m_JobScoutIcon->SetActive(FALSE);
	m_JobRangerIcon->SetActive(FALSE);
	m_JobTreasurehunterIcon->SetActive(FALSE);
	m_JobAssassinIcon->SetActive(FALSE);
	m_JobRunewalkerIcon->SetActive(FALSE);
	m_JobSniperIcon->SetActive(FALSE);
	m_JobEntrapperIcon->SetActive(FALSE);
	m_JobBladetakerIcon->SetActive(FALSE);
	m_JobTempermasterIcon->SetActive(FALSE);
	m_JobArchrangerIcon->SetActive(FALSE);
	//-----------------------------------------------------------------------------------------------------------------------
	for( count = 0 ; count < MAX_CLASS_STATIC ; ++count )
	{
		m_pClassStatic[ count ] = ( cStatic* )GetWindowForID( CI_CLASS1_STATIC + count ) ;
	}

	m_pRacial = (cStatic*)GetWindowForID(CI_RACIAL) ;

	// 070503 LYW --- CharacterDialog : Add family name.
	m_pFamilyName = (cStatic*)GetWindowForID(CI_FAMILYNAME) ;
}
void CCharacterDialog::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
	//m_type = WT_CHARINFODIALOG;
}

void CCharacterDialog::UpdateData()
{
	BASEOBJECT_INFO ObjInfo;
	CHARACTER_TOTALINFO ChaInfo;
	HERO_TOTALINFO HeroInfo;
	OBJECTMGR->GetHero()->GetBaseObjectInfo(&ObjInfo);
	OBJECTMGR->GetHero()->GetCharacterTotalInfo(&ChaInfo);
	OBJECTMGR->GetHero()->GetHeroTotalInfo(&HeroInfo);

	
	m_pChangeText[1]->SetStaticValue(ChaInfo.BadFame);//pk
	m_pChangeText[2]->SetStaticText(ObjInfo.ObjectName);
	m_MarryName->SetStaticText(ChaInfo.MarryName);

	// 070416 LYW --- CharacterDialog : Setting Class name.
	WORD idx = 1;
	if( ChaInfo.JobGrade > 1 )
	{
		idx = ChaInfo.Job[ChaInfo.JobGrade - 1];
	}
	WORD job = ( ChaInfo.Job[0] * 1000 ) + ( ( ChaInfo.Race + 1 ) * 100 ) + ( ChaInfo.JobGrade * 10 ) + idx;

	m_pChangeText[5]->SetStaticText( RESRCMGR->GetMsg(RESRCMGR->GetClassNameNum(job)) ) ; 

	// 070416 LYW --- CharacterDialog : Setting racial.
	// 090504 ONS 캐릭터다이얼로그 신규종족명 추가 처리
	if(ChaInfo.Race == RaceType_Human)
	{
		m_pRacial->SetStaticText(RESRCMGR->GetMsg(247)) ;
	}
	else if(ChaInfo.Race == RaceType_Elf)
	{
		m_pRacial->SetStaticText(RESRCMGR->GetMsg(248)) ;
	}
	else
	{
		m_pRacial->SetStaticText(RESRCMGR->GetMsg(1129)) ;
	}

	// 080214 LYW --- CharacterDialog : 패밀리 정보 새로고침 추가.
	RefreshFamilyInfo() ;
}

void CCharacterDialog::SetExpPointPercent(float perc)
{
	static char buf[66];
	sprintf(buf, "[%.2f%%]", perc);

#ifdef _CHEATENABLE_	
	if(CHEATMGR->IsCheatEnable())
	{
		char buf1[66];
		wsprintf(buf1, " : %d/%d", HERO->GetExpPoint(), HERO->GetMaxExpPoint());
		strcat(buf, buf1);
	}
#endif

	m_ppStatic.expPercent->SetStaticText(buf);
}
void CCharacterDialog::SetLevel(WORD level)
{
	// 070113 LYW --- Modified this line.
	//m_ppStatic.level->SetStaticValue(level);
	m_pChangeText[0]->SetStaticValue(level);
}

void CCharacterDialog::SetStrength()
{
	char buf[256] = {0,};
	DWORD resVal = HERO->GetStrength();		//KES 070530 : WORD --> DWORD
	DWORD baseVal = HERO->GetHeroTotalInfo()->Str;	//KES 070530 : WORD --> DWORD
	DWORD gapVal = 0;

	// 070412 LYW --- CharacterDialog : Apply extend color to strength
	if( m_pChangeText[6]->IsHasExtendColor() )
	{
		if( baseVal > resVal )
		{
			m_pChangeText[6]->SetFGColor( m_pChangeText[6]->GetExtendColor(1) ) ;

			gapVal = baseVal - resVal;

			sprintf( buf, "%d - %d", baseVal, gapVal );
		}
		else if( baseVal < resVal )
		{
			m_pChangeText[6]->SetFGColor( m_pChangeText[6]->GetExtendColor(2) ) ;

			gapVal = resVal - baseVal;
			
			sprintf( buf, "%d+%d", baseVal, gapVal );
		}
		else
		{
			m_pChangeText[6]->SetFGColor( m_pChangeText[6]->GetExtendColor(0) ) ;

			sprintf( buf, "%d", baseVal );
		}
	}

	m_pChangeText[6]->SetStaticText( buf );
}
void CCharacterDialog::SetDexterity()
{
	char buf[256] = {0,};
	DWORD resVal = HERO->GetDexterity();		//KES 070530 : WORD --> DWORD
	DWORD baseVal = HERO->GetHeroTotalInfo()->Dex;	//KES 070530 : WORD --> DWORD
	DWORD gapVal = 0;

	// 070412 LYW --- CharacterDialog : Apply extend color to strength
	if( m_pChangeText[7]->IsHasExtendColor() )
	{
		if( baseVal > resVal )
		{
			m_pChangeText[7]->SetFGColor( m_pChangeText[6]->GetExtendColor(1) ) ;

			gapVal = baseVal - resVal;

			sprintf( buf, "%d - %d", baseVal, gapVal );
		}
		else if( baseVal < resVal )
		{
			m_pChangeText[7]->SetFGColor( m_pChangeText[6]->GetExtendColor(2) ) ;

			gapVal = resVal - baseVal;
			
			sprintf( buf, "%d+%d", baseVal, gapVal );
		}
		else
		{
			m_pChangeText[7]->SetFGColor( m_pChangeText[6]->GetExtendColor(0) ) ;

			sprintf( buf, "%d", baseVal );
		}
	}

	m_pChangeText[7]->SetStaticText( buf );
}
void CCharacterDialog::SetVitality()
{
	char buf[256] = {0,};
	DWORD resVal = HERO->GetVitality();		//KES 070530 : WORD --> DWORD
	DWORD baseVal = HERO->GetHeroTotalInfo()->Vit;	//KES 070530 : WORD --> DWORD
	DWORD gapVal = 0;

	// 070412 LYW --- CharacterDialog : Apply extend color to strength
	if( m_pChangeText[8]->IsHasExtendColor() )
	{
		if( baseVal > resVal )
		{
			m_pChangeText[8]->SetFGColor( m_pChangeText[6]->GetExtendColor(1) ) ;

			gapVal = baseVal - resVal;

			sprintf( buf, "%d - %d", baseVal, gapVal );
		}
		else if( baseVal < resVal )
		{
			m_pChangeText[8]->SetFGColor( m_pChangeText[6]->GetExtendColor(2) ) ;

			gapVal = resVal - baseVal;
			
			sprintf( buf, "%d+%d", baseVal, gapVal );
		}
		else
		{
			m_pChangeText[8]->SetFGColor( m_pChangeText[6]->GetExtendColor(0) ) ;

			sprintf( buf, "%d", baseVal );
		}
	}

	m_pChangeText[8]->SetStaticText( buf );
}
void CCharacterDialog::SetWisdom()
{
	char buf[256] = {0,};
	DWORD resVal = HERO->GetWisdom();		//KES 070530 : WORD --> DWORD
	DWORD baseVal = HERO->GetHeroTotalInfo()->Wis;	//KES 070530 : WORD --> DWORD
	DWORD gapVal = 0;

	// 070412 LYW --- CharacterDialog : Apply extend color to strength
	if( m_pChangeText[9]->IsHasExtendColor() )
	{
		if( baseVal > resVal )
		{
			m_pChangeText[9]->SetFGColor( m_pChangeText[6]->GetExtendColor(1) ) ;

			gapVal = baseVal - resVal;

			sprintf( buf, "%d - %d", baseVal, gapVal );
		}
		else if( baseVal < resVal )
		{
			m_pChangeText[9]->SetFGColor( m_pChangeText[6]->GetExtendColor(2) ) ;

			gapVal = resVal - baseVal;
			
			sprintf( buf, "%d+%d", baseVal, gapVal );
		}
		else
		{
			m_pChangeText[9]->SetFGColor( m_pChangeText[6]->GetExtendColor(0) ) ;

			sprintf( buf, "%d", baseVal );
		}
	}

	m_pChangeText[9]->SetStaticText( buf );
}

void CCharacterDialog::SetIntelligence()
{
	char buf[256] = {0,};
	DWORD resVal = HERO->GetIntelligence();		//KES 070530 : WORD --> DWORD
	DWORD baseVal = HERO->GetHeroTotalInfo()->Int;	//KES 070530 : WORD --> DWORD
	DWORD gapVal = 0;

	// 070412 LYW --- CharacterDialog : Apply extend color to strength
	if( m_pChangeText[10]->IsHasExtendColor() )
	{
		if( baseVal > resVal )
		{
			m_pChangeText[10]->SetFGColor( m_pChangeText[6]->GetExtendColor(1) ) ;

			gapVal = baseVal - resVal;

			sprintf( buf, "%d - %d", baseVal, gapVal );
		}
		else if( baseVal < resVal )
		{
			m_pChangeText[10]->SetFGColor( m_pChangeText[6]->GetExtendColor(2) ) ;

			gapVal = resVal - baseVal;
			
			sprintf( buf, "%d+%d", baseVal, gapVal );
		}
		else
		{
			m_pChangeText[10]->SetFGColor( m_pChangeText[6]->GetExtendColor(0) ) ;

			sprintf( buf, "%d", baseVal );
		}
	}

	m_pChangeText[10]->SetStaticText( buf );
}

void CCharacterDialog::SetBadFame(FAMETYPE badfame)
{
	m_pChangeText[1]->SetStaticValue(badfame); 
}

void CCharacterDialog::SetAttackRate()
{
	char buf1[66] = {0, } ;
	wsprintf( buf1, "%d", HERO->GetAttackRate() ) ;
	m_pChangeText[12]->SetStaticText(buf1) ;
}
void CCharacterDialog::SetDefenseRate()
{
	char buf1[66] = {0, } ;
	wsprintf( buf1, "%d", HERO->GetDefenseRate() ) ;
	m_pChangeText[17]->SetStaticText(buf1) ;
}
void CCharacterDialog::SetCritical()
{
	char buf1[66] = {0, } ;
	wsprintf( buf1, "%d", HERO->GetCritical() ) ;
	m_pChangeText[14]->SetStaticText(buf1) ;
}

// 070411 LYW --- CharacterDialog : Add function to print values of physical part.
void CCharacterDialog::PrintAttackRate() 
{
	char buf1[66] = {0, } ;
	wsprintf( buf1, "%d", HERO->GetAttackRate() ) ;
	m_pChangeText[12]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintDefenseRate()
{
	char buf1[66] = {0, } ;
	wsprintf( buf1, "%d", HERO->GetDefenseRate() ) ;
	m_pChangeText[17]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintMagAttackRate() 
{
	char buf1[66] = {0, } ;
	wsprintf( buf1, "%d", HERO->GetMagAttackRate() ) ;
	m_pChangeText[18]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintMagDefenseRate() 
{
	char buf1[66] = {0, } ;
	wsprintf( buf1, "%d", HERO->GetMagDefenseRate() ) ;
	m_pChangeText[21]->SetStaticText(buf1) ;
}
void CCharacterDialog::PrintAccuracyRate()
{
	char buf1[66] = { 0, };
	wsprintf(buf1, "%d", HERO->GetAccuracyRate());
	m_pChangeText[16]->SetStaticText(buf1);
}

void CCharacterDialog::PrintEvasionRate()
{
	char buf1[66] = { 0, };
	wsprintf(buf1, "%d", HERO->GetEvasionRate());
	m_pChangeText[15]->SetStaticText(buf1);
}

void CCharacterDialog::PrintCriticalRate()
{
	char buf1[66] = { 0, };
	wsprintf(buf1, "%d", HERO->DoGetCriticalRate());
	m_pChangeText[14]->SetStaticText(buf1);
}

void CCharacterDialog::PrintCriticalDamage()
{
	float	fPlus = HERO->GetPassiveStatus()->CriticalDamage
		+ HERO->GetBuffStatus()->CriticalDamage
		+ HERO->GetItemOptionStats().mCriticalDamage.mPlus
		+ HERO->GetSetItemStats().mCriticalDamage.mPlus;

	float	fPercent = HERO->GetRatePassiveStatus()->CriticalDamage
		+ HERO->GetRateBuffStatus()->CriticalDamage
		+ (HERO->GetItemOptionStats().mCriticalDamage.mPercent * 100.f)
		+ (HERO->GetSetItemStats().mCriticalDamage.mPercent * 100.f);

	DWORD CDamagePlus = max(0, fPlus);
	DWORD CDamagePercentage = max(0, fPercent);
	DWORD RealCriticalDamage = CDamagePlus + CDamagePercentage;

	char buf1[66] = { 0, };
	if (CDamagePercentage > 0)		wsprintf(buf1, "+%d%%", RealCriticalDamage);
	else							wsprintf(buf1, "%d%%", RealCriticalDamage);
	m_pChangeText[29]->SetStaticText(buf1);
}

void CCharacterDialog::PrintMagicCriticalRate()
{
	char buf1[66] = {0, } ;
	const int nCriticalRate = (int)HERO->DoGetMagicCriticalRate();
	if( nCriticalRate > 0 )			wsprintf( buf1, "+%d%%", nCriticalRate );
	else							wsprintf( buf1, "%d%%", nCriticalRate );
	m_pChangeText[24]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintMoveSpeedRate()
{
	char buf1[66] = {0, } ;
	const int nMoveSpeedRate = (int)HERO->DoGetMoveSpeedRate();
	if( nMoveSpeedRate > 0 )		wsprintf( buf1, "+%d%%", nMoveSpeedRate );
	else							wsprintf( buf1, "%d%%", nMoveSpeedRate );
	m_pChangeText[20]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintAttackSpeedRate()
{
	char buf1[66] = {0, } ;
	const int nAttackSpeedRate = (int)HERO->DoGetAttackSpeedRate();
	if( nAttackSpeedRate > 0 )		wsprintf( buf1, "+%d%%", nAttackSpeedRate );
	else							wsprintf( buf1, "%d%%", nAttackSpeedRate );
	m_pChangeText[19]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintCastingSpeedRate()
{
	char buf1[66] = {0, } ;
	const int nCastingSpeedRate = (int)HERO->DoGetCastingSpeedRate(); // -(int)HERO->DoGetCastingSpeedRate();
	if( nCastingSpeedRate > 0 )		wsprintf( buf1, "+%d%%", nCastingSpeedRate );
	else							wsprintf( buf1, "%d%%", nCastingSpeedRate );
	m_pChangeText[23]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintRange() 
{
	DWORD Range = HERO->GetBonusRange();

	char buf1[66] = {0, } ;
	wsprintf( buf1, "%d", Range );
	m_pChangeText[31]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintBlockRate()
{
	float	fPlus = HERO->GetPassiveStatus()->Block
		+ HERO->GetBuffStatus()->Block
		+ HERO->GetItemOptionStats().mBlock.mPlus
		+ HERO->GetSetItemStats().mBlock.mPlus;

	float	fPercent = HERO->GetRatePassiveStatus()->Block
		+ HERO->GetRateBuffStatus()->Block
		+ HERO->GetTotalBlock()
		+ (HERO->GetItemOptionStats().mBlock.mPercent * 100.f)
		+ (HERO->GetSetItemStats().mBlock.mPercent * 100.f);

	DWORD CBlockPlus = max(0, fPlus);
	DWORD CBlockPercentage = max(0, fPercent);

	if(HERO->GetArmorType(eWearedItem_Shield) != 0)
	{
		char buf1[66] = { 0, };
		if (CBlockPercentage > 0)		wsprintf(buf1, "+%d%%", CBlockPercentage);
		else							wsprintf(buf1, "%d%%", CBlockPercentage);
		m_pChangeText[30]->SetStaticText(buf1);
	}
	else
	{
		CBlockPercentage = 0;
		char buf1[66] = { 0, };
		if (CBlockPercentage > 0)		wsprintf(buf1, "+%d%%", CBlockPercentage);
		else							wsprintf(buf1, "%d%%", CBlockPercentage);
		m_pChangeText[30]->SetStaticText(buf1);
	}

}


void CCharacterDialog::SetAttackRange()
{
	// 070114 LYW --- Modified this line.
	//m_ppStatic.attackdistance->SetStaticText("-");
	m_pChangeText[13]->SetStaticText( "-" ) ;
}
void CCharacterDialog::SetPointLeveling(BOOL val, WORD point)
{
	m_nocoriPoint = point;	
	m_pChangeText[11]->SetStaticValue( m_nocoriPoint );
	// 070113 LYW --- Delete this line.
	/*
	for(int i = 0 ; i < MAX_BTN_POINT ; i++)
		m_pPointBtn[i]->SetActive(val);
		*/
	
	HERO->SetHeroLevelupPoint( point );

	// 070124 LYW --- Setting status button.
	if( m_nocoriPoint > 0 )
	{
		for(int i = 0 ; i < MAX_BTN_POINT ; i++)
			m_pStatusBtn[i]->SetActive(TRUE);
	}
	else
	{
		for(int i = 0 ; i < MAX_BTN_POINT ; i++)
			m_pStatusBtn[i]->SetActive(FALSE);
	}
}
void CCharacterDialog::SetPointLevelingHide()
{
	WORD lvpoint = 0;
	m_pChangeText[11]->SetStaticValue( lvpoint );
	// 070113 LYW --- Delete this line.
	/*
	for(int i = 0 ; i < MAX_BTN_POINT ; i++)
		m_pPointBtn[i]->SetActive(FALSE);
		*/
	
	HERO->SetHeroLevelupPoint( 0 );
}
//old add stat
/*void CCharacterDialog::OnAddPoint(BYTE whatsPoint)
{
	MSG_WORD msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_POINTADD_SYN;
	msg.dwObjectID = HEROID;
	msg.wData = whatsPoint;

	
	if(--m_nocoriPoint <= 0)
	{
		SetPointLeveling(FALSE);
		//return;
	}
	// 070114 LYW --- Modified this line.
	//m_ppStatic.point->SetStaticValue(m_nocoriPoint);
	m_pChangeText[11]->SetStaticValue(m_nocoriPoint);
	
	NETWORK->Send(&msg,sizeof(MSG_WORD));
} */
//new add stat
//Alemuri Add Multiple Stat Option
void CCharacterDialog::OnAddPoint(BYTE whatsPoint)
{
	CMouse*	mouse = g_UserInput.GetMouse();
	long mouseLeft = mouse->GetMouseEventX(); //Mouse Left
	long mouseTop = mouse->GetMouseEventY(); //Mouse Top
	cDivideBox * pDivideBox = WINDOWMGR->DivideBox(DBOX_DIVIDE_INV, mouseLeft, mouseTop, OnInputStatsDivideOk, OnInputStatsDivideCancel, this, 0, CHATMGR->GetChatMsg(185));
	m_whatStat = whatsPoint;
}

void CCharacterDialog::OnInputStatsDivideOk(LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2)
{
	GAMEIN->GetCharacterDialog()->m_addedStatPoints = param1;
	GAMEIN->GetCharacterDialog()->SetAddedStats();
}

void CCharacterDialog::OnInputStatsDivideCancel(LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2)
{

}

void CCharacterDialog::SetAddedStats()
{
	MSG_WORD_ADDSTATPOINTS msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_POINTADD_SYN;
	msg.dwObjectID = HEROID;
	msg.wData = m_whatStat;

	if (m_addedStatPoints > m_nocoriPoint)
	{
		m_addedStatPoints = m_nocoriPoint;
	}

	msg.quantity = m_addedStatPoints;

	m_nocoriPoint = m_nocoriPoint - m_addedStatPoints;

	if (m_nocoriPoint <= 0)
	{
		SetPointLeveling(FALSE);
		//return;
	}
	// 070114 LYW --- Modified this line.
	//m_ppStatic.point->SetStaticValue(m_nocoriPoint);
	m_pChangeText[11]->SetStaticValue(m_nocoriPoint);

	NETWORK->Send(&msg, sizeof(MSG_WORD_ADDSTATPOINTS));
	m_addedStatPoints = 0;
}

void CCharacterDialog::RefreshGuildInfo()
{
	if( strlen(HERO->GetGuildName()) <= 0 ) 
	{
		m_pChangeText[3]->SetStaticText("") ;
	}
	else
	{
		m_pChangeText[3]->SetStaticText(HERO->GetGuildName());
	}
}

// 070503 LYW --- CharacterDialog : Add function to setting family name
void CCharacterDialog::RefreshFamilyInfo()
{
	CSHFamily* pFamily = HERO->GetFamily() ;

	if( pFamily )																	// 패밀리 정보가 유효한지 체크한다.
	{
		m_pFamilyName->SetStaticText(pFamily->Get()->szName) ;						// 패밀리명을 세팅한다.
	}
	else
	{
		m_pFamilyName->SetStaticText("") ;
	}
}

void CCharacterDialog::SetActive( BOOL val )
{
	cDialog::SetActive(
		val);

	GAMEIN->GetMiniMapDialog()->ChangePushLevelUpBTN();

	RefreshInfo();
	TUTORIALMGR->Check_OpenDialog(
		GetID(),
		val);
}

void CCharacterDialog::RefreshInfo()
{
	if(FALSE == IsActive())
	{
		return;
	}

	HERO->PassiveSkillCheckForWeareItem();
	CheckHeroImage();

	SetStrength();
	SetDexterity();
	SetVitality();
	SetIntelligence() ;
	SetWisdom();
	SetAttackRate();
	SetDefenseRate();
	SetCritical();
	UpdateData();
	//aziz MallShop in Game Method 1
	//SetVipInfo();
	//aziz Fish 24 Sep
	//SetFishInfo();
	//aziz Reborn 24 Sep
	//SetRebornInfo();
	//aziz Kill Shop 30 Sep
	SetKillInfo();
	//aziz Reborn Point 13 Oct
	//SetRBPointInfo();

	PrintAttackRate() ;
	PrintDefenseRate() ;
	PrintMagAttackRate() ;
	PrintMagDefenseRate() ;
	PrintAccuracyRate() ;
	PrintEvasionRate() ;
	PrintCriticalRate() ;
	PrintCriticalDamage();
	PrintMagicCriticalRate() ;
	PrintMoveSpeedRate() ;
	PrintAttackSpeedRate() ;
	PrintCastingSpeedRate() ;
	PrintBlockRate();
	PrintRange();
}


// 070111 LYW --- Add function to process events.
//=================================================================================================
// NAME			: ActionEvent
// PURPOSE		: Processing envet function.
// ATTENTION	:
//=================================================================================================
DWORD CCharacterDialog::ActionEvent( CMouse* mouseInfo )
{
	DWORD we = WE_NULL ;

	we |= cDialog::ActionEvent( mouseInfo ) ;

	return we ;
}


//=================================================================================================
// NAME			: OnActionEvent
// PURPOSE		: Processing envet function.
// ATTENTION	:
//=================================================================================================
void CCharacterDialog::OnActionEvent( LONG lId, void* p, DWORD we )
{
	if( lId == CI_CLASSTREEBTN )
	{
		if( !m_bCheckedTreeInfo )
		{
			m_bCheckedTreeInfo = TRUE ;
			// 070416 LYW --- CharacterDialog : Delete call check tree.
			CheckTreeInfo() ;
		}

		if( m_bShowTreeDlg )
		{
			m_bShowTreeDlg = FALSE ;
			ShowTreeDlg() ;
		}
		else
		{
			m_bShowTreeDlg = TRUE ;
			ShowTreeDlg() ;
		}
	}
}


// 070111 LYW --- Add function to check and show tree information.
//=================================================================================================
// NAME			: CheckTreeInfo
// PURPOSE		: Check information of class tree.
// ATTENTION	:
//=================================================================================================
void CCharacterDialog::CheckTreeInfo()
{
	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( gHeroID );
    
	if( !pPlayer )
		return;

	BYTE jobGrade	= pPlayer->GetCharacterTotalInfo()->JobGrade ;
	BYTE job		= pPlayer->GetCharacterTotalInfo()->Job[0] ;
	
	// skr : jobtreevie 19 oct 2019
	//BYTE job2		= pPlayer->GetCharacterTotalInfo()->Job[1] ;
	//BYTE job3		= pPlayer->GetCharacterTotalInfo()->Job[2] ;
	//BYTE job4		= pPlayer->GetCharacterTotalInfo()->Job[3] ;
	//BYTE job5		= pPlayer->GetCharacterTotalInfo()->Job[4] ;
	//BYTE job6		= pPlayer->GetCharacterTotalInfo()->Job[5] ;
	BYTE racez = pPlayer->GetCharacterTotalInfo()->Race; 
	
	if( jobGrade >= 0 ){
		for( int ii = jobGrade ; ii < 6 ; ii++){
			m_pClassStatic[ii]->SetStaticText( RESRCMGR->GetMsg( 1080) ) ;
		}

	}
			// 070118 LYW --- Modified this part.
			
			//m_pClassStatic[0]->SetStaticText( "Human log" ) ;
			//m_pClassStatic[1]->SetStaticText( "Sheep" ) ;
			//m_pClassStatic[2]->SetStaticText( "Hawk" ) ;
			//m_pClassStatic[3]->SetStaticText( "Hunter" ) ;
			//m_pClassStatic[4]->SetStaticText( "Phantom" ) ;
			//m_pClassStatic[5]->SetStaticText( "Dagger master" ) ;
			//m_pClassStatic[6]->SetStaticText( "" ) ;
			//m_pClassStatic[7]->SetStaticText( "Assassin" ) ;
			//m_pClassStatic[8]->SetStaticText( "" ) ;
	
	switch( job ){
		case 1: m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 41) ) ; break;
		case 2: m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 42) ) ; break;
		case 3: m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 43) ) ; break;
		case 4: m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 44) ) ; break;
		case 5: m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 45) ) ; break;
		case 6: m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 46) ) ; break;
	} 
	
	WORD idx[6] = { 0 };
	int rescmsg[6] = { 0 };
	idx[0] = 1;
	WORD jiba = 0;
	if( jobGrade > 1 )
	{
		for( int yun = 1; yun < jobGrade; yun++){
			idx[ yun ] = pPlayer->GetCharacterTotalInfo()->Job[ yun ];
			jiba = WORD(( job * 1000 ) + ( ( racez + 1 ) * 100 ) + ( ( yun + 1 ) * 10 ) + idx[ yun ]);
			
			rescmsg[ yun ] = RESRCMGR->GetClassNameNum( jiba );
			m_pClassStatic[ yun ]->SetStaticText( RESRCMGR->GetMsg( rescmsg[ yun ] ));
		}
		//idx = ChaInfo.Job[ChaInfo.JobGrade - 1];
	}
	//WORD job = ( ChaInfo.Job[0] * 1000 ) + ( ( ChaInfo.Race + 1 ) * 100 ) + ( ChaInfo.JobGrade * 10 ) + idx;

	//m_pChangeText[5]->SetStaticText( RESRCMGR->GetMsg(RESRCMGR->GetClassNameNum(job)) ) ; 
/*
	switch( job )
	{
	case 1 :
		{
			// 070122 LYW --- Modified this line.
			//m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 41) ) ;
			m_pClassStatic[1]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[2]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[3]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[4]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[5]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[6]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[7]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[8]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
		}
		break ;

	case 2 :
		{
			m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 42) ) ;
			m_pClassStatic[1]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[2]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[3]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[4]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[5]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[6]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[7]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[8]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
		}
		break ;

	case 3 :
		{
			m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 43) ) ;
			//m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[1]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[2]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[3]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[4]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[5]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[6]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[7]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[8]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
		}
		break ;

	case 4 :
		{
			m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 44) ) ;
			m_pClassStatic[1]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[2]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[3]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[4]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[5]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[6]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[7]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[8]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
		}
		break ;

	case 5 :
		{
			m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 45) ) ;
			m_pClassStatic[1]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[2]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[3]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[4]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[5]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[6]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[7]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[8]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
		}
		break ;

	case 6 :
		{
			m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 46) ) ;
			m_pClassStatic[1]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[2]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[3]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[4]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[5]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[6]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[7]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[8]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
		}
		break ;
	}
*/
	for(int i=0; i<6; ++i)
	{
		m_pClassStatic[i]->SetFGColor( RGB(255, 255, 255) ) ;
	}

	// skr : 
	m_pClassStatic[ jobGrade - 1 ]->SetFGColor( RGB(0, 255, 255) ) ; // current job is yellow~

	/*
	switch( jobGrade )
	{
	case 1 :
		{
			m_pClassStatic[0]->SetFGColor( RGB(0, 255, 255) ) ;
			//m_pChangeText[5]->SetStaticText( m_pClassStatic[0]->GetStaticText() ) ;
		}
		break ;

	case 2 :
		{
			m_pClassStatic[1]->SetFGColor( RGB(0, 255, 255) ) ;
			//m_pChangeText[5]->SetStaticText( m_pClassStatic[1]->GetStaticText() ) ;
		}
		break ;

	case 3 :
		{
			BYTE thirdJob = pPlayer->GetCharacterTotalInfo()->Job[2] ;

			if( (thirdJob%2) != 0 )
			{
				m_pClassStatic[2]->SetFGColor( RGB(0, 255, 255) ) ;
				//m_pChangeText[5]->SetStaticText( m_pClassStatic[2]->GetStaticText() ) ;
			}
			else
			{
				m_pClassStatic[3]->SetFGColor( RGB(0, 255, 255) ) ;
				//m_pChangeText[5]->SetStaticText( m_pClassStatic[3]->GetStaticText() ) ;
			}
		}
		break ;

	case 4 :
		{
			m_pClassStatic[4]->SetFGColor( RGB(0, 255, 255) ) ;
			//m_pChangeText[5]->SetStaticText( m_pClassStatic[4]->GetStaticText() ) ;
		}
		break ;

	case 5 :
		{
			BYTE thirdJob = pPlayer->GetCharacterTotalInfo()->Job[4] ;

			char tempClassName[128] = {0, } ;

			if( (thirdJob%2) != 0 )
			{
				m_pClassStatic[5]->SetFGColor( RGB(0, 255, 255) ) ;
				m_pClassStatic[6]->SetFGColor( RGB(0, 255, 255) ) ;

				sprintf( tempClassName, "%s%s", m_pClassStatic[5]->GetStaticText(), m_pClassStatic[6]->GetStaticText() ) ;
				//m_pChangeText[5]->SetStaticText( tempClassName ) ;
			}
			else
			{
				m_pClassStatic[7]->SetFGColor( RGB(0, 255, 255) ) ;
				m_pClassStatic[8]->SetFGColor( RGB(0, 255, 255) ) ;

				sprintf( tempClassName, "%s%s", m_pClassStatic[7]->GetStaticText(), m_pClassStatic[8]->GetStaticText() ) ;
				//m_pChangeText[5]->SetStaticText( tempClassName ) ;
			}
		}
		break ;
	} */
}


//=================================================================================================
// NAME			: ShowTreeDlg
// PURPOSE		: Setting whether controls for tree dialog to show or not.
// ATTENTION	:
//=================================================================================================
void CCharacterDialog::ShowTreeDlg()
{
	//Alemuri Update Job Text when you show the Tree Dialog------------------------------------------------------
	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( gHeroID );

	BYTE job		 = pPlayer->GetCharacterTotalInfo()->Job[0] ;
	BYTE jobGrade	= pPlayer->GetCharacterTotalInfo()->JobGrade ;
	BYTE racez = pPlayer->GetCharacterTotalInfo()->Race; 

	switch( job ){
		case 1: m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 41) ) ; break;
		case 2: m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 42) ) ; break;
		case 3: m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 43) ) ; break;
		case 4: m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 44) ) ; break;
		case 5: m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 45) ) ; break;
		case 6: m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 46) ) ; break;
	} 

	WORD idx[6] = { 0 };
	int rescmsg[6] = { 0 };
	idx[0] = 1;
	WORD jiba = 0;
	if( jobGrade > 1 )
	{
		for( int yun = 1; yun < jobGrade; yun++){
			idx[ yun ] = pPlayer->GetCharacterTotalInfo()->Job[ yun ];
			jiba = WORD(( job * 1000 ) + ( ( racez + 1 ) * 100 ) + ( ( yun + 1 ) * 10 ) + idx[ yun ]);
			
			rescmsg[ yun ] = RESRCMGR->GetClassNameNum( jiba );
			m_pClassStatic[ yun ]->SetStaticText( RESRCMGR->GetMsg( rescmsg[ yun ] ));
		}
		//idx = ChaInfo.Job[ChaInfo.JobGrade - 1];
	}
	//------------------------------------------------------------------------------------------------------

	int count = 0 ;

	if( m_bShowTreeDlg )
	{

		m_pTreeBgImg->SetActive( TRUE ) ;
		m_pTreeMark->SetActive( TRUE ) ; //The entire Background image including subimage
		m_pTreeTitle->SetActive( TRUE ) ; //Title
		m_MarryLabel->SetActive( TRUE ); //Alemuri Changed marry label
		m_KillLabel->SetActive( TRUE );
		m_KillPoints->SetActive( TRUE );
		m_KillLabelFighter->SetActive( TRUE );
		m_KillPointsFighter->SetActive( TRUE );
		m_KillLabelRogue->SetActive( TRUE );
		m_KillPointsRogue->SetActive( TRUE );
		m_KillLabelMage->SetActive( TRUE );
		m_KillPointsMage->SetActive( TRUE );
		m_KillLabelDevil->SetActive( TRUE );
		m_KillPointsDevil->SetActive( TRUE );
		m_MarryName->SetActive( TRUE );
		//m_pChangeText[29]->SetActive( TRUE ); //Alemuri Changed Marry Name TRUE

		for( count = 0 ; count < MAX_SUBITEM ; ++count )
		{
			m_pSubItem[ count ]->SetActive( TRUE ) ;
		}

		for( count = 0 ; count < MAX_CLASS_STATIC ; ++count )
		{
			m_pClassStatic[ count ]->SetActive( TRUE ) ;
		}

		//Alemuri Job Icons---------------------------------------------------
		CHARACTER_TOTALINFO* TotalInfo = HERO->GetCharacterTotalInfo();
		BYTE job0 = TotalInfo->Job[0];
		BYTE job1 = TotalInfo->Job[1]; //Warrior=2 //Infantryman=1
		BYTE job2 = TotalInfo->Job[2];
		BYTE job3 = TotalInfo->Job[3];
		BYTE job4 = TotalInfo->Job[4];
		if (TotalInfo->Job[0] == 1) //Fighter
		{
			//Job 0-------------------------------------
			m_JobFighterIcon->SetActive(TRUE);
			//------------------------------------------

			//Job 1-------------------------------------
			if (TotalInfo->Job[1] == 1) 
			{
				m_JobGuardIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[1] == 2)
			{
				m_JobWarriorIcon->SetActive(TRUE);
			}
			//------------------------------------------
			
			//Job 2-------------------------------------
			if (TotalInfo->Job[2] == 1) 
			{
				m_JobInfantrymanIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[2] == 2)
			{
				m_JobSwordsmanIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[2] == 3)
			{
				m_JobMercenaryIcon->SetActive(TRUE);
			}
			//------------------------------------------

			//Job 3-------------------------------------
			if (TotalInfo->Job[3] == 1) 
			{
				m_JobPhalanxIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[3] == 2)
			{
				m_JobKnightIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[3] == 3)
			{
				m_JobGladiatorIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[3] == 4)
			{
				m_JobRuneknightIcon->SetActive(TRUE);
			}
			//------------------------------------------

			//Job 4-------------------------------------
			if (TotalInfo->Job[4] == 1) 
			{
				m_JobPaladinIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[4] == 2)
			{
				m_JobPanzerIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[4] == 3)
			{
				m_JobCrusaderIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[4] == 4)
			{
				m_JobDestroyerIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[4] == 5)
			{
				m_JobSwordmasterIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[4] == 6)
			{
				m_JobMagnusIcon->SetActive(TRUE);
			}
			//------------------------------------------
		}
		else if (TotalInfo->Job[0] == 2) //Mage
		{
			//Job 0-------------------------------------
			m_JobMageIcon->SetActive(TRUE);
			//------------------------------------------

			//Job 1-------------------------------------
			if (TotalInfo->Job[1] == 1) 
			{
				m_JobClericIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[1] == 2)
			{
				m_JobWizardIcon->SetActive(TRUE);
			}
			//------------------------------------------
			
			//Job 2-------------------------------------
			if (TotalInfo->Job[2] == 1) 
			{
				m_JobPriestIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[2] == 2)
			{
				m_JobSorcererIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[2] == 3)
			{
				m_JobMonkIcon->SetActive(TRUE);
			}
			//------------------------------------------

			//Job 3-------------------------------------
			if (TotalInfo->Job[3] == 1) 
			{
				m_JobBishopIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[3] == 2)
			{
				m_JobWarlockIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[3] == 3)
			{
				m_JobInquirerIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[3] == 4)
			{
				m_JobElementalmasterIcon->SetActive(TRUE);
			}
			//------------------------------------------

			//Job 4-------------------------------------
			if (TotalInfo->Job[4] == 1) 
			{
				m_JobCardinalIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[4] == 2)
			{
				m_JobSoularbiterIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[4] == 3)
			{
				m_JobGrandmasterIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[4] == 4)
			{
				m_JobNecromancerIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[4] == 5)
			{
				m_JobRunemasterIcon->SetActive(TRUE);
			}
			//------------------------------------------
		}
		else if (TotalInfo->Job[0] == 3)
		{
			//Job 0-------------------------------------
			m_JobRogueIcon->SetActive(TRUE);
			//------------------------------------------

			//Job 1-------------------------------------
			if (TotalInfo->Job[1] == 1) 
			{
				m_JobVoyagerIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[1] == 2)
			{
				m_JobRuffianIcon->SetActive(TRUE);
			}
			//------------------------------------------
			
			//Job 2-------------------------------------
			if (TotalInfo->Job[2] == 1) 
			{
				m_JobArcherIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[2] == 2)
			{
				m_JobThiefIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[2] == 3)
			{
				m_JobScoutIcon->SetActive(TRUE);
			}
			//------------------------------------------

			//Job 3-------------------------------------
			if (TotalInfo->Job[3] == 1) 
			{
				m_JobRangerIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[3] == 2)
			{
				m_JobTreasurehunterIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[3] == 3)
			{
				m_JobAssassinIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[3] == 4)
			{
				m_JobRunewalkerIcon->SetActive(TRUE);
			}
			//------------------------------------------

			//Job 4-------------------------------------
			if (TotalInfo->Job[4] == 1) 
			{
				m_JobSniperIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[4] == 2)
			{
				m_JobEntrapperIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[4] == 3)
			{
				m_JobBladetakerIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[4] == 4)
			{
				m_JobTempermasterIcon->SetActive(TRUE);
			}
			else if (TotalInfo->Job[4] == 5)
			{
				m_JobArchrangerIcon->SetActive(TRUE);
			}
			//------------------------------------------
		}
		//--------------------------------------------------------------------
	}
	else
	{
		m_pTreeBgImg->SetActive( FALSE ) ;
		m_pTreeMark->SetActive( FALSE ) ;
		m_pTreeTitle->SetActive( FALSE ) ;
		m_MarryLabel->SetActive( FALSE ); //Alemuri marry label
		m_KillLabel->SetActive( FALSE );
		m_KillPoints->SetActive( FALSE );
		m_KillLabelFighter->SetActive( FALSE );
		m_KillPointsFighter->SetActive( FALSE );
		m_KillLabelRogue->SetActive( FALSE );
		m_KillPointsRogue->SetActive( FALSE );
		m_KillLabelMage->SetActive( FALSE );
		m_KillPointsMage->SetActive( FALSE );
		m_KillLabelDevil->SetActive( FALSE );
		m_KillPointsDevil->SetActive( FALSE );
		m_MarryName->SetActive( FALSE );
		//m_pChangeText[29]->SetActive( FALSE ); //Alemuri Changed Marry Name FALSE

		for( count = 0 ; count < MAX_SUBITEM ; ++count )
		{
			m_pSubItem[ count ]->SetActive( FALSE ) ;
		}

		for( count = 0 ; count < MAX_CLASS_STATIC ; ++count )
		{
			m_pClassStatic[ count ]->SetActive( FALSE ) ;
		}

		//Alemuri Job Icons---------------------------------------------------
		CHARACTER_TOTALINFO* TotalInfo = HERO->GetCharacterTotalInfo();
		BYTE job0 = TotalInfo->Job[0];
		BYTE job1 = TotalInfo->Job[1]; //Warrior=2 
		BYTE job2 = TotalInfo->Job[2];
		BYTE job3 = TotalInfo->Job[3];
		BYTE job4 = TotalInfo->Job[4];
		if (TotalInfo->Job[0] == 1) //Fighter
		{
			m_JobFighterIcon->SetActive(FALSE);
			m_JobGuardIcon->SetActive(FALSE);
			m_JobWarriorIcon->SetActive(FALSE);
			m_JobInfantrymanIcon->SetActive(FALSE);
			m_JobSwordsmanIcon->SetActive(FALSE);
			m_JobMercenaryIcon->SetActive(FALSE);
			m_JobPhalanxIcon->SetActive(FALSE);
			m_JobKnightIcon->SetActive(FALSE);
			m_JobGladiatorIcon->SetActive(FALSE);
			m_JobRuneknightIcon->SetActive(FALSE);
			m_JobPaladinIcon->SetActive(FALSE);
			m_JobPanzerIcon->SetActive(FALSE);
			m_JobCrusaderIcon->SetActive(FALSE);
			m_JobDestroyerIcon->SetActive(FALSE);
			m_JobSwordmasterIcon->SetActive(FALSE);
			m_JobMagnusIcon->SetActive(FALSE);
		}
		else if (TotalInfo->Job[0] == 2) //Mage
		{
			m_JobMageIcon->SetActive(FALSE);
			m_JobClericIcon->SetActive(FALSE);
			m_JobWizardIcon->SetActive(FALSE);
			m_JobPriestIcon->SetActive(FALSE);
			m_JobSorcererIcon->SetActive(FALSE);
			m_JobMonkIcon->SetActive(FALSE);
			m_JobBishopIcon->SetActive(FALSE);
			m_JobWarlockIcon->SetActive(FALSE);
			m_JobInquirerIcon->SetActive(FALSE);
			m_JobElementalmasterIcon->SetActive(FALSE);
			m_JobCardinalIcon->SetActive(FALSE);
			m_JobSoularbiterIcon->SetActive(FALSE);
			m_JobGrandmasterIcon->SetActive(FALSE);
			m_JobNecromancerIcon->SetActive(FALSE);
			m_JobRunemasterIcon->SetActive(FALSE);			
		}
		else if (TotalInfo->Job[0] == 3) //Rogue
		{
			m_JobRogueIcon->SetActive(FALSE);
			m_JobVoyagerIcon->SetActive(FALSE);
			m_JobRuffianIcon->SetActive(FALSE);
			m_JobArcherIcon->SetActive(FALSE);
			m_JobThiefIcon->SetActive(FALSE);
			m_JobScoutIcon->SetActive(FALSE);
			m_JobRangerIcon->SetActive(FALSE);
			m_JobTreasurehunterIcon->SetActive(FALSE);
			m_JobAssassinIcon->SetActive(FALSE);
			m_JobRunewalkerIcon->SetActive(FALSE);
			m_JobSniperIcon->SetActive(FALSE);
			m_JobEntrapperIcon->SetActive(FALSE);
			m_JobBladetakerIcon->SetActive(FALSE);
			m_JobTempermasterIcon->SetActive(FALSE);
			m_JobArchrangerIcon->SetActive(FALSE);
		}
		//--------------------------------------------------------------------
	}
}


//=================================================================================================
// NAME			: ShowTreeDlg
// PURPOSE		: Add function to check character image.
// ATTENTION	: // 070112 LYW --- Add function to check character image.
//=================================================================================================
void CCharacterDialog::CheckHeroImage()
{
	int count = 0 ;

	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( HERO->GetID() );
	if( NULL == pPlayer )
	{
		return;
	}
	m_byCharImgNum	= pPlayer->GetCharacterTotalInfo()->Race ;

	// 090423 ONS 신규종족 캐릭터 정보 다이얼로그 히어로 이미지 추가
	for( count = 0 ; count < MAX_CHAR_IMAGE ; ++count )
	{
		m_pCharImage[ count ]->SetActive( FALSE ) ;
	}

	switch( m_byCharImgNum )
	{
		// 070122 LYW --- Modified this part.

	case RaceType_Human :
		{
			if( pPlayer->GetCharacterTotalInfo()->Gender == 0 )
			{
				m_pCharImage[2]->SetActive( TRUE ) ;
			}
			else
			{
				m_pCharImage[3]->SetActive( TRUE ) ;
			}
		}
		break ;

	case RaceType_Elf :
		{
			if( pPlayer->GetCharacterTotalInfo()->Gender == 0 )
			{
				m_pCharImage[0]->SetActive( TRUE ) ;
			}
			else
			{
				m_pCharImage[1]->SetActive( TRUE ) ;
			}
		}
		break ;
	// 090423 ONS 신규종족 캐릭터 정보 다이얼로그 히어로 이미지 추가
	case RaceType_Devil:
		{
			if( pPlayer->GetCharacterTotalInfo()->Gender == 0 )
			{
				m_pCharImage[4]->SetActive( TRUE ) ;
			}
			else
			{
				m_pCharImage[5]->SetActive( TRUE ) ;
			}
		}
		break;
	}
}
/*//aziz MallShop in Game Method 1
void CCharacterDialog::SetVipInfo()
{
	if(VIPMGR->GetCurVipPoint() >= MAX_FISHPOINT)
		m_pChangeText[30]->SetStaticText("Invalid Value !!!");
	else
	m_pChangeText[30]->SetStaticValue(VIPMGR->GetVipPoint());
}
//aziz Fish 24 Sep
void CCharacterDialog::SetFishInfo()
{
	m_pChangeText[31]->SetStaticValue(FISHINGMGR->GetFishPoint());
}
//aziz Reborn 24 Sep
void CCharacterDialog::SetRebornInfo()
{
	if(UTILITYMGR->GetCurRebornData() >= MAX_FISHPOINT)
		m_pChangeText[32]->SetStaticText("Invalid Value !!!");
	else
	m_pChangeText[32]->SetStaticValue(UTILITYMGR->GetRebornData());
} */
//aziz Kill Shop 30 Sep
void CCharacterDialog::SetKillInfo()
{
	if(KILLMGR->GetCurKillPoint() >= MAX_FISHPOINT)
		m_KillPoints->SetStaticText("Invalid Value !!!");
		//m_pChangeText[33]->SetStaticText("Invalid Value !!!");
	else
	{
		m_KillPoints->SetStaticValue(KILLMGR->GetKillPoint());
		m_KillPointsFighter->SetStaticValue(KILLMGR->GetKillPointFighter());
		m_KillPointsRogue->SetStaticValue(KILLMGR->GetKillPointRogue());
		m_KillPointsMage->SetStaticValue(KILLMGR->GetKillPointMage());
		m_KillPointsDevil->SetStaticValue(KILLMGR->GetKillPointDevil());
	}
	//m_pChangeText[33]->SetStaticValue(KILLMGR->GetKillPoint());
}
/*//aziz Reborn Point 13 Oct
void CCharacterDialog::SetRBPointInfo()
{
	if(UTILITYMGR->GetCurRebornPoint() >= MAX_FISHPOINT)
		m_pChangeText[34]->SetStaticText("Invalid Value !!!");
	else
	m_pChangeText[34]->SetStaticValue(UTILITYMGR->GetRebornPoint());
} */