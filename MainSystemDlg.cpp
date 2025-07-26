//=================================================================================================
//	FILE		: CMainSystemDlg.cpp
//	PURPOSE		: Implementation part for main system dialog.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 19, 2006
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================


//=================================================================================================
//	INCLUDE PART
//=================================================================================================
#include "stdafx.h"
#include ".\mainsystemdlg.h"
#include "WindowIDEnum.h"

#include "./interface/cTabDialog.h"

#include "GameIn.h"

#include "CharacterDialog.h"
#include "QuestDialog.h"
#include "ChatManager.h"
#include "ExitManager.h"
#include "FriendManager.h"
#include "ObjectManager.h"
#include "NoteManager.h"
#include "cWindowManager.h"
#include "StorageDialog.h"

// 070402 LYW --- Include cButton.
#include "./interface/cButton.h"
#include "cMsgBox.h"

// 070115 LYW --- Include header file tp need.
#include "../Input/Mouse.h"

// 070122 LYW --- Include header file.
#include "cResourceManager.h"

#include "../DateMatchingDlg.h"
#include "../FamilyDialog.h"

// 071025 LYW --- MainSystemDlg : Include helperdlg.
#include "HelperDlg.h"
#include "..\hseos\Family\SHFamily.h"
#include "ChatRoomMgr.h"
#include "cHousingWebDlg.h"
#include "MainTitle.h"
#include "DealDialog.h"
#include "cMapMoveDialog.h"
#include "MoveManager.h"
#include "ConsignmentMgr.h"	
#include "ConsignmentDlg.h"

#include "VideoCaptureManager.h"
#include "MiniMapDlg.h"
#include "AdditionalButtonDlg.h"
#include "ItemShopDialog.h"
#include "InventoryExDialog.h"
#include "CharSelect.h"
#include "ChannelDialog.h"
#include "MHMap.h"
#include "SlotDialog.h"
#include "ReportDialog.h"

#ifdef _TW_LOCAL_
#include "ItemShopDlg.h"
#include <Wincrypt.h>
extern int g_nServerSetNum;
extern HWND _g_hWnd;
#endif
extern char g_szHeroIDName[];
//=================================================================================================
// NAME			: CMainSystemDlg()
// PURPOSE		: The Function Constructor.
// ATTENTION	:
//=================================================================================================
CMainSystemDlg::CMainSystemDlg(void)
{
	// 070115 LYW --- Add variable for count.
	int count = 0 ;

	// 070115 LYW --- Add static controls for background image of sub dialog.
	m_pSystemSet			= NULL ;
	m_pGameSystem			= NULL ;
	m_pSystemSet_Bottom		= NULL ;


	// 070115 LYW --- Add controls for systemset part.
	for( count = 0 ; count < MAX_SYSTEMSET_BTN ; ++count )
	{
		m_pMenuSystemSet[ count ] = NULL ;
	}
	m_pTitleSystemSet		= NULL ;
	m_pMarkSystemSet		= NULL ;

	// 070115 LYW --- Add controls for gamesystem part.
	for( count = 0 ; count < MAX_GAMESYSTEM_BTN ; ++count )
	{
		m_pMenuGameSystem[ count ] = NULL ;
	}
	m_pTitleGameSystem		= NULL ;
	m_pMarkGameSystem		= NULL ;

	// 070115 LYW --- Add variables whether show sub dialog or not.
	m_bShowSystemSet	= FALSE ;
	m_bShowGameSystem	= FALSE ;

	// 090420 ONS ĳ�������� �޴��� "�������̵�" �߰�
	m_pCharInfo		= NULL ;
	for( count = 0 ; count < MAX_CHARINFO_BTN ; ++count )
	{
		m_pMenuCharInfo[ count ] = NULL;
	}
	m_pTitleCharInfo		= NULL;
	m_pMarkCharInfo			= NULL;
	m_bShowCharInfo			= FALSE;

	// 100106 ONS ������/�κ��丮 �޴� �߰�
	for( count = 0 ; count < MAX_WEBINFO_BTN ; ++count )
	{
		m_pMenuWebInfo[ count ] = NULL;
	}
	m_pTitleWebInfo		= NULL;
	m_pMarkWebInfo		= NULL;
	m_pBGWebInfoTop		= NULL;
	m_pBGWebInfoBottom	= NULL;
	m_bShowWebInfo		= FALSE;

	for( count = 0 ; count < MAX_INVENTORY_BTN ; ++count )
	{
		m_pMenuInventory[ count ] = NULL;
	}
	m_pTitleInventory		= NULL;
	m_pMarkInventory		= NULL;
	m_pBGInventoryTop		= NULL;
	m_pBGInventoryBottom	= NULL;
	m_bShowInventory		= FALSE;

	m_eMainMenuKind			= eMainMenu_None;
}


//=================================================================================================
// NAME			: ~CMainSystemDlg()
// PURPOSE		: The Function Destructor.
// ATTENTION	:
//=================================================================================================
CMainSystemDlg::~CMainSystemDlg(void)
{
}


