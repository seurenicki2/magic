#include "stdafx.h"
#include "ShoutDialog.h"
#include "WindowIDEnum.h"
#include ".\interface\cTextArea.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "FilteringTable.h"
#include "Item.h"
#include "ItemManager.h"
#include "MHTimemanager.h"

//Alemuri Link Item To Chat----------------------------------
#include "InventoryExDialog.h"
#include "Gamein.h"
//-----------------------------------------------------------

CShoutDialog::CShoutDialog()
{
	m_type = WT_SHOUT_DLG;
	m_pItem = NULL;
}

CShoutDialog::~CShoutDialog()
{}

void CShoutDialog::Linking()
{
	m_pMsgText = (cTextArea*)GetWindowForID(SHOUT_TEXTAREA_MSG);
	m_pMsgText->SetEnterAllow(FALSE);
	m_pMsgText->SetScriptText("");
}

void CShoutDialog::SetActive(BOOL val)
{
	if (m_bDisable) return;

	__time64_t forbidtime = CHATMGR->GetForbidChatTime();
	__time64_t curTime = MHTIMEMGR->GetServerTime();
	if (forbidtime > curTime)
	{
		struct tm curTimeWhen = *_localtime64(&forbidtime);
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2312), curTimeWhen.tm_year + 1900, curTimeWhen.tm_mon + 1, curTimeWhen.tm_mday, curTimeWhen.tm_hour, curTimeWhen.tm_min, curTimeWhen.tm_sec);
		return;
	}

	if (val == TRUE)
	{
		m_pMsgText->SetScriptText("");
		m_pMsgText->SetFocusEdit(TRUE);
	}
	else
	{
		m_pItem->SetLock(FALSE);
		m_pItem = NULL;
		m_pMsgText->SetScriptText("");
		m_pMsgText->SetFocusEdit(FALSE);
	}
	cDialog::SetActive(val);
}

