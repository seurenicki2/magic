#include "stdafx.h"
#include "WindowIDEnum.h"
#include "cBrowser.h"
#include ".\cHousingWebDlg.h"
#include <Wincrypt.h>
#include "ObjectManager.h"
#include "MacroManager.h"
#include "./Interface/cButton.h"
#include "MHMap.h"
#include "MHTimeManager.h"
#include "hero.h"


cHousingWebDlg::cHousingWebDlg(void)
{
	m_pBrowser = NULL;
}

cHousingWebDlg::~cHousingWebDlg(void)
{
}

void cHousingWebDlg::Linking()
{
	m_pGambar = (cStatic*)GetWindowForID( ISD_BROWSER_IMAGETITLE );
	m_pBody = (cStatic*)GetWindowForID( ISD_BROWSER_BODY );
	m_pTitle = (cStatic*)GetWindowForID( ISD_BROWSER_TITLE );
	m_pBrowser = (cBrowser*)GetWindowForID( ISD_BROWSER );
	m_pBvote = (cButton*)GetWindowForID( WEB_IDVOTE );
	m_pBedit = (cButton*)GetWindowForID( WEB_IDEDIT );
	m_pBshop = (cButton*)GetWindowForID( WEB_SHOP );
	m_pBforum = (cButton*)GetWindowForID( WEB_FORUM );
	m_pBreport = (cButton*)GetWindowForID( WEB_BUGREPORT );
	m_pDonate = (cButton*)GetWindowForID( WEB_DONATE );
	m_pGlobalchat = (cButton*)GetWindowForID( WEB_GLOBALCHAT );
	SetAlwaysTop( TRUE );
}

void cHousingWebDlg::SetActive( BOOL val )
{
	cDialog::SetActiveRecursive( val );

	if(val == FALSE)
	{
		m_pBrowser->Navigate("about:blank");
		m_pBrowser->SetActive(FALSE);
		MACROMGR->ForceSetFocus();
	} else {
		if (m_typebrowser == 0)
		{
			m_pGambar->SetActive(TRUE);
			m_pBody->SetActive(FALSE);
			m_pTitle->SetActive(TRUE);
		} else {
			m_pGambar->SetActive(FALSE);
			m_pBody->SetActive(TRUE);
			m_pTitle->SetActive(TRUE);
		}
	}
}

void cHousingWebDlg::SetType( WORD val )
{
	m_typebrowser = val;
	if (val == 0)
	{
		m_pGambar->SetActive(TRUE);
		m_pBody->SetActive(FALSE);
		m_pTitle->SetActive(TRUE);
	} else {
		m_pGambar->SetActive(FALSE);
		m_pBody->SetActive(TRUE);
		m_pTitle->SetActive(TRUE);
	}
}

void cHousingWebDlg::Navigate( char* pURL )
{
	m_pBrowser->Navigate( pURL );
}