//=================================================================================================
//	NAME		: Linking()
//	PURPOSE		: Add function to linking controls to window manager.
//	DATE		: January 15, 2007 LYW
//	ATTENTION	:
//=================================================================================================
void CMainSystemDlg::Linking()
{
	int count = 0 ;

	m_pSystemSet		= ( cStatic* )GetWindowForID( MSD_BG_SYSTEMSET ) ;
	m_pSystemSet_Bottom	= ( cStatic* )GetWindowForID( MSD_BG_SYSTEMSET_BOTTOM ) ;
	m_pGameSystem		= ( cStatic* )GetWindowForID( MSD_BG_GAMESYSTEM ) ;

	for( count = 0 ; count < MAX_SYSTEMSET_BTN ; ++count )
	{
		m_pMenuSystemSet[ count ] = ( cButton* )GetWindowForID( MSD_SSBTN_GAME + count ) ;
		m_pMenuSystemSet[ count ]->SetActive( FALSE ) ;
	}
	m_pTitleSystemSet		= ( cStatic* )GetWindowForID( MSD_TITLE_SYSTEMSET ) ;
	m_pMarkSystemSet		= ( cStatic* )GetWindowForID( MSD_MARK_SYSTEMSET ) ;

	for( count = 0 ; count < MAX_GAMESYSTEM_BTN ; ++count )
	{
		m_pMenuGameSystem[ count ] = ( cButton* )GetWindowForID( MSD_GSBTN_CHARINFO + count ) ;
		m_pMenuGameSystem[ count ]->SetActive( FALSE ) ;
	}
	m_pTitleGameSystem		= ( cStatic* )GetWindowForID( MSD_TITLE_GAMESYSTEM ) ;
	m_pMarkGameSystem		= ( cStatic* )GetWindowForID( MSD_MARK_GAMESYSTEM ) ;

	// 090420 ONS ĳ�������� �޴��� "�������̵�" �߰�
	m_pCharInfo			= ( cStatic* )GetWindowForID( MSD_BG_CHARINFO ) ;
	for( count = 0 ; count < MAX_CHARINFO_BTN ; ++count )
	{
		m_pMenuCharInfo[ count ] = ( cButton* )GetWindowForID( MSD_CIBTN_CHARINFO + count ) ;
		m_pMenuCharInfo[ count ]->SetActive( FALSE ) ;
	}
	m_pTitleCharInfo		= ( cStatic* )GetWindowForID( MSD_TITLE_CHARINFO ) ;
	m_pMarkCharInfo			= ( cStatic* )GetWindowForID( MSD_MARK_CHARINFO ) ;

	// 100106 ONS ������/�κ��丮 �޴� �߰�
	for( count = 0 ; count < MAX_WEBINFO_BTN ; ++count )
	{
		m_pMenuWebInfo[ count ] = ( cButton* )GetWindowForID( MSD_CIBTN_HOMEPAGE + count ) ;
		m_pMenuWebInfo[ count ]->SetActive( FALSE ) ;
	}
	m_pTitleWebInfo			= ( cStatic* )GetWindowForID( MSD_TITLE_WEBINFO ) ;
	m_pMarkWebInfo			= ( cStatic* )GetWindowForID( MSD_MARK_WEBINFO ) ;
	m_pBGWebInfoTop			= ( cStatic* )GetWindowForID( MSD_BG_WEBINFO_TOP ) ;
	m_pBGWebInfoBottom		= ( cStatic* )GetWindowForID( MSD_BG_WEBINFO_BOTTOM ) ;

	for( count = 0 ; count < MAX_INVENTORY_BTN ; ++count )
	{
		m_pMenuInventory[ count ] = ( cButton* )GetWindowForID( MSD_CIBTN_INVENTORY + count ) ;
		m_pMenuInventory[ count ]->SetActive( FALSE ) ;
	}
	m_pTitleInventory		= ( cStatic* )GetWindowForID( MSD_TITLE_INVENTORY ) ;
	m_pMarkInventory		= ( cStatic* )GetWindowForID( MSD_MARK_INVENTORY ) ;
	m_pBGInventoryTop		= ( cStatic* )GetWindowForID( MSD_BG_INVENTORY_TOP ) ;
	m_pBGInventoryBottom	= ( cStatic* )GetWindowForID( MSD_BG_INVENTORY_BOTTOM ) ;
}


//=================================================================================================
//	NAME		: ActionEvent()
//	PURPOSE		: Add function ActionEvent.
//	DATE		: January 15, 2007 LYW
//	ATTENTION	:
//=================================================================================================
DWORD CMainSystemDlg::ActionEvent( CMouse* mouseInfo )
{
	DWORD we = WE_NULL ;

	we |= cDialog::ActionEvent( mouseInfo ) ;

	// 091207 ShinJS --- ��Ŀ���� ���� ��� �ľ�(�޴��� �����ִ� ��츸 �˻�)
	BOOL bOpenMenu = m_bShowSystemSet || m_bShowGameSystem || m_bShowCharInfo || m_bShowWebInfo || m_bShowInventory;
	if( m_bActive && bOpenMenu )
	{
		BOOL bLostFocus = FALSE;
		BOOL bMouseBtnClick = mouseInfo->LButtonDown() || mouseInfo->RButtonDown() || mouseInfo->LButtonDoubleClick() || mouseInfo->RButtonDoubleClick();

		if( bMouseBtnClick && !PtInWindow(mouseInfo->GetMouseEventX(), mouseInfo->GetMouseEventY()) )
		{
			bLostFocus = TRUE;

			PTRLISTPOS pos = m_pComponentList.GetTailPosition();
			while(pos)
			{
				cWindow* window = (cWindow*)m_pComponentList.GetPrev(pos);
				if( bMouseBtnClick &&
					window->IsActive() &&
					window->PtInWindow( mouseInfo->GetMouseEventX(), mouseInfo->GetMouseEventY() ) )
					bLostFocus = FALSE;
			}
		}

		// ��Ŀ���� ���� ���
		if( bLostFocus )
		{
			// �޴��� �ݴ´�
			m_bShowSystemSet = FALSE;
			m_bShowGameSystem = FALSE;
			m_bShowCharInfo = FALSE;
			m_bShowWebInfo = FALSE;
			m_bShowInventory = FALSE;
			SetSystemSet();
			SetGameSystem();
			SetCharInfo();
			SetWebInfo();
			SetInventory();
		}
	}

	return we ;
}


