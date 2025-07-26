#include "stdafx.h"																	// ǥ�� �ý��۰�, ������Ʈ�� ������, ���־��� �ش����� ���� �ش������� �ҷ��´�.
#include "WindowIdEnum.h"															// ������ ���̵� ���� �Ǿ� �ִ� ��� ������ �ҷ��´�.
#include "MHFile.h"																	// ���� ���� Ŭ���� ��������� �ҷ��´�.
#include "../[Client]LUNA/Interface/cScriptManager.h"								// ��ũ��Ʈ �Ŵ��� ��������� �ҷ��´�.
#include "./interface/cWindowManager.h"												// ������ �Ŵ��� ��������� �ҷ��´�.
#include ".\npcimagedlg.h"															// NPC �̹��� ���̾�α� Ŭ���� ��������� �ҷ��´�.

cNpcImageDlg::cNpcImageDlg(void)													// ������ �Լ�.
{
	COLOR = RGBA_MAKE(255,255,255,150);												// ������ �����Ѵ�.
	COLOR2 = RGBA_MAKE(255,255,255,255);												// ������ �����Ѵ�.

	SCALE.x = 450.f/512.f;															// �������� �����Ѵ�.
	SCALE.y = 450.f/512.f;
	BACK_POS.x = 0;																	// ��� ��ġ�� �����Ѵ�.
	BACK_POS.y = 0;

	mEmotion = EMOTION_NORMAL;
	
	m_pTopImage	= NULL ;															// ž �̹��� �����͸� NULL ó���� �Ѵ�.
	m_pBottomImage = NULL ;															// ���� �̹��� �����͸� NULL ó���� �Ѵ�.
	ZeroMemory(
		&mScreenRect,
		sizeof(mScreenRect));
	ZeroMemory(
		&mpCurImage,
		sizeof(mpCurImage));
	ZeroMemory(
		&mImageBack,
		sizeof(mImageBack));
}

cNpcImageDlg::~cNpcImageDlg(void)													// �Ҹ��� �Լ�.
{}

void cNpcImageDlg::Linking()														// ��ũ �Լ�.
{
	m_pTopImage	= (cStatic*)GetWindowForID(NI_TOPIMG) ;								// ž �̹����� ��ũ�Ѵ�.
	m_pBottomImage = (cStatic*)GetWindowForID(NI_BOTTOMIMG) ;						// ���� �̹����� ��ũ�Ѵ�.

	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&mScreenRect,
		&value,
		&value);
}

void cNpcImageDlg::LoadingImage(WORD idx)											// �̹����� �ε��ϴ� �Լ�.
{
	CMHFile file;																	// ���� ������ �����Ѵ�.

	file.Init("Data/Interface/Windows/NpcImageList.bin","rb");						// npc �̹��� ����Ʈ�� �б���� ����.

	if(file.IsInited() == FALSE)													// ���⿡ �����ϸ�,
	{
		return;																		// ���� ó���� �Ѵ�.
	}

	SCRIPTMGR->GetImage( 12, &mImageBack, PFT_HARDPATH );							// �޹�� ���� �̹����� �޴´�.

	VECTOR2 vDisp1, vDisp2, vDisp3 ;												// ��ġ ���͸� �����Ѵ�.

	vDisp1.x = file.GetFloat() ;													// ��ġ ���� 1�� ������ �о���δ�.
	vDisp1.y = file.GetFloat() ;

	vDisp2.x = file.GetFloat() ;													// ��ġ ���� 2�� ������ �о���δ�.
	vDisp2.y = file.GetFloat() ;

	vDisp3.x = file.GetFloat() ;													// ��ġ ���� 3�� ������ �о���δ�.
	vDisp3.y = file.GetFloat() ;

	BACK_POS.y = (float)mScreenRect.bottom - mImageBack.GetImageRect()->bottom;
	BACK_SCALE.x = (float)mScreenRect.right / 2;
	BACK_SCALE.y = 1.0f;

	switch(mScreenRect.right)
	{
	case 800:
		{
			POS.x = vDisp1.x;
			POS.y = vDisp1.y;
		}
		break;
	case 1024:
		{
			POS.x = vDisp2.x;
			POS.y = vDisp2.y;
		}
		break;
	case 1280:
		{
			POS.x = vDisp3.x;
			POS.y = vDisp3.y;
		}
		break ;
	default:
		{
			const SIZE standardResolution = {1024, 768};
			const float ratioWidth = float(mScreenRect.right) / standardResolution.cx;
			const float ratioHeight = float(mScreenRect.bottom) / standardResolution.cy;
			POS.x = vDisp2.x * ratioWidth;
			POS.y = vDisp2.y * ratioHeight;
		}
		break;
	}

	char buf[256] = {0};
	ZeroMemory(
		&buf,
		sizeof(buf));
	ZeroMemory(
		&mpCurImage,
		sizeof(mpCurImage));

	while(FALSE == file.IsEOF())
	{
		WORD wIndex  = file.GetWord();
		BYTE emotion = file.GetByte();
		char*	szFileName	= file.GetString();

		if( wIndex == idx )															// �ӽ� �ε����� ���� �ε����� ���ٸ�,
		{
			if( strlen( szFileName) == 0) break;									// ���ϸ��� 0�� ���ٸ�, while���� Ż���Ѵ�.

			sprintf( buf, "%s%s", "./data/interface/2dimage/npciamge/", szFileName);// �ӽ� ���ۿ� ��ι�, ���ϸ��� �����Ѵ�.

			mpCurImage.idx = idx;													// ���� �̹����� �ε����� �����Ѵ�.
			mpCurImage.image[ emotion ].LoadSprite( buf );							// ���� �̹����� ��ǿ� ���� ��������Ʈ�� �ε��Ѵ�.
		}
	}
}

