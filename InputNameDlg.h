// 090525 ShinJS --- Party �ٸ��ʿ� �ִ� ������ ���� �ʴ�� �̸��� �Է��ϴ� Dialog �߰�
#pragma once
#include "./interface/cDialog.h"

class cEditBox;
class cButton;

class CInputNameDlg : public cDialog
{
	cEditBox*		m_CharName;

public:
	CInputNameDlg();
	virtual ~CInputNameDlg();

	void Linking();
	virtual void OnActionEvent(LONG lId, void* p, DWORD we) ;
	virtual void SetActive(BOOL val);
};
