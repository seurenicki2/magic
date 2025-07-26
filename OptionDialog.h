#ifndef _OPTIONDIALOG_H
#define _OPTIONDIALOG_H


#include "./Interface/cTabDialog.h"
#include "OptionManager.h"

//class cButton;

class COptionDialog : public cTabDialog
{
protected:
	sGAMEOPTION		m_GameOption;

public :
	void UpdateData( BOOL bSave ) ;

public :
	COptionDialog() ;
	virtual ~COptionDialog() ;

	virtual void Add( cWindow* window ) ;
	virtual void SetActive( BOOL val ) ;

	void Linking() ;
	void OnActionEvent( LONG lId, void* p, DWORD we ) ;
	
	// 100112 ONS ���, ȿ���� �������� ���콺 �̺�Ʈ ó���� ���� �߰�
	virtual DWORD ActionEvent( CMouse* mouseInfo );

	// 090116 ShinJS --- ĳ���� �Ⱥ��̱� üũ�� ���� üũ�ڽ������� Disable ��Ű�� �Լ�
	void DisableOtherPetRender(BOOL bDisable);


#ifndef _JAPAN_LOCAL_
	void DisableGraphicTab(BOOL bDisable) ;
	void DisableGraphicHideParts(BOOL bDisable) ;
#endif //_JAPAN_LOCAL_
};

////class cButton;
//
//class COptionDialog : public cTabDialog
//{
//protected:
//
//	sGAMEOPTION		m_GameOption;
////	cButton*		m_pBtnPreview;
//	// 061206 LYW --- Add Flag For CheckBox. ( Chatting Option Part )
//	BOOL			m_bCheckSystem ;
//
//protected:
//
//	void UpdateData( BOOL bSave );
//public:
//
//	COptionDialog();
//	virtual ~COptionDialog();
//
//	virtual void Add(cWindow * window);
//	virtual void SetActive(BOOL val);
//
//	void Linking();
//	void OnActionEvent(LONG lId, void * p, DWORD we);
//
//	void DisableGraphicTab(BOOL bDisable);
//
//	// 2005.12.28�� �߰� �� On/Off
//	int GetEffectSnow() { return m_GameOption.nEffectSnow;}
//
//	// 061206 LYW --- Add Function to Setting checkbox.
//	void SetCheckBox( int nTabNum, LONG lId, BOOL* bOptionValue ) ;
///*
//	/// 061206 LYW --- Processing Event Functions.
//	virtual DWORD ActionEvent( CMouse* mouseInfo ) ;
//	*/
//};





#endif
