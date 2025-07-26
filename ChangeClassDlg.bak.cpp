#include "stdafx.h"
#include "WindowIDEnum.h"
#include "./Interface/cResourceManager.h"
#include "./interface/cStatic.h"
#include "./interface/cComboBox.h"
#include "ObjectManager.h"
#include "ChangeClassDlg.h"
#include "InventoryExDialog.h"
#include "GameIn.h"
#include "ItemManager.h"
#include "Interface/cWindowManager.h"
#include "ChatManager.h"
#include "cMsgBox.h"
#include "MHMap.h"

void PressSubmitInMessageBox(LONG, LPVOID, DWORD windowEvent)
{
	CInventoryExDialog* pInvenDlg = GAMEIN->GetInventoryDialog() ;
	if( !pInvenDlg )
	{
		return;
	}

	CItem* pItem = pInvenDlg->Get_QuickSelectedItem() ;
	if( !pItem )
	{
		return;
	}

	if( MBI_YES == windowEvent)
	{
		ITEMMGR->UseItem_ChangeClass( pItem );
		pItem->SetLock(FALSE) ;
	}

	pItem->SetLock(FALSE) ;
	pInvenDlg->Set_QuickSelectedItem(NULL) ;
}

CChangeClassDlg::CChangeClassDlg()
{
	for( int i = 0; i < MAX_CLASS_LEVEL; i++ )
	{
		m_pClassKindStatic[i] = NULL;
		m_pClassKindCombo[i] = NULL;
	}
}

CChangeClassDlg::~CChangeClassDlg()
{

}

void CChangeClassDlg::Linking()
{
	// ������ Ÿ��Ʋ
	for( int i = 0; i < MAX_CLASS_LEVEL; i++ )
	{
		char buf[32] = {0,};
		m_pClassKindStatic[i] = ( cStatic* )GetWindowForID( CHANGE_CLASS_STATIC_1ST + i );
		sprintf(buf, "[ %d%s ]", i + 1, RESRCMGR->GetMsg(1446));
		m_pClassKindStatic[i]->SetStaticText(buf);
	}

	// �⺻������
	m_pFirstClassKind = ( cStatic* )GetWindowForID( CHANGE_CLASS_FIX_1ST );
		
	// 1���������� ������
	for( int i = 0; i < MAX_CLASS_LEVEL - 1; i++ )
	{
		m_pClassKindCombo[i] = ( cComboBox* )GetWindowForID(CHANGE_CLASS_COMBOBOX_2ND + i);
	}
}

void CChangeClassDlg::SetActive( BOOL val )
{
	if(MAP->GetMapNum() == StreetTournament && val)
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, "Change Option is disable at this map" );
		val = FALSE;
	}

	if( val )
	{
		SettingClassList();
	}
	cDialog::SetActive( val );
}

void CChangeClassDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	switch( lId )
	{
	case CHANGE_CLASS_BTN_SUBMIT:
		{
			cMsgBox* const messageBox = WINDOWMGR->MsgBox(
				MBI_CHANGE_CLASS,
				MBT_YESNO,
				CHATMGR->GetChatMsg(2224));

			if(0 == messageBox)
			{
				break;
			}

			SetActive(
				FALSE);
			messageBox->SetMBFunc(
				PressSubmitInMessageBox);
			SetDisable(
				TRUE);
		}
		break;
	case CHANGE_CLASS_BTN_CANCEL:
		{
			CInventoryExDialog* pInvenDlg = GAMEIN->GetInventoryDialog() ;
			if( !pInvenDlg ) return ;

			CItem* pItem = pInvenDlg->Get_QuickSelectedItem() ;
			if( !pItem ) return ;

			pItem->SetLock(FALSE) ;
			pInvenDlg->Set_QuickSelectedItem(NULL) ;
			SetActive(FALSE);
		}
		break;
	}
}

