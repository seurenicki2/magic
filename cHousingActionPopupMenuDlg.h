#pragma once
#include "stdafx.h"
#include "./interface/cdialog.h"

class cStatic;
class cButton;


//�˾��Ŵ� �� ���Կ� ���� ���� 
struct stActionPopupMenuSlot
{
	cButton* m_pBtAction;
	stHouseActionInfo*	m_pActionInfo;

	stActionPopupMenuSlot()
	{
		memset(this, 0, sizeof(stActionPopupMenuSlot));
	}
};

//�����׼� �˾��Ŵ�UI
class cHousingActionPopupMenuDlg :
	public cDialog
{
	stFurniture* m_pstSelectedFurniture;
	stActionPopupMenuSlot m_PopupMenuSlotArr[MAX_HOUSE_ACTION_SLOT];

	//��Ƽ��Ǵ� ������ ���콺�����Ϳ� ����� ���� �ѹ��� �����Ͽ� �ִ´�. 
	DWORD m_bySortedNearBoneNumArr[MAX_HOUSE_ATTATCH_SLOT];

public:
	cHousingActionPopupMenuDlg(void);
	virtual ~cHousingActionPopupMenuDlg(void);

	void Linking();
	virtual void OnActionEvent(LONG lId, void* p, DWORD we) ;
	virtual void SetActive(BOOL val);
	virtual DWORD ActionEvent(CMouse *mouseInfo);

	DWORD* GetSortedNearBoneNumArr(){return m_bySortedNearBoneNumArr;} 
};
