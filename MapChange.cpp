// MapChange.cpp: implementation of the CMapChange class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapChange.h"

#include "MainGame.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cResourceManager.h"
#include "cWindowSystemFunc.h"
#include "MHMap.h"
#include "Input/Mouse.h"
#include "Input/Keyboard.h"
#include "Interface/cScriptManager.h"
#include "Input/UserInput.h"
#include "Effect/EffectManager.h"
#include "MotionManager.h"
#include "UserInfoManager.h"
#include "ChatManager.h"
#include "GMNotifyManager.h"
#include "GuildTournamentMgr.h"
#include "cFont.h"
#include "cImageSelf.h"
#include "cImage.h"

#include "GameResourceManager.h" //Alemuri Map Change With Image

extern HWND _g_hWnd;

GLOBALTON(CMapChange)
CMapChange::CMapChange() :
mImageBar(new cImage),
m_MapNum(0),
m_GameInInitKind(eGameInInitKind_Login),
mIsNoResourceLoaded(TRUE)
{
	const VECTOR2 emptyVector = {0};
	mImageTipPosition = emptyVector;
	mImageTipScale = emptyVector;
	mImageBlackScreenPosition = emptyVector;
	mImageBlackScreenScale = emptyVector;
	mImageBarScale = emptyVector;
	mImageBarPosition = emptyVector;
	ZeroMemory(
		&mTip,
		sizeof(mTip));

	LoadTipText();
	LoadTipImage();
}

CMapChange::~CMapChange()
{
	SAFE_DELETE(mImageBar);
	SAFE_DELETE(mImageGradeMark);

	while(false == mTipImageQueue.empty())
	{
		TipImagePair imagePair = mTipImageQueue.front();
		SAFE_DELETE(imagePair.second);
		mTipImageQueue.pop();
	}
}

void CMapChange::LoadTipText()
{
	CMHFile file;
	file.Init(
		"Data/Interface/Windows/LoadingMsg.bin",
		"rb");

	while(FALSE == file.IsEOF())
	{
		TCHAR buffer[MAX_PATH] = {0};
		file.GetLine(
			buffer,
			sizeof(buffer) / sizeof(*buffer));

		LPCTSTR seperator = _T("\"\t");
		LPCTSTR textIndex = _tcstok(
			buffer,
			seperator);
		LPCTSTR textTip = _tcstok(
			0,
			seperator);

		if(0 == textIndex)
		{
			continue;
		}
		else if(0 == textTip)
		{
			continue;
		}

		mUnusedTipSet.insert(textTip);
	}
}

void CMapChange::LoadTipImage()
{
	CMHFile file;
	file.Init(
		"Data/Interface/Windows/LoadingMsg.bin",
		"rb");

	while(FALSE == file.IsEOF())
	{
		TCHAR buffer[MAX_PATH] = {0};
		file.GetLine(
			buffer,
			sizeof(buffer) / sizeof(*buffer));

		LPCTSTR seperator = _T("\"\t");
		LPCTSTR textImageSize = _tcstok(
			buffer,
			seperator);

		if(0 == textImageSize)
		{
			continue;
		}

		for(int i = _ttoi(textImageSize); 0 < i; --i)
		{
			mTipImageQueue.push(
				TipImagePair(i, 0));
		}
		
		break;
	}
}

void CMapChange::GetTipText(LPTSTR textTip, size_t size)
{
	//Alemuri Map Change With Image (Commenting this to remove tip)-------
	//ZeroMemory(
	//	textTip,
	//	size);

	//if(mUnusedTipSet.empty())
	//{
	//	if(mUsedTipSet.empty())
	//	{
	//		return;
	//	}

	//	mUnusedTipSet = mUsedTipSet;
	//	mUsedTipSet.clear();
	//}

	//TipSet::iterator iterator = mUnusedTipSet.begin();
	//std::advance(
	//	iterator,
	//	rand() % mUnusedTipSet.size());
	//const std::string& tip = *iterator;

	//SafeStrCpy(
	//	textTip,
	//	tip.c_str(),
	//	size);
	//mUsedTipSet.insert(tip);
	//mUnusedTipSet.erase(iterator);
	//-------------------------------------------------------------------
}