void cNpcImageDlg::SetActive( BOOL val )											// Ȱ��, ��Ȱ��ȭ �Լ�.
{
	if( val )																		// val�� TRUE�� ���ٸ�,
	{
		if( IsActive() ) return ;													// Ȱ��ȭ ���¶��,

		//WINDOWMGR->m_pActivedWindowList->RemoveAll() ;								// Ȱ��ȭ �� ��� â�� ����.
		//WINDOWMGR->CloseAllWindow();												// ��� �����츦 �ݴ´�.

		//090119 pdy Window Hide Mode
		WINDOWMGR->HideAllWindow();													// ��� �����츦 �����.
	}
	else																			// val�� FALSE�� ���ٸ�,
	{									
		//WINDOWMGR->ShowAllActivedWindow();										// ��� Ȱ��ȭ �����츦 �����ش�.

		//090119 pdy Window Hide Mode
		WINDOWMGR->UnHideAllActivedWindow();										// ���������츦 �ٽ� �����ش�.

		WINDOWMGR->SetOpendAllWindows( TRUE ) ;										// ��� Ȱ��ȭ �����츦 ����.

		WINDOWMGR->ShowBaseWindow() ;												// �⺻ �����츦 �����ش�.
	}
	
	cDialog::SetActive(val);														// ���̾�α��� Ȱ��ȭ ���θ� �����Ѵ�.
}

void cNpcImageDlg::SetNpc( WORD NpcUniqueIdx )										// Npc������ �����ϴ� �Լ�.
{
	mNpcIdx = NpcUniqueIdx;															// Npc�ε����� �����Ѵ�.

	LoadingImage(mNpcIdx) ;															// �̹����� �ε��Ѵ�.
}

void cNpcImageDlg::SetEmotion(BYTE emotion)
{
	mEmotion = emotion;

	int nWidth  = mpCurImage.image[ mEmotion ].GetImageRect()->right;
	int nHeight = mpCurImage.image[ mEmotion ].GetImageRect()->bottom;

	POS.x = (float)(mScreenRect.right - nWidth);
	POS.y = (float)(mScreenRect.bottom - nHeight);
}

void cNpcImageDlg::Render()
{
	if( FALSE == IsActive() )
	{
		return;
	}

	mImageBack.RenderSprite(
		&BACK_SCALE,
		NULL,
		0,
		&BACK_POS,
		COLOR);	
	mpCurImage.image[mEmotion].RenderSprite(
		NULL,
		NULL,
		0,
		&POS,
		COLOR2);
}