//=================================================================================================
// NAME			: OnActionEvent()
// PURPOSE		: The function for linking controls to window manager.
// ATTENTION	:
//=================================================================================================
void CMainSystemDlg::OnActionEvent(LONG lId, void* p, DWORD we )
{
	cDialog* pHandler = NULL ;

	// 070115 LYW --- Delete this part.
	/*
	if( we & WE_PUSHDOWN || we & WE_PUSHUP )
	{
		switch( lId )
		{
		case MSD_MB_GAMESYSTEM :
			{
				if( GetTabSheet(0)->IsActive() )
				{
					GetTabSheet(0)->SetActive( FALSE ) ;
				}
				else
				{
					GetTabSheet(0)->SetActive( TRUE ) ;
					GetTabSheet(1)->SetActive( FALSE ) ;
				}
			}
			break ;

		case MSD_MB_SYSTEMSET :			
			{
				if( GetTabSheet(1)->IsActive() )
				{
					GetTabSheet(1)->SetActive( FALSE ) ;
				}
				else
				{
					GetTabSheet(1)->SetActive( TRUE ) ;
					GetTabSheet(0)->SetActive( FALSE ) ;
				}
			}
			break ;
		}
	}
	*/
	// 070115 LYW --- Modified this line.
	//else if( we & WE_BTNCLICK )
	if( we & WE_BTNCLICK )
	{
		switch( lId )
		{
		// 070115 LYW --- Add two part.
		// 100106 ONS ������/�κ��丮 �޴� ���� 
		case MSD_MB_GAMESYSTEM :
			{
				if( m_eMainMenuKind != eMainMenu_GameSystem )	
					CloseSubMenu();

				m_bShowGameSystem = !m_bShowGameSystem;
				SetGameSystem() ;
			}
			break ;

		case MSD_MB_SYSTEMSET :			
			{
				if( m_eMainMenuKind != eMainMenu_GameOption )	
					CloseSubMenu();
	
				m_bShowSystemSet = !m_bShowSystemSet;
				SetSystemSet() ;
			}
			break ;

		case MSD_MB_WEBINFO :
			{
				if( m_eMainMenuKind != eMainMenu_WebInfo )	
					CloseSubMenu();
	
				m_bShowWebInfo = !m_bShowWebInfo;
				SetWebInfo();
			}
			break;
		case MSD_GSBTN_CHARINFO : 
			{
				if( m_eMainMenuKind != eMainMenu_CharInfo )	
					CloseSubMenu();
	
				//090420 ONS ĳ�������� �޴��� "�������̵�" �߰�(���� ó���� case MSD_CIBTN_CHARINFO: �� �̵�)
				m_bShowCharInfo = !m_bShowCharInfo;
				SetCharInfo() ;
			}
			break ;
		case MSD_GSBTN_INVENTORY : 
			{
				if( m_eMainMenuKind != eMainMenu_Inventory )	
					CloseSubMenu();

				m_bShowInventory = !m_bShowInventory;
				SetInventory();
			}
			break ;

		// 081118 LYW --- MainSystemDlg : ��ų â ȣ�� ��ư ó�� �߰�.
		case MSG_MP_SKILL :
		case MSD_CIBTN_SKILL:		//090422 ONS ĳ���� ���� �޴��� ��ųâ ȣ�� ��ư�߰�
			{
				pHandler = (cDialog*)GAMEIN->GetSkillTreeDlg();
				if( !pHandler ) return ;

				if( pHandler->IsActive() )
				{
					//ANIMGR->StartFade(ANI_FADE_OUT, 255, 0, 50, pHandler );
					pHandler->SetActive( FALSE );
				}
				else 
				{
					//ANIMGR->StartFade(ANI_FADE_IN, 0, 255, 100, pHandler );
					pHandler->SetActive( TRUE );
				}
			}
			break ;

		case MSD_GSBTN_QUEST : 
			{
				pHandler = (cDialog*)GAMEIN->GetQuestTotalDialog();
				if( !pHandler ) return ;

				if( pHandler->IsActive() )
				{
					pHandler->SetActive(FALSE);
				}
				else
				{
					pHandler->SetActive(TRUE);
					
					CQuestDialog* pQuestDialog = GAMEIN->GetQuestDialog() ;
					if( pQuestDialog )
					{
						if( pQuestDialog->GetQuestCount() > 0 )
						{
							DWORD dwQuestIdx = pQuestDialog->GetSelectedQuestID();
							if( dwQuestIdx < 1 ) 
							{
								DWORD questIndex = pQuestDialog->GetSelectedQuestIDFromTree( 0 );
								pQuestDialog->SetSelectedQuestIdx( questIndex );
							}
							pQuestDialog->RefreshQuestList();
						}
					}
				}
			}
			break ;

		case MSD_GSBTN_GUILD : 
			{
				pHandler =  (cDialog*)GAMEIN->GetGuildDlg();
				if( !pHandler ) return ;
				if( pHandler->IsActive() )
				{
					pHandler->SetActive(FALSE);
				}
				else
				{
					if(HERO->GetGuildIdx() != 0)
						pHandler->SetActive(TRUE);
					else
						// 070330 LYW --- MainSystemDlg : Modified message number of guild part.
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 578 ));
				}
			}
			break ;

		case MSD_GSBTN_FRIEND : 
			{
				pHandler = (cDialog*)GAMEIN->GetFriendDialog();
				if( !pHandler ) return ;
				if( pHandler->IsActive() )
				{
					pHandler->SetActive(FALSE);
				}
				else
				{
					FRIENDMGR->FriendListSyn(1);
				}
			}
			break ;

		case MSD_GSBTN_NOTE : 
			{
		//		pHandler = (cDialog*)GAMEIN->GetNoteDialog();
		//		if( !pHandler ) return ;
		//		if( pHandler->IsActive() )
		//		{
		//			pHandler->SetActive(FALSE);
		//		}
		//		else
		//		{
		//			NOTEMGR->NoteListSyn(1);
		//		}
			}
			break ;

			// 070618 LYW --- MainSystemDlg : Add matching menu.
		case MSD_GSBTN_MATCHING :
			{
				const DATE_MATCHING_INFO& dateInfo = HERO->GetDateMatchingInfo() ;

				if(!dateInfo.bIsValid)
				{
					// notice can't open dialog.
					WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, CHATMGR->GetChatMsg( 1284 ) );
				}
				else
				{
					// open dialog.
					CDateMatchingDlg* pDlg = GAMEIN->GetDateMatchingDlg() ;

					if( pDlg )
					{
						if( pDlg->IsActive() )
						{
							// check chatting.
							CDateMatchingChatDlg* pChatDlg = GAMEIN->GetDateMatchingDlg()->GetChatingDlg() ;
							if( pChatDlg )
							{
								if( pChatDlg->IsOnChatMode() )
								{
									// notice end chatting.
									WINDOWMGR->MsgBox( MBI_END_CHATTING_CLOSE, MBT_YESNO, CHATMGR->GetChatMsg( 1304 ) );
								}
								else
								{
									pDlg->SetActive(FALSE) ;
								}
							}
						}
						else
						{
							pDlg->GetPartnerListDlg()->SettingControls() ;
							//pDlg->GetPartnerListDlg()->UpdateInfo() ;
							pDlg->SetActive(TRUE) ;
						}
					}
				}
			}
			break ;

			// 070618 LYW --- MainSystemDlg : Add family menu.
		case MSD_GSBTN_FAMILY :
			{
				CSHFamily* pFamily = HERO->GetFamily() ;	

				if( pFamily && pFamily->Get()->nMasterID != 0 )
				{
					CFamilyDialog* pDlg = GAMEIN->GetFamilyDlg() ;

					if( pDlg )
					{
						if( pDlg->IsActive() )
						{
							pDlg->SetActive(FALSE) ;
						}
						else
						{
							pDlg->UpdateAllInfo() ;
							pDlg->SetActive(TRUE) ;
						}
					}
				}
				else
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 369 ));
				}
			}
			break ;

		// 100106 ONS ä�ù� �޴� �߰�
		case MSD_GSBTN_CHATROOM:
			{
				CHATROOMMGR->ToggleChatRoomMainDlg();
			}
			break;

		case MSD_SSBTN_GAME : 
			{
				pHandler = (cDialog*)GAMEIN->GetOptionDialog();
				if( !pHandler ) return ;
				if( pHandler->IsActive() )
				{
					pHandler->SetActive(FALSE);
				}
				else
				{
					pHandler->SetActive(TRUE);
				}
			}
			break;
		case MSD_SSBTN_CHARSELECT : 
			{
				WINDOWMGR->MsgBox( MBI_MOVE_TO_CHARSELECT, MBT_YESNO, CHATMGR->GetChatMsg( 1329 ) );
			}
			break ;

		// 100106 ONS ��ȭ�޴� �߰�
		case MSD_SSBTN_RECORD:
			{
				// ��ȭ Dialog �� ����
				cDialog* pVideoCaptureDlg = WINDOWMGR->GetWindowForID( VIDEOCAPTURE_DLG );
				if( !pVideoCaptureDlg )
					return;

				pVideoCaptureDlg->SetActive( !pVideoCaptureDlg->IsActive() );
			}
			break;

		case MSD_SSBTN_RECORD_STOP:
			{
				CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
				if( !pMiniMapDlg )
					return;

				// �̴ϸ��� ��µ��� �ʴ� �ʰ� �����ؼ� ó���Ѵ�.
				if( pMiniMapDlg->HaveMinimap() )
				{
					pMiniMapDlg->ShowVideoCaptureStartBtn( TRUE );
				}
				else
				{
					cAdditionalButtonDlg* pAdditionBtnDlg = GAMEIN->GetAdditionalButtonDlg();
					if( !pAdditionBtnDlg )
						return;
					pAdditionBtnDlg->SetActive( FALSE );
					pAdditionBtnDlg->ShowRecordStartBtn( FALSE );
				}


				if( VIDEOCAPTUREMGR->IsCapturing() )
				{
					VIDEOCAPTUREMGR->CaptureStop();
					m_pMenuSystemSet[eRecordBtnState_Start]->SetActive(TRUE);
					m_pMenuSystemSet[eRecordBtnState_Stop]->SetActive(FALSE);
				}
			}
			break;
		case MSD_SSBTN_ENDGAME : 
			{
				WINDOWMGR->MsgBox( MBI_EXIT, MBT_YESNO, RESRCMGR->GetMsg( 263 ) );
			}
			break ;

		// 071025 LYW --- MainSystemDlg : Add help button.
		case MSD_GSBTN_HELP :
			{
				cHelperDlg* pDlg = NULL ;
				pDlg = GAMEIN->GetHelpDlg() ;

				if( !pDlg )
				{
					#ifdef _GMTOOL_
					MessageBox(NULL, "Failed to open help dlg", "MainSystemDlg::OnActionEvent", MB_OK);
					#endif //_GMTOOL

					return ;
				}

				pDlg->SetActive( !pDlg->IsActive() ) ;
			}
			break ;

		// 090420 ONS ĳ�������� �޴��� "�������̵�" �߰�
		case MSD_CIBTN_CHARINFO:
			{
				pHandler = GAMEIN->GetCharacterDialog();
				if( !pHandler )	return;

				if( pHandler->IsActive() )
				{
					pHandler->SetActive( FALSE );

					// 070115 LYW --- Close tree dialog.
					GAMEIN->GetCharacterDialog()->SetViewTreeDlg( FALSE ) ;
				}
				else 
				{
					((CCharacterDialog*)pHandler)->UpdateData();
					pHandler->SetActive( TRUE );
				}
			}
			break;

		case MSD_CIBTN_GRADECLASS:
			{
				pHandler = (cDialog*)GAMEIN->GetGradeClassDlg();
				if( !pHandler ) return ;
				if( pHandler->IsActive() )
				{
					pHandler->SetActive(FALSE);
				}
				else
				{
					pHandler->SetActive(TRUE);
				}
			}
			break;
		// 091211 ONS �̴�Ȩ�� �޴� �߰�
		case MSD_CIBTN_MINIHOMEPAGE:
			{
				// ���������� �����. 
			//	cHousingWebDlg* pDlg = GAMEIN->GetHousingWebDlg();
			//	if( pDlg )
			//	{
			//		pDlg->OpenMiniHomePage( TITLE->GetUserIdx() );
			//	}
			}
			break;
		case MSD_CIBTN_ITEMMALL:
			{
#ifndef _TW1_LOCAL_//_TW_LOCAL_
				// 080801 LUJ, �����۸��� �����Ѵ�
				// 080807 LUJ, ��ó����� �������� ��ũ�� �и��Ѵ�
				ShellExecute( 
					0, 
					"open", 
					"explorer",
#ifdef	_TL_LOCAL_
					"http://www.lunaonline.in.th/itemmall",
#else
					"http://luna.eyainteractive.com/item_zone/shop_zone/itemMall.asp",
#endif
					0,
					SW_SHOWNORMAL );
#else
				if( GAMEIN->GetItemShopDlg() == NULL ) break;
				if( GAMEIN->GetItemShopDlg()->IsActive() )
				{
					GAMEIN->GetItemShopDlg()->SetActive( FALSE );
					break;
				}

				char csBuffer[1024];
				char csDigest[1024];

				sprintf( csBuffer, "%d%d%s%s%s",
				g_nServerSetNum,
				HEROID,
				g_szHeroIDName,
				HERO->GetObjectName(),
				"fnsk@hsfk@ls!!"
				);

/*
				sprintf( csBuffer, "%d%d%s%s%s",
				1,
				114,
				"payletter2",
				"payletter",
				"fnsk@hsfk@ls!!"
				);
*/
				HCRYPTPROV hCryptProv; 
				HCRYPTHASH hHash; 
				BYTE bHash[0x7f]; 
				DWORD dwHashLen= 16; // The MD5 algorithm always returns 16 bytes. 
				DWORD cbContent= strlen(csBuffer);//csBuffer.GetLength(); 
				BYTE* pbContent= (BYTE*)csBuffer;//csBuffer.GetBuffer(cbContent); 


				if(CryptAcquireContext(&hCryptProv, 
					NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET)) 
				{

					if(CryptCreateHash(hCryptProv, 
						CALG_MD5,	// algorithm identifier definitions see: wincrypt.h
						0, 0, &hHash)) 
					{
						if(CryptHashData(hHash, pbContent, cbContent, 0))
						{

							if(CryptGetHashParam(hHash, HP_HASHVAL, bHash, &dwHashLen, 0)) 
							{
								// Make a string version of the numeric digest value
								//csDigest.Empty();
								memset( csDigest, 0, sizeof(char)*1024 );
								//CString tmp;
								char tmp[32];
								for (int i = 0; i<16; i++)
								{
									//tmp.Format("%02x", bHash[i]);
									sprintf( tmp, "%02x", bHash[i] );
									strcat( csDigest, tmp );//csDigest+=tmp;
								}

							}
							//else csDigest=_T("Error getting hash param"); 

						}
						//else csDigest=_T("Error hashing data"); 
					}
					//else csDigest=_T("Error creating hash"); 

				}
				//else csDigest=_T("Error acquiring context"); 


				CryptDestroyHash(hHash); 
				CryptReleaseContext(hCryptProv, 0); 
				//csBuffer.ReleaseBuffer();

				char URL[1024];

				sprintf( URL, "%s%s%d%s%d%s%s%s%s%s%s",
					"http://bill5.omg.com.tw/charge/lunaplus/LunaPlusItemList.asp?",
					"server_index=", g_nServerSetNum,
					"&m_idPlayer=", HEROID,
					"&m_szName=", HERO->GetObjectName(),
					"&user_id=", g_szHeroIDName,
					"&md5=", csDigest
					);

/*
				sprintf( URL, "%s%s%d%s%d%s%s%s%s%s%s",
					"http://bill3.omg.com.tw/charge/luna/lunaitemlist.asp?",
					"server_index=", 1,
					"&m_idPlayer=", 114,
					"&m_szName=", "payletter",
					"&user_id=", "payletter2",
					"&md5=", csDigest
					);
*/
			
				if( GAMEIN->GetItemShopDlg() )
				{
					GAMEIN->GetItemShopDlg()->Navigate( URL );
					GAMEIN->GetItemShopDlg()->SetActive( TRUE );				
				}
#endif
			}
			break;

		case MSD_CIBTN_PCROOMSHOP:
			{

				// PC�� ����
				if( GAMEIN->GetDealDialog()->IsActive() )
				{
					// �̹� PC�� ������ ���� ���
					if( GAMEIN->GetDealDialog()->IsActivePointInfo() )
					{
						// 100223 ONS ������ �����ִ°�� �ݴ´�.
						GAMEIN->GetDealDialog()->ShowDealDialog( FALSE );
						break;
					}
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1077 ) );		// ������ ���� ��������
					break;
				}

				if( HERO->GetState() == eObjectState_Die)
					break;

				MSGBASE msg;
				ZeroMemory( &msg, sizeof(msg) );
				msg.Category = MP_PCROOM;
				msg.Protocol = MP_PCROOM_OPEN_MALL_SYN;
				msg.dwObjectID = HEROID;
				NETWORK->Send( &msg, sizeof(msg) );
			}
			break;
		case MSD_CIBTN_PCROOMSHOP_MATERIAL:
			{
/*
				// PC�� ����
				if( GAMEIN->GetDealDialog()->IsActive() )
				{
					// �̹� PC�� ������ ���� ���
					if( GAMEIN->GetDealDialog()->IsActivePointInfo() )
					{
						// 100223 ONS ������ �����ִ°�� �ݴ´�.
						GAMEIN->GetDealDialog()->ShowDealDialog( FALSE );
						break;
					}
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1077 ) );		// ������ ���� ��������
					break;
				}

				if( HERO->GetState() == eObjectState_Die)
					break;

				//CHero* const cekvip = OBJECTMGR->GetHero();
				//
				//if( cekvip->GetVIPstatus() < 2)
				//{
				//	CHATMGR->AddMsg( CTC_SYSMSG, "Minimum required VIP Level 2, for use this shop"  );		// ������ ���� ��������
				//	break;
				//}
				MSGBASE msg;
				ZeroMemory( &msg, sizeof(msg) );
				msg.Category = MP_PCROOM;
				msg.Protocol = MP_PCROOM_OPEN_MALL_SYN_MATERIAL;
				msg.dwObjectID = HEROID;
				NETWORK->Send( &msg, sizeof(msg) ); */
			}
			break;
		case MSD_CIBTN_PCROOMSHOP_PET: //petshop btn
			{
/*
				// PC�� ����
				if( GAMEIN->GetDealDialog()->IsActive() )
				{
					// �̹� PC�� ������ ���� ���
					if( GAMEIN->GetDealDialog()->IsActivePointInfo() )
					{
						// 100223 ONS ������ �����ִ°�� �ݴ´�.
						GAMEIN->GetDealDialog()->ShowDealDialog( FALSE );
						break;
					}
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1077 ) );		// ������ ���� ��������
					break;
				}

				if( HERO->GetState() == eObjectState_Die)
					break;

				//CHero* const cekvip = OBJECTMGR->GetHero();
				//
				//if( cekvip->GetVIPstatus() < 2)
				//{
				//	CHATMGR->AddMsg( CTC_SYSMSG, "Minimum required VIP Level 2, for use this shop"  );		// ������ ���� ��������
				//	break;
				//}

				MSGBASE msg;
				ZeroMemory( &msg, sizeof(msg) );
				msg.Category = MP_PCROOM;
				msg.Protocol = MP_PCROOM_OPEN_MALL_SYN_PET;
				msg.dwObjectID = HEROID;
				NETWORK->Send( &msg, sizeof(msg) ); */
			}
			break;
		case MSD_CIBTN_PCROOMSHOP_COSTUME: //costume
			{
/*
				// PC�� ����
				if( GAMEIN->GetDealDialog()->IsActive() )
				{
					// �̹� PC�� ������ ���� ���
					if( GAMEIN->GetDealDialog()->IsActivePointInfo() )
					{
						// 100223 ONS ������ �����ִ°�� �ݴ´�.
						GAMEIN->GetDealDialog()->ShowDealDialog( FALSE );
						break;
					}
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1077 ) );		// ������ ���� ��������
					break;
				}

				if( HERO->GetState() == eObjectState_Die)
					break;

				//CHero* const cekvip = OBJECTMGR->GetHero();
				//
				//if( cekvip->GetVIPstatus() < 3)
				//{
				//	CHATMGR->AddMsg( CTC_SYSMSG, "Minimum required VIP Level 3, for use this shop"  );		// ������ ���� ��������
				//	break;
				//}

				MSGBASE msg;
				ZeroMemory( &msg, sizeof(msg) );
				msg.Category = MP_PCROOM;
				msg.Protocol = MP_PCROOM_OPEN_MALL_SYN_COSTUME;
				msg.dwObjectID = HEROID;
				NETWORK->Send( &msg, sizeof(msg) ); */
			}
			break;
		case MSD_CIBTN_PCROOMSHOP_FISHING: //fishing
			{
/*
				// PC�� ����
				if( GAMEIN->GetDealDialog()->IsActive() )
				{
					// �̹� PC�� ������ ���� ���
					if( GAMEIN->GetDealDialog()->IsActivePointInfo() )
					{
						// 100223 ONS ������ �����ִ°�� �ݴ´�.
						GAMEIN->GetDealDialog()->ShowDealDialog( FALSE );
						break;
					}
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1077 ) );		// ������ ���� ��������
					break;
				}

				if( HERO->GetState() == eObjectState_Die)
					break;

				//CHero* const cekvip = OBJECTMGR->GetHero();
				//
				//if( cekvip->GetVIPstatus() < 3)
				//{
				//	CHATMGR->AddMsg( CTC_SYSMSG, "Minimum required VIP Level 3, for use this shop"  );		// ������ ���� ��������
				//	break;
				//}

				MSGBASE msg;
				ZeroMemory( &msg, sizeof(msg) );
				msg.Category = MP_PCROOM;
				msg.Protocol = MP_PCROOM_OPEN_MALL_SYN_FISHING;
				msg.dwObjectID = HEROID;
				NETWORK->Send( &msg, sizeof(msg) ); */
			}
			break;
		case MSD_CIBTN_VIP_NOTE:
			{
				if( GAMEIN->GetDealDialog()->IsActive() )
				{
					if( GAMEIN->GetDealDialog()->IsActivePointInfo() )
					{
						GAMEIN->GetDealDialog()->ShowDealDialog( FALSE );
						break;
					}
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1077 ) );		// ������ ���� ��������
					break;
				}

				if( HERO->GetState() == eObjectState_Die)
					break;

				//CHero* const cekvip = OBJECTMGR->GetHero();
				//if( cekvip->GetVIPstatus() < 1)
				//{
				//	CHATMGR->AddMsg( CTC_SYSMSG, "Minimum required VIP Level 1, for use this menu"  );		// ������ ���� ��������
				//	break;
				//}
				pHandler = (cDialog*)GAMEIN->GetNoteDialog();
				if( !pHandler ) return ;
				if( pHandler->IsActive() )
				{
					pHandler->SetActive(FALSE);
				}
				else
				{
					NOTEMGR->NoteListSyn(1);
				}
			}
			break;
		case MSD_CIBTN_VIP_AUCTION:
			{
				if( GAMEIN->GetDealDialog()->IsActive() )
				{
					if( GAMEIN->GetDealDialog()->IsActivePointInfo() )
					{
						GAMEIN->GetDealDialog()->ShowDealDialog( FALSE );
						break;
					}
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1077 ) );		// ������ ���� ��������
					break;
				}

				if( HERO->GetState() == eObjectState_Die)
					break;

				//CHero* const cekvip = OBJECTMGR->GetHero();
				//if( cekvip->GetVIPstatus() < 4)
				//{
				//	CHATMGR->AddMsg( CTC_SYSMSG, "Disable Menu"  );
				//	//CHATMGR->AddMsg( CTC_SYSMSG, "Minimum required VIP Level 1, for use this menu"  );		// ������ ���� ��������
				//	break;
				//}

				CConsignmentDlg* pDlg = GAMEIN->GetConsignmentDlg();
				if( pDlg && !pDlg->IsActive())
				{
					pDlg->SetActive(TRUE);
					pDlg->SetMode(CConsignmentDlg::eConsignment_Mode_Buy);
				}
				else
				{
					pDlg->SetActive(FALSE);
				}
			}
			break;
		case MSD_CIBTN_VIP_WAREHOUSE: //Alemuri MapMove Info Dlg
			{
				if( GAMEIN->GetDealDialog()->IsActive() )
				{
					if( GAMEIN->GetDealDialog()->IsActivePointInfo() )
					{
						GAMEIN->GetDealDialog()->ShowDealDialog( FALSE );
						break;
					}
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1077 ) );		// ������ ���� ��������
					break;
				}

				if( HERO->GetState() == eObjectState_Die)
					break;

				//CHero* const cekvip = OBJECTMGR->GetHero();
				//if( cekvip->GetVIPstatus() < 1)
				//{
				//	CHATMGR->AddMsg( CTC_SYSMSG, "Minimum required VIP Level 1, for use this shop"  );		// ������ ���� ��������
				//	break;
				//}

				if(GAMEIN->GetStorageDialog()->IsItemInit())									// â�� �������� ������ �ʱ�ȭ�� �Ǿ�����,
				{
					if( !GAMEIN->GetInventoryDialog()->IsActive() )								// �κ��丮 ���̾�αװ� Ȱ��ȭ ���̸�,
					{
						GAMEIN->GetStorageDialog()->ShowStorageDlg(TRUE) ;							// â�� ���̾�α׸� Ȱ��ȭ �Ѵ�.
					}
					else
					{
						GAMEIN->GetStorageDialog()->SetActive(TRUE) ;							// â�� ���̾�α׸� Ȱ��ȭ �Ѵ�.
					}

					if(OBJECTMGR->GetHero()->GetStorageNum() == 0)												// ������ â�� ���ٸ�,
					{

						GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_NoWare);		// â�� ���ٴ� ���� �����Ѵ�.
					}
					else																		// ������ â�� �ִٸ�,
					{
						GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_StorageWare1);	// â�� 1�� Ȱ��ȭ �� ���� �Ѵ�.
						GAMEIN->GetStorageDialog()->AddStorageMode(eStorageMode_StorageWare1);	// â�� ��带 �߰��Ѵ�.
					}
				}
				else																			// â���� �������� ������ �ʱ�ȭ�� ���� �ʾҴٸ�,
				{
					if(HERO->GetStorageNum() == 0)												// ������ â�� ������,
					{
						return;
					}
					else																		// ������ â�� �ִٸ�,
					{					
						MSG_WORD2 msg;														// �޽��� ����ü�� �����Ѵ�.
						msg.Category = MP_ITEM;												// ī�װ��� ���������� �����Ѵ�.
						msg.Protocol = MP_ITEM_STORAGE_ITEM_INFO_SYN;						// ���������� ������ â��, ������ ���� ��ũ�� �����Ѵ�.
						msg.dwObjectID = HEROID;											// HERO ���̵� �����Ѵ�.
						msg.wData1=79 ;													// �����͸� 79�� �����Ѵ�.
						msg.wData2 = eNpcParam_ShowpStorage;								// ������ 2�� NPC �����ͷ� �����Ѵ�.

						NETWORK->Send(&msg, sizeof(msg));									// �޽����� �����Ѵ�.
					}
				}
			}
			break;
		case MSD_CIBTN_VIP_MAPMOVE: 
			{
				if( GAMEIN->GetDealDialog()->IsActive() )
				{
					if( GAMEIN->GetDealDialog()->IsActivePointInfo() )
					{
						GAMEIN->GetDealDialog()->ShowDealDialog( FALSE );
						break;
					}
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1077 ) );		// ������ ���� ��������
					break;
				}

				if( HERO->GetState() == eObjectState_Die)
					break;

				//CHero* const cekvip = OBJECTMGR->GetHero();
				//if( cekvip->GetVIPstatus() < 1)
				//{
				//	CHATMGR->AddMsg( CTC_SYSMSG, "Minimum required VIP Level 1, for use this shop"  );		// ������ ���� ��������
				//	break;
				//}
				cMapMoveDialog* pDlg = NULL ;														// �� �̵� â�� �޴´�.
				pDlg = GAMEIN->GetMapMoveDlg() ;
				if( !pDlg ) return ;																// �� �̵� â�� ���Ἲ üũ.
				
				// 080919 LYW --- ItemManager : ��� �뵵�� �����Ѵ�.
				pDlg->SetUseCase(eNormalStyle) ;
				pDlg->SetTypePre(1);
				if( pDlg && !pDlg->IsActive())
				{
					pDlg->SetActive(TRUE) ;																// �� �̵�â�� Ȱ��ȭ�Ѵ�.
					pDlg->GetMapListDlg()->ResetGuageBarPos() ;	
				} else {
					pDlg->SetActive(FALSE) ;
				}
				
			}
			break;
			
		case MSD_CIBTN_HOMEPAGE:
			{

/*					
				cHousingWebDlg* pDlg = GAMEIN->GetHousingWebDlg();
				if( pDlg )
				{
					if (pDlg->IsActive())
					{
						pDlg->SetActive(FALSE);
					} else {

						MSGBASE msg;																	// �޽��� ����ü�� �����Ѵ�.
						msg.Category = MP_NOTE;															// ī�װ��� ���������� �����Ѵ�.
						msg.Protocol = MP_NOTE_BROWSER_OPEN_SYN;										// ���������� ������ �� ���� ��ũ�� �����Ѵ�.
						msg.dwObjectID = gHeroID;

						NETWORK->Send(&msg, sizeof(msg));	
					}
				}
*/					
			}
			break;
		case MSD_CIBTN_GMREPORT:
			{	
				
				CReportDialog* pHandler = (CReportDialog*)GAMEIN->GetReportDlg();
				if( pHandler->IsActive() )
				{
					pHandler->SetActive(FALSE);
				} else {
					pHandler->SetActive(TRUE);
				}
					
			}
			break;

		case MSD_CIBTN_INVENTORY:
			{
				// �κ��丮
				pHandler = (cDialog*)GAMEIN->GetInventoryDialog();
				if( !pHandler ) return ;
//				if( pHandler->IsActive() )
//				{
					pHandler->SetActive( !pHandler->IsActive() );
//				}
//				else 
//				{
//					pHandler->SetActive( TRUE );
//				}
			}
			break;

		case MSD_CIBTN_ITEMMALLSHOP:
			{
				// �����۸� â��
				// 091105 pdy ����/��æƮ/��ȭ/���� ���ΰ�� �����۸� ���� �Ұ��� 
				BOOL isOpen = FALSE;
				{
					cDialog* mixDialog			= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
					cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
					cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
					cDialog* dissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );

					ASSERT( mixDialog && enchantDialog && reinforceDialog && dissoloveDialog );

					isOpen =	mixDialog->IsActive()		||
								enchantDialog->IsActive()	||
								reinforceDialog->IsActive()	||
								dissoloveDialog->IsActive();
				}

				if( isOpen == TRUE )
				{
					CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) ) ;
					return;
				}
				// 100223 ONS �����۸����� �����ִ°�� �ݴ´�.
				if( GAMEIN->GetItemShopDialog()->IsActive() )
				{
					GAMEIN->GetItemShopDialog()->SetActive(FALSE);
					GAMEIN->GetInventoryDialog()->SetActive(FALSE);
					return;
				}
				
				MSGBASE msg;																	// �޽��� ����ü�� �����Ѵ�.
				msg.Category = MP_ITEM;															// ī�װ��� ���������� �����Ѵ�.
				msg.Protocol = MP_ITEM_SHOPITEM_INFO_SYN;										// ���������� ������ �� ���� ��ũ�� �����Ѵ�.
				msg.dwObjectID = gHeroID;

				NETWORK->Send(&msg, sizeof(msg));												// �޽����� �����Ѵ�.
			}
			break;
		}
	}
}


