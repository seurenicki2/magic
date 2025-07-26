#include "stdafx.h"
#include ".\cmoneydividebox.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cButton.h"
#include "./Interface/cStatic.h"
#include "./Interface/cSpin.h"
#include "WindowIDEnum.h"
#include "cIMEEX.h"

cMoneyDivideBox::cMoneyDivideBox(void)
{
	mMoney = 0;
	mColor = TTTC_ONDERMILLION;
}

cMoneyDivideBox::~cMoneyDivideBox(void)
{
}

void cMoneyDivideBox::Render()
{
	if( m_bActive )
	{
		if( mMoney != MONEYTYPE(GetValue()))
		{
			mMoney = MONEYTYPE(GetValue());
			//mColor = GetMoneyColor(mMoney);
			mColor = RGB(105, 105, 105);
		}
		m_pSpin->SetActiveTextColor( DWORD(mColor) );
		m_pSpin->SetNonactiveTextColor( DWORD(mColor) );
	}

	cDivideBox::Render();
}

void cMoneyDivideBox::CreateDivideBox( LONG x, LONG y, LONG ID, cbDivideFUNC cbFc1, cbDivideFUNC cbFc2, void * vData1, void* vData2, char* strTitle )
{
	m_cbDivideFunc = cbFc1;
	m_cbCancelFunc = cbFc2;
	m_vData1 = vData1;
	m_vData2 = vData2;
	SetAbsXY(x,y);
	SetID(ID);

	m_pOKBtn = (cButton *)GetWindowForID(CMI_MONEYDIVIDEOK);
	m_pCancelBtn = (cButton *)GetWindowForID(CMI_MONEYDIVIDECANCEL);

	m_pSpin = (cSpin *)GetWindowForID(CMI_MONEYDIVIDESPIN);
	m_pSpin->SetValidCheck(VCM_NUMBER); // ���ڸ� �Է� �ǵ��� �Ѵ�.
	m_pSpin->SetAlign( TXT_RIGHT );		// ��������
	m_pSpin->SetFocusEdit( TRUE );
//	m_pSpin->SetUnit( 0 );

	// Title Text �Է� LBS 03.11.19 : ���� KES
	if( strTitle )
	{
		cStatic* pStatic = new cStatic;
		
		// 070504 LYW --- Ÿ��Ʋ ��ġ ����
		//pStatic->Init( 13, 2, 0, 0, NULL );
		pStatic->Init( 13, 10, 0, 0, NULL );
		pStatic->SetShadow( TRUE );
		pStatic->SetFontIdx( 2 );
		pStatic->SetAlign( TXT_LEFT );
		pStatic->SetStaticText( strTitle );
		
		Add( pStatic );
	}
}