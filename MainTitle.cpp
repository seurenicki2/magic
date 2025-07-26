// MainTitle.cpp: implementation of the CMainTitle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "MainTitle.h"
#include "MainGame.h"

#include "cWindowSystemFunc.h"
#include "WindowIDEnum.h"
#include "MHCamera.h"

#include "AppearanceManager.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cResourceManager.h"
#include "GameResourceManager.h"
#include "ChatManager.h"
#include "MHTimeManager.h"
#include "cMsgBox.h"

#include "mhMap.h"
#include "ServerListDialog.h"
#ifdef _GMTOOL_
#include "GMToolManager.h"
#endif
#include "GMNotifyManager.h"
#include "cImageSelf.h"
#include "UserInfoManager.h"
#include "CameraConfiguration.h"
// desc_hseos_��������01
// S �������� �߰� added by hseos 2007.06.16
#include "CharMakeManager.h"

#define _LOGOWINDOW_

#ifdef _NPROTECT_
#include "NProtectManager.h"
#endif

extern char g_AgentAddr[16];
extern WORD g_AgentPort;
extern char g_szHeroIDName[];
#ifdef _TW_LOCAL_
extern char g_szHeroKey[];
extern char g_CLIENTVERSION[];
#endif
extern int	g_nServerSetNum;
extern ScriptCheckValue g_Check;
extern HWND _g_hWnd;
//========== Dynamic Login Title ==========//
extern int MapNumFromCamera = 0;

#define ALPHA_PROCESS_TIME	1200
#define WAIT_CONNECT_TIME	30000	//������Ʈ�� ���ӱ�ٸ� �ð� 90��

void SendOutMsg()
{
	MSGBASE msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_REQUEST_DISTOUT;
	NETWORK->Send(&msg, sizeof(msg));
}

BOOL bFirst;
GLOBALTON(CMainTitle)
CMainTitle::CMainTitle() :
mCameraConfiguration( new CCameraConfiguration( CCameraConfiguration::TypeLogin ) )
// 090930 ONS 2D�α����̹������� ó��
,m_2DLoginImage( new cImageSelf )
,m_2DLoginLoadingImage( new cImageSelf )
,mImageBar( new cImageSelf )
, mImageBarBottom(new cImageSelf)
,mImageLogo( new cImageSelf )
, mImageLogoCompany(new cImageSelf)
{
#ifdef TAIWAN_LOCAL
	m_pAdvice		= NULL;
#endif
	m_pLogoWindow	= NULL;
	m_bInit			= FALSE;

	m_pServerListDlg = NULL;
	memset( m_DistributeAddr, 0, 16 );
	m_DistributePort = 0;
	m_bServerList = FALSE;

	m_bDisconntinToDist = FALSE;
	m_bDynamicDlg = FALSE;

	const VECTOR2 emptyVector = {0};
	mPositionBar = emptyVector;
	mScaleBar = emptyVector;
	mPositionBarBottom = emptyVector;
	mScaleBarBottom = emptyVector;
	mPositionLogo = emptyVector;
	mScaleLogo = emptyVector;
	mPositionLogoCompany = emptyVector;
	mScaleLogoCompany = emptyVector;
	iLastNum = 0;

	//Alemuri Memorize ID after wrong Pass----
	SetPreviousID("");
	//----------------------------------------

}

CMainTitle::~CMainTitle()
{
	SAFE_DELETE( mCameraConfiguration );
	// 090930 ONS 2D�α����̹������� �޸�����
    SAFE_DELETE( m_2DLoginImage );
	SAFE_DELETE( m_2DLoginLoadingImage );
	SAFE_DELETE( mImageLogo );
	SAFE_DELETE( mImageBar );
	SAFE_DELETE(mImageBarBottom);
	SAFE_DELETE(mImageLogoCompany);
}

static BOOL g_bResourceLoaded = FALSE;

//cImageSelf image;

