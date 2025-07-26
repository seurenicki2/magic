#include "stdafx.h"
#include ".\ChatManager.h"
#include "MHFile.h"
#include "Interface/cResourceDef.h"
#include "ChattingDlg.h"

#ifdef _CHEATENABLE_			
#include "CheatMsgParser.h"
#endif	//_CHEATENABLE_		

#include "ChatManager.h"
#include "MacroManager.h"
#include "ObjectManager.h"
#include "PartyManager.h"
#include "GuildManager.h"
#include "GameIn.h"
#include "GameResourceManager.h"
#include "cIMEex.h"
#include "cMsgBox.h"
#include "Input/cIMEWnd.h"

#include "Interface/cWindowManager.h"
#include "Interface/cEditBox.h"
#include "Interface/cListDialog.h"
#include "Interface/cTextArea.h"

// 070106 LYW --- Include option manager.
#include "OptionManager.h"
#include "FilteringTable.h"


#ifdef _GMTOOL_
#include "GMToolManager.h"
#include "MainGame.h"
#endif

#include "DateMatchingDlg.h"
#include "cMonsterSpeechManager.h"
#include "GMNotifyManager.h"
#include "ChatRoomMgr.h"
#include "ChatRoomDlg.h"
#include "MHTimeManager.h"

//Alemuri Search Drop from Monsters---
#include "./Input/UserInput.h"
#include "ItemManager.h"
//------------------------------------

//Alemuri GMBuff Auto Code------------
#include "QuickManager.h"
#include "XSleep.h"
//------------------------------------

CChatManager::CChatManager(void)
{
	ZeroMemory(
		&m_ChatOption,
		sizeof(m_ChatOption));
	m_dwLastChatTime = 0;
	m_ForbidChatTime = 0;

	InitializeWhisperName();
	LoadChatMsg();
	LoadFaceCommandList();
	LoadActionCommandList();

	m_dwProgressTimeSearchDrop = gCurTime;
}


CChatManager::~CChatManager(void)
{
	PTRLISTPOS pos = m_FaceCommandList.GetHeadPosition() ;									// ¾ó±¼ ±³Ã¼ ¸í·É¾î ¸®½ºÆ®¿¡¼­ À§Ä¡ Æ÷Áö¼ÇÀ» ¹Þ¾Æ¿Â´Ù.

	for(sFACEINFO* pFaceInfo = (sFACEINFO*)m_FaceCommandList.GetNext( pos );
		0 < pFaceInfo;
		pFaceInfo = (sFACEINFO*)m_FaceCommandList.GetNext( pos ))
	{
		SAFE_DELETE( pFaceInfo );
	}

	m_FaceCommandList.RemoveAll() ;															// ¾ó±¼ º¯°æ ¸í·É ¸®½ºÆ®¸¦ ¸ðµÎ ºñ¿î´Ù.

	sACTION_COMMAND* pDeleteCommand ;

	PTRLISTPOS deletePos = NULL ;
	deletePos = m_ActionCommandList.GetHeadPosition() ;

	while( deletePos )																	// ÆÄÀÏÀÇ ³¡±îÁö whileÀ» µ¹¸°´Ù.
	{
		pDeleteCommand = NULL ;
		pDeleteCommand = (sACTION_COMMAND*)m_ActionCommandList.GetNext(deletePos) ;

		if( !pDeleteCommand ) continue ;

		m_ActionCommandList.Remove(pDeleteCommand) ;

		delete pDeleteCommand ;
	}

	m_ActionCommandList.RemoveAll() ;
}

void	CChatManager::LoadChatMsg()
{
	CMHFile fp;
	fp.Init("Data/Interface/Windows/SystemMsg.bin", "rb" );
	char msg[MAX_PATH] = {0};

	while(FALSE == fp.IsEOF())
	{
		const DWORD nIndex = fp.GetDword();
		fp.GetStringInQuotation(msg);

		if(0 < _tcslen(msg))
		{
			mTextContainer[nIndex] = msg;
			fp.GetLine(
				msg,
				_countof(msg));
		}
	}
}


char*	CChatManager::GetChatMsg( int nMsgNum )
{
	const TextContainer::iterator iterator = mTextContainer.find(
		nMsgNum);

	return mTextContainer.end() == iterator ? "" : iterator->second.c_str();
}

