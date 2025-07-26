#include "stdafx.h"															// ǥ�� ��������� �����Ѵ�.
#include "../WindowIDEnum.h"
#include "../Interface/cWindowManager.h"
#include "../ChatManager.h"
#include "../cMsgBox.h"
#include "..\MHFile.h"
#include "ChatManager.h"
#include "./Input/Mouse.h"
#include "ObjectManager.h"
#include "Item.h"
#include "./Interface/cListDialog.h"
#include ".\cmapmovedialog.h"
#include "ObjectStateManager.h"

// 081006 LYW --- cMapMoveDialog : ���� �Ŵ��� ��� ����.
#include "./SiegeWarfareMgr.h"

// 081125 LYW --- cMapMoveDialog : ����Ʈ ���� �Ŵ��� ��� ����.
#include "./LimitDungeonMgr.h"
#include "./MHMap.h"
#include "DungeonMgr.h"
#include "Npc.h"

struct stMAPMOVEITEM
{
	MAPTYPE mapnum;
	TCHAR mapName[MAX_PATH];
};

cMapMoveDialog::cMapMoveDialog(void)
{
	m_pMapListDlg = 0;
	m_pMoveBtn = 0;
	m_pCloseBtn = 0;
	m_pXCloseBtn = 0;
	m_nSelectedMapNum = -1;
	m_pItem = 0;
	m_byUseCase = eMapMoveNone;
	m_TypePre = 0;
}





cMapMoveDialog::~cMapMoveDialog(void)
{
	ReleaseMapList();
}





void cMapMoveDialog::Linking()
{
	m_pMapListDlg	= (cListDialog*)GetWindowForID(MMD_MAPLIST) ;				// �� ����Ʈ�� ��ũ�Ѵ�.

	m_pMoveBtn		= (cButton*)GetWindowForID(MMD_MOVEBTN) ;					// �̵� ��ư�� ��ũ�Ѵ�.
	m_pCloseBtn		= (cButton*)GetWindowForID(MMD_CLOSEBTN) ;					// �ݱ� ��ư�� ��ũ�Ѵ�.
	m_pXCloseBtn	= (cButton*)GetWindowForID(MMD_XCLOSEBTN) ;					// X�ݱ� ��ư�� ��ũ�Ѵ�.

	InitializeMapList() ;														// �� ����Ʈ ������ �ʱ�ȭ �Ѵ�.
}

DWORD cMapMoveDialog::ActionEvent(CMouse * mouseInfo)
{
	if(IsDisable())
	{
		return WE_NULL;
	}

	const DWORD we = cDialog::ActionEvent( mouseInfo );

	if(m_pMapListDlg->PtInWindow(mouseInfo->GetMouseX(), mouseInfo->GetMouseY()))
	{
		if(we & WE_LBTNCLICK)
		{
			const ITEM* const pItem =  m_pMapListDlg->GetItem(m_pMapListDlg->GetCurSelectedRowIdx()) ;

			if(pItem)
			{
				m_nSelectedMapNum = pItem->line;
			}
	
			switch(m_byUseCase)
			{
			case eNormalStyle:
				SetMapMoveInfo();
				break;
			case eCastle_Rushen:
			case eCastle_Zevyn:
				SetSiegeWarMoveInfo();
				break;
			case eLimitDungeon:
				SetLimitDungeon();
				break;
			case eDungeon:
				SetDungeon();
				break;
			}
		}
		else if( we & WE_LBTNDBLCLICK)
		{
			MoveNotice() ;														// �� �̵� ��ũ�� ��û�Ѵ�.
		}
	}

	return we ;
}





void cMapMoveDialog::OnActionEvent( LONG id, void* p, DWORD event ) 
{
	if( event != WE_BTNCLICK ) return ;

	switch( id )
	{
	case MMD_MOVEBTN :
		{
			MoveNotice() ;
		}
		break ;

	case MMD_XCLOSEBTN :
	case MMD_CLOSEBTN :
		{
			// 080919 LYW --- cMapMoveDialog : ��� �뵵�� ���� ���� ó��.
			if( m_byUseCase == eNormalStyle )
			{
				if( m_pItem )
				{
					m_pItem->SetLock(FALSE) ;
					m_pItem = NULL ;
				}
			}
			
			SetActive( FALSE ) ;
		}
		break ;
	}
}

