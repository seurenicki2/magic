// GTScoreInfoDailog.h: interface for the CGTScoreInfoDailog class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_PWSCOREINFODAILOG_H__49E37D19_391B_480A_B515_AE1DEA41C09F__INCLUDED_)
//#define AFX_PWSCOREINFODAILOG_H__49E37D19_391B_480A_B515_AE1DEA41C09F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Interface/cDialog.h"
#include "../ImageNumber.h"


class cStatic;
class cButton;
class cPushupButton;



class CPWScoreInfoDialog : public cDialog
{
	CImageNumber	m_ScoreImage[2];
	cPushupButton*	m_pScoreBack[2];
	//cPushupButton*	m_pLeaderName[2];

	cStatic*		m_pTeamScore[2];

	int				m_Score[2];
	BOOL			m_bStart;

public:
	CPWScoreInfoDialog();
	virtual ~CPWScoreInfoDialog();

	void Linking();
	void Render();
	void Process();
	
	void SetBattleInfo( int m_nScore[2] );
	void StartBattle()			{	m_bStart = TRUE;	}
	void EndBattle()			{	m_bStart = FALSE;	}
	BOOL IsBattle()				{   return m_bStart;}
	//
	void SetTeamScore( DWORD Team, DWORD Count )		{m_Score[Team] = Count;}
};

//#endif // !defined(AFX_PWSCOREINFODAILOG_H__49E37D19_391B_480A_B515_AE1DEA41C09F__INCLUDED_)
