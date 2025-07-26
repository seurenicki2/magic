#include "stdafx.h"
#include "MainGame.h"
#include "MainTitle.h"
#include "ServerConnect.h"
#include "CharSelect.h"
#include "CharMake.h"
#include "GameIn.h"
#include "MapChange.h"
#include "MHTimeManager.h"
#include "Engine/Engine.h"
#include "Interface/cResourceManager.h"
#include "Interface/cWindowManager.h"
#include "ConductManager.h"
#include "FilteringTable.h"
#include "cMonsterSpeechManager.h"
#include "ItemManager.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[CC]Header/GameResourceManager.h"
#include "AppearanceManager.h"
#include "ObjectManager.h"
#include "OptionManager.h"
#include "MovePoint.h"
#include "Input/cIMEWnd.h"
#include "Audio/MHAudioManager.h"
#include "MHMap.h"
#include "Input/UserInput.h"
#include "MacroManager.h"
#include "NpcScriptManager.h"
#include "CheatMsgparser.h"
#include "MouseCursor.h"
#include "ChatManager.h"
#include "cChatTipManager.h"
#include "PetManager.h"
#ifdef _GMTOOL_
#include "GMToolManager.h"
#endif
#ifdef _TESTCLIENT_
#include "TSToolManager.h"
#endif
#include "GuildManager.h"
#include "MoveManager.h"
#include "MiniDumper.h"
#include "Engine/EngineObject.h"
#include "Engine/EngineObjectCache.h"
#include "../hseos/SHMain.h"
#include "BalloonOutline.h"
#include "TutorialManager.h"
#include "SiegeWarfareMgr.h"
#include "NpcRecallMgr.h"
#include "UiSettingManager.h"
#include "VideoCaptureManager.h"
#include "CameraConfiguration.h"
GLOBALTON(CMainGame);
CMainGame* pTempMainGame = NULL;
HWND gHwnd = NULL;
volatile LONG threadBlock = 0;

CMainGame::CMainGame()
{
	m_bChangeState = FALSE;
	m_ChangeState = eGAMESTATE_NONE;
	m_pCurrentGameState = NULL;
	m_pEngine = NULL;
	m_bEndGame = FALSE;
	m_pStateInitParam = NULL;

	pTempMainGame = this;
	m_nUserLevel = eUSERLEVEL_USER;	
}

CMainGame::~CMainGame()
{}

