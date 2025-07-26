#pragma once
#include ".\interface\cdialog.h"
#include ".\interface\cButton.h"

class CNewWeatherDlg :
	public cDialog
{

	cButton*		m_pWeatherBtn[eWS_Max];

public:
	CNewWeatherDlg(void);
	virtual ~CNewWeatherDlg(void);

	void Linking();

	
	virtual void SetActive(BOOL val) ;

	void SetWeather(WORD wState);
};
