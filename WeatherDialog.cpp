#include "stdafx.h"
#include "weatherdialog.h"
#include "WindowIDEnum.h"
#include ".\Interface\cWindowManager.h"

// 081020 LYW --- WeatherDialog : SetActive �Լ� �߰�(������������ �˾Ƽ� Ȱ��ȭ ���� �ʵ��� �����Ѵ�. - �۰���).
#include "./SiegeWarfareMgr.h"

//090525 pdy �Ͽ�¡ �߰�(�Ͽ�¡������ �˾Ƽ� Ȱ��ȭ ���� �ʵ��� �����Ѵ� ).
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
//	DESC		: SetActive �Լ� �߰�(������������ �˾Ƽ� Ȱ��ȭ ���� �ʵ��� �����Ѵ�. - �۰���).
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 20, 2008
//-------------------------------------------------------------------------------------------------
void CWeatherDialog::SetActive(BOOL val)
{
	// 090525 pdy --- MiniMapDlg : �Ͽ�¡ ���̸�, CWeatherDialog�� ��Ȱ��ȭ �Ѵ�.
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