#include "stdafx.h"
#include "VideoCaptureManager.h"
#include "ChatManager.h"
#include "VideoCaptureDlg.h"
#include "GameIn.h"

CVideoCaptureManager::CVideoCaptureManager()
{
}

CVideoCaptureManager::~CVideoCaptureManager()
{
}

BOOL CVideoCaptureManager::IsCapturing()
{
	return m_bandiCaptureLibrary.IsCapturing();
}

BOOL CVideoCaptureManager::CaptureStart()
{
	// ĸó�� �����մϴ�.
	if( m_bandiCaptureLibrary.IsCapturing() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1999 ) );		//"��ȭ���Դϴ�."
		return FALSE;
	}

	void* pd3d9Device = g_pExecutive->GetRenderer()->GetD3DDevice();
	if( !pd3d9Device )
		return FALSE;

	// ���� ����� DLL �� �ε��ϰ� ���� �ν��Ͻ��� �����մϴ�. 
	if( m_bandiCaptureLibrary.IsCreated() == FALSE )
	{
		// ���н� DLL�� .H������ ������ �������� ���ε��� Ȯ���� ���ñ� �ٶ��ϴ�.
		if( FAILED( m_bandiCaptureLibrary.Create( BANDICAP_RELEASE_DLL_FILE_NAME ) ) )
		{
#ifdef _GMTOOL_
			CHATMGR->AddMsg( CTC_SYSMSG, "Video Capture DLL Load Error" );
#endif
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2000 ) );		// "��ȭ�� �����Ͽ����ϴ�."
			return FALSE;
		}

		// 100218 ShinJS --- �ݵ� ����Ʈ ����Ű ���
		if( FAILED( m_bandiCaptureLibrary.Verify( "EYA_LUNAPLUS_20100218", "e5b68af6" ) ) )
		{
#ifdef _GMTOOL_
			CHATMGR->AddMsg( CTC_SYSMSG, "Video Capture DLL Verify Failed" );
#endif
		}
	}

	if( m_bandiCaptureLibrary.IsCreated() )
	{
		BCAP_CONFIG cfg;
		m_bandiCaptureLibrary.SetMinMaxFPS( 60, 60 );	// �ּ�, �ִ� ������ ����

		// ��ȭ �ɼ� ����
		SetCaptureOpt();

		// ������ Direct�� �����Ѵ�.
		char strDirectory[MAX_PATH];
		sprintf( strDirectory, "%sScreenShot", DIRECTORYMGR->GetFullDirectoryName( eLM_Root ) );
		if( ! ::CreateDirectory( strDirectory, NULL ) &&
			GetLastError() != ERROR_ALREADY_EXISTS )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2000 ) );		// "��ȭ�� �����Ͽ����ϴ�."
			return FALSE;
		}

		// ����ð��� �̿��ؼ� ���ϸ��� ����ϴ�. 
		TCHAR pathName[MAX_PATH];
		m_bandiCaptureLibrary.MakePathnameByDate( strDirectory, _T("Capture"), _T("avi"), pathName, MAX_PATH);


		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2001 ), pathName );		// "��ȭ�� �����մϴ�. %s �� ����˴ϴ�."

		// ĸó�� �����մϴ�.
		HRESULT hr = m_bandiCaptureLibrary.Start( pathName, NULL, BCAP_MODE_D3D9_SCALE, (LONG_PTR)pd3d9Device );//BCAP_MODE_D3D9_SCALE
		if( FAILED( hr ) )
			return FALSE;
	}

	return TRUE;
}

void CVideoCaptureManager::CaptureStop()
{
	m_bandiCaptureLibrary.Stop();

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2002 ) );		// "��ȭ�� �����մϴ�."
}

void CVideoCaptureManager::Process()
{
	if( !m_bandiCaptureLibrary.IsCapturing() )
		return;

	// DX Device ���ϱ�
	void* pd3d9Device = g_pExecutive->GetRenderer()->GetD3DDevice();
	if( !pd3d9Device )
		return;

	m_bandiCaptureLibrary.Work( (LONG_PTR)pd3d9Device );
}

void CVideoCaptureManager::SetCaptureOpt()
{
	CVideoCaptureDlg* pVideoCaptureDlg = GAMEIN->GetVideoCaptureDlg();
	if( !pVideoCaptureDlg )
		return;

	BCAP_CONFIG cfg;
	m_bandiCaptureLibrary.GetConfig( &cfg );

	// �ػ� ����
	switch( (CVideoCaptureDlg::eVideoCaptureSize)pVideoCaptureDlg->GetSelecedSize() )
	{
	case CVideoCaptureDlg::eVideoCaptureSize_Default:
		{
			cfg.Default();
		}
		break;

	case CVideoCaptureDlg::eVideoCaptureSize_500x375:
		{
			cfg.VideoSizeW = 500;
			cfg.VideoSizeH = 375;
		}
		break;
	}

	// ȭ�� ����
	switch( (CVideoCaptureDlg::eVideoCaptureOpt)pVideoCaptureDlg->GetSelecedOpt() )
	{
	case CVideoCaptureDlg::eVideoCaptureOpt_High:
		{
			cfg.VideoCodec		= FOURCC_MJPG;
			cfg.VideoQuality	= 90;
			cfg.VideoFPS		= 60;

			cfg.AudioChannels	= 2;
			cfg.AudioCodec		= WAVETAG_MP2;
			cfg.AudioSampleRate = 44100;
		}
		break;

	case CVideoCaptureDlg::eVideoCaptureOpt_Low:
		{
			cfg.VideoCodec		= FOURCC_MPEG;
			cfg.VideoQuality	= 70;
			cfg.VideoFPS		= 60;

			cfg.AudioChannels	= 1;
			cfg.AudioCodec		= WAVETAG_PCM;
			cfg.AudioSampleRate = 22050;
		}
		break;
	}

	cfg.AdjustAudioMixer	= TRUE;

	// Ŀ�� ���� ����
	cfg.IncludeCursor		= pVideoCaptureDlg->IsIncludeCursor();

	// �߸��� ���� Ȯ��
	m_bandiCaptureLibrary.CheckConfig( &cfg );

	// ���� ����
	m_bandiCaptureLibrary.SetConfig( &cfg );
}