void CChatManager::AddMsg(eTextClass nClass, LPCTSTR str, ...)
{
	if(0 == m_pChattingDlg)
	{
		return;
	}

	char msg[512] = {0};
	va_list argList;
	va_start(
		argList,
		str);
	vsprintf(
		msg,
		str,
		argList);
	va_end(
		argList);

	switch(nClass)
	{
	case CTC_SHOUT:
		{
			m_pChattingDlg->AddShoutMsg(
				msg,
				GetMsgColor(nClass));
			break;
		}
	case CTC_OPERATOR:
		{
			m_pChattingDlg->AddMsg(CTC_OPERATOR, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			NOTIFYMGR->AddMsg(msg, eNTC_REMAINTIME);
			break;
		}
	case CTC_OPERATOR2:
		{
			NOTIFYMGR->AddMsg(msg, eNTC_RESIDENTREGIST);
			break;
		}
	case CTC_BILLING:
		{
			m_pChattingDlg->AddMsg(CTC_BILLING, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			NOTIFYMGR->AddMsg(msg, eNTC_REMAINTIME);
			break;
		}
	case CTC_GMCHAT:
		{
			m_pChattingDlg->AddMsg(CTC_GMCHAT, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_GENERALCHAT:
		{
			if(!m_ChatOption.bOption[CTO_NOCHATMSG])
			{
				m_pChattingDlg->AddMsg(CTC_GENERALCHAT, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_PARTYCHAT:
		{
			if(!m_ChatOption.bOption[CTO_NOPARTYMSG])
			{
				m_pChattingDlg->AddMsg(CTC_PARTYCHAT, e_CONVERSATION_PARTY, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_FAMILY:
		{
			if(!m_ChatOption.bOption[CTO_NOFAMILYMSG])
			{
				m_pChattingDlg->AddMsg(CTC_FAMILY, e_CONVERSATION_FAMILY, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_GUILDCHAT:
		{
			if(!m_ChatOption.bOption[CTO_NOGUILDMSG])
			{
				m_pChattingDlg->AddMsg(CTC_GUILDCHAT, e_CONVERSATION_GUILD, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_ALLIANCE:
		{
			if(!m_ChatOption.bOption[CTO_NOALLIANCEMSG])
			{
				m_pChattingDlg->AddMsg(CTC_ALLIANCE, e_CONVERSATION_ALLIANCE, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_TRADE:
		{
			if(!m_ChatOption.bOption[CTO_NOSHOUTMSG])
			{
				m_pChattingDlg->AddMsg(CTC_TRADE, e_CONVERSATION_TRADE, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_SYSMSG:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_SYSMSG, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_WHISPER:
		{
			m_pChattingDlg->AddMsg(CTC_WHISPER, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_ATTACK:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_ATTACK, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_ATTACKED:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_ATTACKED, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_DEFENCE:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_DEFENCE, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_DEFENCED:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_DEFENCED, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));	
			}

			break;
		}
	case CTC_KILLED:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_KILLED, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_MPWARNING:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_MPWARNING, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_HPWARNING:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_HPWARNING, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_GETMONEY:
	case CTC_GETITEM:
		{
			if(!m_ChatOption.bOption[CTO_NOITEMMSG])
			{
				m_pChattingDlg->AddMsg(CTC_GETITEM, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			}


			break;
		}
	case CTC_GETEXP:
		{
			if(!m_ChatOption.bOption[CTO_NOEXPMSG])
			{
				m_pChattingDlg->AddMsg(CTC_GETEXP,e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_TOWHOLE:
		{
			m_pChattingDlg->AddMsg(CTC_TOWHOLE, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_TOPARTY:
		{
			m_pChattingDlg->AddMsg(CTC_TOPARTY, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_TOFAMILY:
		{
			m_pChattingDlg->AddMsg(CTC_TOFAMILY, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_TOGUILD:
		{
			m_pChattingDlg->AddMsg(CTC_TOGUILD, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_MONSTERSPEECH:
		{
			m_pChattingDlg->AddMsg(CTC_MONSTERSPEECH, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_MONSTERSHOUT:
		{
			m_pChattingDlg->AddMsg(CTC_MONSTERSHOUT, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_EXITCOUNT:
		{
			m_pChattingDlg->AddMsg(CTC_EXITCOUNT, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_ENCHANT:
		{
			
			NOTIFYMGR->AddMsg(msg, eNTC_ENCHANT);
			/*m_pChattingDlg->AddShoutMsg(
				msg,
				GetMsgColor(nClass)); */
			break;
		}
	case CTC_EVENT:
		{
			m_pChattingDlg->AddMsg(CTC_EVENT, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			NOTIFYMGR->AddMsg(msg, eNTC_EVENT);
			break;
		}
	default:
		{
			m_pChattingDlg->AddMsg(CTC_SYSMSG, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			break;
		}
	}
	
}

void CChatManager::LoadActionCommandList()
{
	CMHFile	fp;
	fp.Init(
		"Data/interface/Windows/Action_Command.bin",
		"rb");

	int nIndex = 0 ;
	char tempBuf[32] = {0, } ;
	BOOL bVehicleOpt = FALSE; 

	while( ! fp.IsEOF() )																	// ÆÄÀÏÀÇ ³¡±îÁö whileÀ» µ¹¸°´Ù.
	{
		// 091029 ShinJS --- ÁÖ¼®Ã³¸® Ãß°¡
		char buf[ MAX_PATH ] = {0,};

		fp.GetLine(buf, MAX_PATH);
		
		if( buf[0] == '/' && buf[1] == '/' )
		{
			fp.GetLineX( buf, MAX_PATH );
			continue;
		}
		
		memset(tempBuf, 0, sizeof(tempBuf)) ;
		sscanf(buf, "%d %s %d", &nIndex, tempBuf, &bVehicleOpt);

		if( strlen(tempBuf) <= 2 ) continue ;

		sACTION_COMMAND* pCommand = new sACTION_COMMAND ;									// ¾ó±¼ Á¤º¸ ±¸Á¶Ã¼ ¸¸Å­ ¸Þ¸ð¸®¸¦ ÇÒ´çÇÏ¿© Æ÷ÀÎÅÍ·Î ³Ñ±ä´Ù.

		pCommand->index = nIndex ;															// ÀÎµ¦½º¸¦ ÀÐ´Â´Ù.
		strcpy(pCommand->string, tempBuf) ;													// ½ºÆ®¸µÀ» ÀÐ¾î Descript¿¡ º¹»çÇÑ´Ù.

		// 091022 ShinJS --- Å»°Í Å¾½Â½Ã Á¦ÇÑ ¿©ºÎ
		pCommand->bInvalidOnVehicle = bVehicleOpt;

		m_ActionCommandList.AddTail(pCommand) ;												// Á¤º¸¸¦ ¸®½ºÆ®¿¡ Ãß°¡ÇÑ´Ù.
	}
}

void CChatManager::LoadFaceCommandList()
{
	CMHFile	fp;
	fp.Init(
		"Data/interface/Windows/Chat_FaceList.bin",
		"rb");

	while( ! fp.IsEOF() )																	// ÆÄÀÏÀÇ ³¡±îÁö whileÀ» µ¹¸°´Ù.
	{
		sFACEINFO* pFaceInfo = new sFACEINFO ;												// ¾ó±¼ Á¤º¸ ±¸Á¶Ã¼ ¸¸Å­ ¸Þ¸ð¸®¸¦ ÇÒ´çÇÏ¿© Æ÷ÀÎÅÍ·Î ³Ñ±ä´Ù.

		pFaceInfo->nIndex = fp.GetInt() ;													// ÀÎµ¦½º¸¦ ÀÐ´Â´Ù.
		strcpy(pFaceInfo->szDescript, fp.GetString()) ;										// ½ºÆ®¸µÀ» ÀÐ¾î Descript¿¡ º¹»çÇÑ´Ù.
		strcpy(pFaceInfo->szCommand, fp.GetStringInQuotation()) ;							// ½ºÆ®¸µÀ» ÀÐ¾î Command¿¡ º¹»çÇÑ´Ù.
		pFaceInfo->nFaceNum = fp.GetInt() ;													// ¾ó±¼ ¹øÈ£À» ÀÐ´Â´Ù.
		pFaceInfo->nLen = strlen(pFaceInfo->szCommand) ;									// ¸í·É¾îÀÇ ±æÀÌ¸¦ ±¸ÇÑ´Ù.

		m_FaceCommandList.AddTail(pFaceInfo) ;												// Á¤º¸¸¦ ¸®½ºÆ®¿¡ Ãß°¡ÇÑ´Ù.
	}
}

int CChatManager::GetFaceNum( const TCHAR* str ) const
{
	// ¹®ÀÚ¸¦ ÇÏ³ª¾¿ Áõ°¡½ÃÄÑ¼­ ÇØ´ç ¹®ÀÚ°¡ Æ÷ÇÔµÇ¾ú´ÂÁö Ã£´Â´Ù.
	// ¹®ÀÚ¿­¿¡ ¸ÖÆ¼¹ÙÀÌÆ® ¹®ÀÚ°¡ Æ÷ÇÔµÇ¾î ÀÖÀ¸¹Ç·Î, Áõ°¡½ÃÅ³ ¶§´Â ¸ÖÆ¼¹ÙÀÌÆ® ´ÜÀ§·Î Áõ°¡½ÃÄÑ¾ß ÇÑ´Ù
	for(	const TCHAR* subString = str;
		*subString; 
		subString = _tcsinc( subString ) )
	{
		for(	PTRLISTPOS pos = m_FaceCommandList.GetHeadPosition();
			pos;
			)
		{
			const sFACEINFO* pFaceInfo = ( sFACEINFO* )m_FaceCommandList.GetNext( pos );
			ASSERT( pFaceInfo );

			if( ! _tcsncmp( subString, pFaceInfo->szCommand, _tcslen( pFaceInfo->szCommand ) ) )
			{
				return pFaceInfo->nFaceNum;
			}
		}
	}

	return -1;
}

void CChatManager::MsgProc( UINT msg, WPARAM wParam )
{
	if(		msg		== WM_KEYDOWN	&& 
		wParam	== VK_RETURN	&&
		!	CIMEWND->IsDocking() )
	{
		cMsgBox* pMsgBox = WINDOWMGR->GetFirstMsgBox();

		if( pMsgBox &&
			pMsgBox->PressDefaultBtn() )
		{
			return;
		}
	}

	if( ! m_pChattingDlg )
	{
		return;
	}

	cEditBox* pEdit = m_pChattingDlg->GetInputBox() ;
	if( pEdit == NULL ) return;

	// Ã¤ÆÃ¹æÀÇ ÅØ½ºÆ® ÀÔ·ÂÀ» ¹Þ´Â´Ù.
	//cTextArea* pChatRoomCtrl = NULL ;
	//pChatRoomCtrl = CHATROOMMGR->GetChatRoomTextArea() ;
	cEditBox* pChatRoomCtrl = NULL ;
	pChatRoomCtrl = CHATROOMMGR->GetChatRoomInputBox() ;

	ASSERT(pChatRoomCtrl) ;

	if(!pChatRoomCtrl)
	{
		CHATROOMMGR->Throw_Error("Failed to receive chatroom textarea!!", __FUNCTION__) ;
		return ;
	}


	//Alemuri Search Drop from Monsters------------------------------------------------------------------------
	std::string editBoxText(pEdit->GetEditText());
	std::string firstWord = editBoxText.substr(0, editBoxText.find(" "));
	//Alemuri convert firstWord all lowercase-----------------------
	std::for_each(firstWord.begin(), firstWord.end(), [](char & c){ 
	 c = ::tolower(c);
	});
	//--------------------------------------------------------------

	//if( strcmp( firstWord.c_str(), "//wheredrop" ) == 0  || strcmp( firstWord.c_str(), "//whatbox" ) == 0 || strcmp( firstWord.c_str(), "//whatnpc" ) == 0)
	//{
	//	BOOL TabAlreadyTriggered = FALSE;
	//	//if (g_UserInput.GetKeyboard()->GetKeyPressed( KEY_TAB ) && pEdit->IsFocus())	// && pEdit->IsFocus())
	//	if (wParam == VK_TAB && pEdit->IsFocus())
	//	{
	//		if( gCurTime - m_dwProgressTimeSearchDrop >= 300 ) //1000 = 1 Second, 60000 = 1 minute, 600000 = 10 minutes, 3600000 = 1 hour
	//		{
	//			//if (currentChatEditBoxText == pEdit->GetEditText())
	//			//{
	//			//	goto ignoreTab;
	//			//}
	//			//std::string strTemp(pEdit->GetEditText());
	//			//currentChatEditBoxText = strTemp;

	//			std::string s(pEdit->GetEditText());
	//			std::vector<std::string> chatSplitMsg = splitStringOnVector((char*)s.c_str(), " ", " ");
	//			
	//			ITEM_INFO* info = NULL;
	//			for (int i = 0; i < chatSplitMsg.size(); i++)
	//			{
	//				std::string partialSearch = "";
	//				for (int ii = 2; ii < chatSplitMsg.size(); ii++)
	//					partialSearch.append(chatSplitMsg[ii]);

	//				//Multi Tab--------------------------------------------
	//				if (m_tempSearchDropText == "")
	//				{
	//					m_tempSearchDropText = partialSearch.c_str();
	//				}
	//				//-----------------------------------------------------

	//				std::for_each(partialSearch.begin(), partialSearch.end(), [](char & c){ 
	//				c = ::tolower(c);
	//				});
	//				
	//				if (!TabAlreadyTriggered)
	//				{
	//					info = ITEMMGR->FindItemInfoForPartialNameFast((char*)m_tempSearchDropText.c_str()); //Multi Tab use m_tempSearchDropText instead of partialSearch
	//					TabAlreadyTriggered = TRUE;
	//				}
	//				
	//				
	//				if (info)
	//				{
	//					std::string completeChatLine = "";
	//					completeChatLine.append(chatSplitMsg[0]);
	//					completeChatLine.append(chatSplitMsg[1]);
	//					completeChatLine.append(info->ItemName);
	//					pEdit->SetEditText((char*)completeChatLine.c_str());
	//				}
	//			}
	//			m_dwProgressTimeSearchDrop = gCurTime;
	//		}
	//	}
	//}
	////ignoreTab:
	////-------------------------------------------------------------------------------------------------------

	//if( strcmp( firstWord.c_str(), "//mobstats" ) == 0)
	//{
	//	BOOL TabAlreadyTriggered = FALSE;
	//	//if (g_UserInput.GetKeyboard()->GetKeyPressed( KEY_TAB ) && pEdit->IsFocus())	// && pEdit->IsFocus())
	//	if (wParam == VK_TAB && pEdit->IsFocus())
	//	{
	//		if( gCurTime - m_dwProgressTimeSearchDrop >= 300 ) //1000 = 1 Second, 60000 = 1 minute, 600000 = 10 minutes, 3600000 = 1 hour
	//		{
	//			//if (currentChatEditBoxText == pEdit->GetEditText())
	//			//{
	//			//	goto ignoreTab;
	//			//}
	//			//std::string strTemp(pEdit->GetEditText());
	//			//currentChatEditBoxText = strTemp;

	//			std::string s(pEdit->GetEditText());
	//			std::vector<std::string> chatSplitMsg = splitStringOnVector((char*)s.c_str(), " ", " ");
	//			
	//			BASE_MONSTER_LIST* info = NULL;
	//			for (int i = 0; i < chatSplitMsg.size(); i++)
	//			{
	//				std::string partialSearch = "";
	//				for (int ii = 2; ii < chatSplitMsg.size(); ii++)
	//					partialSearch.append(chatSplitMsg[ii]);

	//				//Multi Tab--------------------------------------------
	//				if (m_tempSearchDropText == "")
	//				{
	//					m_tempSearchDropText = partialSearch.c_str();
	//				}
	//				//-----------------------------------------------------

	//				std::for_each(partialSearch.begin(), partialSearch.end(), [](char & c){ 
	//				c = ::tolower(c);
	//				});
	//				
	//				std::string MonsterName = "";
	//				if (!TabAlreadyTriggered)
	//				{
	//					//info = GAMEIN->FindMobInfoForPartialName((char*)m_tempSearchDropText.c_str()); //Multi Tab use m_tempSearchDropText instead of partialSearch
	//					MonsterName = GAMEIN->FindMobInfoForPartialName((char*)m_tempSearchDropText.c_str());
	//					TabAlreadyTriggered = TRUE;
	//				}
	//				
	//				
	//				if (MonsterName != "")
	//				{
	//					std::string completeChatLine = "";
	//					completeChatLine.append(chatSplitMsg[0]);
	//					completeChatLine.append(chatSplitMsg[1]);
	//					completeChatLine.append(MonsterName);
	//					pEdit->SetEditText((char*)completeChatLine.c_str());
	//				}
	//			}
	//			m_dwProgressTimeSearchDrop = gCurTime;
	//		}
	//	}
	//}
	////ignoreTab:
	//-------------------------------------------------------------------------------------------------------

	switch( msg )
	{
	case WM_CHAR:
	case WM_IME_CHAR:
	case WM_IME_COMPOSITION:
		{
			if( MACROMGR->IsChatMode()	&& !pEdit->IsFocus() && !CIMEWND->IsDocking() && pEdit->GetIME()->IsValidChar((unsigned char*)&wParam) && !pChatRoomCtrl->IsFocus())		
			{
				pEdit->SetFocusEdit( TRUE );
			}
		}
		break;

	case WM_KEYDOWN :
		{
			//Alemuri Search Drop Items from Mobs (if you change text manually in the chat, tab index reset
			if (wParam != VK_TAB)
			{
				m_tempSearchDropText = "";
				GAMEIN->resetTabIndex();
				ITEMMGR->resetTabIndex();	
			}
			//---------------------------------------------------------------------------------------------

			if( wParam == VK_RETURN )
			{
				// 080410 LYW --- ChatManager : Modified message proc.
				cEditBox*  pInputBox = GAMEIN->GetDateMatchingDlg()->GetChatingDlg()->GetEditBox() ;
				cTextArea* pTextArea = GAMEIN->GetDateMatchingDlg()->GetMyInfoDlg()->GetIntroduce() ;

				// ÀÏ¹Ý Ã¤ÆÃÃ¢ Ã³¸® Ã¼Å©.
				if (pInputBox->IsFocus())
				{
					char szText[256] = {0, } ;
					DWORD dwColor = RGB(0, 255, 255) ;

					sprintf(szText, "[%s]:%s", HERO->GetObjectName(), pInputBox->GetEditText()) ;		

					GAMEIN->GetDateMatchingDlg()->GetChatingDlg()->AddMsg(szText, dwColor);
					g_csResidentRegistManager.CLI_SendDateMatchingChatText(pInputBox->GetEditText()) ;
					pInputBox->SetEditText("");
				}
				// Ã¤ÆÃ¹æ ÂÊ Ã³¸® Ã¼Å©.
				else if(pChatRoomCtrl->IsFocus())
				{
					CHATROOMMGR->MsgProc() ;
				}
				// ¸ÅÄª ÂÊ Ã³¸® Ã¼Å©.
				else if( ! pTextArea->IsFocus() )
				{
					if( !CIMEWND->IsDocking() )		//´Ù¸¥¿¡µðÅÍ¿¡ Æ÷Ä¿½º°¡ ¾ø´Ù¸é.
					{	
						pEdit->SetFocusEdit( TRUE );
					}
					else
					{
						char* strChat = pEdit->GetEditText();

						if( *strChat )
						{
							OnInputChatMsg( strChat );
							pEdit->SetEditText("");
						}

						m_pChattingDlg->SetCommandToInputBox();

						pEdit->SetFocusEdit( FALSE );
					}
				}
			}		
		}
		break ;
	}
}

void CChatManager::OnInputChatMsg( char* str )
{
#ifdef _CHEATENABLE_
	if( !CHEATMGR->IsCheatEnable() )
#endif
		if( FILTERTABLE->FilterChat( str ) )
		{
			AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1225) );
			return;
		}

		// 100618 ONS ¼­¹ö½Ã°£°ú Ã¤ÆÃ±ÝÁö½Ã°£ÀÇ ºñ±³¸¦ ÅëÇØ Ã¤ÆÃ±ÝÁö¿©ºÎ¸¦ ÆÇ´ÜÇÑ´Ù.
		// Å¬¶óÀÌ¾ðÆ®°¡ Ã¼Å©ÇÔÀ¸·Î½á ¼­¹ö¿¡ ÆÐÅ¶À» º¸³»Áö ¾Êµµ·Ï Ã³¸®.
		if( m_ForbidChatTime > 0 )
		{
			__time64_t curTime = MHTIMEMGR->GetServerTime();
			if( curTime < m_ForbidChatTime )
			{
				ShowForbidChatTime( m_ForbidChatTime );
				return;
			}
			else
			{
				m_ForbidChatTime = 0;
			}
		}

		char nToken = *str;

		if( CanChatTime( nToken ) )
		{
			m_pChattingDlg->InspectMsg(str);
		}
}

BOOL CChatManager::CanChatTime( char cToken )
{
	if( gCurTime - m_dwLastChatTime < 1000 )	// 1ÃÊ
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(248) );
		return FALSE;
	}

	m_dwLastChatTime = gCurTime;
	return TRUE;
}

void	CChatManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch(Protocol)
	{
	case MP_CHAT_ALL:
	case MP_CHAT_GM:			// 090731 ShinJS --- GM Ã¤ÆÃ Ãß°¡
	case MP_CHAT_NORMAL :						Chat_Normal( pMsg ) ;										break ;
	case MP_CHAT_PARTY :						Chat_Party( pMsg ) ;										break ;
	case MP_CHAT_FAMILY :						Chat_Family( pMsg ) ;										break ;
	case MP_CHAT_GUILD :						Chat_Guild( pMsg ) ;										break ;
	case MP_CHAT_GUILDUNION :					Chat_Alliance( pMsg ) ;										break ;
	case MP_CHAT_TRADE :						Chat_Trade( pMsg ) ;										break ;
	case MP_CHAT_SYSTEM :						Chat_System( pMsg ) ;										break ;
	case MP_CHAT_WHOLE :						Chat_Whole( pMsg ) ;										break ;
	case MP_CHAT_WHISPER_GM:	// 090731 ShinJS --- GM Ã¤ÆÃ Ãß°¡
	case MP_CHAT_WHISPER:						Chat_Whisper( pMsg ) ;										break ;
	case MP_CHAT_WHISPER_ACK:					Chat_Whisper_Ack( pMsg ) ;									break ;
	case MP_CHAT_WHISPER_NACK:					Chat_Whisper_Nack( pMsg ) ;									break ;
	case MP_CHAT_MONSTERSPEECH:					Chat_Monster_Speech( pMsg ) ;								break ;
	case MP_CHAT_SHOUT_ACK:						Chat_Shout_Ack(pMsg);										break ;
	case MP_CHAT_SHOUT_NACK:					Chat_Shout_Nack(pMsg);										break ;
	
	case MP_CHAT_ALL_ITEMCHAT:					Chat_Normal_ItemChat(pMsg);									break ;
	case MP_CHAT_PARTY_ITEMCHAT:				Chat_Party_ItamChat(pMsg);									break ;
	case MP_CHAT_FAMILY_ITEMCHAT :				Chat_Family_ItemChat( pMsg ) ;								break ;
	case MP_CHAT_GUILDUNION_ITEMCHAT:			Chat_Alliance_ItemChat(pMsg) ;								break ;
	case MP_CHAT_GUILD_ITEMCHAT :				Chat_Guild_ItemChat( pMsg ) ;								break ;
	case MP_CHAT_TRADE_ITEMCHAT :				Chat_Trade_ItemChat( pMsg ) ;								break ;
	case MP_CHAT_SHOUT_ITEM_ACK:				Chat_Shout_Item_Ack(pMsg);									break ;
	case MP_CHAT_WHISPER_ITEM:					Chat_WhisperItem( pMsg ) ;									break ;
	case MP_CHAT_WHISPER_ITEM_ACK:				Chat_Whisper_Item_Ack( pMsg ) ;								break ;
	
//---KES CHEAT
	case MP_CHAT_FROMMONSTER_ALL:
		{
			TESTMSG* data = (TESTMSG*)pMsg;

			CObject* pObject = OBJECTMGR->GetObject(data->dwObjectID);
			if( !pObject ) return;

			OBJECTMGR->ShowChatBalloon( pObject, data->Msg );

		}
		break;
	case MP_CHAT_FORBIDCHAT:
		{
			stTime64t* pmsg = (stTime64t*)pMsg;
			m_ForbidChatTime = pmsg->time64t;
			ShowForbidChatTime( pmsg->time64t );
		}
		break;
	case MP_CHAT_PERMITCHAT:
		{
			m_ForbidChatTime = 0;
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(2313) );
		}
		break;
		//Alemuri Custom Commands----------------------
	case MP_CHAT_CUSTOMCOMMAND:
		{
			MSG_CHAT_CUSTOMCOMMAND* data = (MSG_CHAT_CUSTOMCOMMAND*)pMsg;
			
			if (strcmp(data->command, "/wheredrop") == 0)
			{
				std::string restoreApo(data->Msg);
				std::replace( restoreApo.begin(), restoreApo.end(), 'ƒ', '\''); //Restore back Apo
				SafeStrCpy(data->Msg, restoreApo.c_str(), MAX_CHAT_LENGTH+1);

				GAMEIN->SearchDropFromMobs(data->Msg);
				CHATMGR->resetSearDropPartialText();
				ITEMMGR->resetTabIndex();
			}			
			
			if (strcmp(data->command, "/whatbox") == 0)
			{
				std::string restoreApo(data->Msg);
				std::replace( restoreApo.begin(), restoreApo.end(), 'ƒ', '\''); //Restore back Apo
				SafeStrCpy(data->Msg, restoreApo.c_str(), MAX_CHAT_LENGTH+1);

				ITEMMGR->ListAllChangeItemsForItem(data->Msg);
				CHATMGR->resetSearDropPartialText();
				ITEMMGR->resetTabIndex();
			}

			if (strcmp(data->command, "/mobstats") == 0)
			{
				std::string restoreApo(data->Msg);
				std::replace( restoreApo.begin(), restoreApo.end(), 'ƒ', '\''); //Restore back Apo
				SafeStrCpy(data->Msg, restoreApo.c_str(), MAX_CHAT_LENGTH+1);

				GAMEIN->MobStats(data->Msg);
				CHATMGR->resetSearDropPartialText();
				ITEMMGR->resetTabIndex();
			}

			if (strcmp(data->command, "/whatnpc") == 0)
			{
				std::string restoreApo(data->Msg);
				std::replace( restoreApo.begin(), restoreApo.end(), 'ƒ', '\''); //Restore back Apo
				SafeStrCpy(data->Msg, restoreApo.c_str(), MAX_CHAT_LENGTH+1);

				GAMEIN->WhatNpc(data->Msg);
				CHATMGR->resetSearDropPartialText();
				ITEMMGR->resetTabIndex();
			}
			break;
		}
		//---------------------------------------------
//------------
	}
}
//extern int ChatType = 0;
void	CChatManager::Chat_Normal( void* pMsg )
{
	TESTMSG* const data = (TESTMSG*)pMsg;
	CObject* object = OBJECTMGR->GetObject(data->dwObjectID);

	if(0 == object)
	{
		return;
	}

	LPCTSTR keyword = _T("SystemMsg.bin\'");
	// 091019 LUJ, ¹®ÀÚ¿­ÀÌ ÁöÁ¤µÈ ¿¹¾à¾î·Î ½ÃÀÛµÇ¸é ½Ã½ºÅÛ ¸Þ½ÃÁö·Î °£ÁÖÇÑ´Ù
	const BOOL isSystemMessage = (0 == _tcsnicmp(data->Msg, keyword, _tcslen(keyword)));
	LPTSTR textMessage = textMessage = data->Msg;

	if(isSystemMessage)
	{
		LPCTSTR token = data->Msg + _tcslen(keyword);
		const DWORD messageIndex = _ttoi(token ? token : "");
		static TCHAR text[MAX_PATH];
		ZeroMemory(text, sizeof(text));
		_stprintf(
			text,
			"%s",
			GetChatMsg(messageIndex));
		textMessage = text;
	}
	//Alemuri GMBuff Auto Code----------------------------------
	std::string grabText(textMessage);
	std::for_each(grabText.begin(), grabText.end(), [](char & c){ 
	 c = ::tolower(c);
	});

	//std::string buffCode1 = "thesinofkala is cool";
	//std::string buffCode2 = "thesinofkala is cute";
	//std::string buffCode3 = "thesinofkala is handsome";
	//std::string buffCode4 = "thesinofkala is awesome";
	//std::string buffCode5 = "thesinofkala is the best";
	//std::string buffCode6 = "thesinofkala is love";
	//std::string buffCode7 = "thesinofkala is my hero";

	//std::string buffCode1 = "fumu is cool";
	//std::string buffCode2 = "fumu is cute";
	//std::string buffCode3 = "fumu is handsome";
	//std::string buffCode4 = "fumu is awesome";
	//std::string buffCode5 = "fumu is the best";
	//std::string buffCode6 = "fumu is love";
	//std::string buffCode7 = "fumu is my hero";
	
	//if (grabText == buffCode1)
	//{
	//	QUICKMGR->UseQuickItem( 1, ( WORD )( 0 ) );
	//}
	//else if (grabText == buffCode2)
	//{
	//	QUICKMGR->UseQuickItem( 1, ( WORD )( 1 ) );
	//}
	//else if (grabText == buffCode3)
	//{
	//	QUICKMGR->UseQuickItem( 1, ( WORD )( 2 ) );
	//}
	//else if (grabText == buffCode4)
	//{
	//	QUICKMGR->UseQuickItem( 1, ( WORD )( 3 ) );
	//}
	//else if (grabText == buffCode5)
	//{
	//	QUICKMGR->UseQuickItem( 1, ( WORD )( 4 ) );
	//}
	//else if (grabText == buffCode6)
	//{
	//	QUICKMGR->UseQuickItem( 1, ( WORD )( 5 ) );
	//}
	//else if (grabText == buffCode7)
	//{
	//	QUICKMGR->UseQuickItem( 1, ( WORD )( 6 ) );
	//}
	//----------------------------------------------------------
	//ChatType = data->Type;
	AddMsg(
		CTC_GENERALCHAT,
		"[%s]: %s",
		object->GetObjectName(),
		textMessage);
	OBJECTMGR->ShowChatBalloon(
		OPTIONMGR->GetGameOption()->bNoBalloon ? 0 : object,
		textMessage);
}

void	CChatManager::Chat_Party( void* pMsg )
{
	MSG_CHAT_WITH_SENDERID* data = (MSG_CHAT_WITH_SENDERID*)pMsg;

	AddMsg( CTC_PARTYCHAT, "[%s]: %s", data->Name, data->Msg );
}
void	CChatManager::Chat_Family( void* pMsg )
{
	// desc_hseos_ÆÐ¹Ð¸®01
	// S ÆÐ¹Ð¸® Ãß°¡ added by hseos 2007.07.13
	// SEND_FAMILY_CHAT* data = (SEND_FAMILY_CHAT*)pMsg;
	MSG_CHAT* pstPacket = (MSG_CHAT*)pMsg;
	// E ÆÐ¹Ð¸® Ãß°¡ added by hseos 2007.07.13

	AddMsg( CTC_FAMILY, "[%s]: %s", pstPacket->Name, pstPacket->Msg );
}
void	CChatManager::Chat_Guild( void* pMsg )
{
	// 090317 ONS ¿¡ÀÌÀüÆ®¼­¹öÀÇ ÆÐÅ¶Á¶ÀÛ ¹æÁöÃ³¸®¿¡ µû¸¥ ¸Þ¼¼Áö º¯°æ
	// TESTMSGID -> MSG_GUILD_CHAT
	// pmsg->dwSenderID -> pmsg->dwObjectID
	MSG_GUILD_CHAT* pData = (MSG_GUILD_CHAT*)pMsg;
	char Sendername[MAX_NAME_LENGTH+1];
	SafeStrCpy(Sendername, GUILDMGR->GetMemberName(pData->dwSenderIdx), MAX_NAME_LENGTH+1);
	if(strcmp(Sendername, "")== 0)
		return;
	AddMsg( CTC_GUILDCHAT, "[%s]: %s", Sendername, pData->Msg );
}
void	CChatManager::Chat_Alliance( void* pMsg )
{
	// 090317 ONS ¿¡ÀÌÀüÆ®¼­¹öÀÇ ÆÐÅ¶Á¶ÀÛ ¹æÁöÃ³¸®¿¡ µû¸¥ ¸Þ¼¼Áö º¯°æ
	// MSG_CHAT_WITH_SENDERID -> MSG_GUILDUNION_CHAT
	MSG_GUILDUNION_CHAT* data = (MSG_GUILDUNION_CHAT*)pMsg;

	AddMsg( CTC_ALLIANCE, "[%s]: %s", data->Name, data->Msg );
}
void	CChatManager::Chat_Trade( void* pMsg )
{
	MSG_CHAT_WITH_SENDERID* data = (MSG_CHAT_WITH_SENDERID*)pMsg;

	AddMsg( CTC_TRADE, "[%s]: %s", data->Name, data->Msg );
}

void	CChatManager::Chat_System( void* pMsg )
{
	MSG_CHAT_WITH_SENDERID* data = (MSG_CHAT_WITH_SENDERID*)pMsg;

	AddMsg( CTC_SYSMSG, "[%s]: %s", data->Name, data->Msg );
}
void	CChatManager::Chat_Whole( void* pMsg )
{
	MSG_CHAT_WITH_SENDERID* data = (MSG_CHAT_WITH_SENDERID*)pMsg;

	AddMsg( CTC_WHOLE, "[%s]: %s", data->Name, data->Msg );
}
void	CChatManager::Chat_Whisper( void* pMsg )
{
	MSG_CHAT* data = (MSG_CHAT*)pMsg;

	AddMsg( CTC_WHISPER, "From %s: %s", data->Name, data->Msg );
#ifdef _GMTOOL_
	GMTOOLMGR->AddChatMsg( data->Name, data->Msg, 2, 1 );	//2:¹Þ¾Ò´Ù.
#endif
	//m_pChattingDlg->AddReceiverName( data->Name ) ;
	AddWhisperName(data->Name) ;
}

void	CChatManager::Chat_Whisper_Ack( void* pMsg )
{
	MSG_CHAT* data = (MSG_CHAT*)pMsg;

#ifdef _GMTOOL_
	if( MAINGAME->GetUserLevel() == eUSERLEVEL_GM )
		AddMsg( CTC_GMCHAT, "To %s: %s", data->Name, data->Msg );
	else
		AddMsg( CTC_WHISPER, "To %s: %s", data->Name, data->Msg );
#else
	AddMsg( CTC_WHISPER, "To %s: %s", data->Name, data->Msg );
#endif

#ifdef _GMTOOL_
	GMTOOLMGR->AddChatMsg( data->Name, data->Msg, 1, 1 );	//1:Çß´Ù.
#endif
	//m_pChattingDlg->AddSenderName( data->Name ) ;

	AddWhisperName(data->Name) ;

	// 071223 LYW --- ChatManager : ±Ó¸»ÀÌ ¼º°øÇÏ¸é, Ã¤ÆÃ Ã¢¿¡ ±Ó¸» ÀÔ·Â ´ë±â·Î ¼¼ÆÃÇÑ´Ù.
	cEditBox* pInputBox = NULL ;
	pInputBox = m_pChattingDlg->GetInputBox() ;

	if( pInputBox )
	{
		char tempName[32] = {0, } ;
		sprintf(tempName, "/%s ", data->Name) ;
		pInputBox->SetEditText(tempName) ;
	}
}

void CChatManager::AddWhisperName(LPCTSTR name)
{
	if(4 > strlen(name))
	{
		return;
	}

	for(size_t count = 0 ; count < _countof(whisperName) ; ++count)
	{
		if(0 == strcmp(whisperName[count], name))
		{
			return;
		}
	}

	for(size_t count = 0; count < MAX_WNAME_COUNT - 1; ++count)
	{
		SafeStrCpy(
			whisperName[count],
			whisperName[count+1],
			_countof(whisperName[count]));
	}

	SafeStrCpy(
		whisperName[MAX_WNAME_COUNT - 1],
		name,
		_countof(whisperName[MAX_WNAME_COUNT - 1]));
}

char* CChatManager::GetWhisperName(size_t index)
{
	if(index >= _countof(whisperName))
	{
		return "";
	}

	return whisperName[index];
}

// 071001 LYW --- Add function to initialize whisper buffer.
void CChatManager::InitializeWhisperName()
{
	ZeroMemory(
		whisperName,
		sizeof(whisperName));
}


void	CChatManager::Chat_Whisper_Nack( void* pMsg )
{
	const MSG_BYTE* data = (MSG_BYTE*)pMsg;

	switch( data->bData )
	{
	case CHATERR_NO_NAME:
		{
			AddMsg( CTC_WHISPER, GetChatMsg(210) );
			break;
		}
	case CHATERR_NOT_CONNECTED:
		{
			AddMsg( CTC_WHISPER, GetChatMsg(374) );//confirm
			break;
		}
	case CHATERR_OPTION_NOWHISPER:
		{
			AddMsg( CTC_WHISPER, GetChatMsg(201) );//confirm
			break;
		}
	default:
		{
			AddMsg( CTC_WHISPER, GetChatMsg(202) );//confirm
			break;
		}
	}
}


void	CChatManager::Chat_Monster_Speech( void* pMsg )
{
	MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
	char* temp = MON_SPEECHMGR->GetMonsterSpeech(pmsg->dwData2);
	if( temp )
	{
		switch(pmsg->dwData1)
		{
		case 0:	//¸»Ç³¼±
			{
				CMonster* pMonster = (CMonster*)OBJECTMGR->GetObject(pmsg->dwObjectID);

				if( pMonster )
				{
					OBJECTMGR->ShowChatBalloon( pMonster, temp);
				}
			}
			break;
		case 1:	//¸»Ç³¼± + Ã¤ÆÃÃ¢
			{
				CMonster* pMonster = (CMonster*)OBJECTMGR->GetObject(pmsg->dwObjectID);
				if( pMonster )
				{
					OBJECTMGR->ShowChatBalloon( pMonster, temp);
				}


				AddMsg( CTC_MONSTERSPEECH, "%s: %s", pMonster->GetSInfo()->Name, temp );
			}
			break;
		case 2:	//Ã¤³Î ¿ÜÄ¡±â
			{
				BASE_MONSTER_LIST* pMonInfo = GAMERESRCMNGR->GetMonsterListInfo(pmsg->dwData4);
				if(pMonInfo) 
					AddMsg( CTC_MONSTERSHOUT, "%s: %s", pMonInfo->Name, temp );

				CMonster* pMonster = (CMonster*)OBJECTMGR->GetObject(pmsg->dwData3);
				if( pMonster )
				{
					OBJECTMGR->ShowChatBalloon( pMonster, temp);
				}
			}
			break;
		}
	}
}

void CChatManager::Chat_Shout_Ack(void*pMsg)
{
	MSG_CHAT_WORD* data = (MSG_CHAT_WORD*)pMsg;
	AddMsg(CTC_SHOUT, "%s", data->Msg);
}

void CChatManager::Chat_Shout_Nack(void*pMsg)
{
	AddMsg( CTC_WHISPER, GetChatMsg(1461));
}

void CChatManager::SaveChatList()
{
	cPtrList* pPtr = m_pChattingDlg->GetChatListDlg()->GetListItem() ;
	PTRLISTPOS pos = pPtr->GetHeadPosition() ;

	while(pos)
	{
		ITEM* pItem = new ITEM ;
		*pItem = *(ITEM*)pPtr->GetNext( pos ) ;
		m_ChatList.AddTail( pItem ) ;
	}

	cPtrList* pPtr2 = m_pChattingDlg->GetAssistListDlg()->GetListItem() ;
	PTRLISTPOS pos2 = pPtr2->GetHeadPosition();

	while( pos2 )
	{
		ITEM* pItem = new ITEM;
		*pItem = *(ITEM*)pPtr2->GetNext( pos2 );
		m_AssistList.AddTail( pItem );
	}

	cPtrList* pPtr3 = m_pChattingDlg->GetShoutListDlg()->GetListItem() ;
	PTRLISTPOS pos3 = pPtr3->GetHeadPosition();

	while( pos3 )
	{
		ITEM* pItem = new ITEM;
		*pItem = *(ITEM*)pPtr3->GetNext( pos3 );
		m_ShoutList.AddTail( pItem );
	}
}

void CChatManager::RestoreChatList()
{
	PTRLISTPOS pos = m_ChatList.GetHeadPosition() ;

	ITEM* pItem = NULL ;

	while( pos )
	{
		pItem = (ITEM*)m_ChatList.GetNext( pos ) ;

		if( pItem )
		{
			m_pChattingDlg->GetChatListDlg()->AddItem( pItem->string, pItem->rgb, pItem->line ) ;
		}
	}

	PTRLISTPOS pos2 = m_AssistList.GetHeadPosition() ;

	ITEM* pItem2 = NULL ;
	while( pos2 )
	{
		pItem2 = (ITEM*)m_AssistList.GetNext( pos2 ) ;

		if( pItem2 )
		{
			m_pChattingDlg->GetAssistListDlg()->AddItem( pItem2->string, pItem2->rgb, pItem2->line ) ;
		}
	}

	PTRLISTPOS pos3 = m_ShoutList.GetHeadPosition() ;

	ITEM* pItem3 = NULL ;
	while( pos3 )
	{
		pItem3 = (ITEM*)m_ShoutList.GetNext( pos3 ) ;

		if( pItem3 )
		{
			m_pChattingDlg->GetShoutListDlg()->AddItem( pItem3->string, pItem3->rgb, pItem3->line ) ;
		}
	}

	ReleaseChatList() ;
}

void CChatManager::ReleaseChatList()
{
	PTRLISTPOS pos = m_ChatList.GetHeadPosition();
	ITEM* pItem = 0;

	while( pos )
	{
		pItem = (ITEM*)m_ChatList.GetNext( pos ) ;

		if( pItem ) delete pItem ;
	}

	m_ChatList.RemoveAll() ;

	PTRLISTPOS pos2 = m_AssistList.GetHeadPosition() ;

	ITEM* pItem2 = NULL ;

	while( pos2 )
	{
		pItem2 = (ITEM*)m_AssistList.GetNext( pos2 ) ;

		if( pItem2 ) delete pItem2 ;
	}

	m_AssistList.RemoveAll() ;

	PTRLISTPOS pos3 = m_ShoutList.GetHeadPosition() ;

	ITEM* pItem3 = NULL ;

	while( pos3 )
	{
		pItem3 = (ITEM*)m_ShoutList.GetNext( pos3 ) ;

		if( pItem3 ) delete pItem3 ;
	}

	m_ShoutList.RemoveAll() ;
}

CChatManager* CChatManager::GetInstance()
{
	static CChatManager instance;

	return &instance;
}

void CChatManager::ShowForbidChatTime( __time64_t time )
{
	struct tm curTimeWhen = *_localtime64( &time );
	CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(2312), curTimeWhen.tm_year + 1900, curTimeWhen.tm_mon + 1, curTimeWhen.tm_mday, curTimeWhen.tm_hour, curTimeWhen.tm_min, curTimeWhen.tm_sec );
}

__time64_t CChatManager::GetForbidChatTime()
{
	return m_ForbidChatTime;
}


void	CChatManager::Chat_Normal_ItemChat( void* pMsg )
{
	ITEM_CHAT_LINK* const data = (ITEM_CHAT_LINK*)pMsg;
	CObject* object = OBJECTMGR->GetObject(data->dwObjectID);

	if(0 == object)
	{
		return;
	}

	//----------------------------------------------------------------------------------------------------------------------------
	std::string tempStrText(data->toolTipTextArray);
	std::string tempStrColors(data->toolTipColorsArray);
	std::vector<std::string> containerText;
	std::vector<std::string> containerColors;
	containerText.clear();
	containerColors.clear();
	const std::string& delims = "ƒ";

	//Array Text to ContainerText
    std::size_t current, previous = 0;
    current = tempStrText.find_first_of(delims);
    while (current != std::string::npos) {
        containerText.push_back(tempStrText.substr(previous, current - previous));
        previous = current + 1;
        current = tempStrText.find_first_of(delims, previous);
    }
    containerText.push_back(tempStrText.substr(previous, current - previous));
 
 	//Array Colors to ContainerColors
 	  current = 0;
 	  previous = 0;
 	  current = tempStrColors.find_first_of(delims);
    while (current != std::string::npos) {
        containerColors.push_back(tempStrColors.substr(previous, current - previous));
        previous = current + 1;
        current = tempStrColors.find_first_of(delims, previous);
    }
    containerColors.push_back(tempStrColors.substr(previous, current - previous));

	//Alemuri------------------------------------------------------------------------------------------------------------------
	DWORD txtColor = RGBA_MAKE(75,244,231,255);//(255, 255, 255, 255) ; //SM Changed

	//Convert the Colors Array from text format to DWORD
	std::vector<DWORD> containerColorsNumType;
	containerColorsNumType.clear();
	
	for (unsigned int i = 0; i < containerColors.size(); ++i)
	{
		containerColorsNumType.push_back(strtol(containerColors[i].c_str(), 0, 0));
	}
	//----------------------------------------------------------------------------------------------------------------------------

	LPCTSTR keyword = _T("SystemMsg.bin\'");
	// 091019 LUJ, ¹®ÀÚ¿­ÀÌ ÁöÁ¤µÈ ¿¹¾à¾î·Î ½ÃÀÛµÇ¸é ½Ã½ºÅÛ ¸Þ½ÃÁö·Î °£ÁÖÇÑ´Ù
	const BOOL isSystemMessage = (0 == _tcsnicmp(data->Msg, keyword, _tcslen(keyword)));
	LPTSTR textMessage = textMessage = data->Msg;

	if(isSystemMessage)
	{
		LPCTSTR token = data->Msg + _tcslen(keyword);
		const DWORD messageIndex = _ttoi(token ? token : "");
		static TCHAR text[MAX_PATH];
		ZeroMemory(text, sizeof(text));
		_stprintf(
			text,
			"%s",
			GetChatMsg(messageIndex));
		textMessage = text;
	}
	//ChatType = data->Type;
	AddMsgItemChat(containerText, containerColorsNumType, data->itemIdx,
		CTC_GENERALCHAT,
		"[%s]: %s",
		object->GetObjectName(),
		textMessage);
	OBJECTMGR->ShowChatBalloon(
		OPTIONMGR->GetGameOption()->bNoBalloon ? 0 : object,
		textMessage);
}

void	CChatManager::Chat_Party_ItamChat( void* pMsg )
{
	ITEM_CHAT_LINK* const data = (ITEM_CHAT_LINK*)pMsg;
	CObject* object = OBJECTMGR->GetObject(data->dwObjectID);

	if(0 == object)
	{
		return;
	}

	//----------------------------------------------------------------------------------------------------------------------------
	std::string tempStrText(data->toolTipTextArray);
	std::string tempStrColors(data->toolTipColorsArray);
	std::vector<std::string> containerText;
	std::vector<std::string> containerColors;
	const std::string& delims = "ƒ";

	//Array Text to ContainerText
    std::size_t current, previous = 0;
    current = tempStrText.find_first_of(delims);
    while (current != std::string::npos) {
        containerText.push_back(tempStrText.substr(previous, current - previous));
        previous = current + 1;
        current = tempStrText.find_first_of(delims, previous);
    }
    containerText.push_back(tempStrText.substr(previous, current - previous));
 
 	//Array Colors to ContainerColors
 	  current = 0;
 	  previous = 0;
 	  current = tempStrColors.find_first_of(delims);
    while (current != std::string::npos) {
        containerColors.push_back(tempStrColors.substr(previous, current - previous));
        previous = current + 1;
        current = tempStrColors.find_first_of(delims, previous);
    }
    containerColors.push_back(tempStrColors.substr(previous, current - previous));

	//Alemuri------------------------------------------------------------------------------------------------------------------
	DWORD txtColor = RGBA_MAKE(75,244,231,255);//(255, 255, 255, 255) ; //SM Changed

	//Convert the Colors Array from text format to DWORD
	std::vector<DWORD> containerColorsNumType;
	
	for (unsigned int i = 0; i < containerColors.size(); ++i)
	{
		containerColorsNumType.push_back(strtol(containerColors[i].c_str(), 0, 0));
	}

	AddMsgItemChat(containerText, containerColorsNumType, data->itemIdx,
		CTC_PARTYCHAT, "[%s]: %s", data->Name, data->Msg );

}

void	CChatManager::Chat_Family_ItemChat( void* pMsg )
{

	ITEM_CHAT_LINK* dataItem = (ITEM_CHAT_LINK*)pMsg;

	CObject* object = OBJECTMGR->GetObject(dataItem->dwObjectID);

	if(0 == object)
	{
		return;
	}
	
	std::string tempStrText(dataItem->toolTipTextArray);
	std::string tempStrColors(dataItem->toolTipColorsArray);
	std::vector<std::string> containerText;
	std::vector<std::string> containerColors;
	const std::string& delims = "ƒ";

	//Array Text to ContainerText
    std::size_t current, previous = 0;
    current = tempStrText.find_first_of(delims);
    while (current != std::string::npos) {
        containerText.push_back(tempStrText.substr(previous, current - previous));
        previous = current + 1;
        current = tempStrText.find_first_of(delims, previous);
    }
    containerText.push_back(tempStrText.substr(previous, current - previous));
 
 	//Array Colors to ContainerColors
 	  current = 0;
 	  previous = 0;
 	  current = tempStrColors.find_first_of(delims);
    while (current != std::string::npos) {
        containerColors.push_back(tempStrColors.substr(previous, current - previous));
        previous = current + 1;
        current = tempStrColors.find_first_of(delims, previous);
    }
    containerColors.push_back(tempStrColors.substr(previous, current - previous));

	//Alemuri------------------------------------------------------------------------------------------------------------------
	DWORD txtColor = RGBA_MAKE(75,244,231,255);//(255, 255, 255, 255) ; //SM Changed

	//Convert the Colors Array from text format to DWORD
	std::vector<DWORD> containerColorsNumType;
	
	for (unsigned int i = 0; i < containerColors.size(); ++i)
	{
		containerColorsNumType.push_back(strtol(containerColors[i].c_str(), 0, 0));
	}

	AddMsgItemChat(containerText, containerColorsNumType, dataItem->itemIdx, CTC_FAMILY, "[%s]: %s", dataItem->Name, dataItem->Msg );
}

void	CChatManager::Chat_Alliance_ItemChat( void* pMsg )
{
	// 090317 ONS ¿¡ÀÌÀüÆ®¼­¹öÀÇ ÆÐÅ¶Á¶ÀÛ ¹æÁöÃ³¸®¿¡ µû¸¥ ¸Þ¼¼Áö º¯°æ
	// MSG_CHAT_WITH_SENDERID -> MSG_GUILDUNION_CHAT
	ITEM_CHAT_LINK* data = (ITEM_CHAT_LINK*)pMsg;

	std::string tempStrText(data->toolTipTextArray);
	std::string tempStrColors(data->toolTipColorsArray);
	std::vector<std::string> containerText;
	std::vector<std::string> containerColors;
	const std::string& delims = "ƒ";

	//Array Text to ContainerText
    std::size_t current, previous = 0;
    current = tempStrText.find_first_of(delims);
    while (current != std::string::npos) {
        containerText.push_back(tempStrText.substr(previous, current - previous));
        previous = current + 1;
        current = tempStrText.find_first_of(delims, previous);
    }
    containerText.push_back(tempStrText.substr(previous, current - previous));
 
 	//Array Colors to ContainerColors
 	  current = 0;
 	  previous = 0;
 	  current = tempStrColors.find_first_of(delims);
    while (current != std::string::npos) {
        containerColors.push_back(tempStrColors.substr(previous, current - previous));
        previous = current + 1;
        current = tempStrColors.find_first_of(delims, previous);
    }
    containerColors.push_back(tempStrColors.substr(previous, current - previous));

	//Alemuri------------------------------------------------------------------------------------------------------------------
	DWORD txtColor = RGBA_MAKE(75,244,231,255);//(255, 255, 255, 255) ; //SM Changed

	//Convert the Colors Array from text format to DWORD
	std::vector<DWORD> containerColorsNumType;
	
	for (unsigned int i = 0; i < containerColors.size(); ++i)
	{
		containerColorsNumType.push_back(strtol(containerColors[i].c_str(), 0, 0));
	}

	AddMsgItemChat(containerText, containerColorsNumType, data->itemIdx, CTC_ALLIANCE, "[%s]: %s", data->Name, data->Msg );
}

void	CChatManager::Chat_Guild_ItemChat( void* pMsg )
{
	// 090317 ONS ¿¡ÀÌÀüÆ®¼­¹öÀÇ ÆÐÅ¶Á¶ÀÛ ¹æÁöÃ³¸®¿¡ µû¸¥ ¸Þ¼¼Áö º¯°æ
	// TESTMSGID -> MSG_GUILD_CHAT
	// pmsg->dwSenderID -> pmsg->dwObjectID
	ITEM_CHAT_LINK* pData = (ITEM_CHAT_LINK*)pMsg;
	char Sendername[MAX_NAME_LENGTH+1];
	SafeStrCpy(Sendername, GUILDMGR->GetMemberName(pData->dwSenderIdx), MAX_NAME_LENGTH+1);
	if(strcmp(Sendername, "")== 0)
		return;

	std::string tempStrText(pData->toolTipTextArray);
	std::string tempStrColors(pData->toolTipColorsArray);
	std::vector<std::string> containerText;
	std::vector<std::string> containerColors;
	const std::string& delims = "ƒ";

	//Array Text to ContainerText
    std::size_t current, previous = 0;
    current = tempStrText.find_first_of(delims);
    while (current != std::string::npos) {
        containerText.push_back(tempStrText.substr(previous, current - previous));
        previous = current + 1;
        current = tempStrText.find_first_of(delims, previous);
    }
    containerText.push_back(tempStrText.substr(previous, current - previous));
 
 	//Array Colors to ContainerColors
 	  current = 0;
 	  previous = 0;
 	  current = tempStrColors.find_first_of(delims);
    while (current != std::string::npos) {
        containerColors.push_back(tempStrColors.substr(previous, current - previous));
        previous = current + 1;
        current = tempStrColors.find_first_of(delims, previous);
    }
    containerColors.push_back(tempStrColors.substr(previous, current - previous));

	//SM Changed------------------------------------------------------------------------------------------------------------------
	DWORD txtColor = RGBA_MAKE(75,244,231,255);//(255, 255, 255, 255) ; //SM Changed

	//Convert the Colors Array from text format to DWORD
	std::vector<DWORD> containerColorsNumType;
	
	for (unsigned int i = 0; i < containerColors.size(); ++i)
	{
		containerColorsNumType.push_back(strtol(containerColors[i].c_str(), 0, 0));
	}

	AddMsgItemChat(containerText, containerColorsNumType, pData->itemIdx, CTC_GUILDCHAT, "[%s]: %s", Sendername, pData->Msg );
}

void	CChatManager::Chat_Trade_ItemChat( void* pMsg )
{
	//MSG_CHAT_WITH_SENDERID* data = (MSG_CHAT_WITH_SENDERID*)pMsg;
	ITEM_CHAT_LINK* data = (ITEM_CHAT_LINK*)pMsg;

	std::string tempStrText(data->toolTipTextArray);
	std::string tempStrColors(data->toolTipColorsArray);
	std::vector<std::string> containerText;
	std::vector<std::string> containerColors;
	const std::string& delims = "ƒ";

	//Array Text to ContainerText
    std::size_t current, previous = 0;
    current = tempStrText.find_first_of(delims);
    while (current != std::string::npos) {
        containerText.push_back(tempStrText.substr(previous, current - previous));
        previous = current + 1;
        current = tempStrText.find_first_of(delims, previous);
    }
    containerText.push_back(tempStrText.substr(previous, current - previous));
 
 	//Array Colors to ContainerColors
 	  current = 0;
 	  previous = 0;
 	  current = tempStrColors.find_first_of(delims);
    while (current != std::string::npos) {
        containerColors.push_back(tempStrColors.substr(previous, current - previous));
        previous = current + 1;
        current = tempStrColors.find_first_of(delims, previous);
    }
    containerColors.push_back(tempStrColors.substr(previous, current - previous));

	//Alemuri------------------------------------------------------------------------------------------------------------------
	DWORD txtColor = RGBA_MAKE(75,244,231,255);//(255, 255, 255, 255) ; //SM Changed

	//Convert the Colors Array from text format to DWORD
	std::vector<DWORD> containerColorsNumType;
	
	for (unsigned int i = 0; i < containerColors.size(); ++i)
	{
		containerColorsNumType.push_back(strtol(containerColors[i].c_str(), 0, 0));
	}

	AddMsgItemChat(containerText, containerColorsNumType, data->itemIdx, CTC_TRADE, "[%s]: %s", data->Name, data->Msg );
}

void CChatManager::Chat_Shout_Item_Ack(void*pMsg)
{
	//MSG_CHAT_WORD* data = (MSG_CHAT_WORD*)pMsg;
	SEND_SHOUTBASE_ITEMINFO* data = (SEND_SHOUTBASE_ITEMINFO*)pMsg;

	std::string tempStrText(data->toolTipTextArray);
	std::string tempStrColors(data->toolTipColorsArray);
	std::vector<std::string> containerText;
	std::vector<std::string> containerColors;
	const std::string& delims = "ƒ";

	//Array Text to ContainerText
    std::size_t current, previous = 0;
    current = tempStrText.find_first_of(delims);
    while (current != std::string::npos) {
        containerText.push_back(tempStrText.substr(previous, current - previous));
        previous = current + 1;
        current = tempStrText.find_first_of(delims, previous);
    }
    containerText.push_back(tempStrText.substr(previous, current - previous));
 
 	//Array Colors to ContainerColors
 	  current = 0;
 	  previous = 0;
 	  current = tempStrColors.find_first_of(delims);
    while (current != std::string::npos) {
        containerColors.push_back(tempStrColors.substr(previous, current - previous));
        previous = current + 1;
        current = tempStrColors.find_first_of(delims, previous);
    }
    containerColors.push_back(tempStrColors.substr(previous, current - previous));

	//Alemuri------------------------------------------------------------------------------------------------------------------
	DWORD txtColor = RGBA_MAKE(255,250,129,255);//(255, 255, 255, 255) ; //SM Changed

	//Convert the Colors Array from text format to DWORD
	std::vector<DWORD> containerColorsNumType;
	
	for (unsigned int i = 0; i < containerColors.size(); ++i)
	{
		containerColorsNumType.push_back(strtol(containerColors[i].c_str(), 0, 0));
	}

	AddMsgItemChat(containerText, containerColorsNumType, data->linkedItemIdx, CTC_SHOUT, "%s", data->Msg);
}


void	CChatManager::Chat_Whisper_Item_Ack( void* pMsg )
{
	//TESTMSGITEMCHAT* data = (TESTMSGITEMCHAT*)pMsg;
	MSG_CHAT* data = (MSG_CHAT*)pMsg;

	//std::string tempStrText(data->toolTipTextArray);
	//std::string tempStrColors(data->toolTipColorsArray);
	std::vector<std::string> containerText;
	std::vector<std::string> containerColors;
	const std::string& delims = "ƒ";

	//Array Text to ContainerText
    std::size_t current, previous = 0;
    //current = tempStrText.find_first_of(delims);
    while (current != std::string::npos) {
    //    containerText.push_back(tempStrText.substr(previous, current - previous));
        previous = current + 1;
   //     current = tempStrText.find_first_of(delims, previous);
    }
   // containerText.push_back(tempStrText.substr(previous, current - previous));
 
 	//Array Colors to ContainerColors
 	  current = 0;
 	  previous = 0;
 	 // current = tempStrColors.find_first_of(delims);
    while (current != std::string::npos) {
   //     containerColors.push_back(tempStrColors.substr(previous, current - previous));
        previous = current + 1;
   //     current = tempStrColors.find_first_of(delims, previous);
    }
  //  containerColors.push_back(tempStrColors.substr(previous, current - previous));

	//Alemuri------------------------------------------------------------------------------------------------------------------
	DWORD txtColor = RGBA_MAKE(75,244,231,255);//(255, 255, 255, 255) ; //SM Changed

	//Convert the Colors Array from text format to DWORD
	std::vector<DWORD> containerColorsNumType;
	
	for (unsigned int i = 0; i < containerColors.size(); ++i)
	{
		containerColorsNumType.push_back(strtol(containerColors[i].c_str(), 0, 0));
	}
	
#ifdef _GMTOOL_
//	if( MAINGAME->GetUserLevel() == eUSERLEVEL_GM )
	//	AddMsgItemChat(containerText, containerColorsNumType, data->linkedItemIdx, CTC_GMCHAT, "To %s: %s", data->Name, data->Msg );
//	else
	//	AddMsgItemChat(containerText, containerColorsNumType,data->linkedItemIdx, CTC_WHISPER, "To %s: %s", data->Name, data->Msg );
#else
	//AddMsgItemChat(containerText, containerColorsNumType, data->linkedItemIdx, CTC_WHISPER, "To %s: %s", data->Name, data->Msg );
#endif

#ifdef _GMTOOL_
	GMTOOLMGR->AddChatMsg( data->Name, data->Msg, 1, 1 );	//1:Çß´Ù.
#endif
	//m_pChattingDlg->AddSenderName( data->Name ) ;

	AddWhisperName(data->Name) ;

	// 071223 LYW --- ChatManager : ±Ó¸»ÀÌ ¼º°øÇÏ¸é, Ã¤ÆÃ Ã¢¿¡ ±Ó¸» ÀÔ·Â ´ë±â·Î ¼¼ÆÃÇÑ´Ù.
	cEditBox* pInputBox = NULL ;
	pInputBox = m_pChattingDlg->GetInputBox() ;

	if( pInputBox )
	{
		char tempName[32] = {0, } ;
		sprintf(tempName, "/%s ", data->Name) ;
		pInputBox->SetEditText(tempName) ;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------

void	CChatManager::Chat_WhisperItem( void* pMsg )
{
	MSG_CHAT* data = (MSG_CHAT*)pMsg;

//	std::string tempStrText(data->toolTipTextArray);
//	std::string tempStrColors(data->toolTipColorsArray);
	std::vector<std::string> containerText;
	std::vector<std::string> containerColors;
	const std::string& delims = "ƒ";

	//Array Text to ContainerText
    std::size_t current, previous = 0;
//    current = tempStrText.find_first_of(delims);
    while (current != std::string::npos) {
//        containerText.push_back(tempStrText.substr(previous, current - previous));
        previous = current + 1;
//        current = tempStrText.find_first_of(delims, previous);
    }
//    containerText.push_back(tempStrText.substr(previous, current - previous));
 
 	//Array Colors to ContainerColors
 	  current = 0;
 	  previous = 0;
// 	  current = tempStrColors.find_first_of(delims);
    while (current != std::string::npos) {
//        containerColors.push_back(tempStrColors.substr(previous, current - previous));
        previous = current + 1;
//        current = tempStrColors.find_first_of(delims, previous);
    }
//    containerColors.push_back(tempStrColors.substr(previous, current - previous));

	//SM Changed------------------------------------------------------------------------------------------------------------------
	DWORD txtColor = RGBA_MAKE(75,244,231,255);//(255, 255, 255, 255) ; //SM Changed

	//Convert the Colors Array from text format to DWORD
	std::vector<DWORD> containerColorsNumType;
	
	for (unsigned int i = 0; i < containerColors.size(); ++i)
	{
		containerColorsNumType.push_back(strtol(containerColors[i].c_str(), 0, 0));
	}

//	AddMsgItemChat(containerText, containerColorsNumType, data->linkedItemIdx, CTC_WHISPER, "From %s: %s", data->Name, data->Msg );

#ifdef _GMTOOL_
	GMTOOLMGR->AddChatMsg( data->Name, data->Msg, 2, 1 );	//2:¹Þ¾Ò´Ù.
#endif
	//m_pChattingDlg->AddReceiverName( data->Name ) ;
	AddWhisperName(data->Name) ;
}
//--------------------------------------------------------------------------------------------------------------------------------------------

void CChatManager::AddMsgItemChat(std::vector<std::string> toolTipTextArray, std::vector<DWORD> toolTipColorArray, DWORD itemIdx, eTextClass nClass, LPCTSTR str, ...)
{
	if(0 == m_pChattingDlg)
	{
		return;
	}

	if (toolTipTextArray.size() == 0 || toolTipColorArray.size() == 0)
		return;

	char msg[512] = {0};
	va_list argList;
	va_start(
		argList,
		str);
	vsprintf(
		msg,
		str,
		argList);
	va_end(
		argList);

	switch(nClass)
	{
	case CTC_SHOUT:
		{
			m_pChattingDlg->AddShoutMsgWithItem(toolTipTextArray, toolTipColorArray,
				msg,
				GetMsgColor(nClass),
				itemIdx);
			break;
		}
	case CTC_OPERATOR:
		{
			m_pChattingDlg->AddMsgItemChat(CTC_OPERATOR, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			NOTIFYMGR->AddMsg(msg, eNTC_REMAINTIME);
			break;
		}
	case CTC_OPERATOR2:
		{
			NOTIFYMGR->AddMsg(msg, eNTC_RESIDENTREGIST);
			break;
		}
	case CTC_BILLING:
		{
			m_pChattingDlg->AddMsgItemChat(CTC_BILLING, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			NOTIFYMGR->AddMsg(msg, eNTC_REMAINTIME);
			break;
		}
	case CTC_GMCHAT:
		{
			m_pChattingDlg->AddMsgItemChat(CTC_GMCHAT, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			break;
		}
	case CTC_GENERALCHAT:
		{
			if(!m_ChatOption.bOption[CTO_NOCHATMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_GENERALCHAT, e_CONVERSATION_ALL, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			}

			break;
		}
	case CTC_PARTYCHAT:
		{
			if(!m_ChatOption.bOption[CTO_NOPARTYMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_PARTYCHAT, e_CONVERSATION_PARTY, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			}

			break;
		}
	case CTC_FAMILY:
		{
			if(!m_ChatOption.bOption[CTO_NOFAMILYMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_FAMILY, e_CONVERSATION_FAMILY, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			}

			break;
		}
	case CTC_GUILDCHAT:
		{
			if(!m_ChatOption.bOption[CTO_NOGUILDMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_GUILDCHAT, e_CONVERSATION_GUILD, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			}

			break;
		}
	case CTC_ALLIANCE:
		{
			if(!m_ChatOption.bOption[CTO_NOALLIANCEMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_ALLIANCE, e_CONVERSATION_ALLIANCE, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			}

			break;
		}
	case CTC_TRADE:
		{
			if(!m_ChatOption.bOption[CTO_NOSHOUTMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_TRADE, e_CONVERSATION_TRADE, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			}

			break;
		}
	case CTC_SYSMSG:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_SYSMSG, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			}

			break;
		}
	case CTC_WHISPER:
		{
			m_pChattingDlg->AddMsgItemChat(CTC_WHISPER, e_CONVERSATION_ALL, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			break;
		}
	case CTC_ATTACK:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_ATTACK, e_CONVERSATION_ALL, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			}

			break;
		}
	case CTC_ATTACKED:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_ATTACKED, e_CONVERSATION_ALL, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			}

			break;
		}
	case CTC_DEFENCE:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_DEFENCE, e_CONVERSATION_ALL, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			}

			break;
		}
	case CTC_DEFENCED:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_DEFENCED, e_CONVERSATION_ALL, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);	
			}

			break;
		}
	case CTC_KILLED:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_KILLED, e_CONVERSATION_ALL, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			}

			break;
		}
	case CTC_MPWARNING:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_MPWARNING, e_CONVERSATION_ALL, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			}

			break;
		}
	case CTC_HPWARNING:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_HPWARNING, e_CONVERSATION_ALL, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			}

			break;
		}
	case CTC_GETMONEY:
	case CTC_GETITEM:
		{
			if(!m_ChatOption.bOption[CTO_NOITEMMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_GETITEM, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			}


			break;
		}
	case CTC_GETEXP:
		{
			if(!m_ChatOption.bOption[CTO_NOEXPMSG])
			{
				m_pChattingDlg->AddMsgItemChat(CTC_GETEXP,e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			}

			break;
		}
	case CTC_TOWHOLE:
		{
			m_pChattingDlg->AddMsgItemChat(CTC_TOWHOLE, e_CONVERSATION_ALL, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			break;
		}
	case CTC_TOPARTY:
		{
			m_pChattingDlg->AddMsgItemChat(CTC_TOPARTY, e_CONVERSATION_ALL, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			break;
		}
	case CTC_TOFAMILY:
		{
			m_pChattingDlg->AddMsgItemChat(CTC_TOFAMILY, e_CONVERSATION_ALL, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			break;
		}
	case CTC_TOGUILD:
		{
			m_pChattingDlg->AddMsgItemChat(CTC_TOGUILD, e_CONVERSATION_ALL, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			break;
		}
	case CTC_MONSTERSPEECH:
		{
			m_pChattingDlg->AddMsgItemChat(CTC_MONSTERSPEECH, e_CONVERSATION_ALL, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			break;
		}
	case CTC_MONSTERSHOUT:
		{
			m_pChattingDlg->AddMsgItemChat(CTC_MONSTERSHOUT, e_CONVERSATION_ALL, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			break;
		}
	case CTC_EXITCOUNT:
		{
			m_pChattingDlg->AddMsgItemChat(CTC_EXITCOUNT, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			break;
		}
	case CTC_ENCHANT:
		{
			
			NOTIFYMGR->AddMsg(msg, eNTC_ENCHANT);
			/*m_pChattingDlg->AddShoutMsg(
				msg,
				GetMsgColor(nClass)); */
			break;
		}
	case CTC_EVENT:
		{
			m_pChattingDlg->AddMsgItemChat(CTC_EVENT, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			NOTIFYMGR->AddMsg(msg, eNTC_EVENT);
			break;
		}
	default:
		{
			m_pChattingDlg->AddMsgItemChat(CTC_SYSMSG, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass), toolTipTextArray, toolTipColorArray, itemIdx);
			break;
		}
	}
	
}
//-------------------------------------------------------------------------------------------------------------------------------------------------

//Alemuri Search Drop From Monsters (Function to split a string in a vector)
std::vector<std::string> CChatManager::splitStringOnVector(char *phrase, string delimiter, string delimiterBetweenWords)
{
    std::vector<std::string> list;
    string s = string(phrase);
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos)
	{
        token = s.substr(0, pos);
        list.push_back(token);
		list.push_back(delimiterBetweenWords);
        s.erase(0, pos + delimiter.length());
    }
	list.push_back(s);
    return list;
}
//--------------------------------------------------------------------------