BOOL CMainTitle::Init(void* pInitParam)
{
	/*if(IMAGE_NODE* const imageNode = RESRCMGR->GetInfo(24)) //top bar
	{
		mImageBar->LoadSprite(
			"./data/interface/2dimage/image/login_bar00.tif",
			1024.f,
			128.f);
	}
	*/
	/*if (IMAGE_NODE* const imageNode = RESRCMGR->GetInfo(25)) //bottom bar
	{
		mImageBar->LoadSprite(
			imageNode->szFileName,
			imageNode->size.x,
			imageNode->size.y);
		/*mImageBarBottom->LoadSprite(
			"./data/interface/2dimage/image/login_bar01.tif",
			1024.f,
			128.f);
	} */

	if(IMAGE_NODE* const imageNode = RESRCMGR->GetInfo(26)) // logo
	{
		mImageLogo->LoadSprite(
			imageNode->szFileName,
			imageNode->size.x,
			imageNode->size.y);
		mImageLogo->LoadSprite(
			"./data/interface/2dimage/image/logo6.tif",
			512.f,
			256.f);
	}

	if (IMAGE_NODE* const imageNode = RESRCMGR->GetInfo(26)) // logo
	{
		mImageLogoCompany->LoadSprite(
			"./data/interface/2dimage/image/logo4.tif",
			512.f,
			256.f);
	}

	ScriptCheckValue check;
	SetScriptCheckValue( check );

	if( check.mValue != g_Check.mValue )
	{
#ifdef _SAVEVERSION_
		// 090120 LUJ, ��ġ ������ ���ʷ� ������
		{
			const char* const	versionFile = SENC("LunaVerInfo.ver");
			FILE* const			file		= fopen( versionFile, "r+" );
			if( file )
			{
				// 090109 LUJ, 4���ڷ� ������ ���� ����� ���Ͽ��� �о�� ���ڿ� ������ ��´�
				const size_t headerSize = 4;
				char header[ headerSize + 1 ] = { 0 };
				fread(
					header,
					headerSize,
					sizeof( *header ),
					file );

				// 090109 LUJ, ��ü ��ġ�� ������ ���� ��ȣ�� �����
				char versionText[ MAX_PATH ] = { 0 };
				sprintf(
					versionText,
					"0");
				// 090109 LUJ, ��ġ ���� ���ڿ��� ���Ͽ� ����
				fseek(
					file,
					0,
					SEEK_SET );
				fwrite(
					versionText,
					sizeof( *versionText ),
					strlen( versionText ),
					file );
				fclose( file );
			}
		}

		PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		return FALSE;
#endif
	}

	m_pLogoWindow	= NULL;
	m_bInit			= FALSE;
	m_pServerListDlg = NULL;
	memset( m_DistributeAddr, 0, 16 );
	m_DistributePort = 0;
	//
	
	m_bDisconntinToDist = FALSE;
	m_bNoDiconMsg		= FALSE;	//KES ������ ������ ���������� �޼����� �ȶ������� ����
	m_bDynamicDlg = FALSE;


	NETWORK->SetCurState(this);

	//old login
	//if(MAP->IsInited() == FALSE)
	//{
	//if (!MapNumForCamera)
	//MAP->InitMap(200);
	//}

	//========== Dynamic Login Title ==========//
generatenum:
	MapNumFromCamera = mCameraConfiguration->MapIDList();
	if (iLastNum == MapNumFromCamera && MapNumFromCamera != 67)
	{
		//iLastNum = MapNumFromCamera;
		goto generatenum;
	}
	iLastNum = MapNumFromCamera;

	MAP->InitMap(MapNumFromCamera); //MapNumFromCamera
	
/*
	m_2DLoginImage->LoadSprite(
		"Data/Interface/2DImage/image/login.tga");
*/	
	if(g_bResourceLoaded == FALSE)
	{
		APPEARANCEMGR->Init();
		GAMERESRCMNGR->LoadServerList();
		
		g_bResourceLoaded = TRUE;
	}

	CAMERA->SetCameraMode( eCM_LOGIN );
	CAMERA->SetCurCamera( 0 );
	WINDOWMGR->SetcbDropProcess(cbDragDropProcess);

	CreateMainTitle_m();	
	m_pLogoWindow = WINDOWMGR->GetWindowForID( MT_LOGODLG );
	if( m_pLogoWindow )
		m_pLogoWindow->SetActive( TRUE );

#ifdef TAIWAN_LOCAL
	m_pAdvice = WINDOWMGR->GetWindowForID( CNA_CNADVICEDLG );
#endif

	m_pServerListDlg = (CServerListDialog*)WINDOWMGR->GetWindowForID( SL_SERVERLISTDLG );
	if( m_pServerListDlg )
	{
		m_pServerListDlg->Linking();
		m_pServerListDlg->SetActive( FALSE );
	}
//
	WINDOWMGR->AfterInit();

///	MHTIMEMGR->Process();
	m_bWaitConnectToAgent = FALSE;
	bFirst = TRUE;
	
	
#ifndef _LOGOWINDOW_
	m_pLogoWindow = NULL;	
	m_bServerList = TRUE;
#endif
	//========== Dynamic Login Title ==========//
	mCameraConfiguration->SetType(mCameraConfiguration->TypeLogin);
	mCameraConfiguration->Update();
	//=========================================//
	mCameraConfiguration->UpdateCamera();
	SetPositionScale();
	AdjustBar();
	// 100601 �α���â�� �����е��� �������� �ʿ� ���� //
//	AlignWindow();
#ifdef _GMTOOL_
	GMTOOLMGR->LogOut();
#endif

	return TRUE;
}

void CMainTitle::SetPositionScale()
{
	SHORT_RECT screenRect = {0};
	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&screenRect,
		&value,
		&value);

	const SIZE standardResolution = {1280, 1024};
	m_scale.x = float(screenRect.right) / standardResolution.cx;
	m_scale.y = float(screenRect.bottom) / standardResolution.cy;
	m_tr.x = 0;
	m_tr.y = 0;

	VECTOR2 imageSize = {0};
	m_2DLoginLoadingImage->GetImageOriginalSize(
		&imageSize);

	const float fixedScale = min(m_scale.x, m_scale.y);
	mScaleLoadingImage.x = fixedScale;
	mScaleLoadingImage.y = fixedScale;
	mPositionLoadingImage.x = (screenRect.right - standardResolution.cx * fixedScale) / 2;
	mPositionLoadingImage.y = 0;
}

void CMainTitle::AdjustBar()
{
	SHORT_RECT screenRect = {0};
	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&screenRect,
		&value,
		&value);

	if(cImageRect* const imageRect = mImageBar->GetImageRect())
	{
		mScaleBar.x = float(screenRect.right) / imageRect->right;
		mScaleBar.y = 1;
		mPositionBar.x = 0;
		mPositionBar.y = float(screenRect.top) - imageRect->top;
	}

	if (cImageRect* const imageRect = mImageBarBottom->GetImageRect())
	{
		mScaleBarBottom.x = float(screenRect.right) / imageRect->right;
		mScaleBarBottom.y = 1;
		mPositionBarBottom.x = 0;
		mPositionBarBottom.y = float(screenRect.bottom) - imageRect->bottom;
	}

	if(cImageRect* const imageRect = mImageLogo->GetImageRect())
	{
		mScaleLogo.x = 1;
		mScaleLogo.y = 1;
		mPositionLogo.x = float(screenRect.right) - imageRect->right * mScaleLogo.x;
		mPositionLogo.y = float(screenRect.bottom) - imageRect->bottom * mScaleLogo.y;
	}

	if (cImageRect* const imageRect = mImageLogoCompany->GetImageRect())
	{
		mScaleLogoCompany.x = 1;
		mScaleLogoCompany.y = 1;
		mPositionLogoCompany.x = float(screenRect.left) - imageRect->left * mScaleLogoCompany.x;
		mPositionLogoCompany.y = float(screenRect.top) - imageRect->top * mScaleLogoCompany.y;
	}
}