//=================================================================================================
//	NAME		: SetSystemSet()
//	PURPOSE		: Add function to setting systemset part.
//	DATE		: January 15, 2007 LYW
//	ATTENTION	:
//=================================================================================================
void CMainSystemDlg::SetSystemSet()
{
	for( int count = 0 ; count < MAX_SYSTEMSET_BTN ; ++count ) 
	{
		m_pMenuSystemSet[ count ]->SetActive( m_bShowSystemSet ) ;
	}
	m_pTitleSystemSet->SetActive( m_bShowSystemSet ) ;
	m_pMarkSystemSet->SetActive( m_bShowSystemSet ) ;
	m_pSystemSet->SetActive( m_bShowSystemSet ) ;
	m_pSystemSet_Bottom->SetActive( m_bShowSystemSet ) ;

	// 100106 ONS ��ȭ�޴� ����ó��( ��ȭ <=> ��ȭ���� )
	if( m_bShowSystemSet )
	{
		m_eMainMenuKind = eMainMenu_GameOption;
		if(VIDEOCAPTUREMGR->IsCapturing())
		{
			m_pMenuSystemSet[eRecordBtnState_Start]->SetActive(FALSE);
			m_pMenuSystemSet[eRecordBtnState_Stop]->SetActive(TRUE);
		}
		else
		{
			m_pMenuSystemSet[eRecordBtnState_Start]->SetActive(TRUE);
			m_pMenuSystemSet[eRecordBtnState_Stop]->SetActive(FALSE);
		}
	}
}