cImageSelf* CMapChange::GetTipImage()
{
	//Alemuri Map Change With Image----------------------------------------------------------
	char imagePath[100];
	_stprintf(imagePath, "Data/Interface/2DImage/image/maploadingimage%02d.tga", m_MapNum);

	SHORT_RECT screenRect = {0};
	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&screenRect,
		&value,
		&value);
	//---------------------------------------------------------------------------------------
	if(mTipImageQueue.empty())
	{
		return 0;
	}

	TipImagePair tipImagePair = mTipImageQueue.front();

	if(0 == tipImagePair.second)
	{
		//Alemuri Map Change With Image----------------------------------------------------------
		if (strcmp(imagePath, "") != 0)
		{
			tipImagePair.second = new cImageSelf;
			//const DISPLAY_INFO& screen	= GAMERESRCMNGR->m_GameDesc.dispInfo;
			const SIZE imageSize = {1024, 768};
			//const SIZE imageSize = {screen.dwWidth, screen.dwHeight};

			TCHAR path[MAX_PATH] = {0};
			strcpy(path, imagePath);

			if(FALSE == tipImagePair.second->LoadSprite(
				path,
				imageSize.cx,
				imageSize.cy))
			{
				tipImagePair.second->LoadSprite(
					"Data/Interface/2DImage/image/maploadingimage01.tga",
					imageSize.cx,
					imageSize.cy);
			}
		//---------------------------------------------------------------------------------------
		}
		else
		{
			tipImagePair.second = new cImageSelf;

			const SIZE imageSize = {1280, 1024};

			TCHAR path[MAX_PATH] = {0};
			_stprintf(
				path,
				"Data/Interface/2DImage/image/maploadingimage%02d.tga",
				tipImagePair.first);

			if(FALSE == tipImagePair.second->LoadSprite(
				path,
				imageSize.cx,
				imageSize.cy))
			{
				tipImagePair.second->LoadSprite(
					"Data/Interface/2DImage/image/maploadingimage01.tga",
					imageSize.cx,
					imageSize.cy);
			}
		}
	}

	mTipImageQueue.pop();
	mTipImageQueue.push(tipImagePair);

	return tipImagePair.second;
}

BOOL CMapChange::Init(void* pInitParam)
{
	m_MapNum = *((MAPTYPE*)pInitParam);
	
	GetTipText(
		mTip.mText,
		sizeof(mTip.mText) / sizeof(*mTip.mText));
	mImageTip = GetTipImage();

	const RECT rectGradeMark = {0, 0, 477, 278};
	mImageGradeMark = new cImageSelf;

	//Alemuri Map Change With Image (Comment this to remove grade_mark image)-----------
	//mImageGradeMark->LoadSprite(
	//	"Data/Interface/2DImage/image/grade_mark.tif",
	//	rectGradeMark.right,
	//	rectGradeMark.bottom);
	//mImageGradeMark->SetImageSrcRect(
	//	&rectGradeMark);
	//----------------------------------------------------------------------------------

	CreateGameLoading_m();
	WINDOWMGR->AfterInit();
	NETWORK->SetCurState(this);	
	g_UserInput.SetInputFocus(FALSE);
	USERINFOMGR->SetMapChage(FALSE);
	SetPositionScale();

	cImageRect rt = {951, 217, 952, 256};
	SCRIPTMGR->GetImage(
		4,
		mImageBar,
		&rt);

	AfterRender();

	// 091221 LUJ, �� �̵��� ���� ���, �� ������ �����Ǳ� ����
	//			�������� �ʿ� ���� ����(���� ��� ī�޶� ����)��
	//			���޵ǰ�, �� ������ ������ ���� Ŭ���̾�Ʈ�� ����
	//			���� �ȴ�. �̸� ���� ���� �ٷ� �ʱ�ȭ�ϵ��� �Ѵ�
	MAP->InitMap(m_MapNum);

	if(mIsNoResourceLoaded)
	{
		EFFECTMGR->Init();
		MOTIONMGR->LoadMotionList();

		mIsNoResourceLoaded = FALSE;
	}

	return TRUE;
}