void CMainTitle::Release(CGameState* pNextGameState)
{
	m_2DLoginImage->Release();
	m_2DLoginLoadingImage->Release();
	mImageBar->Release();
	mImageLogo->Release();
	mImageBarBottom->Release();
	mImageLogoCompany->Release();
	WINDOWMGR->DestroyWindowAll();
	RESRCMGR->ReleaseResource(24);

	if(pNextGameState == NULL)
		MAP->Release();
}
#ifdef _TW_LOCAL_
void CMainTitle::Process()
{
#ifndef TAIWAN_LOCAL
	//trustpak 2005/04/01
//	ProcessMainTitleMonsters();
	///
#endif

	//�����̻� -_-
	if(bFirst == TRUE)	//�̷��� �ص� �����µ�?
	{
		bFirst = FALSE;
		CAMERA->MouseRotate(1,0);
		m_dwStartTime = MHTIMEMGR->GetNewCalcCurTime();
	}

	if( m_pLogoWindow )
	{
		DWORD dwElapsed = MHTIMEMGR->GetNewCalcCurTime() - m_dwStartTime;
		
		if( dwElapsed > ALPHA_PROCESS_TIME )
		{
			m_pLogoWindow->SetAlpha( 0 );
			WINDOWMGR->AddListDestroyWindow( m_pLogoWindow );
			m_pLogoWindow = NULL;
			
#ifdef TAIWAN_LOCAL
			m_pAdvice->SetActive( TRUE );
#else
			m_bServerList = TRUE;
#endif
		}
		else
		{
			m_pLogoWindow->SetAlpha( (BYTE)(255 - ( dwElapsed * 255 / ALPHA_PROCESS_TIME )) );
		}
	}

	if( m_bServerList )
	{
		m_pServerListDlg->SetActive( TRUE );
		m_bServerList = FALSE;
	}

	// Dist Server�� ���ӵǾ��ٸ�...
	if( m_bInit )
	{
		if( g_szHeroIDName[0] )

		{
			if(	!m_bWaitConnectToAgent )

			{
				ScriptCheckValue check;
				SetScriptCheckValue( check );

				if( check.mValue != g_Check.mValue )

				{
#ifdef _SAVEVERSION_
					// 090120 LUJ, ��ġ ������ ���ʷ� ������
					{
						const char* const	versionFile = SENC("LunaVerInfo.ver");
						FILE* const			file		= fopen( versionFile, "r+" );

						if( file )
						{
							// 090109 LUJ, 4���ڷ� ������ ���� ����� ���Ͽ��� �о�� ���ڿ� ������ ��´�
							const size_t headerSize = 4;
							char header[ headerSize + 1 ] = { 0 };
							fread(
								header,
								headerSize,
								sizeof( *header ),
								file );

							// 090109 LUJ, ��ü ��ġ�� ������ ���� ��ȣ�� �����
							char versionText[ MAX_PATH ] = { 0 };
							sprintf(
								versionText,
								"0");
							// 090109 LUJ, ��ġ ���� ���ڿ��� ���Ͽ� ����
							fseek(
								file,
								0,
								SEEK_SET );
							fwrite(
								versionText,
								sizeof( *versionText ),
								strlen( versionText ),
								file );
							fclose( file );
						}
					}

					PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
					return;
#endif

				}
				//aziz thai
				// Death Comment all dont resend Messsage Only want to show LoginDialog
				//MSG_LOGIN_SYN msg ;														// �α��� ������ ���� �޽��� ����ü�� �����Ѵ�.
				//memset(&msg, 0, sizeof(MSG_LOGIN_SYN)) ;								// �޽��� ����ü�� �ʱ�ȭ �Ѵ�.

				//msg.Category = MP_USERCONN ;											// ī�װ��� ���������� �����Ѵ�.
				//msg.Protocol = MP_USERCONN_LOGIN_SYN ;

				//SafeStrCpy(msg.id, g_szHeroIDName, 21 ) ;				// ���̵� ����.
				//SafeStrCpy(msg.pw, g_szHeroKey, 21 ) ;					// ��й�ȣ ����.

				//SafeStrCpy(msg.Version,g_CLIENTVERSION, 16 ) ;							// Ŭ���̾�Ʈ ���� ����.

				//msg.AuthKey = TITLE->GetDistAuthKey() ;									// ����Ű�� ��´�.
				//msg.Check.mValue = check.mValue;
				
				//[06-02-2012] Death Test to Don't Resend Login Message to Server
				//NETWORK->Send(&msg,sizeof(msg));										// �޽����� �����Ѵ�.
				cDialog* pDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
				if ( !pDlg->IsActive() )
				{
					pDlg->SetActive(true);
				}
				//enable auto focus edit
				cEditBox* editboxId = (cEditBox *)pDlg->GetWindowForID(MT_IDEDITBOX);
				cEditBox* editboxPwd = (cEditBox *)pDlg->GetWindowForID(MT_PWDEDITBOX);
				if (editboxId)
				{
					//editboxId->SetEditText("");
					//Alemuri Memorize ID after wrong Pass
					const char *cstr = previousID.c_str();
					editboxId->SetEditText((char*)cstr);
					//------------------------------------
					editboxId->SetFocusEdit(TRUE);
				}
				if (editboxPwd)
				{
					editboxPwd->SetEditText("");
					editboxPwd->SetFocusEdit(FALSE);
				}
				//---KES �α��� ������ �� �α����� �� �ȵǴ� �� ����
				//NETWORK->m_bDisconnecting = FALSE;
				//---------------------------------------------

				TITLE->StartWaitConnectToAgent( TRUE ) ;								// ������Ʈ ������ ��ٸ���.

				//USERINFOMGR->SetUserID( g_szHeroIDName ) ;								// ���� ���̵� �����Ѵ�.
			}
		}
		else
		{
			//���ӱ�ٸ��� ���̶��
			if( m_bWaitConnectToAgent == TRUE )
			{
				DWORD dwElapsed = gCurTime - m_dwWaitTime;

				if( dwElapsed > WAIT_CONNECT_TIME )
				{
					//��ҹ�ư�� ������ ó��
					cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_WAIT_LOGINCHECK );
					if( pMsgBox )
						pMsgBox->ForceClose();

					m_bWaitConnectToAgent = FALSE;

					// 070125 LYW --- MainTitle : Modified message number.
					//WINDOWMGR->MsgBox( MBI_TIMEOVER_TOCONNECT, MBT_OK, CHATMGR->GetChatMsg( 445 ) );
					WINDOWMGR->MsgBox( MBI_TIMEOVER_TOCONNECT, MBT_OK, CHATMGR->GetChatMsg( 302 ) );
					//��������Ʈ�� ������???
					m_pServerListDlg->SetActive( FALSE );
					m_bServerList = FALSE;

					// 080111 LYW --- MainTitle : ���� �е� ��Ȱ��ȭ ó��.
					cDialog* pDlg = NULL ;
					pDlg = WINDOWMGR->GetWindowForID( NUMBERPAD_DLG ) ;

					if( !pDlg ) return ;
					pDlg->SetActive(FALSE) ;
				}
			}
			else
			{
				if( m_bDynamicDlg == FALSE )
				{
					cDialog* pDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
					if( pDlg )
						if( !pDlg->IsActive() )
						{
							pDlg->SetDisable( FALSE );
							pDlg->SetActive( TRUE );
							cEditBox* pEdit = (cEditBox*)pDlg->GetWindowForID( MT_IDEDITBOX );
							pEdit->SetFocusEdit( TRUE );

							// 080109 LYW --- MainTitle : 2�� ��й�ȣ�� ���� �������̽� ���.
							cDialog* pPad = NULL ;
							pPad = WINDOWMGR->GetWindowForID( NUMBERPAD_DLG ) ;

							if( !pPad )
							{
								//MessageBox(NULL, "Failed to receive number pad.", "login", MB_OK) ;
							}
							else
							{
								pPad->SetActive(TRUE) ;
							}
						}	
				}
			}
		}
	}	
}
#else
void CMainTitle::Process()
{
	if(bFirst == TRUE)	//�̷��� �ص� �����µ�?
	{
		bFirst = FALSE;
		CAMERA->MouseRotate(1,0);
		m_dwStartTime = MHTIMEMGR->GetNewCalcCurTime();
	}

	if( m_pLogoWindow )
	{
		DWORD dwElapsed = MHTIMEMGR->GetNewCalcCurTime() - m_dwStartTime;
		
		if( dwElapsed > ALPHA_PROCESS_TIME )	
		{
			m_pLogoWindow->SetAlpha( 0 );
			WINDOWMGR->AddListDestroyWindow( m_pLogoWindow );
			m_pLogoWindow = NULL;
			
#ifdef TAIWAN_LOCAL
			m_pAdvice->SetActive( TRUE );
#else
			m_bServerList = TRUE;
#endif
		}
		else
		{
			m_pLogoWindow->SetAlpha( (BYTE)(255 - ( dwElapsed * 255 / ALPHA_PROCESS_TIME )) );
		}
	}

	if( m_bServerList )
	{
		m_pServerListDlg->SetActive( TRUE );
		m_bServerList = FALSE;
	}

	// Dist Server�� ���ӵǾ��ٸ�...
	if( m_bInit )
	{
		// 090930 ONS 2D�α��ν�, ���ӹ�ư�� ������ ���̵�/�н����� �Է� ���̾�α׿� �����е带 ��Ȱ��ȭ��Ų��.
		if( FALSE == m_bDynamicDlg && m_2DLoginLoadingImage->IsNull())
		{
			cDialog* pDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pDlg )
			if( !pDlg->IsActive() )
			{
				pDlg->SetDisable( FALSE );
				pDlg->SetActive( TRUE );
				cEditBox* pEdit = (cEditBox*)pDlg->GetWindowForID( MT_IDEDITBOX );
				pEdit->SetFocusEdit( TRUE );

			}	
		}

		//���ӱ�ٸ��� ���̶��
		if( m_bWaitConnectToAgent == TRUE )
		{
			DWORD dwElapsed = gCurTime - m_dwWaitTime;
		
			if( dwElapsed > WAIT_CONNECT_TIME )
			{
				//��ҹ�ư�� ������ ó��
				cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_WAIT_LOGINCHECK );
				if( pMsgBox )
					pMsgBox->ForceClose();

				m_bWaitConnectToAgent = FALSE;

				WINDOWMGR->MsgBox(
					MBI_TIMEOVER_TOCONNECT,
					MBT_OK,
					CHATMGR->GetChatMsg(302));

				m_pServerListDlg->SetActive( FALSE );
				m_bServerList = FALSE;

				m_2DLoginLoadingImage->Release();
			}
		}
	}