//=================================================================================================
//	NAME		: SetGameSystem()
//	PURPOSE		: Add function to setting gamesystem part.
//	DATE		: January 15, 2007 LYW
//	ATTENTION	:
//=================================================================================================
void CMainSystemDlg::SetGameSystem()
{
	for( int count = 0 ;  count < MAX_GAMESYSTEM_BTN ; ++count ) 
	{
		m_pMenuGameSystem[ count ]->SetActive( m_bShowGameSystem ) ;
	}
	m_pTitleGameSystem->SetActive( m_bShowGameSystem ) ;
	m_pMarkGameSystem->SetActive( m_bShowGameSystem ) ;
	m_pGameSystem->SetActive( m_bShowGameSystem ) ;
	if( m_bShowGameSystem )
	{
		m_eMainMenuKind = eMainMenu_GameSystem;
	}
}


//=================================================================================================
//	NAME		: SetCharInfo()
//	PURPOSE		: Add function to Character Information part.
//	DATE		: April 20, 2009 ONS
//	ATTENTION	:
//=================================================================================================
void CMainSystemDlg::SetCharInfo()
{
	for( int count = 0 ;  count < MAX_CHARINFO_BTN ; ++count ) 
	{
		m_pMenuCharInfo[ count ]->SetActive( m_bShowCharInfo ) ;
	}
	m_pTitleCharInfo->SetActive( m_bShowCharInfo ) ;
	m_pMarkCharInfo->SetActive( m_bShowCharInfo ) ;
	m_pCharInfo->SetActive( m_bShowCharInfo ) ;
	if( m_bShowCharInfo )
	{
		m_eMainMenuKind = eMainMenu_CharInfo;
	}
}

