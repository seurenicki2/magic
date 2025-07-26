#include "stdafx.h"
#include "MonsterTargetDlg.h"
#include "ObjectGuagen.h"
#include "WindowIDEnum.h"
#include "./Interface/cStatic.h"
#include "./Interface/cWindowManager.h"
#include "MonsterGuageDlg.h"
#include "cCheckBox.h"
#include "Monster.h"
#include "GameResourceManager.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "StatusIconDlg.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[CC]Skill/Client/Info/BuffSkillInfo.h"
#include "BattleGuageDlg.h"
#include "GameIn.h"
#include "MHMap.h"
#include "StreetTournamentMgr.h"
#include "ItemManager.h"
#include "cButton.h"


//Alemuri Align Vector Strings---
#include <iomanip>
#include <string>  
#include <iostream> 
#include <sstream>   
//-------------------------------

cMonsterTargetDlg::cMonsterTargetDlg()
{
	m_pName				=	NULL;
	m_pLevel			=	NULL;
	m_pGuage			=	NULL;
	m_pGuageMain_Left	=	NULL;
	m_pGuageMain_Middle	=	NULL;
	m_pGuageMain_Right	=	NULL;
	m_pBuffCheckBox		=	NULL;
	m_pSightCheckBox	=	NULL;
	m_bVisibleBuff		=	NULL;
	m_bVisibleSight		=	NULL;
	m_dwLifePoint		=	NULL;
	m_dwAlphaStartTime	=	NULL;

	m_pLifePoint_Empty	=	NULL;
	for( int i = 0; i < MAXLIFEPOINT; ++i )
	{
		m_pLifePoint[i]	=	NULL;
	}
	m_pSpecies			=	NULL;
	m_pFace				=	NULL;

	m_pVSMain_Left		=	NULL;
	m_pVSMain_Right		=	NULL;
	m_pVS				=	NULL;
	m_pVSGuage			=	NULL;
	m_pVSName			=	NULL;
	mAlphaTick			=	0;
	m_pCurTargetObject	=	NULL;

	m_bVisibleBuff		=	TRUE;
	m_bVisibleSight		=	TRUE;

	m_pMonsterBuffIconDlg	=	NULL;

	m_dwLvColor			=	0;
}

cMonsterTargetDlg::~cMonsterTargetDlg()
{
	SAFE_DELETE( m_pMonsterBuffIconDlg );
}

void cMonsterTargetDlg::Render()
{

	if(mAlphaTick > gCurTime)
		SetMonsterTargetAlpha( BYTE((mAlphaTick - gCurTime) / 20 ) + 5 );

	if( m_bVisibleBuff )
		m_pMonsterBuffIconDlg->Render();

	if( !m_bVisibleSight )
		SetMonsterVSUIActive( false );


	if( m_dwLifePoint == 1 )
	{
		DWORD	dwCurTick		=	GetTickCount();
		DWORD	dwResultTime	=	dwCurTick - m_dwAlphaStartTime;
		if( dwResultTime > 1000 )
		{
			m_dwAlphaStartTime = GetTickCount();
		}
		m_pLifePoint[LP_ONE]->SetAlpha((BYTE)(3.5f * (dwResultTime/10) ) );
	}
	else
	{
		m_pLifePoint[LP_ONE]->SetAlpha( 255 );
	}
	cDialog::Render();
}

void cMonsterTargetDlg::SetAbsXY( LONG x, LONG y )
{
	if( m_pMonsterBuffIconDlg )
	{
		VECTOR2 position = {
			(float)x + m_BuffIconStartPoint.x,
			(float)y + m_BuffIconStartPoint.y };
		m_pMonsterBuffIconDlg->SetPosition( position );
	}

	cDialog::SetAbsXY( x, y );
}

