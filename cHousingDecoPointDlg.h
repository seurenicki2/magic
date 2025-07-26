#pragma once

#include "stdAfx.h"
#include ".\interface\cWindowHeader.h"
#include "./Interface/cTabDialog.h"


class cListCtrl;
class cPrtList;

enum eBonusListDlgType
{
	eBonusNameList = 0,
	eDecoPointList = 1,
	eUsePointList = 2,
	eBonusListMaxType,
};

//�ٹ̱� ���ʽ� ��� UI
//090818 pdy �Ͽ�¡ UI��ɺ��� cHousingDecoPointDlg�� �Ǵ��̾˷α׷� ����
class cHousingDecoPointDlg : public cTabDialog
{
	cStatic*		m_pDecoPointStatic;
	cStatic*		m_pStarPointStatic;
	int				m_nSelectBonus;

	cPtrList		m_DecoNormalBonusList;
	cPtrList		m_DecoSpacialBonusList;

public:
	cHousingDecoPointDlg();
	virtual ~cHousingDecoPointDlg();
	virtual DWORD ActionEvent(CMouse * mouseInfo);

	void Linking();
	void SetActive( BOOL val );

	void RefreshBonusList(DWORD dwDecoPoint,DWORD dwStarPoint);
	void InitBonusList();

	stHouseBonusInfo* GetCurSelectBonusInfo();

	void Add(cWindow * window);
	virtual void Render();
	virtual void SelectTab(BYTE idx);

	// 091211 pdy ��Ʈ ������ ListDlg�� 3���� Ȯ�� 
	void OnActionEvent( LONG id, void* p, DWORD event );
	void GetListDialogArrByTabNum(cListDialog** pArr,BYTE TabNum);
};

