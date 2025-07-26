/*************************************************************************************************************
 *
 *	File Name	::	cAntiBotDialog.h
 *
 *	Owner		::	Death
 *
 *	Purpose Application	::	AntiBot , Auto Joy By has dialog display if player atk more one hour
 *
 *	Change Log	::
 *
 *	Death , Sep 05		Intitial Version
 *
 *
 *************************************************************************************************************/
#pragma once

//=================================================================================================
//	INCLUDE PART
//=================================================================================================
#include ".\interface\cdialog.h"

class cButton;
class cStatic;
class cEditBox;
class cTextArea;

class CAntiBotDialog : public cDialog
{
protected :
	/// Buttons for choice ok or cancel.
	cButton*	m_pBtnOk ;
	cButton*	m_pBtnCancel ;

	/// Static for notice random number.
	cStatic*	m_pRanNum ;

	/// Editbox for input text from user.
	cEditBox*	m_pEditBox ;

	/// Text area for notice text.
	cTextArea*	m_pText ;

	/// variable for random number.
	int m_nRanNum  ;
public:
	CAntiBotDialog(void);
	virtual ~CAntiBotDialog(void);

	/// The function for linking components to window manager.
	void Linking() ;

	/// The function for process event.
	void OnActionEvent( LONG lId, void* p, DWORD we ) ;

	/// Make random number for certificate some condition.
	void MakeRandomNumber() ;
};