// 100106 ONS ������ �޴� �߰�
void CMainSystemDlg::SetWebInfo()
{
#ifdef _TL_LOCAL_ // _TW_LOCAL_
	if( GAMEIN->GetItemShopDlg() == NULL ) return;
	if( m_bShowWebInfo )
	{
		char csBuffer[1024];
		char csDigest[1024];

		sprintf( csBuffer, "%d%d%s%s%s",
			g_nServerSetNum,
			HEROID,
			g_szHeroIDName,
			HERO->GetObjectName(),
			"fnsk@hsfk@ls!!"
			);

		/*
		sprintf( csBuffer, "%d%d%s%s%s",
		1,
		114,
		"payletter2",
		"payletter",
		"fnsk@hsfk@ls!!"
		);
		*/
		HCRYPTPROV hCryptProv; 
		HCRYPTHASH hHash; 
		BYTE bHash[0x7f]; 
		DWORD dwHashLen= 16; // The MD5 algorithm always returns 16 bytes. 
		DWORD cbContent= strlen(csBuffer);//csBuffer.GetLength(); 
		BYTE* pbContent= (BYTE*)csBuffer;//csBuffer.GetBuffer(cbContent); 


		if(CryptAcquireContext(&hCryptProv, 
			NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET)) 
		{

			if(CryptCreateHash(hCryptProv, 
				CALG_MD5,	// algorithm identifier definitions see: wincrypt.h
				0, 0, &hHash)) 
			{
				if(CryptHashData(hHash, pbContent, cbContent, 0))
				{

					if(CryptGetHashParam(hHash, HP_HASHVAL, bHash, &dwHashLen, 0)) 
					{
						// Make a string version of the numeric digest value
						//csDigest.Empty();
						memset( csDigest, 0, sizeof(char)*1024 );
						//CString tmp;
						char tmp[32];
						for (int i = 0; i<16; i++)
						{
							//tmp.Format("%02x", bHash[i]);
							sprintf( tmp, "%02x", bHash[i] );
							strcat( csDigest, tmp );//csDigest+=tmp;
						}

					}
					//else csDigest=_T("Error getting hash param"); 

				}
				//else csDigest=_T("Error hashing data"); 
			}
			//else csDigest=_T("Error creating hash"); 

		}
		//else csDigest=_T("Error acquiring context"); 


		CryptDestroyHash(hHash); 
		CryptReleaseContext(hCryptProv, 0); 
		//csBuffer.ReleaseBuffer();

		char URL[1024];

		sprintf( URL, "%s%s%d%s%d%s%s%s%s%s%s",
			"http://bill5.omg.com.tw/charge/lunaplus/LunaPlusItemList.asp?",
			"server_index=", g_nServerSetNum,
			"&m_idPlayer=", HEROID,
			"&m_szName=", HERO->GetObjectName(),
			"&user_id=", g_szHeroIDName,
			"&md5=", csDigest
			);

		/*
		sprintf( URL, "%s%s%d%s%d%s%s%s%s%s%s",
		"http://bill3.omg.com.tw/charge/luna/lunaitemlist.asp?",
		"server_index=", 1,
		"&m_idPlayer=", 114,
		"&m_szName=", "payletter",
		"&user_id=", "payletter2",
		"&md5=", csDigest
		);
		*/

		if( GAMEIN->GetItemShopDlg() )
		{
			GAMEIN->GetItemShopDlg()->Navigate( URL );
			GAMEIN->GetItemShopDlg()->SetActive( TRUE );				
		}
	}
	else
	{
		if( GAMEIN->GetItemShopDlg()->IsActive() )
		{
			GAMEIN->GetItemShopDlg()->SetActive( FALSE );
			return;
		}
	}
#else
	for( int count = 0 ;  count < MAX_WEBINFO_BTN ; ++count ) 
	{
		m_pMenuWebInfo[ count ]->SetActive( m_bShowWebInfo ) ;
	}
	m_pTitleWebInfo->SetActive( m_bShowWebInfo ) ;
	m_pMarkWebInfo->SetActive( m_bShowWebInfo ) ;
	m_pBGWebInfoTop->SetActive( m_bShowWebInfo ) ;
	m_pBGWebInfoBottom->SetActive( m_bShowWebInfo ) ;
	if( m_bShowWebInfo )
	{
		m_eMainMenuKind = eMainMenu_WebInfo;
	}
#endif
}