void cMapMoveDialog::SetUseCase(eMapMoveType type, DWORD dwSelectNpcID)
{
	// 091106 LUJ, GM���� �ƴ� ��쿡�� �ݺ� ó������ �ʵ��� �Ѵ�
#ifndef _GMTOOL_
	if(m_byUseCase == type)
	{
		return;
	}
#endif

	m_byUseCase = type;
	m_dwSelectedNpcID = dwSelectNpcID;

	switch(type)
	{
	case eNormalStyle:
		SetMapMoveInfo();
		break;
	case eCastle_Rushen:
	case eCastle_Zevyn:
		SetSiegeWarMoveInfo();
		break;
	case eLimitDungeon:
		SetLimitDungeon();
		break;
	case eDungeon:
		SetDungeon();
		break;
	}

	m_pMapListDlg->SetCurSelectedRowIdx(-1);
}

void cMapMoveDialog::InitializeMapList()
{
	CMHFile file;
	file.Init(
		"./System/Resource/MapMoveScroll.bin",
		"rb");

	while(!file.IsEOF())
	{
		stMAPMOVEITEM* const pItem = new stMAPMOVEITEM;

		if(0 == pItem)
		{
			continue;
		}
		
		m_MapMoveList.AddTail(pItem);
		ZeroMemory(pItem, sizeof(*pItem));

		pItem->mapnum = file.GetWord();
		SafeStrCpy(
			pItem->mapName,
			GetMapName(pItem->mapnum),
			sizeof(pItem->mapName) / sizeof(*pItem->mapName));
	}
}

void cMapMoveDialog::ReleaseMapList()
{
	PTRLISTPOS pDeletePos = m_MapMoveList.GetHeadPosition() ;

	while(pDeletePos)
	{
		stMAPMOVEITEM* pItem = (stMAPMOVEITEM*)m_MapMoveList.GetNext(pDeletePos) ;
		SAFE_DELETE(pItem);
	}

	m_MapMoveList.RemoveAll();
}





