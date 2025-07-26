// GTScoreInfoDailog.cpp: implementation of the CGTScoreInfoDailog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PWScoreInfoDialog.h"
#include "WindowIDEnum.h"
#include "./interface/cStatic.h"
#include "./interface/cButton.h"
#include "..\effect\DamageNumber.h"
#include "./Interface/cPushupButton.h"
#include "cScriptManager.h"
#include "ChatManager.h"
#include "Battle.h"

CPWScoreInfoDialog::CPWScoreInfoDialog()
{
	m_bStart = FALSE;
}

CPWScoreInfoDialog::~CPWScoreInfoDialog()
{

}


void CPWScoreInfoDialog::Linking()
{
	m_pTeamScore[0] = (cStatic*)GetWindowForID( PW_SCORE1 );
	m_pTeamScore[1] = (cStatic*)GetWindowForID( PW_SCORE2 );

	m_pScoreBack[0] = (cPushupButton*)GetWindowForID( PW_SCOREBACK1 );
	m_pScoreBack[1] = (cPushupButton*)GetWindowForID( PW_SCOREBACK2 );
	//m_pTimer = (cPushupButton*)GetWindowForID( GT_TIMER );

	m_Score[0] = 0;
	m_Score[1] = 0;

	for( int i=0; i<2; ++i)
	{
		m_ScoreImage[i].Init( 20, 0 );
		m_ScoreImage[i].SetFillZero( TRUE );
		m_ScoreImage[i].SetLimitCipher( 3 );
		//m_ScoreImage[i].Visible( TRUE );
		m_ScoreImage[i].SetPosition( (int)m_pTeamScore[i]->GetAbsX()+65, (int)m_pTeamScore[i]->GetAbsY()+2 );
		m_ScoreImage[i].SetScale( 1.0f, 1.0f );

		for(int j=0; j<10; j++)
		{
			m_ScoreImage[i].InitDamageNumImage(CDamageNumber::GetImage(eDNK_Yellow, j), j);
		}
	}
}

void CPWScoreInfoDialog::Render()
{
	if( m_bActive )
	{
		cDialog::Render();
		//Process();

		//m_ScoreImage[i].SetPosition((int)m_pTeamScore[i]->GetAbsX()+65, (int)m_pTeamScore[i]->GetAbsY()+2);
		//m_ScoreImage[1].SetNumber( 100 );

		for(int i=0; i<2; ++i)
		{
			m_ScoreImage[i].SetPosition((int)m_pTeamScore[i]->GetAbsX()+65, (int)m_pTeamScore[i]->GetAbsY()+2);
			m_ScoreImage[i].SetNumber( m_Score[i] );
			m_ScoreImage[i].RenderWithDamageNumImage();
		}
	}
}

void CPWScoreInfoDialog::Process()
{
	if(FALSE == m_bStart)
	{
		return;
	}
}


void CPWScoreInfoDialog::SetBattleInfo( int m_nScore[2] )
{
	m_bStart = TRUE;
	for( int i=0; i<2; ++i)
	{
		//m_pTeamScore[0] = 1;
		//m_pTeamScore[1] = 2;
		m_Score[i] = m_nScore[i];
		//m_Score[1] = m_nScore2;
	}


//	if(pInfo->State == eBATTLE_STATE_FIGHT)
//	{
//		m_bStart = TRUE;
//	}
}