// 070208 LYW --- MapChange : Add function to setting position and scale of image.
void CMapChange::SetPositionScale()
{
	SHORT_RECT screenRect = {0};
	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&screenRect,
		&value,
		&value);

	//const SIZE standardResolution = {1280, 1024};
	const SIZE standardResolution = {1024, 768};
	
	//Alemuri----------------------------------------------------------------------
	//const DISPLAY_INFO& screen	= GAMERESRCMNGR->m_GameDesc.dispInfo;
	//const SIZE standardResolution = {screen.dwWidth, screen.dwHeight};
	//const SIZE standardResolution = {1024, 768};
	//-----------------------------------------------------------------------------

	const float ratioWidth = float(screenRect.right) / standardResolution.cx;
	const float ratioHeight = float(screenRect.bottom) / standardResolution.cy;
	float fixedScale = min(ratioWidth, ratioHeight);
	
	// 091221 LUJ, ǥ�� �ػ��� ��� �� �̹����� ȭ�鿡 ���� ���� ǥ���Ѵ�
	switch(screenRect.right)
	{
	case 800:
	case 1024:
	case 1280:
		{
			fixedScale = max(ratioWidth, ratioHeight);
			break;
		}
	}

	mImageBarScale.x = float(screenRect.right);
	mImageBarScale.y = 1;

	mImageBarPosition.x = 0;
	mImageBarPosition.y = float(screenRect.bottom) * 0.8f;

	mImageTipScale.x = fixedScale;
	mImageTipScale.y = fixedScale;
	mImageTipPosition.x = (screenRect.right - standardResolution.cx * fixedScale) / 2;
	mImageTipPosition.y = 0;

	mImageBlackScreenScale.x = ratioWidth;
	mImageBlackScreenScale.y = ratioHeight;

	// 091221 LUJ, ��Ʈ�� X��ǥ�� 0���� ���� ���, ����� ���� �ʴ´�
	//			�̸� ���� ����, ��Ʈ ũ�⸦ �۰� �ϸ鼭 �����
	const LONG length = CFONT_OBJ->GetTextExtentEx(
		8,
		mTip.mText,
		_tcslen(mTip.mText));
	mTip.mRect.left = (LONG(screenRect.right) - length) / 2;
	mTip.mRect.right = (LONG(screenRect.right) + length) / 2;
	mTip.mRect.top = LONG(mImageBarPosition.y) + 10;
	mTip.mRect.bottom = mTip.mRect.top + 39;
	mTip.mFont = cFont::FONT8;

	if(0 > mTip.mRect.left)
	{
		mTip.mRect.left = 0;
		mTip.mRect.right = screenRect.right;
	}

	// ��� ��� �̹��� ��ġ/ũ�� ����
	{
		mImageGradeMarkScale.x = mImageGradeMarkScale.y = ratioHeight;

		RECT rectGradeMark = *mImageGradeMark->GetImageRect();
		const float gapForGradeMark = 10.0f;
		mImageGradeMarkPosition.x = screenRect.right - mImageTipPosition.x - (rectGradeMark.right * mImageGradeMarkScale.x) - gapForGradeMark;
		mImageGradeMarkPosition.y = gapForGradeMark;
	}
}


void CMapChange::Release(CGameState* pNextGameState)
{
	WINDOWMGR->DestroyWindowAll();
	RESRCMGR->ReleaseResource(28);	//28:�ε��̹������̾�
}