void cMapMoveDialog::MoveMapSyn()
{
	// 081125 LYW --- cMapMoveDialog : ���̵� ��û�� ��Ȳ�� ���� ó�� �Ѵ�.
	switch( m_byUseCase )
	{
	case eNormalStyle :
		{
			if( m_nSelectedMapNum <= 0 || m_nSelectedMapNum >= 170 )
			{
				WINDOWMGR->MsgBox( MBI_MAPMOVE_NOTICE, MBT_OK, CHATMGR->GetChatMsg( 56 ) ) ;	
				break;
			}
			//aziz limit enter pvp map //battle arena
			if( HERO->GetLevel() < 2 && m_nSelectedMapNum==PVP)
			{
				CHATMGR->AddMsg(
				CTC_OPERATOR, "You Must Level 2 to Enter");//johan
				break;
			}
			else if( HERO->IsPKMode() && MAP->GetMapNum() != PVP)
			{
				WINDOWMGR->MsgBox( MBI_MAPMOVE_NOTICE, MBT_OK, CHATMGR->GetChatMsg( 358 ) ) ;	
				break;
			}
			if (m_TypePre == 1)
			{
				MSG_DWORD4 msg;
				ZeroMemory(&msg, sizeof(msg));
				msg.Category	= MP_ITEM ;
				msg.Protocol	= MP_ITEM_MAPMOVE_SCROLL_SYN ;
				msg.dwObjectID	= HEROID ;
				msg.dwData1		= m_nSelectedMapNum ;
				msg.dwData2		= 0 ;
				msg.dwData3		= 0 ;
				msg.dwData4		= 1;
				NETWORK->Send( &msg, sizeof(MSG_DWORD4) ) ;
			}
			else
			{
				MSG_DWORD4 msg;
				ZeroMemory(&msg, sizeof(msg));
				msg.Category	= MP_ITEM ;
				msg.Protocol	= MP_ITEM_MAPMOVE_SCROLL_SYN ;
				msg.dwObjectID	= HEROID ;
				msg.dwData1		= m_nSelectedMapNum ;
				msg.dwData2		= m_pItem->GetPosition() ;
				msg.dwData3		= m_pItem->GetItemIdx() ;
				msg.dwData4		= 0;
				NETWORK->Send( &msg, sizeof(MSG_DWORD4) ) ;
			}
			//aziz PK and PVP can use map move scroll
		/*	else if( HERO->IsPKMode() && MAP->GetMapNum()==PVP )
			{
				MSG_DWORD3 msg;
				ZeroMemory(&msg, sizeof(msg));
				msg.Category	= MP_ITEM ;
				msg.Protocol	= MP_ITEM_MAPMOVE_SCROLL_SYN ;
				msg.dwObjectID	= HEROID ;
				msg.dwData1		= m_nSelectedMapNum ;
				msg.dwData2		= m_pItem->GetPosition() ;
				msg.dwData3		= m_pItem->GetItemIdx() ;

				NETWORK->Send( &msg, sizeof(MSG_DWORD3) ) ;
			}
			
			MSG_DWORD3 msg;
			ZeroMemory(&msg, sizeof(msg));
			msg.Category	= MP_ITEM ;
			msg.Protocol	= MP_ITEM_MAPMOVE_SCROLL_SYN ;
			msg.dwObjectID	= HEROID ;
			msg.dwData1		= m_nSelectedMapNum ;
			msg.dwData2		= m_pItem->GetPosition() ;
			msg.dwData3		= m_pItem->GetItemIdx() ;

			NETWORK->Send( &msg, sizeof(MSG_DWORD3) ) ;
*/
		}
		break ;

	case eCastle_Rushen :
	case eCastle_Zevyn :
		{
			MSG_WORD2 msg ;

			msg.Category	= MP_SIEGEWARFARE ;
			msg.Protocol	= MP_SIEGEWARFARE_CHANGEMAP_GUILDCOMMITTEE_SYN ;
			msg.dwObjectID	= gHeroID ;
			msg.wData1		= WORD(m_byUseCase);
			msg.wData2		= WORD(m_nSelectedMapNum);

			NETWORK->Send( &msg, sizeof(MSG_WORD2) ) ;
		}
		break;
	}
}


