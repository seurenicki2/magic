#pragma once

#include "stdAfx.h"
#include ".\interface\cWindowHeader.h"
#include "interface/cDialog.h"

//090409 pdy �Ͽ�¡ �Ͽ콺 �˻� UI�߰�
class cHousingMainPointDlg : public cDialog  
{
	cStatic* m_pHouseName_Static;
	cStatic* m_pVisitCount_Static;
	cStatic* m_pDecoPoint_Static;

	//091012 pdy �Ͽ�¡ �ٹ̱� ����Ʈ ��ư ��ȹ����
	cButton* m_BtDecoPoint;		
	
public:
	cHousingMainPointDlg();
	virtual ~cHousingMainPointDlg();
	void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	void Linking();
	virtual void SetActive(BOOL val);

	void SetHouseName(char* szHouseName);
	void SetVisitCount(DWORD dwVisitCount);
	void SetDecoPoint(DWORD dwDecoPoint);

	//091012 pdy �Ͽ�¡ �ٹ̱� ����Ʈ ��ư ��ȹ����
	void OnActionEvent(LONG lId, void* p, DWORD we);
};