void cMonsterTargetDlg::OnActionEvent( LONG lId, void* p, DWORD we )
{
	switch(lId)
	{
	case CG_MONSTER_BUFFICON_CHECKBOX:
	case CG_BOSS_BUFFICON_CHECKBOX:
	case CG_PLAYER_BUFFICON_CHECKBOX:
		{
			if( m_pBuffCheckBox->IsChecked() )
				m_bVisibleBuff	=	TRUE;
			else
				m_bVisibleBuff	=	FALSE;
		}
		break;

	case CG_MONSTER_SIGHTICON_CHECKBOX:
	case CG_BOSS_SIGHTICON_CHECKBOX:
	case CG_PLAYER_SIGHTICON_CHECKBOX:
		{
			if( m_pSightCheckBox->IsChecked() )
			{
				m_bVisibleSight	=	TRUE;
			}
			else
			{
				m_bVisibleSight	=	FALSE;
			}
		}
		break;
	case CG_MONSTER_DROP_BUTTON:
	{
		ItemDropList();
	}
	break;
	}
}

void cMonsterTargetDlg::SetMonsterUIActive( bool val )	
{
	m_pName->SetActive( val );
	m_pLevel->SetActive( val );
	m_pGuage->SetActive( val );
	m_pGuageMain_Left->SetActive( val );
	m_pGuageMain_Middle->SetActive( val );
	m_pGuageMain_Right->SetActive( val );
	m_pBuffCheckBox->SetActive( val );
	m_pSightCheckBox->SetActive( val );

	m_pLifePoint_Empty->SetActive( val );
	m_pSpecies->SetActive( val );
	m_pFace->SetActive( val );
	m_pMonsterBuffIconDlg->RemoveAllSkillIcon();

	for( int i = 0; i < MAXLIFEPOINT; ++i )
	{
		m_pLifePoint[i]->SetActive( val );
	}
	cDialog* itemDropButton = (cDialog*)GetWindowForID(CG_MONSTER_DROP_BUTTON);

	if (itemDropButton == FALSE)
	{
		return;
	}
	itemDropButton->SetActive(val);
}

void cMonsterTargetDlg::SetMonsterVSUIActive( bool val )
{
	m_pVSMain_Left->SetActive( val );
	m_pVSMain_Right->SetActive( val );
	m_pVS->SetActive( val );
	m_pVSGuage->SetActive( val );
	m_pVSName->SetActive( val );
}


void cMonsterTargetDlg::SetTargetGuageInfo( CObject* pObject )
{
	CMonster* pMonster	=	(CMonster*)pObject;

	m_dwLifePoint	=	0;

	float	fGuage	=	(float)pMonster->GetLife()/(float)((CMonster*)pMonster)->GetSInfo()->Life;
	SetMonsterLife( fGuage );
	UpdateLifePoint();
}

void cMonsterTargetDlg::SetTargetNameInfo( CObject* pObject )
{
	char targetName[128]	=	{ 0, };
	char targetLevel[128]	=	{ 0, };

	CPlayer*	pPlayer		=	NULL;
	CMonster*	pMonster	=	NULL;

	if( m_eTargetType == CHARACTER )
	{
		m_pCurTargetObject	=	pObject;
		pPlayer	=	(CPlayer*)pObject;
		m_dwLvColor	=	GAMEIN->GetBattleGuageDlg()->SetTargetLevelSignal( pPlayer->GetLevel() );

		if(MAP->GetMapNum() == StreetTournament && STREETTOURNAMENTMGR->GetState() == 3 && pPlayer->GetID() != HEROID)
		{
			sprintf( targetName, "Enemy");
		} 
		else 
		{
		sprintf( targetName, "%s",pPlayer->GetObjectName() );
		sprintf( targetLevel, "Lv.%d", pPlayer->GetLevel() );
		}
	}
	else
	{
		m_pCurTargetObject	=	pObject;
		pMonster		=	(CMonster*)pObject;
		m_dwLvColor	=	GAMEIN->GetBattleGuageDlg()->SetTargetLevelSignal( pMonster->GetSInfo()->Level );

		sprintf( targetName, "%s",pMonster->GetSInfo()->Name );
		sprintf( targetLevel, "Lv.%d", pMonster->GetSInfo()->Level );
	}

	SetMonsterUIActive( true );

	m_pName->SetFGColor( RGBA_MAKE( 255, 255, 255, 255 ) );
	m_pName->SetStaticText( targetName );
	m_pName->SetTextAlpha( true );

	m_pLevel->SetFGColor( m_dwLvColor );
	m_pLevel->SetStaticText( targetLevel );
	m_pLevel->SetTextAlpha( true );

	mAlphaTick = gCurTime;
	SetMonsterTargetAlpha( 255 );
	cDialog* itemDropButton = (cDialog*)GetWindowForID(CG_MONSTER_DROP_BUTTON);
	if (itemDropButton == FALSE)
	{
		return;
	}
	itemDropButton->SetToolTip("Item Drop", RGBA_MAKE(255, 255, 255, 255));
}

