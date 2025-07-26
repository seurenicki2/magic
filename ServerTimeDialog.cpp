#include "stdafx.h"
#include "ServerTimeDialog.h"
#include "GameIn.h"
#include "GameResourceManager.h"
#include "ObjectManager.h"
#include "WindowIDEnum.h"
#include "cStatic.h"
#include "..\effect\DamageNumber.h"
#include "ObjectBalloon.h"

#include "GMNotifyManager.h"
#include "./interface/cFont.h"

// 081020 LYW --- WeatherDialog : SetActive 함수 추가(공성전에서는 알아서 활성화 되지 않도록 수정한다. - 송가람).
#include "./SiegeWarfareMgr.h"

// 090820 pdy 맵이동시 Gift이벤트 유무에따라 활성화/비활성화가 정상작동안하는 버그 수정 
#include "ItemManager.h"

CServerTimeDlg::CServerTimeDlg()
{
	m_DateText = NULL;

	//m_type = WT_SERVERTIMEDLG;
}

CServerTimeDlg::~CServerTimeDlg()
{
}

void CServerTimeDlg::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
	m_type = WT_SERVERTIMEDLG;
}

void CServerTimeDlg::SetActive( BOOL val )
{
	
	m_DateText->SetFGColor( RGBA_MAKE( 255, 255, 255, 255 ) ) ;
	m_DateText->SetStaticText("");
	m_DateText->SetActive(TRUE);
	if( SIEGEWARFAREMGR->IsSiegeWarfareZone() )
  	{
  		cDialog::SetActive( FALSE ) ;
  		return ;
  	}
	else
	{
		cDialog::SetActive( val ) ;
	}
	cDialog::SetMovable(TRUE);
}

void CServerTimeDlg::Linking()
{
 	m_DateText = (cStatic*)GetWindowForID(SERVERTIME_DLG_TIME);
}

void CServerTimeDlg::SetTime(DWORD year, DWORD month, DWORD day, DWORD hour, DWORD Minute)
{
	char servertime[30];
	sprintf( servertime, "%d-%02d-%02d     %02d : %02d", year, month, day, hour, Minute) ;
	m_DateText->SetStaticText(servertime);

}

void CServerTimeDlg::Render()
{
 	if( !IsActive() ) return ;
	cDialog::RenderWindow();
	cDialog::RenderComponent();
}