void CMainGame::Init(HWND hMainWnd)
{
	srand( GetTickCount() );

#ifndef _HACK_SHIELD_
	DIRECTORYMGR->Init();
#endif

	DIRECTORYMGR->SetLoadMode(eLM_EffectScript);
	LoadEffectFileTable("list_h_m.befl");
	LoadEffectFileTable("list_h_w.befl");
	LoadEffectFileTable("list_e_m.befl");
	LoadEffectFileTable("list_e_w.befl");
	// 090624 ONS �ű����� ����Ʈ ó�� �߰�
	LoadEffectFileTable("list_d_m.befl");
	LoadEffectFileTable("list_d_w.befl");
	DIRECTORYMGR->SetLoadMode(eLM_Root);

	m_pEngine = new CEngine;
	m_pEngine->Init(hMainWnd,&GAMERESRCMNGR->m_GameDesc.dispInfo,
		GAMERESRCMNGR->m_GameDesc.MaxShadowNum,GAMERESRCMNGR->m_GameDesc.MaxShadowTexDetail,0,
		GAMERESRCMNGR->m_GameDesc.FramePerSec);

	// 080507 LUJ, ���� ���Ͽ��� ���ҽ� ���� �̸��� �о���δ�
	{
		char fileName[ MAX_PATH ] = { 0 };
		{
			const char* defaultFile = "data\\interface\\windows\\image_path.bin";

			strcpy( fileName, defaultFile );
		}

		FILE* file = fopen( "system\\launcher.sav", "r" );

		if( file )
		{
			char		buffer[ MAX_PATH ]	= { 0 };
			const char* separator			= " =\n";

			while( fgets( buffer, sizeof( buffer ), file ) )
			{
				const char* token = strtok( buffer, separator );

				if( 0 == token ||	// empty line
					';' == token[ 0 ] )	// comment
				{
					continue;
				}
				else if( ! strcmpi( "imagePath",  token ) )
				{
					token = strtok( 0, separator );

					if( token &&
						fopen( token, "r" ) )
					{
						//const char* defaultFile = "data\\interface\\windows\\image_path.bin";
						strcpy(fileName, token); //strcpy(fileName, defaultFile);//token
					}
				}
			}
		}

		fclose( file );

		RESRCMGR->Init( fileName, FILE_IMAGE_MSG );
	}

	NETWORK->Init(hMainWnd);
	WINDOWMGR->Init();

	gHwnd = hMainWnd;
	OPTIONMGR->Init();
	OPTIONMGR->ApplySettings();
	MACROMGR->Init();
	AUDIOMGR->Initialize(
		hMainWnd);
	AUDIOMGR->PlayBGM(
		1);
	// Ʃ�丮�� �Ŵ�����, ����, npc ���� ���ҽ����� ��� �ε� �� �� �ε��ϵ��� �Ѵ�. ��!
	TUTORIALMGR->Init();	
	MOVEPOINT->Init();
	g_UserInput.Init();
	GUILDMGR->Init();
	CHATTIPMGR->Init();
	SIEGEWARFAREMGR->Init();
	NPCRECALLMGR->Initialize();

	g_pEngineObjectCache = new CEngineObjectCache;
	g_pEngineObjectCache->Init(g_pExecutive, CEngineObjectCache::MAX_ENGINE_OBJECT_CACHE_SIZE);

	// Ŭ���̾�Ʈ�� �̴ϴ����� �����ϵ��� _DEBUG ����. by hseos 2007.06.24 �ϴ� ���� �뿡���� �̴ϴ��� ���X
#ifdef _DEBUG
	MiniDumper md(MiniDumper::DUMP_LEVEL_0);
#else if _GMTOOL_
	MiniDumper md(MiniDumper::DUMP_LEVEL_0);
#endif

	BALLOONOUTLINE->InitChatBalloon();
}



void CMainGame::Release()
{
	EFFECTMGR->Release();

	MOVEPOINT->Release();

	if(m_pCurrentGameState)
	{
		m_pCurrentGameState->Release(NULL);
		m_pCurrentGameState = NULL;
	}

	CHATTIPMGR->Release();
	UISETTINGMGR->SaveSettingInfo();
	WINDOWMGR->Release();
	RESRCMGR->Release();
	NETWORK->Release();

	SAFE_DELETE_ARRAY(m_pStateInitParam);

	AUDIOMGR->Terminate();
	g_UserInput.Release();
	CURSOR->Release();
	GUILDMGR->Release();

	SAFE_RELEASE(
		g_pEngineObjectCache);
	SAFE_RELEASE(
		m_pEngine);
}

void CMainGame::SetGameState(eGAMESTATE StateNum, LPVOID pStateInitParam,int ParamLen)
{
	m_bChangeState = TRUE;
	m_ChangeState = StateNum;
	if(m_ChangeState == eGAMESTATE_END)
	{
		m_bEndGame = TRUE;
#ifdef _GMTOOL_
		GMTOOLMGR->DestroyGMDialog();
#elif _TESTCLIENT_
		TSTOOLMGR->DestroyTSDialog();
#endif
	}

	if(pStateInitParam && ParamLen)
	{ 
		m_pStateInitParam = new char[ParamLen];
		memcpy(m_pStateInitParam,pStateInitParam,ParamLen);
	}
}