void cMonsterTargetDlg::SetVersusInfo( DWORD dwTargetVSID )
{
	if( m_pCurTargetObject->GetID() == dwTargetVSID ) 
		return;

	CObject* pVS	=	OBJECTMGR->GetObject( dwTargetVSID );

	if( pVS && !pVS->IsDied() )
	{
		SetMonsterVSUIActive( true );

		m_pVSName->SetStaticText( pVS->GetObjectName() );
		m_pVSName->SetTextAlpha( true );

		m_dwTargetVSID	=	dwTargetVSID;
	}
	else
		SetMonsterVSUIActive( false );
}

void cMonsterTargetDlg::SetLifePointInfo( float fHP )
{
	DWORD	dwCurrentLifePoint	=	0;
	DWORD	dwPreLifePoint	=	m_dwLifePoint;

	if( fHP == 0.0f )
	{
		dwCurrentLifePoint	=	0;
		TargetDie();
	}
	else if( fHP <= LIFEPOINT_ONE )
	{
		dwCurrentLifePoint	=	1;
	}
	else if( fHP <= LIFEPOINT_TWO )
	{
		dwCurrentLifePoint	=	2;
	}
	else if( fHP <= LIFEPOINT_THREE )
	{
		dwCurrentLifePoint	=	3;
	}
	else if( fHP <= LIFEPOINT_FORE )
	{ 
		dwCurrentLifePoint	=	4;
	}
	else if( fHP <= LIFEPOINT_FIVE )
	{
		dwCurrentLifePoint	=	5;
	}

	if( dwCurrentLifePoint != m_dwLifePoint )
	{
		m_dwLifePoint	=	dwCurrentLifePoint;

		if( dwPreLifePoint != 0 )
			ChangeLifePoint();

		UpdateLifePoint();
		if( dwCurrentLifePoint == 1 )
		{
			m_dwAlphaStartTime	=	GetTickCount() - 2000;
			m_pLifePoint[LP_ONE]->SetAlpha( 255 );
		}
	}
}

void cMonsterTargetDlg::UpdateLifePoint()
{
	for( int i = 0; i < (int)m_dwLifePoint; ++i )
	{
		m_pLifePoint[i]->SetActive( true );
		m_pLifePoint[i]->SetAlpha( 255 );
	}

	for( int i = (int)m_dwLifePoint; i < MAXLIFEPOINT; ++i )
	{
		m_pLifePoint[i]->SetActive( false );
	}
}

void cMonsterTargetDlg::SetMonsterLife( float fMonsterLife )
{
	float	fGuage	=	fMonsterLife;

	SetLifePointInfo( fMonsterLife );

	m_pGuage->SetValue( fGuage, 0 );
}