#ifdef _GMTOOL_
	mCameraConfiguration->Process();
#endif
}
#endif

void CMainTitle::BeforeRender()
{
	g_pExecutive->GetRenderer()->BeginRender(0,0,0);
	g_pExecutive->GetRenderer()->EndRender();
	MAP->Process( gCurTime );
}
void CMainTitle::AfterRender()
{
	// 090929 ONS 2D�α��� �̹����� �������ĸ� ������ �������Ѵ�.
	if(m_2DLoginLoadingImage->IsNull())
	{
		m_2DLoginImage->RenderSprite(
			&m_scale,
			0,
			0,
			&m_tr,
			0xffffffff);
		mImageBar->RenderSprite(
			&mScaleBar,
			0,
			0,
			&mPositionBar,
			0xffffffff);
		mImageBarBottom->RenderSprite(
			&mScaleBarBottom,
			0,
			0,
			&mPositionBarBottom,
			0xffffffff);
		mImageLogo->RenderSprite(
			&mScaleLogo,
			0,
			0,
			&mPositionLogo,
			0xffffffff);
		mImageLogoCompany->RenderSprite(
			&mScaleLogoCompany,
			0,
			0,
			&mPositionLogoCompany,
			0xffffffff);
	}
	else
	{
		m_2DLoginLoadingImage->RenderSprite(
			&mScaleLoadingImage,
			0,
			0,
			&mPositionLoadingImage,
			0xffffffff);
	}

	WINDOWMGR->Render();
}
void CMainTitle::NetworkMsgParse(BYTE Category,BYTE Protocol,void* pMsg, DWORD dwMsgSize)
{
	MSG_LOGIN_ACK* pmsg = (MSG_LOGIN_ACK*)pMsg;
	switch(Category)
	{
	case MP_USERCONN:
		{
			switch(Protocol) 
			{
			case MP_USERCONN_USE_DYNAMIC_ACK:
				{
					UserConn_Use_Dynamic_Ack( pMsg ) ;
				}
				break;
			case MP_USERCONN_USE_DYNAMIC_NACK:
				{
				}
				break;

			case MP_USERCONN_DIST_CONNECTSUCCESS:
				{
					UserConn_Dist_ConnectSuccess( pMsg ) ;
				}
				return;
			case MP_USERCONN_LOGIN_ACK: //Connection Success
				{
					if (pmsg->clientAllowed == 1)
					{
						if (pmsg->clientAllowed == 1)
						{
							UserConn_Login_Ack(pMsg);
						}
						else
						{
							UserConn_Server_NotReady() ;
							//__asm int 3
						}
						//UserConn_Login_Ack( pMsg ) ;
					}
					else
					{
						//__asm int 3
					}
					//UserConn_Login_Ack( pMsg ) ;
				}
				break;
			case MP_USERCONN_SERVER_NOTREADY:
				{
					UserConn_Server_NotReady() ;
				}
				break;

			case MP_USERCONN_LOGIN_NACK: //Connection Denied
				{
					UserConn_Login_Nack( pMsg ) ;
				}
				break;
			case MP_USERCONN_CHARACTERLIST_ACK:
				{
					UserConn_CharacterList_Ack( pMsg ) ;
				}
				return;
			case MP_USERCONN_CHARACTERLIST_NACK:
				{
					UserConn_CharacterList_Nack() ;
				}
				return;
			case MP_USERCONN_AGENT_CONNECTSUCCESS:
				{
					UserConn_Agent_ConnectSuccess( pMsg ) ;
				}
				break;

			// desc_hseos_��������01
			// S �������� �߰� added by hseos 2007.06.15
			case MP_USERCONN_USER_SEXKIND:
				{
					UserConn_Character_SexKind(pMsg);
				}
				return;
			// E �������� �߰� added by hseos 2007.06.15

			// 100503 ShinJS --- ����ó �α��ν� LoginID�� ���޹���
			//case MP_USERCONN_GETLOGINID_ACK:
			//	{
			//		MSG_NAME* pmsg = (MSG_NAME*)pMsg;
			//		SafeStrCpy( g_szHeroIDName, pmsg->Name, MAX_NAME_LENGTH );
			//	}
			//	break;

			}
		}
		break;
	case MP_CHEAT:
		{
			switch(Protocol) 
			{
				case MP_CHEAT_EVENTNOTIFY_ON:
				{
					Cheat_EventNotify_On( pMsg ) ;
				}
				break;
		
				case MP_CHEAT_EVENTNOTIFY_OFF:
				{
					Cheat_EventNotify_Off( pMsg ) ;
				}	
				break;
			}
		}
		break;		
	}
//	LOG(EC_UNKNOWN_PROTOCOL);
}



