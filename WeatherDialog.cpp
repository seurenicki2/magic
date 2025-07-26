#include "stdafx.h"
#include "weatherdialog.h"
#include "WindowIDEnum.h"
#include ".\Interface\cWindowManager.h"

// 081020 LYW --- WeatherDialog : SetActive 함수 추가(공성전에서는 알아서 활성화 되지 않도록 수정한다. - 송가람).
#include "./SiegeWarfareMgr.h"

//090525 pdy 하우징 추가(하우징에서는 알아서 활성화 되지 않도록 수정한다 ).
#include "cHousingMgr.h"

CWeatherDialog::CWeatherDialog(void)
{
	for(int i=0; i<eWS_Max; i++){
		m_pWeatherBtn[i] = NULL;
	}
}

CWeatherDialog::~CWeatherDialog(void)
{
}

void CWeatherDialog::Linking()
{
	// [ 11-May-2055 ] Death Open this function
	int i;
	for(i=0; i<eWS_Max; i++)
	{
		//m_pWeatherBtn[i] = (cButton*)GetWindowForID(WEATHER_BTN_CLEAR + i);
		// ---- weather custom
		m_pWeatherBtn[i] = (cButton*)GetWindowForID(1001 + i);
		if( m_pWeatherBtn[i] ){
		m_pWeatherBtn[i]->SetActive(FALSE);
		m_pWeatherBtn[i]->SetDisable(TRUE);
		}
	}
	cButton* onoff = (cButton*)GetWindowForID(1011);
	if( onoff ){
		onoff->SetDisable(FALSE);
		onoff->SetActive(TRUE);
	}
}

void CWeatherDialog::SetWeather(WORD wState)
{
	// ---- weather custom 
	int i , z=0;
	/*
	for(i=0; i<eWS_Max; i++)
	{
		m_pWeatherBtn[i]->SetDisable(FALSE);

		if(i == (int)wState)
			m_pWeatherBtn[i]->SetActive(TRUE);
		else
			m_pWeatherBtn[i]->SetActive(FALSE);

		m_pWeatherBtn[i]->SetDisable(TRUE);
	}
	*/
		for(i=0; i<eWS_Max; i++)
		{
			if( !m_pWeatherBtn[i] ) {
				break; }
			if (m_pWeatherBtn[i]->GetID() == 1011)
			{
				//m_pWeatherBtn[i]->SetDisable(FALSE);
				//m_pWeatherBtn[i]->SetActive(TRUE);
				//m_pWeatherBtn[i]->SetDisable(TRUE);
				continue;
			}
			m_pWeatherBtn[i]->SetDisable(FALSE);
			if( wState == 0 && i == 0 ){
				m_pWeatherBtn[i]->SetActive(TRUE);
				z = 9;
			}
			else if( z == 0 && m_pWeatherBtn[i]->GetID() == wState){
				m_pWeatherBtn[i]->SetActive(TRUE);
				z = 9;
			}
			else {
				m_pWeatherBtn[i]->SetActive(FALSE);
			}
			m_pWeatherBtn[i]->SetDisable(TRUE);


		}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: SetActive
//	DESC		: SetActive 함수 추가(공성전에서는 알아서 활성화 되지 않도록 수정한다. - 송가람).
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 20, 2008
//-------------------------------------------------------------------------------------------------
void CWeatherDialog::SetActive(BOOL val)
{
	// 090525 pdy --- MiniMapDlg : 하우징 맵이면, CWeatherDialog을 비활성화 한다.
  	if( SIEGEWARFAREMGR->IsSiegeWarfareZone() || 
		HOUSINGMGR->IsHousingMap() )
  	{
  		cDialog::SetActive( FALSE ) ;
  		return ;
  	}
	else
	{
		// ---- weather custom
		if( IsActive() != val ){
		
		cDialog::SetActive( val ) ;
		
		}
	}
}