void cMonsterTargetDlg::TargetDlgLink()
{
	m_pName		=	(cStatic*)GetWindowForID(CG_MONSTERGUAGENAME);
	m_pLevel	=	(cStatic*)GetWindowForID(CG_MONSTERGUAGELEVEL);
	m_pGuage	=	(CObjectGuagen*)GetWindowForID(CG_MONSTERGUAGE_HP);
	m_pFace		=	(cStatic*)GetWindowForID(CG_MONSTERRACE);

	m_pGuageMain_Left	=	(cStatic*)GetWindowForID(CG_MONSTERGUAGE_MAIN_LEFT);
	m_pGuageMain_Middle	=	(cStatic*)GetWindowForID(CG_MONSTERGUAGE_MAIN_MIDDLE);
	m_pGuageMain_Right	=	(cStatic*)GetWindowForID(CG_MONSTERGUAGE_MAIN_RIGHT);

	//몬스터 버프, 시선 체크박스
	m_pBuffCheckBox		=	(cCheckBox*)GetWindowForID(CG_MONSTER_BUFFICON_CHECKBOX);
	m_pSightCheckBox	=	(cCheckBox*)GetWindowForID(CG_MONSTER_SIGHTICON_CHECKBOX);

	//일반몬스터 타겟

	m_pVSMain_Left		=	(cStatic*)GetWindowForID(CG_MONSTERVERSUS_MAIN_LEFT);
	m_pVSMain_Right		=	(cStatic*)GetWindowForID(CG_MONSTERVERSUS_MAIN_RIGHT);
	m_pVS				=	(cStatic*)GetWindowForID(CG_MONSTERVERSUS_IMAGE);
	m_pVSGuage			=	(CObjectGuagen*)GetWindowForID(CG_MONSTERVERSUS_LIFEGUAGE);
	m_pVSName			=	(cStatic*)GetWindowForID(CG_MONSTERVERSUS_NAME);

	//몬스터 LifePoint
	m_pLifePoint_Empty	=	(cStatic*)GetWindowForID(CG_MONSTER_LIFEPOINT_EMPTY);

	m_pLifePoint[0]		=	(cStatic*)GetWindowForID(CG_MONSTER_LIFEPOINT_1);
	m_pLifePoint[1]		=	(cStatic*)GetWindowForID(CG_MONSTER_LIFEPOINT_2);
	m_pLifePoint[2]		=	(cStatic*)GetWindowForID(CG_MONSTER_LIFEPOINT_3);
	m_pLifePoint[3]		=	(cStatic*)GetWindowForID(CG_MONSTER_LIFEPOINT_4);
	m_pLifePoint[4]		=	(cStatic*)GetWindowForID(CG_MONSTER_LIFEPOINT_5);

	m_pSpecies			=	(cStatic*)GetWindowForID(CG_MONSTER_SPECIES);
	m_eTargetType		=	NORMAL;
}

void cMonsterTargetDlg::SetTargetVSGuage( float fVsLife )
{
	m_pVSGuage->SetValue( fVsLife, 0 );
}

void cMonsterTargetDlg::LinkMonsterBuffIcon( float fIconSize, POINT IconPos, DWORD dwSkillIconCount, DWORD dwMaxIconPerLine )
{
	m_pMonsterBuffIconDlg	=	new CStatusIconDlg;

	m_BuffIconStartPoint	=	IconPos;

	VECTOR2 position = {
		(float)IconPos.x,
		(float)IconPos.y };
	m_pMonsterBuffIconDlg->SetPosition( position );
	m_pMonsterBuffIconDlg->SetIconSize( fIconSize, fIconSize );
	m_pMonsterBuffIconDlg->SetMaxIconQuantity( dwSkillIconCount );
	m_pMonsterBuffIconDlg->Init( HERO, &position, dwMaxIconPerLine );
}

void cMonsterTargetDlg::InsertMonsterSkillIcon( DWORD ID, DWORD Index, DWORD time, int count )
{
	if( m_pMonsterBuffIconDlg )
		m_pMonsterBuffIconDlg->AddSkillIcon( ID, Index, time, count );
}

void cMonsterTargetDlg::RemoveMonsterSkillIcon( DWORD ID, DWORD Index )
{
	if( m_pMonsterBuffIconDlg )
		m_pMonsterBuffIconDlg->RemoveSkillIcon( ID, Index );
}

void cMonsterTargetDlg::RemoveMonsterAllSkillIcon()
{
	if( m_pMonsterBuffIconDlg )
		m_pMonsterBuffIconDlg->RemoveAllSkillIcon();
}
		
void cMonsterTargetDlg::StartFadeOut()
{
	mAlphaTick = gCurTime + 5000;
}

void cMonsterTargetDlg::SetMonsterTargetAlpha( BYTE al )
{
	SetAlpha( al );
	m_pGuageMain_Left->SetAlpha( al );
	m_pGuageMain_Middle->SetAlpha( al );
	m_pGuageMain_Right->SetAlpha( al );
	m_pBuffCheckBox->SetAlpha( al );
	m_pSightCheckBox->SetAlpha( al );
	m_pLifePoint_Empty->SetAlpha( al );
	m_pVSMain_Left->SetAlpha( al );
	m_pVSMain_Right->SetAlpha( al );
	m_pVS->SetAlpha( al );

	m_pName->SetFGColor( RGBA_MAKE( 255, 255, 255, al ) );
	m_pLevel->SetFGColor( RGBA_MERGE( m_dwLvColor, al ) );
	m_pVSName->SetFGColor( RGBA_MAKE( 255, 255, 255, al ) );
	cDialog* itemDropButton = (cDialog*)GetWindowForID(CG_MONSTER_DROP_BUTTON);
	if (itemDropButton == FALSE)
	{
		return;
	}
	itemDropButton->SetAlpha(al);
}

