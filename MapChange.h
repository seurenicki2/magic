#pragma once

#include "GameState.h"

class cImageSelf;
class cImage;

#define MAPCHANGE USINGTON(CMapChange)

class CMapChange  : public CGameState  
{
	BOOL mIsNoResourceLoaded;
	MAPTYPE m_MapNum;
	int m_GameInInitKind;
	// 09221 LUJ, �� �̹���
	cImageSelf* mImageTip;
	VECTOR2 mImageTipPosition;
	VECTOR2 mImageTipScale;
	VECTOR2 mImageBlackScreenPosition;
	VECTOR2 mImageBlackScreenScale;
	// 091221 LUJ, �� ���ڿ� ���
	cImage* mImageBar;
	VECTOR2	mImageBarScale;
	VECTOR2	mImageBarPosition;
	// 091221 LUJ, �� ���ڿ� ���� ����
	struct Tip
	{
		TCHAR mText[MAX_PATH];
		RECT mRect;
		WORD mFont;
	}
	mTip;
	// 091221 LUJ, �� ���ڿ��� ����ȴ�
	typedef std::set< std::string > TipSet;
	TipSet mUnusedTipSet;
	TipSet mUsedTipSet;
	// 091221 LUJ, �� �̹����� ����ȴ�
	typedef std::pair< DWORD, cImageSelf* > TipImagePair;
	typedef std::queue< TipImagePair > TipImageQueue;
	TipImageQueue mTipImageQueue;

	cImageSelf* mImageGradeMark;
	VECTOR2 mImageGradeMarkPosition;
	VECTOR2 mImageGradeMarkScale;

public:
	CMapChange();
	virtual ~CMapChange();
	BOOL Init(LPVOID pInitParam);
	void Release(CGameState*);
	void Process();
	void BeforeRender();
	void AfterRender();
	void NetworkMsgParse(BYTE Category,BYTE Protocol, LPVOID, DWORD dwMsgSize);
	void SetGameInInitKind(int kind) { m_GameInInitKind = kind; }

private:
	void SetPositionScale();
	void LoadTipImage();
	void LoadTipText();
	void GetTipText(LPTSTR, size_t);
	cImageSelf* GetTipImage();
};
EXTERNGLOBALTON(CMapChange)