// 100106 ONS �κ��丮 �޴� �߰�
void CMainSystemDlg::SetInventory()
{
	for( int count = 0 ;  count < MAX_INVENTORY_BTN ; ++count ) 
	{
		m_pMenuInventory[ count ]->SetActive( m_bShowInventory ) ;
	}
	m_pTitleInventory->SetActive( m_bShowInventory ) ;
	m_pMarkInventory->SetActive( m_bShowInventory ) ;
	m_pBGInventoryTop->SetActive( m_bShowInventory ) ;
	m_pBGInventoryBottom->SetActive( m_bShowInventory ) ;
	if( m_bShowInventory )
	{
		m_eMainMenuKind = eMainMenu_Inventory;
	}
}

// 100106 ONS �ý��� ���� �޴� �ݱ�ó��.
void CMainSystemDlg::CloseSubMenu()
{
	switch(m_eMainMenuKind)
	{
	case eMainMenu_WebInfo:
		{
			m_bShowWebInfo = FALSE;
			SetWebInfo();
		}
		break;
	case eMainMenu_CharInfo:
		{
			m_bShowCharInfo = FALSE;
			SetCharInfo();
		}
		break;
	case eMainMenu_Inventory:
		{
			m_bShowInventory = FALSE;
			SetInventory();
		}
		break;
	case eMainMenu_GameSystem:
		{
			m_bShowGameSystem = FALSE;
			SetGameSystem();
		}
		break;
	case eMainMenu_GameOption:
		{
			m_bShowSystemSet = FALSE;
			SetSystemSet();
		}
		break;
	}
}