void cMapMoveDialog::MoveNotice() 
{
	switch(m_byUseCase)
	{
	case eCastle_Rushen:
		{
			st_GUILDMAPMOVEINFO Info = {0};
			SIEGEWARFAREMGR->Get_GuildMapMoveInfo_Rushen(
				&Info,
				WORD(m_nSelectedMapNum));

			if(0 == Info.mapNum)
			{
				break;
			}

			WINDOWMGR->MsgBox(
				MBI_MAPMOVE_AREYOUSURE,
				MBT_YESNO,
				CHATMGR->GetChatMsg(480),
				GetMapName(Info.mapNum));
		}
		break ;

	case eCastle_Zevyn:
		{
			st_GUILDMAPMOVEINFO Info = {0};
			SIEGEWARFAREMGR->Get_GuildMapMoveInfo_Zevyn(
				&Info,
				WORD(m_nSelectedMapNum));

			if(0 == Info.mapNum)
			{
				break;
			}

			WINDOWMGR->MsgBox(
				MBI_MAPMOVE_AREYOUSURE,
				MBT_YESNO,
				CHATMGR->GetChatMsg(480),
				GetMapName(Info.mapNum));
		}
		break;
	case eNormalStyle:
		{
			PTRLISTPOS pDeletePos = m_MapMoveList.GetHeadPosition();

			while( pDeletePos )
			{
				stMAPMOVEITEM* pItem = (stMAPMOVEITEM*)m_MapMoveList.GetNext(pDeletePos);

				if( !pItem ) continue ;

				if( pItem->mapnum == m_nSelectedMapNum )
				{
					WINDOWMGR->MsgBox(
						MBI_MAPMOVE_AREYOUSURE,
						MBT_YESNO,
						CHATMGR->GetChatMsg(480),
						pItem->mapName);
				}
			}
		}
		break;
	case eLimitDungeon:
		{
			typedef CGameResourceManager::LimitDungeonScriptMap ScriptMap;
			const ScriptMap& scriptMap = g_CGameResourceManager.GetLimitDungeonScriptMap();
			const LimitDungeonScript* script = 0;
			int line = 0;

			for(ScriptMap::const_iterator iter = scriptMap.begin();
				scriptMap.end() != iter;
				++iter)
			{
				if(++line == m_nSelectedMapNum)
				{
					script = &(iter->second);
					LIMITDUNGEONMGR->AskJoin(
						script->mMapType,
						script->mChannel);
					break;
				}
			}
		}
		break;

	case eDungeon:
		{
			const ITEM* const pItem =  m_pMapListDlg->GetItem( m_pMapListDlg->GetClickedRowIdx() );
			if( !pItem )
				break;

			const DWORD dwDungeonKey = pItem->dwData;
			stDungeonKey* pDungeonKeyInfo = GAMERESRCMNGR->GetDungeonKey( dwDungeonKey );
			if( !pDungeonKeyInfo )
				break;

			if(MAP->GetMapNum() == pDungeonKeyInfo->wMapNum)
			{
				CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(1977) );
				break;
			}

			CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( m_dwSelectedNpcID );
			if( !pNpc )
				break;

			// �δ� ���� ��û
			MSG_DWORD4 msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category = MP_DUNGEON;
			msg.Protocol = MP_DUNGEON_PORTAL_ENTRANCE_SYN;
			msg.dwData1 = pDungeonKeyInfo->dwIndex;
			msg.dwData2 = pDungeonKeyInfo->wMapNum;
			msg.dwData3 = pNpc->GetID();
			msg.dwData4 = pNpc->GetNpcUniqueIdx();
			NETWORK->Send(&msg,sizeof(msg));
		}
		break;
	}
}

