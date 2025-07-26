#include "stdafx.h"
#include "OtherCharacterEquip.h"
#include "CharacterDialog.h"
#include "ObjectManager.h"
#include "./Interface/CWindowManager.h"
#include "./Interface/cStatic.h"
#include "./Interface/cButton.h"
#include "WindowIDEnum.h"
#include "GuildManager.h"
#include "CheatMsgParser.h"

#include "GameIn.h"
#include "ChatManager.h"
#include "itemManager.h"

//void CQuipInfoDlg::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID/*=0*/)
//{
//	cDialog::Init(x, y, wid, hei, basicImage, ID);
//}

void CQuipInfoDlg::SetActive(BOOL val)
{
	if (m_bDisable) return;

	cDialog::SetActiveRecursive(val);
}

void CQuipInfoDlg::Linking()
{
	m_pHeroWearDlg = (CWearedExDialog*)GetWindowForID(TQ_WEAREDDLG);
}

CQuipInfoDlg::CQuipInfoDlg(void)
{
	m_type = WT_SHOWEQUIPDLG;
}

CQuipInfoDlg::~CQuipInfoDlg(void)
{

}
BOOL CQuipInfoDlg::AddItem(CItem* pItem)
{
	if (!pItem)
		return FALSE;

	if (TP_WEAR_START <= pItem->GetPosition() && pItem->GetPosition() < TP_WEAR_END)
	{
		// ÀåÂøÃ¢
		return m_pHeroWearDlg->AddIcon(pItem->GetPosition() - TP_WEAR_START, pItem);
	}
	return FALSE;
}

BOOL CQuipInfoDlg::DeleteItem(CItem** ppItem)
{
	POSTYPE Pos;
	Pos = (*ppItem)->GetPosition();
	if (TP_WEAR_START <= Pos && Pos < TP_WEAR_END)
	{
		if (!m_pHeroWearDlg->IsAddable(Pos - TP_WEAR_START))
		{
			return m_pHeroWearDlg->DeleteIcon(Pos - TP_WEAR_START, (cIcon **)ppItem);
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}