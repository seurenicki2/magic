
#pragma once
#include "CharacterDialog.h"
#include "WearedExDialog.h"
#include "Item.h"
class CPlayer;
class CQuipInfoDlg : public cDialog
{
public:
	CQuipInfoDlg(void);
	virtual ~CQuipInfoDlg(void);
	//virtual void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID = 0);
	virtual void SetActive(BOOL val);
	void Linking();
	BOOL AddItem(CItem* pItem);
	BOOL DeleteItem(CItem** ppItem);
private:
	CWearedExDialog* m_pHeroWearDlg;
};