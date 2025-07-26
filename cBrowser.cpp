//---BROWSER
#include "stdafx.h"
#include ".\cbrowser.h"
#include "../[CC]Header/GameResourceManager.h"

extern HWND _g_hWnd;

cBrowser::cBrowser(void)
{
	m_type			= WT_BROWSER;

	m_szURL[0]		= 0;
	m_hwndIE		= NULL;
}

cBrowser::~cBrowser(void)
{
	if( m_hwndIE )
	{
		// 091221 LUJ, �� �̵� �� ���� cBrowser�� �Ҹ��� ȣ�� �ÿ�
		//			ATL ������ �߻��Ѵ�. �̸� ���� ���� ����������
		//			�ٸ� ���� ����� �õ�����
		SendMessage(
			m_hwndIE,
			WM_QUIT,
			0,
			0);
		::DestroyWindow(
			m_hwndIE);
	}

	// 091221 LUJ, �Ҵ���� ���۷��� ī��Ʈ�� ���������ش�
	m_pWebBrowser.Release();
}

void cBrowser::SetAbsXY(LONG x, LONG y)
{
	m_absPos.x=(float)x;
	m_absPos.y=(float)y;
	m_bIsMovedWnd = TRUE ;

//	SetWindowPos( m_hwndIE, NULL, x, y, m_width, m_height, SWP_NOSIZE );
	MoveWindow( m_hwndIE, x, y, m_width, m_height, TRUE );
}

void cBrowser::Navigate( char* pURL )
{
	SafeStrCpy( m_szURL, pURL, 1024 );
	
	if( m_pWebBrowser ) 
	{ 
		LPCONNECTIONPOINTCONTAINER pCPC = NULL;
		LPCONNECTIONPOINT pCP = NULL;

		m_pWebBrowser->QueryInterface(IID_IConnectionPointContainer, (LPVOID*)&pCPC);
		pCPC->FindConnectionPoint(__uuidof(SHDocVw::DWebBrowserEventsPtr), &pCP);

		DWORD dwCookie;
		pCP->Advise((LPUNKNOWN)&m_events, &dwCookie);

		pCPC->Release();

//		CComVariant vempty, vUrl(m_szURL);
		CComVariant vempty;

		m_pWebBrowser->Navigate( _bstr_t(m_szURL), &vempty, &vempty, &vempty, &vempty);             

		pCP->Unadvise(dwCookie);
		pCP->Release();
	}
}


//---�߿�!!! ���̾�αװ� ��Ƽ����°� �ƴ϶� �׷��� ������ ���� ���� ���������̵鵵 �׷����� ������,
//---�� �������� �����찡 �ߴ� ���̶� �� ��Ƽ����¸� ������Ѵ�.
//---���� cDialog::SetActiveRecursive() �� ����϶�. 
//---void CItemShopDlg::SetActive( BOOL val ) ����.
void cBrowser::SetActive(BOOL val)
{
	if(IsDisable())
	{
		return;
	}
	
	m_bActive = val;

	if(val)
	{
		// 091221 LUJ, �� �̵� �� ���� cBrowser�� �Ҹ��� ȣ�� �ÿ�
		//			ATL ������ �߻��Ѵ�. m_hwndIE�� ������ ����
		//			�ڵ�� ��Ÿ����. ���� �󵵸� ���̱� ���� â��
		//			�������� â�� �����ϵ��� ����
		if (m_szURL)
		{
			if(0 == m_hwndIE)
			{
				m_hwndIE = CreateWindow("AtlAxWin71", m_szURL,
					FULLSCREEN_WITH_BLT == GAMERESRCMNGR->GetResolution().dispType ? WS_POPUP : WS_CHILD,
					int(GetAbsX()), int(GetAbsY()), GetWidth(), GetHeight(),
					_g_hWnd, 0, GetModuleHandle(0), 0);
				CComPtr<IUnknown> punkIE = 0;

				if(S_OK == AtlAxGetControl(	m_hwndIE, &punkIE))
				{
					m_pWebBrowser = punkIE;
				}
			}
		} else {
			if(0 == m_hwndIE)
			{
				m_hwndIE = CreateWindow("AtlAxWin71", "about:blank",
					FULLSCREEN_WITH_BLT == GAMERESRCMNGR->GetResolution().dispType ? WS_POPUP : WS_CHILD,
					int(GetAbsX()), int(GetAbsY()), GetWidth(), GetHeight(),
					_g_hWnd, 0, GetModuleHandle(0), 0);
				CComPtr<IUnknown> punkIE = 0;

				if(S_OK == AtlAxGetControl(	m_hwndIE, &punkIE))
				{
					m_pWebBrowser = punkIE;
				}
			}
		}

		ShowWindow( m_hwndIE, SW_SHOW);
	}
	else
	{
		if(m_hwndIE)
		{
			ShowWindow(m_hwndIE, SW_HIDE);
		}
	}
}