void CMainTitle::OnLoginError(DWORD errorcode,DWORD dwParam)
{
	//�������Դϴ� �޼����ڽ� �����
	cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_WAIT_LOGINCHECK );
	if( pMsgBox )
		WINDOWMGR->AddListDestroyWindow( pMsgBox );

	m_bWaitConnectToAgent = FALSE;

	ySWITCH(errorcode)
		yCASE(LOGIN_ERROR_INVALID_VERSION)
			Login_Error_Invalid_Version() ;
		yCASE(LOGIN_ERROR_OVERLAPPEDLOGIN)
			Login_Error_OverLappedLogin() ;			
		yCASE(LOGIN_ERROR_OVERLAPPEDLOGININOTHERSERVER)
			Login_Error_OverLappedLoginiNotherServer( dwParam ) ;
		yCASE(LOGIN_ERROR_NOAGENTSERVER)
			Login_Error_NoAgentServer() ;			
		yCASE(LOGIN_ERROR_NODISTRIBUTESERVER)
			Login_Error_NoDistributeServer() ;
		yCASE(LOGIN_ERROR_INVALIDUSERLEVEL)
			Login_Error_InvalidUserLevel() ;
		yCASE(LOGIN_ERROR_DISTSERVERISBUSY)
			Login_Error_DistServerIsBusy() ;
		yCASE(LOGIN_ERROR_WRONGIDPW)
			Login_Error_WrongIdPw() ;
		yCASE(LOGIN_ERROR_BLOCKUSERLEVEL)
			Login_Error_BlockUserLevel( dwParam ) ;			
		yCASE(LOGIN_ERROR_INVALID_IP)
			Login_Error_Invalid_Ip() ;
		yCASE(LOGIN_ERROR_DISTCONNET_ERROR)
			Login_Error_DistConnet_Error() ;
		yCASE(LOGIN_ERROR_MAXUSER)
			Login_Error_MaxUser() ;			
		yCASE(LOGIN_ERROR_MINOR_INADULTSERVER)
			Login_Error_Minor_InadultServer() ;
		yCASE(LOGIN_ERROR_SECEDEDACCOUNT)
			Login_Error_SecededAccount() ;			
		yCASE(LOGIN_ERROR_NOT_CLOSEBETAWINNER)
			Login_Error_Not_CloseBetaWinner() ;
		yCASE(LOGIN_ERROR_NOREMAINTIME)
			Login_Error_NoreMainTime() ;
		yCASE(LOGIN_ERROR_NOIPREGEN)
			Login_Error_NoIpRegen() ;	
		// 080111 LYW --- MainTitle : ���� ��й�ȣ ���� ���� ó�� �߰�.
		yCASE(LOGIN_ERROR_INVALID_SEC_PASS)
			Login_Error_Invalid_Sec_Pass() ;	
		yCASE(LOGIN_ERROR_CHANGE_SEC_PASS)
			Login_Error_Change_Sec_Pass() ;	
 		yCASE(LOGIN_ERROR_FILE_CRC)
 			Login_Error_File_CRC() ;
		yCASE(LOGIN_ERROR_LOGINSESSION_INVALID)
			Login_Error_LoginSession_Invalid();
	yENDSWITCH
}

void CMainTitle::OnDisconnect()
{
	if(m_bDisconntinToDist)		//������Ʈ�� ������ ���� ����
	{
		if(NETWORK->ConnectToServer(g_AgentAddr, g_AgentPort) == FALSE)
		{
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_VERCHKERROR, MBT_OK, CHATMGR->GetChatMsg(7));
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}
		}
		
		m_bDisconntinToDist = FALSE;
	}
	else if( !m_bNoDiconMsg )	//������ ���� ���� ������ �ƴϸ�
	{
		OnLoginError(LOGIN_ERROR_DISTCONNET_ERROR, 0);

		//�α���â �𽺿��̺� �߰�.
		cDialog* pDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
		if( pDlg )
		{
			pDlg->SetDisable( TRUE );
		}

		//KES �Ʒ��� (����쿡 ���Ͽ� ��ȣȭ ���� �ʿ�)
		//NETWORK->ReleaseKey();
	}

	//---KES Distribute Encrypt 071003
	NETWORK->ReleaseKey();
	//--------------------------------

	m_bNoDiconMsg = FALSE;
}