void CMapChange::Process()
{
	if(MAPTYPE(-1) == m_MapNum)
	{
		return;
	}

//	MAINGAME->LoadPack(
//		"data/3dData/effect.pak");
//	MAINGAME->LoadPack(
//		"data/3dData/monster.pak");
//	MAINGAME->LoadPack(
//		"data/3dData/npc.pak");
	//aziz NPC Image remove useless
	//MAINGAME->LoadPack(
	//	"data/interface/2dImage/npcImage.pak");

	MAINGAME->SetGameState(
		eGAMESTATE_GAMEIN,
		&m_GameInInitKind,
		sizeof(m_GameInInitKind));
}

void CMapChange::BeforeRender()
{}

void CMapChange::AfterRender()
{
	WINDOWMGR->Render();

	mImageTip->RenderSprite(
		&mImageBlackScreenScale,
		0,
		0,
		&mImageBlackScreenPosition,
		RGBA_MAKE(0,0,0,255));
	mImageTip->RenderSprite(
		&mImageTipScale,
		0,
		0,
		&mImageTipPosition,
		RGBA_MAKE(255,255,255,255));
	//Alemuri Map Move With Image (Comment this to remove Tip Rectangle)
	//mImageBar->RenderSprite(
	//	&mImageBarScale,
	//	0,
	//	0,
	//	&mImageBarPosition,
	//	RGBA_MAKE(255,255,255,255));
	//------------------------------------------------------------------
#ifndef _KOR_LOCAL_
	mImageGradeMark->RenderSprite(
		&mImageGradeMarkScale,
		0,
		0,
		&mImageGradeMarkPosition,
		RGBA_MAKE(255,255,255,255));
#endif


	CFONT_OBJ->RenderFont(
		mTip.mFont,
		mTip.mText,
		_tcslen(mTip.mText),
		&mTip.mRect, 
		RGBA_MAKE(0,0,0,255));
	RECT rect = mTip.mRect;
	++rect.left;
	++rect.top;
	CFONT_OBJ->RenderFont(
		mTip.mFont,
		mTip.mText,
		_tcslen(mTip.mText),
		&rect,
		RGBA_MAKE(255,255,255,255));
}

void CMapChange::NetworkMsgParse(BYTE Category,BYTE Protocol,void* pMsg, DWORD dwMsgSize)
{
	switch(Category)
	{
	case MP_USERCONN:
		{
			switch(Protocol) 
			{
			case MP_USERCONN_CHARACTERSELECT_ACK:
				{
					MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;
					m_MapNum = pmsg->bData;
				}
				return;

			case MP_USERCONN_CHARACTERSELECT_NACK:
				{
					LOG(
						EC_MAPSERVER_CLOSED);
					MAINGAME->SetGameState(
						eGAMESTATE_CHARSELECT,
						&m_GameInInitKind,
						sizeof(m_GameInInitKind));
				}
				return;
			}
		}
		break;
	case MP_CHAT:
		CHATMGR->NetworkMsgParse(Protocol, pMsg);
		break;
	case MP_GTOURNAMENT:
		GTMGR->NetworkMsgParse(Protocol, pMsg);
		break;
	case MP_CHEAT:
		{
			switch(Protocol) 
			{
				case MP_CHEAT_EVENTNOTIFY_ON:
				{
					MSG_EVENTNOTIFY_ON* pmsg = (MSG_EVENTNOTIFY_ON*)pMsg;

					NOTIFYMGR->SetEventNotifyStr(pmsg->strTitle, pmsg->strContext);
					NOTIFYMGR->SetEventNotify(TRUE);
					NOTIFYMGR->SetEventNotifyChanged(TRUE);

					NOTIFYMGR->ResetEventApply();
					for(int i=0; i<eEvent_Max; ++i)
					{
						if( pmsg->EventList[i] )
							NOTIFYMGR->SetEventApply( i );
					}
				}
				break;
		
				case MP_CHEAT_EVENTNOTIFY_OFF:
				{
					NOTIFYMGR->SetEventNotify( FALSE );
					NOTIFYMGR->SetEventNotifyChanged( FALSE );
				}	
				break;

			}
		}
		break;
	}
}