void cMonsterTargetDlg::TargetVSDie()
{
	SetMonsterVSUIActive( false );
}

void cMonsterTargetDlg::SetMonsterSpecies( DWORD dwSpecies )
{

}

void cMonsterTargetDlg::SetMonsterAttribute( DWORD dwAttribute )
{

}
void cMonsterTargetDlg::ItemDropList()
{
	
	CMonsterGuageDlg* const pMonsterGuageDlg = (CMonsterGuageDlg*)WINDOWMGR->GetWindowForID(CG_PLAYERGUAGEDLG);
	cDialog*  pMonsterTargetDialog = pMonsterGuageDlg->GetCurTarget();
	CObject* curTarget = pMonsterGuageDlg->GetCurTarget()->GetCurObejct();
	CMonster* curMonster = (CMonster*)curTarget;
	BASE_MONSTER_LIST* pMonInfo = GAMERESRCMNGR->GetMonsterListInfo(curMonster->GetMonsterKind());
	if (pMonsterGuageDlg == FALSE)
	{
		return;
	}
	if (pMonsterTargetDialog == FALSE)
	{
		return;
	}

	cDialog* itemDropButton = (cDialog*)GetWindowForID(CG_MONSTER_DROP_BUTTON);
	if (itemDropButton == FALSE)
	{
		return;
	}

	itemDropButton->SetToolTip("");
	//itemDropButton->SetToolTipWithFont(14, "");
	int countDropItems = 0;
	for (int countArray = 0; countArray < 15; ++countArray)
	{
		if (pMonInfo->dropItem[countArray].wItemIdx != 0)
		{
			countDropItems += 1;
		}
	}
	
	int count = 0;
	//Alignment (requires monospaced fonts) check size of each item name---------------------------
	//std::vector<std::string> checkSizeArray;
	//int sizest = 0;
	//for (int dropnum = 0; dropnum < 15; ++dropnum)
	//{
	//	if (pMonInfo->dropItem[dropnum].wItemIdx != 0)
	//	{
	//		char* itemName = ITEMMGR->GetItemInfo(pMonInfo->dropItem[dropnum].wItemIdx)->ItemName;
	//		std::string str(itemName);
	//		checkSizeArray.push_back(str);
	//		if (str.size() > sizest)
	//			sizest = str.size();
	//	}
	//}
	//std::string str1;
	//std::string str2;
	//std::string str3;
	//std::string str4;
	//----------------------------------------------------------------------------------------------

	int sizest = 0;
	int allSizest = 0;
	for (int i = 0; i < 15; ++i)
	{
		if (pMonInfo->dropItem[i].wItemIdx != 0)
		{
			DWORD serverDropRates = 0;
			serverDropRates = HERO->GetServerDropRatesInfo();
			const float fIncreaseDropRate = (HERO->GetRateBuffStatus()->IncreaseDropRate + HERO->GetRatePassiveStatus()->IncreaseDropRate);
			serverDropRates += fIncreaseDropRate * serverDropRates / 100;
			int multiplier = HERO->GetServerDropRatesInfoMultiplier();
			serverDropRates *= multiplier;
			char* itemName = ITEMMGR->GetItemInfo(pMonInfo->dropItem[i].wItemIdx)->ItemName;
			std::string stdItemName(itemName);
			int itemQty = (int)pMonInfo->dropItem[i].byCount;
			float itemPercentage = (float)pMonInfo->dropItem[i].dwDropPercent;
			itemPercentage = itemPercentage / 1000000 * serverDropRates; //alemuri
			if (itemPercentage > 100)
			{
				itemPercentage = 100;
			}

			//Alignment (requires monospaced fonts)------------------
			//for (int ii = 0; ii < sizest - stdItemName.size(); ii++)
			//	stdItemName.append(" ");
			//-------------------------------------------------------

			char itemPercentagestr[10];
			char itemQtyStr[5];
			sprintf(itemPercentagestr, "%.2f", itemPercentage);
			sprintf(itemQtyStr, "%d", itemQty);
			char fullStr[100];
			strcpy(fullStr, itemName);
			strcat(fullStr, " ");
			strcat(fullStr, " [Qty: ");
			strcat(fullStr, itemQtyStr);
			strcat(fullStr, "]");
			strcat(fullStr, " ");
			strcat(fullStr, " [Chance: ");
			strcat(fullStr, itemPercentagestr);
			strcat(fullStr, "%]");
			const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(pMonInfo->dropItem[i].wItemIdx);
			DWORD color = ITEMMGR->GetItemNameColor(*itemInfo);
			itemDropButton->AddToolTipLine(fullStr, color, pMonInfo->dropItem[i].wItemIdx, count);

			//Alignment (requires monospaced fonts)---------------------------------------------------------------------
			//stringstream itemNameStream;
			//stringstream quantityStream;
			//stringstream chanceStream;
			//stringstream closeParStream;
			//int fillingSize (sizest - stdItemName.size());
			//itemNameStream << stdItemName.c_str() <<  std::setw(2+fillingSize) << setfill(' ') << " ";
			//quantityStream << "[QTY: " << itemQtyStr << "]";
			//chanceStream << " [CHANCE: " << itemPercentagestr << "%";
			//closeParStream << "]";

			//str1 = itemNameStream.str();
			//str2 = quantityStream.str();
			//str3 = chanceStream.str();
			//str4 = closeParStream.str();

			//itemDropButton->AddToolTipLine(itemNameStream.str().c_str(), color, pMonInfo->dropItem[i].wItemIdx, count);
			//itemDropButton->AddToolTipPartialLine(FALSE, quantityStream.str().c_str(), RGB_HALF(255, 255, 0));
			//itemDropButton->AddToolTipPartialLine(FALSE, chanceStream.str().c_str(), RGB_HALF(255, 255, 204));
			//itemDropButton->AddToolTipPartialLine(FALSE, closeParStream.str().c_str(), RGB_HALF(255, 255, 204));
			//----------------------------------------------------------------------------------------------------------
			
			//Alignment (requires monospaced fonts)---------------------------------------------------------------------
			//itemDropButton->AddToolTipLine(stdItemName.c_str(), color, pMonInfo->dropItem[i].wItemIdx, count);
			//----------------------------------------------------------------------------------------------------------

			//itemDropButton->AddToolTipLine(itemName, color, pMonInfo->dropItem[i].wItemIdx, count);
			//itemDropButton->AddToolTipPartialLine(FALSE, " [Qty: ", RGB_HALF(255, 255, 0));
			//itemDropButton->AddToolTipPartialLine(FALSE, itemQtyStr, RGB_HALF(255, 255, 0));
			//itemDropButton->AddToolTipPartialLine(FALSE, "] ", RGB_HALF(255, 255, 0));
			//itemDropButton->AddToolTipPartialLine(FALSE, " [Chance: ", RGB_HALF(255, 255, 204));
			//itemDropButton->AddToolTipPartialLine(FALSE, itemPercentagestr, RGB_HALF(255, 255, 204));
			//itemDropButton->AddToolTipPartialLine(FALSE, "%]", RGB_HALF(255, 255, 204));

			int curRowSize = strlen(itemName) + strlen(itemQtyStr) + strlen(itemPercentagestr) + 22;
			if (curRowSize > sizest)
				sizest = curRowSize;

			allSizest += sizest;
			std::string numSpaces = "";

			for (int index = 0; index < sizest; index++)
				numSpaces.append(" ");

			if (count < countDropItems - 1)
			{
				itemDropButton->AddToolTipLine(" ", color);
				//itemDropButton->AddToolTipPartialLine(FALSE, "");
				//itemDropButton->AddToolTipPartialLine(TRUE, "", RGB_HALF(255, 255, 204));
				//itemDropButton->AddToolTipPartialLine(TRUE, numSpaces.c_str());
			}

			count += 1;
		}
	}
}