void CMainTitle::ConnectToServer( int index )
{
	if( index > -1 )
	{
		SEVERLIST* pServerList = GAMERESRCMNGR->m_ServerList;
		if( !pServerList[index].bEnter )	return;

		m_pServerListDlg->SetDisable( TRUE );
			
		if( NETWORK->ConnectToServer( pServerList[index].DistributeIP, pServerList[index].DistributePort ) == FALSE )
		{
			OnLoginError( LOGIN_ERROR_NODISTRIBUTESERVER, 0 );
		}

		m_pServerListDlg->SetDisable( FALSE );
		m_pServerListDlg->SetActive( FALSE );
		
		g_nServerSetNum = pServerList[index].ServerNo;
	}
	m_ConnectionServerNo = index;
} 

void CMainTitle::ShowServerList()
{
	m_bServerList = TRUE;

	cDialog* pDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pDlg )
	{
		pDlg->SetDisable( FALSE );
		pDlg->SetActive( FALSE );
	}
	pDlg = WINDOWMGR->GetWindowForID(MT_DYNAMICDLG);
	if( pDlg )
	{
		pDlg->SetDisable( FALSE );
		pDlg->SetActive( FALSE );
	}
	m_bDynamicDlg = FALSE;

	if( NETWORK->IsConnected() )
		NETWORK->Disconnect();
	m_bInit = FALSE;

	//---KES Distribute Encrypt 071003
	NETWORK->ReleaseKey();
	m_bWaitConnectToAgent = FALSE;
	//--------------------------------
}

void CMainTitle::StartWaitConnectToAgent( BOOL bStart )
{
	m_dwWaitTime			= gCurTime;
	m_bWaitConnectToAgent	= bStart;
}

void CMainTitle::UserConn_Use_Dynamic_Ack( void* pMsg )
{
	StartWaitConnectToAgent( FALSE );
	cDialog* pDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pDlg )
	{
		pDlg->SetDisable( FALSE );
		pDlg->SetActive( FALSE );
	}
	cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_WAIT_LOGINCHECK );
	if( pMsgBox )
	{
		pMsgBox->SetDisable( FALSE );
		pMsgBox->SetActive( FALSE );
	}

	MSG_USE_DYNAMIC_ACK* pmsg = (MSG_USE_DYNAMIC_ACK*)pMsg;
	m_bDynamicDlg = TRUE;
	pDlg = WINDOWMGR->GetWindowForID(MT_DYNAMICDLG);

	if( pDlg )
	{
		pDlg->SetDisable( FALSE );
		pDlg->SetActive( TRUE );

		cStatic* pStc = (cStatic*)pDlg->GetWindowForID( MT_STC_DYNAMICID );
		pStc->SetStaticText( g_szHeroIDName );
		pStc = (cStatic*)pDlg->GetWindowForID( MT_STC_COORD );
		pStc = (cStatic*)pDlg->GetWindowForID( MT_STC_COORD_MAT );
		pStc->SetStaticText( pmsg->mat );						
		((cEditBox*)pDlg->GetWindowForID(MT_DYNAMICPWDEDITBOX))->SetFocusEdit( TRUE );
		((cEditBox*)pDlg->GetWindowForID(MT_DYNAMICPWDEDITBOX))->SetEditText( "" );
	}	
}


//=================================================================================================
//	NAME		: UserConn_Dist_ConnectSuccess()
//	PURPOSE		: Process network message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::UserConn_Dist_ConnectSuccess( void* pMsg )
{
	//---KES Distribute Encryp 071003
	//MSGBASE* pmsg = (MSGBASE*)pMsg;
	MSG_DIST_CONNECTSUCCESS* pmsg = (MSG_DIST_CONNECTSUCCESS*)pMsg;
	m_DistAuthKey = pmsg->dwObjectID;
	NETWORK->SetKey( pmsg->eninit, pmsg->deinit );	//crypt
	//-------------------------------

	SetBlock(FALSE);

	m_bInit = TRUE;
}


//=================================================================================================
//	NAME		: UserConn_Login_Ack()
//	PURPOSE		: Process network message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::UserConn_Login_Ack( void* pMsg )
{
	MSG_LOGIN_ACK* pmsg = (MSG_LOGIN_ACK*)pMsg;

	//Extra check to avoid client in different servers
	if (pmsg->clientAllowed != 1)
	{
		//DO Nothing
	}
	else
	{
		strcpy(g_AgentAddr,pmsg->agentip);
		g_AgentPort = pmsg->agentport;
		gUserID	= pmsg->userIdx;
		MAINGAME->SetUserLevel( (int)pmsg->cbUserLevel );
#ifdef _GMTOOL_
		if( pmsg->cbUserLevel <= eUSERLEVEL_GM )
			GMTOOLMGR->CanUse( TRUE );
#endif

		USERINFOMGR->SetSaveFolderName( gUserID );

		m_bDisconntinToDist = TRUE;

		SendOutMsg();

		// 090930 ONS �α��� ���̾�α׿� �����е带 �ݴ´�.	
		cDialog* pDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
		if( pDlg )
		{
			pDlg->SetDisable( FALSE );
			pDlg->SetActive( FALSE );
		}
/*	  //old login
		m_2DLoginLoadingImage->LoadSprite(
			"Data/Interface/2DImage/image/loginloading.dds");
*/
#ifdef _NPROTECT_
		NPROTECTMGR->UserIDSend( USERINFOMGR->GetUserID() );
#endif
	}
}


//=================================================================================================
//	NAME		: UserConn_Server_NotReady()
//	PURPOSE		: Process network message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::UserConn_Server_NotReady()
{
	NoDisconMsg();
	OnLoginError( LOGIN_ERROR_NODISTRIBUTESERVER, 0 );
}