void CChangeClassDlg::SettingClassList()
{
	const BYTE MAX_CLASS_COUNT = 24;

	// �� ����, ������ ���� ������ ����� �����Ѵ�. ���� 5���������� ����.
	struct ClassInfo
	{
		WORD	wClassNo;				// Ŭ���� ��ȣ( InterfaceMsg�� ����)
		BYTE	byStage;				// �����ܰ�(1 ~ 5)
		BYTE	byRace;					// ����(0:���� 1:�޸� 2:����)
	}
	const stClassInfo[eClass_MAXCLASS][MAX_CLASS_COUNT] = 
	{
		{
			{364, 1, 0}, {365, 2, 0}, {366, 2, 0}, {1738, 2, 3}, {367, 3, 0}, {368, 3, 0},
			{369, 3, 1}, {1739, 3, 3}, {370, 4, 0}, {371, 4, 0}, {372, 4, 1}, {373, 4, 2},
			{1740, 4, 3}, {374, 5, 0}, {375, 5, 0}, {376, 5, 0}, {377, 5, 1}, {378, 5, 0},
			{379, 5, 2}, {1741, 5, 3},  {380, 6, 0}, {381, 6, 1}, {382, 6, 2}, {1742, 6, 3},
		},
		{
			{383, 1, 0}, {384, 2, 0}, {385, 2, 0}, {1743, 2, 3}, {386, 3, 0}, {387, 3, 1}, 
			{388, 3, 2}, {1744, 3, 3}, {389, 4, 0}, {390, 4, 0}, {391, 4, 1}, {392, 4, 2}, 
			{1745, 4, 3}, {393, 5, 0}, {394, 5, 0}, {395, 5, 1}, {396, 5, 2}, {1106,5, 0}, 
			{1746, 5, 3}, {397, 6, 0}, {398, 6, 1}, {399, 6, 2}, {1747, 6, 3}, {  0, 0, 0},
		},
		{
			{400, 1, 0}, {401, 2, 0}, {402, 2, 0}, {1748, 2, 3}, {403, 3, 0}, {404, 3, 0}, 
			{405, 3, 1}, {1749, 3, 3}, {406, 4, 0}, {407, 4, 0}, {408, 4, 1}, {409, 4, 2},
			{1750, 4, 3}, {410, 5, 0}, {411, 5, 0}, {412, 5, 0}, {413, 5, 1}, {414, 5, 2},  
			{1751, 5, 3}, {415, 6, 0}, {416, 6, 1}, {417, 6, 2}, {1752, 6, 3}, {  0, 0, 0},
		}
	};


	// ���������� ������� ĳ���� ������ ��´�.
	CHARACTER_TOTALINFO ChaInfo;
	OBJECTMGR->GetHero()->GetCharacterTotalInfo(&ChaInfo);
	const BYTE byClassType = ChaInfo.Job[0] - 1;
		
	// �⺻������ ��������Ѵ�.
	m_pFirstClassKind->SetStaticText(RESRCMGR->GetMsg( stClassInfo[byClassType][0].wClassNo ));

	// �ʱ�ȭ
	for( int i = 0; i < MAX_CLASS_LEVEL-1; i++ )
	{
		m_pClassKindCombo[i]->RemoveAll();
		m_pClassKindCombo[i]->SetDisable(FALSE);
		m_pClassKindStatic[i+1]->SetFGColor( RGBA_MAKE( 255, 255, 255, 255 ) );
	}

	int nIndex = 1;
	int nLine = 0;
	BYTE byOldStage = 0;
	BYTE bySelectIndex = 0;
	// ���� ���� ���������� �޺��ڽ��� ó���Ѵ�.
	while(	stClassInfo[byClassType][nIndex].wClassNo > 0 && 
			stClassInfo[byClassType][nIndex].byStage <= ChaInfo.JobGrade )
	{
		BYTE byCurStage = stClassInfo[byClassType][nIndex].byStage - 1;
		if( byOldStage != byCurStage )
		{
			// ���� �ܰ谡 �����������,
			nLine = 0;
			bySelectIndex = 0;
			byOldStage = byCurStage;
		}
		
		ITEM* pItem = new ITEM ;
		pItem->line = nLine;
		pItem->rgb = RGBA_MAKE(255, 255, 255, 255) ;
		strcpy(pItem->string, RESRCMGR->GetMsg( stClassInfo[byClassType][nIndex].wClassNo ));

		// ����Ʈ�� �������� �߰��ϰ�, ���� ���������� ǥ���Ѵ�.
		if( stClassInfo[byClassType][nIndex].byRace == 0 || 
			stClassInfo[byClassType][nIndex].byRace == ChaInfo.Race + 1 )
		{
			m_pClassKindCombo[ byCurStage - 1 ]->AddItem(pItem) ;

			if( nLine == ChaInfo.Job[byCurStage] - 1 )
			{
				m_pClassKindCombo[byCurStage - 1]->SetCurSelectedIdx( bySelectIndex );
				m_pClassKindCombo[byCurStage - 1]->SelectComboText( (WORD)bySelectIndex );
			}

			m_pClassKindCombo[byCurStage - 1]->SetComboTextColor(RGBA_MAKE(0, 255, 255, 255));
			bySelectIndex++;
		}

		nIndex++;
		nLine++;
	}

	// ���� �����ܰ�� ��Ȱ��ȭ ó���Ѵ�.
	for( int nIdx = ChaInfo.JobGrade; nIdx <= MAX_CLASS_LEVEL - 1; nIdx++ )
	{
		m_pClassKindStatic[nIdx]->SetFGColor( RGBA_MAKE( 165, 165, 165, 255 ) );
		m_pClassKindCombo[nIdx - 1]->SetDisable(TRUE);
	}
}

int CChangeClassDlg::GetCurSelectedIdx( WORD wStage )
{
	return m_pClassKindCombo[wStage]->GetCurSelectedIdx();
}

ITEM* CChangeClassDlg::GetItem( WORD wStage, int nIndex )
{
	return m_pClassKindCombo[wStage]->GetItem(nIndex);
}