// 080919 LYW --- cMapMoveDialog : ��� �뵵�� ���� ���̵� ���� �����Լ�.
void cMapMoveDialog::SetMapMoveInfo()
{
	// �ӽ� �������� ���� / �ʱ�ȭ �Ѵ�.
	char tempBuf[512] ;
	DWORD dwColor = RGB_HALF(10, 10, 10) ;

	int BackupLine = 0 ;
	BackupLine = m_pMapListDlg->GetTopListItemIdx() ;


	// ����Ʈ ���̾�α׸� ����.
	m_pMapListDlg->RemoveAll() ;

	stMAPMOVEITEM* pNormalItem ;


	// �� �̵� ������ �����Ѵ�.
	PTRLISTPOS pos ;
	pos = m_MapMoveList.GetHeadPosition() ;	

	// ����Ʈ ���̾�α׸� �����Ѵ�.
	while(pos)
	{
		pNormalItem = NULL ;
		pNormalItem = (stMAPMOVEITEM*)m_MapMoveList.GetNext(pos) ;

		if( !pNormalItem ) continue ;

		if( m_nSelectedMapNum == pNormalItem->mapnum )
		{
			memset(tempBuf, 0, sizeof(tempBuf)) ;
			
			strcpy(tempBuf, "[ ") ;
			strcat(tempBuf, GetMapName(MAPTYPE(m_nSelectedMapNum)));
			strcat(tempBuf, " ]") ;

			m_pMapListDlg->AddItem(tempBuf, RGB_HALF(255, 255, 0), pNormalItem->mapnum) ;
			continue ;
		}

		m_pMapListDlg->AddItem(pNormalItem->mapName, dwColor, pNormalItem->mapnum) ;
	}


	// �������� ���� �ε����� �����Ѵ�.
	m_pMapListDlg->ResetGuageBarPos() ;
	m_pMapListDlg->SetTopListItemIdx(BackupLine) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: SetSiegeWarMoveInfo()
//	DESC		: ���������� ���������� ���� �� �̵��� �� ��, ���̵� ������ �����ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: November 25, 2008
//-------------------------------------------------------------------------------------------------
void cMapMoveDialog::SetSiegeWarMoveInfo()
{
	const int BackupLine = m_pMapListDlg->GetTopListItemIdx();
	m_pMapListDlg->RemoveAll();
	const CSiegeWarfareMgr::L_GUILDMAPMOVEINFO& moveList = (m_byUseCase == eCastle_Zevyn ? SIEGEWARFAREMGR->Get_MapMoveList_Zevyn() : SIEGEWARFAREMGR->Get_MapMoveList_Rushen());

	for(CSiegeWarfareMgr::L_GUILDMAPMOVEINFO::const_iterator it = moveList.begin();
		it != moveList.end();
		++it )
	{
		const st_GUILDMAPMOVEINFO& info = *it ;

		if(m_nSelectedMapNum == info.mapNum)
		{
			TCHAR text[MAX_PATH] = {0};
			_stprintf(
				text,
				"[ %s ]",
				GetMapName(info.mapNum));
			m_pMapListDlg->AddItem
				(text,
				RGB_HALF(255, 255, 0),
				info.mapNum);
		}
		else
		{
			m_pMapListDlg->AddItem(
				GetMapName(info.mapNum),
				RGB_HALF(10, 10, 10),
				info.mapNum);
		}
	}


	// �������� ���� �ε����� �����Ѵ�.
	m_pMapListDlg->ResetGuageBarPos() ;
	m_pMapListDlg->SetTopListItemIdx(BackupLine) ;
}

void cMapMoveDialog::SetLimitDungeon()
{
	const int previousLine = m_pMapListDlg->GetTopListItemIdx();
	m_pMapListDlg->RemoveAll();

	typedef CGameResourceManager::LimitDungeonScriptMap ScriptMap;
	const ScriptMap& scriptMap = g_CGameResourceManager.GetLimitDungeonScriptMap();
	int line = 0;

	for(ScriptMap::const_iterator iter = scriptMap.begin();
		scriptMap.end() != iter;
		++iter)
	{
		const LimitDungeonScript& script = iter->second;
		cListDialog::ToolTipTextList toolTipList;
		LIMITDUNGEONMGR->SetToolTip(
			script,
			toolTipList);
		TCHAR titleText[MAX_PATH] = {0};
		_stprintf(
			titleText,
			"%s (%s)",
			GetMapName(script.mMapType),
			CHATMGR->GetChatMsg(script.mNameIndex));

		if(m_nSelectedMapNum == line + 1)
		{
			TCHAR text[MAX_PATH] = {0};
			_stprintf(
				text,
				"[ %s ]",
				titleText);
			m_pMapListDlg->AddItem(
				text,
				RGB_HALF(255, 255, 0),
				toolTipList,
				++line);
		}
		else
		{
			m_pMapListDlg->AddItem(
				titleText,
				RGB_HALF(10, 10, 10),
				toolTipList,
				++line);
		}
	}

	m_pMapListDlg->ResetGuageBarPos();
	m_pMapListDlg->SetTopListItemIdx(previousLine);
}

void cMapMoveDialog::SetDungeon()
{
	const int prevTopList = m_pMapListDlg->GetTopListItemIdx();
	const int prevClickedList = m_pMapListDlg->GetClickedRowIdx();
	m_pMapListDlg->RemoveAll();


	CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( m_dwSelectedNpcID );
	if( !pNpc )
		return;

	std::multimap< DWORD, DWORD >& entranceNpcJobMap = DungeonMGR->GetEntranceNpcJobMap();
	typedef std::multimap<DWORD, DWORD>::const_iterator EntranceNpcJobIter;
	std::pair<EntranceNpcJobIter, EntranceNpcJobIter> EqualRange;
	EqualRange = entranceNpcJobMap.equal_range( pNpc->GetNpcJob() );

	if( EqualRange.first == entranceNpcJobMap.end() &&
		EqualRange.second == entranceNpcJobMap.end() )
	{
		return;
	}

	int line = 0;
	for( EntranceNpcJobIter npcIter = EqualRange.first ; npcIter != EqualRange.second ; ++npcIter )
	{
		const DWORD dwDungeonKey	= npcIter->second;

		stDungeonKey* pDungeonKeyInfo = GAMERESRCMNGR->GetDungeonKey( dwDungeonKey );
		if( !pDungeonKeyInfo )
			return;

		// 100616 ONS �δ� ���尡�� ������ ���͸� �� ���� ��±�� �߰�
		const WORD ENTRANCE_PLUS_LEVEL = 20;
		LEVELTYPE heroLevel = HERO->GetLevel() ;
		if( heroLevel + ENTRANCE_PLUS_LEVEL <= MAX_CHARACTER_LEVEL_NUM )
		{
			// �ִ뷹���� ���� �ʴ´ٸ� ���緹�� +20���� ���尡���ϵ��� üũ�Ѵ�.
			if( !(heroLevel >= pDungeonKeyInfo->wMinLevel					|| 
				  heroLevel + ENTRANCE_PLUS_LEVEL >= pDungeonKeyInfo->wMinLevel)	||
				  heroLevel > pDungeonKeyInfo->wMaxLevel )
			{
				continue;
			}
		}
		else
		{
			// +20�� �ִ뷹���� �������(����131�̻�)�� �ش�Ǵ� ������ ���尡���ϵ��� üũ�Ѵ�.
			if( heroLevel < pDungeonKeyInfo->wMinLevel	|| 
				heroLevel > pDungeonKeyInfo->wMaxLevel )
			{
				continue;
			}
		}

		cListDialog::ToolTipTextList toolTipList;
		TCHAR toolTipTxt[MAX_PATH] = {0};
		_stprintf( toolTipTxt, "%s", CHATMGR->GetChatMsg( pDungeonKeyInfo->dwTooltipIndex ) );
		toolTipList.push_back(toolTipTxt);

		int messageid = 0;
		switch (pDungeonKeyInfo->difficulty)
		{
			case eDIFFICULTY_EASY:messageid = 1761;break;
			case eDIFFICULTY_NORMAL:messageid = 1762;break;
			case eDIFFICULTY_HARD:messageid = 1763;break;
			default:messageid = 2435;break;
		}
		TCHAR titleText[MAX_PATH] = {0};
		_stprintf( titleText, CHATMGR->GetChatMsg( 1764 ), 
								GetMapName( pDungeonKeyInfo->wMapNum ), 
								CHATMGR->GetChatMsg( messageid ),//warning
								pDungeonKeyInfo->wMinLevel, 
								pDungeonKeyInfo->wMaxLevel );

		if( m_nSelectedMapNum == ++line )
		{
			TCHAR text[MAX_PATH] = {0};
			_stprintf( text, "[ %s ]", titleText );
			m_pMapListDlg->AddItem( text, RGB_HALF(255, 255, 0), toolTipList, line, dwDungeonKey );
		}
		else
		{
			m_pMapListDlg->AddItem( titleText, RGB_HALF(10, 10, 10), toolTipList, line, dwDungeonKey );
		}
	}

	m_pMapListDlg->ResetGuageBarPos();
	m_pMapListDlg->SetTopListItemIdx( prevTopList );
	m_pMapListDlg->SetClickedRowIdx( prevClickedList );
}

void cMapMoveDialog::SetActive(BOOL isActive)
{
	if(MAP->GetMapNum() == StreetTournament && isActive)
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, "Map Move is disable at this map" );
		isActive = FALSE;
	}

	if(isActive)
	{
		OBJECTSTATEMGR->StartObjectState(
			OBJECTMGR->GetObject(gHeroID),
			eObjectState_Deal);
	}
	else
	{
		OBJECTSTATEMGR->EndObjectState(
			OBJECTMGR->GetObject(gHeroID),
			eObjectState_Deal);

		// 100319 ShinJS --- ESCŰ ���� Item Lock�� Ǯ���� �ʴ� ���� ����.
		if( m_pItem )
		{
			m_pItem->SetLock(FALSE) ;
			m_pItem = NULL ;
		}
	}

	cDialog::SetActive(isActive);
}

DWORD cMapMoveDialog::GetMapListCount() const
{ 
	return m_pMapListDlg->GetItemCount(); 
}
void cMapMoveDialog::SetTypePre(WORD type)
{ 
	m_TypePre = type;
	return;
}