extern BOOL g_bActiveApp;
int CMainGame::Process()
{
	if(g_bActiveApp == FALSE)
		Sleep( 10 );

	Sleep(1);		//������ ���ִ� ���콺�� �� �ȿ����̳�.. 1�̶� ����.

	if(m_bEndGame)
	{
		if(m_pCurrentGameState)
		{
			m_pCurrentGameState->Release(GetGameState(m_ChangeState));
			m_pCurrentGameState->SetBlock(FALSE);
			m_pCurrentGameState = NULL;
		}
		Release();
		return -1;
	}

	BeforeRenderFunc();
	BOOL rt = m_pEngine->BeginProcess(NULL,AfterRenderFunc);	//���ϰ��� ������ �������̴�. --;
	if(rt)
	{
		if(m_pCurrentGameState)
		{
			if(m_pCurrentGameState->IsBlock() == FALSE)
			{
				MHTIMEMGR->Process();
				g_UserInput.Process();
				WINDOWMGR->Process();
				
				m_pCurrentGameState->Process();
			}

			AUDIOMGR->Process();
		}
	}

	//YH2DO �ӽ� �ϵ��ڵ�
	if(m_pCurrentGameState == GAMEIN && GAMEIN->IsGameInAcked() == FALSE)
	{
		g_pExecutive->GetRenderer()->BeginRender(0,0,0);
		g_pExecutive->GetRenderer()->EndRender();
	}

	// 091201 ShinJS --- ������ Capture �߰��۾�
	VIDEOCAPTUREMGR->Process();

	m_pEngine->EndProcess();
	
	if(m_bChangeState)
	{
		if(m_pCurrentGameState)
		{
			m_pCurrentGameState->Release(GetGameState(m_ChangeState));
			m_pCurrentGameState->SetBlock(FALSE);
		}
		
		m_pCurrentGameState = GetGameState(m_ChangeState);

		if(!m_pCurrentGameState)		// ����
		{
			Release(); //KES 030827 �߰�
//			GAMEIN->ReleaseForGame();

			return -1;
		}

		m_nCurStateNum = m_ChangeState; //KES 030827 �߰�
		CIMEWND->SetDockingEx( NULL, TRUE ); //KES �߰�.
	}

	// ���μ��� �߿� GXObject�� ����ų� �ϸ� ������ ���⶧���� ���⿡ �־� ��
	// ����� ���� �ٲ��� ����!!!
	APPEARANCEMGR->ProcessReservation();
	APPEARANCEMGR->ProcessAlphaAppearance();	
	APPEARANCEMGR->ProcessAppearance();
	g_pEngineObjectCache->ProcessCacheDelete();

	if(m_bChangeState)
	{
		m_pCurrentGameState->SetBlock(FALSE);
		if( m_pCurrentGameState->Init(m_pStateInitParam) == FALSE )
		{
			if( m_nCurStateNum == eGAMESTATE_TITLE )	//maintitle���ӺҴ�(distribute������)
			{
				m_pCurrentGameState->SetBlock( FALSE );
				TITLE->OnLoginError( LOGIN_ERROR_NODISTRIBUTESERVER, 0 );
			}
			else
			{
				MessageBox( 0, CHATMGR->GetChatMsg(444), 0, 0 );
				SAFE_DELETE_ARRAY(m_pStateInitParam);
				Release();
				return -1;				
			}
		}

		SAFE_DELETE_ARRAY(m_pStateInitParam);

		m_bChangeState = FALSE;
	}
	
	return 0;

}

void CMainGame::BeforeRender()
{
	if(m_pCurrentGameState)
		m_pCurrentGameState->BeforeRender();
}


void CMainGame::AfterRender()
{
	if(m_pCurrentGameState)
		m_pCurrentGameState->AfterRender();
}


void CMainGame::PauseRender( BOOL bPause )
{
	m_pEngine->PauseRender( bPause);
}

CGameState* CMainGame::GetGameState(int StateNum)
{
	CGameState* pState=NULL;
	switch(StateNum) 
	{
	case eGAMESTATE_CONNECT:
		pState = SERVERCONNECT;
		break;
	case eGAMESTATE_TITLE:
		pState = TITLE;
		break;
	case eGAMESTATE_CHARSELECT:
		pState = CHARSELECT;
		break;
	case eGAMESTATE_CHARMAKE:
		pState = CHARMAKE;
		break;
	case eGAMESTATE_GAMEIN:
		pState = GAMEIN;
		break;
	case eGAMESTATE_MAPCHANGE:
		pState = MAPCHANGE;
		break;
	case eGAMESTATE_END:
		pState = NULL;
		break; 
	default:
		break;
	}
	return pState;
}

void CMainGame::LoadPack( LPCTSTR fileName )
{
	GetEngine()->GetGraphicsEngine()->LoadPack(
		fileName);
}

DWORD _stdcall BeforeRenderFunc()
{
	pTempMainGame->BeforeRender();
	return 0;
}

DWORD _stdcall AfterRenderFunc()
{
	pTempMainGame->AfterRender();
	return 0;
}
DWORD _stdcall AfterRenderFunc__()
{
	WINDOWMGR->Render();
	return 0;
}