//=================================================================================================
//	NAME		: UserConn_Login_Nack()
//	PURPOSE		: Process network message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::UserConn_Login_Nack( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	OnLoginError(pmsg->dwData1,pmsg->dwData2);
}


//=================================================================================================
//	NAME		: UserConn_CharacterList_Ack()
//	PURPOSE		: Process network message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::UserConn_CharacterList_Ack( void* pMsg )
{
	SEND_CHARSELECT_INFO* pmsg = (SEND_CHARSELECT_INFO*)pMsg;
#ifdef _CRYPTCHECK_ 
	NETWORK->SetKey( pmsg->eninit, pmsg->deinit );
#endif

	if( NETWORK->IsConnected() )
	{
		MAINGAME->SetGameState(eGAMESTATE_CHARSELECT, (void *)pmsg, sizeof(SEND_CHARSELECT_INFO));
	}
}


//=================================================================================================
//	NAME		: UserConn_CharacterList_Nack()
//	PURPOSE		: Process network message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::UserConn_CharacterList_Nack()
{
	// �ɸ������� �޴µ� �����߽��ϴ�.
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(5));
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}
}

void CMainTitle::UserConn_Agent_ConnectSuccess( void* pMsg )
{
	MSG_DWORD2 msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_CHARACTERLIST_SYN;
	msg.dwData1 = gUserID;
	msg.dwData2 = m_DistAuthKey;
	NETWORK->Send(&msg,sizeof(msg));
}

//=================================================================================================
//	NAME		: Cheat_EventNotify_On()
//	PURPOSE		: Process network message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Cheat_EventNotify_On( void* pMsg )
{
	MSG_EVENTNOTIFY_ON* pmsg = (MSG_EVENTNOTIFY_ON*)pMsg;

	NOTIFYMGR->SetEventNotifyStr( pmsg->strTitle, pmsg->strContext );
	NOTIFYMGR->SetEventNotify( TRUE );

	NOTIFYMGR->SetEventNotifyChanged( TRUE );

	NOTIFYMGR->ResetEventApply();
	for(int i=0; i<eEvent_Max; ++i)
	{
		if( pmsg->EventList[i] )
			NOTIFYMGR->SetEventApply( i );
	}
}


//=================================================================================================
//	NAME		: Cheat_EventNotify_Off()
//	PURPOSE		: Process network message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Cheat_EventNotify_Off( void* pMsg )
{
	NOTIFYMGR->SetEventNotify( FALSE );
	NOTIFYMGR->SetEventNotifyChanged( FALSE );
}


//=================================================================================================
//	NAME		: Login_Error_Invalid_Version()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_Invalid_Version()
{
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(9) );
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}

	SendOutMsg();
	NoDisconMsg();
}


//=================================================================================================
//	NAME		: Login_Error_OverLappedLogin()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_OverLappedLogin()
{
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_OVERLAPPEDLOGIN, MBT_YESNO_EXIT_PROGRAM, CHATMGR->GetChatMsg(8) );
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}
}


//=================================================================================================
//	NAME		: Login_Error_OverLappedLoginiNotherServer()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_OverLappedLoginiNotherServer( DWORD dwParam )
{
	char serverName[128] = {0,};
	for(int n=0;n<30;++n)
	{
		if(GAMERESRCMNGR->m_ServerList[n].ServerNo == dwParam)
		{
			strcpy(serverName,GAMERESRCMNGR->m_ServerList[n].ServerName);
			break;
		}
	}
	//SEVERLIST* pServerList = GAMERESRCMNGR->m_ServerList;
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_OVERLAPPEDLOGININOTHERSERVER, MBT_OK, CHATMGR->GetChatMsg(1),
										serverName, serverName);
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}
	NoDisconMsg();
}


//=================================================================================================
//	NAME		: Login_Error_NoAgentServer()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_NoAgentServer()
{
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(7) );
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}
	
	SendOutMsg();
	NoDisconMsg();
}


//=================================================================================================
//	NAME		: Login_Error_NoDistributeServer()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_NoDistributeServer()
{
	WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(7) );
}


//=================================================================================================
//	NAME		: Login_Error_InvalidUserLevel()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_InvalidUserLevel()
{
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(6));
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}
	
	SendOutMsg();
	NoDisconMsg();
}


//=================================================================================================
//	NAME		: Login_Error_DistServerIsBusy()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_DistServerIsBusy()
{
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(304));
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}
	
	SendOutMsg();
	NoDisconMsg();
}


//=================================================================================================
//	NAME		: Login_Error_WrongIdPw()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_WrongIdPw()
{
	cEditBox* pEdit = (cEditBox*)WINDOWMGR->GetWindowForIDEx( MT_PWDEDITBOX );
	pEdit->SetFocusEdit( FALSE );
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_PASSERROR, MBT_OK, CHATMGR->GetChatMsg(52) );
	//id/pwâ ����¸� Ȥ�� �𽺿��̺��
	//�޽��� �ڽ� Ŭ���ÿ� id/pw����ְ� ��Ŀ�� id�� �ֱ�
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}
}


//=================================================================================================
//	NAME		: Login_Error_BlockUserLevel()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_BlockUserLevel( DWORD dwRemainTime )
{
	char buf[256] = {0,};
	if( dwRemainTime > 0 )
	{
		int hour = dwRemainTime / 60;
		int min = dwRemainTime - hour * 60;

		wsprintf( buf, CHATMGR->GetChatMsg( 1462 ), hour, min ) ;
	}
	else
	{
		wsprintf( buf, CHATMGR->GetChatMsg( 305 ) );
	}

	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK_EXIT_PROGRAM, buf );
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}

	SendOutMsg();
	NoDisconMsg();
}


//=================================================================================================
//	NAME		: Login_Error_Invalid_Ip()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_Invalid_Ip()
{
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_PASSERROR, MBT_OK_EXIT_PROGRAM, CHATMGR->GetChatMsg( 306 ) );
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}
}


//=================================================================================================
//	NAME		: Login_Error_DistConnet_Error()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_DistConnet_Error()
{
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_DISTCONNECT_ERROR, MBT_OK, CHATMGR->GetChatMsg(216) );
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}
}


//=================================================================================================
//	NAME		: Login_Error_MaxUser()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_MaxUser()
{
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(307) );
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}
}