extern int g_nServerSetNum;
void cHousingWebDlg::OpenMiniHomePage(DWORD dwOwnerUserIndex, MAPTYPE Mapnum, DWORD ncharidx)
{
	if(IsActive())
	{
		SetActive(FALSE);
		return;
	}

	if(0 == dwOwnerUserIndex)
		return;

	m_pTitle->SetStaticText("IXLUNA WEB MALL");
	char csBuffer[1024];
	char csBufferMsg[1024];
	char csDigest[1024];
	char url1[32];
	char url2[128];
	char url3[128];
	char url4[128];
	char url5[128];
	char url6[128];
	DWORD dwTickCount = GetTickCount();
	const DWORD dwSecretNum = 13;
	const DWORD dwSecretNum2 = 23;
	DWORDEX dwSecret = dwTickCount * dwSecretNum;
	DWORDEX dwSecret2 = dwTickCount * dwSecretNum2;
	//DWORDEX dwSecretNum4 = dwTickCount * 19;
	__time64_t curTime = MHTIMEMGR->GetServerTime();
	struct tm curTimeWhen = *_localtime64( &curTime );
	DWORD nTmin = curTimeWhen.tm_min * dwSecretNum2;

	CHero* const halah = OBJECTMGR->GetHero();


	sprintf(csBuffer, "%d-%d-%s-%d-%s-%d", dwTickCount, curTimeWhen.tm_min, g_szHeroIDName, Mapnum, HERO->GetObjectName(), ncharidx);
	HCRYPTPROV hCryptProv = 0;
	HCRYPTHASH hHash = 0;
	BYTE bHash[0x7f] = {0};
	DWORD dwHashLen= 16; // The MD5 algorithm always returns 16 bytes. 
	DWORD cbContent= strlen(csBuffer);
	BYTE* pbContent= (BYTE*)csBuffer;


	if(CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET)) 
	{

		if(CryptCreateHash(hCryptProv, CALG_MD5,	// algorithm identifier definitions see: wincrypt.h
			0, 0, &hHash)) 
		{
			if(CryptHashData(hHash, pbContent, cbContent, 0))
			{
				if(CryptGetHashParam(hHash, HP_HASHVAL, bHash, &dwHashLen, 0)) 
				{
					memset( csDigest, 0, sizeof(char)*1024 );
					char tmp[32];
					for (int i = 0; i<16; i++)
					{
						sprintf( tmp, "%02x", bHash[i] );
						strcat( csDigest, tmp );
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
	
	sprintf(url1, "&user=%s",  HERO->GetObjectName());
	sprintf(url2, "&key=%d",  dwSecret);
	sprintf(url3, "&key2=%d",  dwSecret2);
	sprintf(url4, "&lock=%d",  ncharidx);
	sprintf(url5, "&ident=%s",  g_szHeroIDName);
	sprintf(url6, "&counter=%d", nTmin);
	sprintf(csBufferMsg, "%s%s%s%s%s%s", url1, url2, url3, url4, url5, url6);

	char URL[1024] = {0,};
	// web im decade anwar
	sprintf( URL, "%sauth=%s%s", "http://idgamer.net/url/", csDigest, csBufferMsg);
	//ICE06 berhasil encrypttttttttt
	Navigate( URL );
	SetActive( TRUE );
}

void cHousingWebDlg::OpenBrowser(DWORDEX Code, char* Acccode)
{
	m_pTitle->SetStaticText("DECADE LUNA PLUS WEB MALL");

	char URL[1024] = {0,};
	char URL1[128] = {0,};
	char URL2[128] = {0,};

	sprintf(URL1, "&key=%d",  Code);
	sprintf(URL2, "&code=%s",  Acccode);
	sprintf( URL, "%sid=%s%s", "http://luna.idgamer.net/itemmallingame/", HERO->GetObjectName(), "bloodymonday13", URL2);
	Navigate( URL );
	SetActive( TRUE );
}

void cHousingWebDlg::OnActionEvent( LONG lId, void* p, DWORD we )
{
	if( !p ) return ;

	if(we == WE_BTNCLICK)
	{
		switch(lId)
		{
		case WEB_IDVOTE:
			{
				/*
				m_pTitle->SetStaticText("GAME VOTE");
				char URL[1024] = {0,};
				sprintf( URL, "%s%s", deFaultWebsite, "vote.php");
				m_pBrowser->Navigate( URL );
				*/
			}
			break;
		case WEB_IDEDIT:
			{
				/*
				m_pTitle->SetStaticText("EDIT ID");
				char URL[1024] = {0,};
				sprintf( URL, "%s%s", deFaultWebsite, "edit.php");
				m_pBrowser->Navigate( URL );
				*/
			}
			break;
		case WEB_SHOP:
			{
				/*
				m_pTitle->SetStaticText("ITEM MALL");
				char URL[1024] = {0,};
				sprintf( URL, "%s%s", deFaultWebsite, "shop.php");
				m_pBrowser->Navigate( URL );
				*/
			}
			break;
		case WEB_FORUM:
			{
				/*
				m_pTitle->SetStaticText("FORUM");
				char URL[1024] = {0,};
				sprintf( URL, "%s%s", deFaultWebsite, "forum.php");
				m_pBrowser->Navigate( URL );
				*/
			}
			break;
		case WEB_BUGREPORT:
			{
				/*
				m_pTitle->SetStaticText("REPORT BUG");
				char URL[1024] = {0,};
				sprintf( URL, "%s%s", deFaultWebsite, "bugreport.php");
				m_pBrowser->Navigate( URL );
				*/
			}
			break;
		case WEB_DONATE:
			{
				/*
				m_pTitle->SetStaticText("DONATE");
				char URL[1024] = {0,};
				sprintf( URL, "%s%s", deFaultWebsite, "donate.php");
				m_pBrowser->Navigate( URL );
				*/
			}
			break;
		case WEB_GLOBALCHAT:
			{
				/*
				m_pTitle->SetStaticText("GLOBAL NETWORK CHAT");
				char URL[1024] = {0,};
				sprintf( URL, "%sglobalchat.php?nick=%s", deFaultWebsite, HERO->GetObjectName() );
				m_pBrowser->Navigate( URL );
				*/
			}
			break;
		}
	}

}