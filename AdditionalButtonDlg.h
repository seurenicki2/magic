#pragma once
#include "./interface/cDialog.h"

class cButton;

class cAdditionalButtonDlg : public cDialog
{
	// �δ�, ����Ʈ��Ī �� �̴ϸ��� ǥ�õ��� �ʴ� �ʿ��� 
	// �������ȭ�� ���ù�ư�� ����ϵ��� �Ѵ�.
	cButton* m_pButtonRecordStart;
	cButton* m_pButtonRecordStop;

public:
	cAdditionalButtonDlg();
	virtual ~cAdditionalButtonDlg();

	void Linking();
	virtual void OnActionEvent(LONG lId, void* p, DWORD we) ;
	virtual void SetActive(BOOL val);

	void ShowRecordStartBtn( BOOL val );
};