void CShoutDialog::OnActionEvent(LONG windowIndex, LPVOID windowControl, DWORD windowEvent)
{
	switch (windowIndex)
	{
	case SHOUT_BTN_OK:
	{
		if (ITEMMGR->GetIsItemChat())
		{
			__time64_t forbidtime = CHATMGR->GetForbidChatTime();
			__time64_t curTime = MHTIMEMGR->GetServerTime();
			if (forbidtime > curTime)
			{
				struct tm curTimeWhen = *_localtime64(&forbidtime);
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2312), curTimeWhen.tm_year + 1900, curTimeWhen.tm_mon + 1, curTimeWhen.tm_mday, curTimeWhen.tm_hour, curTimeWhen.tm_min, curTimeWhen.tm_sec);
				return;
			}

			char szFullText[MAX_PATH] = { 0 };

			if (m_pMsgText)
			{
				m_pMsgText->GetScriptText(szFullText);
			}

			if (0 == _tcslen(szFullText))
			{
				break;
			}
			else if (FILTERTABLE->FilterChat(szFullText))
			{
				CHATMGR->AddMsg(
					CTC_SYSMSG,
					CHATMGR->GetChatMsg(1225));
				break;
			}

			SEND_SHOUTBASE_ITEMINFO message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_ITEM;
			message.Protocol = MP_ITEM_SHOPITEM_SHOUT_SYN_ITEM;
			message.dwObjectID = gHeroID;
			message.dwItemIdx = m_pItem->GetItemIdx();
			message.dwItemPos = POSTYPE(m_pItem->GetPosition());
			SafeStrCpy(
				message.Name,
				HERO->GetObjectName(),
				_countof(message.Name));

			WCHAR wideTextBuffer[MAX_PATH] = { 0 };
			mbstowcs(
				wideTextBuffer,
				szFullText,
				_countof(wideTextBuffer));
			wcstombs(
				message.Msg,
				wideTextBuffer,
				_countof(message.Msg));

			message.linkedItemIdx = ITEMMGR->GetItemLocked()->GetItemIdx();
			std::string separator = "ƒ";
			std::string arrayTextStr = "";
			std::string arrayColorsStr = "";
			char tempDword[8 + 1];
			unsigned int arraySizeSeparator = ITEMMGR->GetItemChatTextArray().size();
			unsigned int arrayColorsSizeSeparator = ITEMMGR->GetItemChatColorArray().size();
			for (unsigned int i = 0; i < arraySizeSeparator; ++i)
			{
				arrayTextStr += ITEMMGR->GetItemChatTextArray()[i];
				if (i < arraySizeSeparator - 1)
				{
					arrayTextStr += separator;
				}

				sprintf(tempDword, "%d", ITEMMGR->GetItemChatColorArray()[i]);
				std::string tempStr(tempDword);
				arrayColorsStr += tempStr;
				if (i < arrayColorsSizeSeparator - 1)
				{
					arrayColorsStr += separator;
				}
			}
			strcpy(message.toolTipTextArray, arrayTextStr.c_str());
			strcpy(message.toolTipColorsArray, arrayColorsStr.c_str());

			NETWORK->Send(
				&message,
				message.GetMsgLength());
			SetActive(
				FALSE);

			CInventoryExDialog* pWindow = GAMEIN->GetInventoryDialog();
			for (int num = 0; num < pWindow->GetTabNum() * 20; ++num)
			{
				CItem* item = pWindow->GetItemForPos(num);
				if (item)
				{
					if (ITEMMGR->GetItemLocked()->GetItemIdx() == pWindow->GetItemForPos(num)->GetItemIdx())
					{
						if (pWindow->GetItemForPos(num)->IsLocked())
						{
							pWindow->GetItemForPos(num)->SetLock(FALSE);
							ITEMMGR->SetIsItemChat(FALSE);
							ITEMMGR->ResetItemChatLinkOption();
							ITEMMGR->ClearItemChatTextArray();
							ITEMMGR->ClearItemChatColorArray();
						}
					}
				}
			}
			break;
		}
		else
		{
			__time64_t forbidtime = CHATMGR->GetForbidChatTime();
			__time64_t curTime = MHTIMEMGR->GetServerTime();
			if (forbidtime > curTime)
			{
				struct tm curTimeWhen = *_localtime64(&forbidtime);
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2312), curTimeWhen.tm_year + 1900, curTimeWhen.tm_mon + 1, curTimeWhen.tm_mday, curTimeWhen.tm_hour, curTimeWhen.tm_min, curTimeWhen.tm_sec);
				return;
			}

			char szFullText[MAX_PATH] = { 0 };

			if (m_pMsgText)
			{
				m_pMsgText->GetScriptText(szFullText);
			}

			if (0 == _tcslen(szFullText))
			{
				break;
			}
			else if (FILTERTABLE->FilterChat(szFullText))
			{
				CHATMGR->AddMsg(
					CTC_SYSMSG,
					CHATMGR->GetChatMsg(1225));
				break;
			}

			SEND_SHOUTBASE_ITEMINFO message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_ITEM;
			message.Protocol = MP_ITEM_SHOPITEM_SHOUT_SYN;
			message.dwObjectID = gHeroID;
			message.dwItemIdx = m_pItem->GetItemIdx();
			message.dwItemPos = POSTYPE(m_pItem->GetPosition());
			SafeStrCpy(
				message.Name,
				HERO->GetObjectName(),
				_countof(message.Name));

			WCHAR wideTextBuffer[MAX_PATH] = { 0 };
			mbstowcs(
				wideTextBuffer,
				szFullText,
				_countof(wideTextBuffer));
			wcstombs(
				message.Msg,
				wideTextBuffer,
				_countof(message.Msg));

			NETWORK->Send(
				&message,
				message.GetMsgLength());
			SetActive(
				FALSE);
			break;
		}
	}
	case SHOUT_BTN_CANCEL:
	{
		SetActive(
			FALSE);
		break;
	}
	}
}

BOOL CShoutDialog::FakeMoveIcon(LONG x, LONG y, cIcon * pOrigIcon)
{
	if (WT_ITEM != pOrigIcon->GetType() || pOrigIcon->IsLocked() || m_bDisable)
	{
		return FALSE;
	}

	CItem * pOrigItem = (CItem *)pOrigIcon;
	const DWORD				itemIndex = pOrigItem->GetItemIdx();
	const ITEM_INFO*		info = ITEMMGR->GetItemInfo(itemIndex);
	const ITEM_OPTION&		itemoption = ITEMMGR->GetOption(pOrigItem->GetItemBaseInfo());

	if (pOrigItem && info)
	{
		char msgorigin[256] = { 0 };
		char szFullText[MAX_PATH] = { 0 };

		if (m_pMsgText)
		{
			m_pMsgText->GetScriptText(szFullText);
			sprintf(msgorigin, "%s", szFullText);
		}

		char buff[128] = { 0 };
		if (itemoption.mEnchant.mLevel > 0)
		{
			sprintf(buff, "+%d%s", itemoption.mEnchant.mLevel, info->ItemName);
		}
		else
		{
			sprintf(buff, "%s", info->ItemName);
		}

		char buffer[MAX_PATH] = { 0 };
		sprintf(buffer, "%s %s", msgorigin, buff);
		m_pMsgText->SetScriptText(buffer);
	}

	return FALSE;
}