//=================================================================================================
//	NAME		: Login_Error_Minor_InadultServer()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_Minor_InadultServer()
{
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK_EXIT_PROGRAM, CHATMGR->GetChatMsg(308) );
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}
}


//=================================================================================================
//	NAME		: Login_Error_SecededAccount()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_SecededAccount()
{
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK_EXIT_PROGRAM, CHATMGR->GetChatMsg(309) );
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}
	
	SendOutMsg();
	NoDisconMsg();
}


//=================================================================================================
//	NAME		: Login_Error_Not_CloseBetaWinner()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_Not_CloseBetaWinner()
{
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK_EXIT_PROGRAM, CHATMGR->GetChatMsg(310) );
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}

	SendOutMsg();
	NoDisconMsg();
}


//=================================================================================================
//	NAME		: Login_Error_NoreMainTime()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_NoreMainTime()
{
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK_EXIT_PROGRAM, "You have not remain time, please recharge ASAP" );

	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}

	SendOutMsg();
	NoDisconMsg();
}


//=================================================================================================
//	NAME		: Login_Error_NoIpRegen()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_NoIpRegen()
{
	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK_EXIT_PROGRAM, CHATMGR->GetChatMsg(311) );
	cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pIDDlg && pMsgBox )
	{
		pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	}

	SendOutMsg();
	NoDisconMsg();
}

// desc_hseos_��������01
// S �������� �߰� added by hseos 2007.06.16
void CMainTitle::UserConn_Character_SexKind(void *pMsg)
{
	MSG_DWORD* pPacket = (MSG_DWORD*)pMsg;

	CHARMAKEMGR->SetCharSexKindFromDB(pPacket->dwData);
}
// E �������� �߰� added by hseos 2007.06.16






//=================================================================================================
//	NAME		: Login_Error_Invalid_Sec_Pass()
//	PURPOSE		: Process error message.
//	DATE		: January 11 2008 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_Invalid_Sec_Pass()
{
	cMsgBox* pMsgBox = NULL ;
	pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(1265) ) ;

	if( !pMsgBox ) return ;

	cDialog* pIDDlg = NULL ;
	pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG ) ;

	if( !pIDDlg ) return ;

	pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() ) ;

	SendOutMsg() ;
	NoDisconMsg() ;
}





//=================================================================================================
//	NAME		: Login_Error_Change_Sec_Pass()
//	PURPOSE		: Process error message.
//	DATE		: January 11 2008 LYW
//	ATTENTION	: 
//=================================================================================================
void CMainTitle::Login_Error_Change_Sec_Pass()
{	
	cMsgBox* pMsgBox = NULL ;
	pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK_EXIT_PROGRAM, CHATMGR->GetChatMsg(1266) ) ;

	if( !pMsgBox ) return ;

	cDialog* pIDDlg = NULL ;
	pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG ) ;

	if( !pIDDlg ) return ;

	pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() ) ;

	SendOutMsg() ;
	NoDisconMsg() ;
}

void CMainTitle::Login_Error_File_CRC()
{
#ifdef _SAVEVERSION_
	 // 090120 LUJ, ��ġ ������ ���ʷ� ������
	 {
		 const char* const	versionFile = SENC("LunaVerInfo.ver");
		 FILE* const			file		= fopen( versionFile, "r+" );

		 if( file )
		 {
			 // 090109 LUJ, 4���ڷ� ������ ���� ����� ���Ͽ��� �о�� ���ڿ� ������ ��´�
			 const size_t headerSize = 4;
			 char header[ headerSize + 1 ] = { 0 };
			 fread(
				 header,
				 headerSize,
				 sizeof( *header ),
				 file );

			 // 090109 LUJ, ��ü ��ġ�� ������ ���� ��ȣ�� �����

			 char versionText[ MAX_PATH ] = { 0 };
			 sprintf(
				 versionText,
				 "0");
			 // 090109 LUJ, ��ġ ���� ���ڿ��� ���Ͽ� ����
			 fseek(
				 file,
				 0,
				 SEEK_SET );
			 fwrite(
				 versionText,
				 sizeof( *versionText ),
				 strlen( versionText ),
				 file );
			 fclose( file );
		 }
	 }

	 cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg( 9 ) );
	 cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	 if( pIDDlg && pMsgBox )
	 {
		 pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
	 }

	 SendOutMsg();
	 NoDisconMsg();
#endif
}

void CMainTitle::Login_Error_LoginSession_Invalid()
{
	cMsgBox* pMsgBox = NULL ;
	pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg( 2221 ) ); // "�α��� ������ ������ �ֽ��ϴ�. �ٽ� �α������ֽʽÿ�"
	if( !pMsgBox ) return ;

	cDialog* pIDDlg = NULL ;
	pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG ) ;
	if( !pIDDlg ) return ;

	pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() ) ;

	SendOutMsg() ;
	NoDisconMsg() ;
}
 
void CMainTitle::AlignWindow()
{
	RECT numpadRect = {0};
	cWindow* const numpadWindow = WINDOWMGR->GetWindowForID(
		NUMBERPAD_DLG);

	if(numpadWindow)
	{
		numpadRect.left = numpadWindow->GetAbsX();
		numpadRect.top = numpadWindow->GetAbsY();
		numpadRect.right = numpadWindow->GetAbsX() + numpadWindow->GetWidth();
		numpadRect.bottom = numpadWindow->GetAbsY() + numpadWindow->GetHeight();
	}

	RECT loginRect = {0};
	cWindow* const idPassDialog = WINDOWMGR->GetWindowForID(
		MT_LOGINDLG);

	if(idPassDialog)
	{
		loginRect.left = idPassDialog->GetAbsX();
		loginRect.top = idPassDialog->GetAbsY();
		loginRect.right = idPassDialog->GetAbsX() + idPassDialog->GetWidth();
		loginRect.bottom = idPassDialog->GetAbsY() + idPassDialog->GetHeight();
	}

	RECT rect = {0};

	if(FALSE == IntersectRect(
		&rect,
		&numpadRect,
		&loginRect))
	{
		return;
	}

	idPassDialog->SetAbsXY(
		idPassDialog->GetAbsX(),
		numpadWindow->GetAbsY() - idPassDialog->GetHeight());
}

