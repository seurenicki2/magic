#include "stdafx.h"
#include "cHouseSearchDlg.h"
#include "WindowIDEnum.h"
#include "cImeEx.h"
#include "cWindowManager.h"
#include "cMsgBox.h"
#include "../ChatManager.h"
#include "InventoryExDialog.h"
#include "GameIn.h"
#include "Item.h"
#include "ObjectStateManager.h"
#include "ObjectManager.h"
#include "FilteringTable.h"

//090409 pdy �Ͽ�¡ �Ͽ콺 �˻� UI�߰�
cHouseSearchDlg::cHouseSearchDlg()
{
	m_type = WT_HOUSE_SEARCH_DLG;
}

cHouseSearchDlg::~cHouseSearchDlg()
{

}

void cHouseSearchDlg::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
	m_type = WT_MINIFRIENDDLG;

	m_VisitList.clear();
}

void cHouseSearchDlg::Linking()
{
	m_pSearchName = (cStatic*)GetWindowForID(HOUSE_SEARCH_NAME);
	m_pSearchNameEdit = (cEditBox*)GetWindowForID(HOUSE_SEARCH_NAMEEDIT);
	m_pSearchNameEdit->SetValidCheck(VCM_CHARNAME/*VCM_DEFAULT*/);
	m_pSearchNameEdit->SetEditText("");
	m_pSearchBtn = (cButton*)GetWindowForID(HOUSE_SEARCH_SEARCHBTN);
	m_pSearchRandomBtn = (cButton*)GetWindowForID(HOUSE_SEARCH_RANDOMBTN);
	m_pVisitList = (cComboBox*)GetWindowForID(HOUSE_SEARCH_VISITLIST);

	LoadVisitList();
}

void cHouseSearchDlg::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	cDialog::SetActive(val);
	cDialog::SetActiveRecursive(val);

	m_pSearchNameEdit->SetFocusEdit(val);

	if(val)
		m_pSearchNameEdit->SetEditText("");
	else
	{	
		if(eHouseVisitByItem == m_VisitInfo.m_dwVisitKind )
		{
			CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
			if(pDlg)
			{
				pDlg->SetDisable( FALSE );

				CItem* pItem = pDlg->GetItemForPos(m_VisitInfo.m_dwData2);

				if( pItem )
					pItem->SetLock(FALSE);
			}
		}			

		m_VisitInfo.Clear();
	}
}

void cHouseSearchDlg::SetName(char* Name)
{
	m_pSearchNameEdit->SetEditText(Name);
}

char* cHouseSearchDlg::GetName()
{
	return m_pSearchNameEdit->GetEditText();
}

void cHouseSearchDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	switch(lId)
	{
		case HOUSE_SEARCH_NAMEEDIT:
		{
			if(we == WE_RETURN)
			{
				char* name = "";
				name = m_pSearchNameEdit->GetEditText();
				int len = strlen(name);

				//090605 �Ͽ콺 �˻��� ��ȿ�����ʴ� ĳ�����̸� �˻� �߰� 
				BOOL bCheck = FALSE;

				if( len == 0 )
				{
					return;
				}
				else if( len < 4 )
				{
					bCheck = TRUE;
				}
				else if( len > MAX_NAME_LENGTH )
				{
					bCheck = TRUE;
				}
				
				if( FILTERTABLE->IsInvalidCharInclude((unsigned char*)name) )
				{
					bCheck = TRUE;	
				}

				if( !FILTERTABLE->IsUsableName(name) )
				{
					bCheck = TRUE;	
				}

				if( bCheck == TRUE )
				{
					//090605 pdy �Ͽ�¡ �ý��۸޼��� �Ͽ콺 �˻��� ����Ҽ����� �̸� [�Ͽ콺 �˻��湮]  
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 551 ) );		//551	"����� �� ���� �̸��Դϴ�."
					return;
				}

				char MsgBuf[128] = {0,};
				sprintf(MsgBuf,CHATMGR->GetChatMsg(1890), name); //1890	"%s�� ������ �̵� �Ͻðڽ��ϱ�?"

				//090527 pdy �Ͽ�¡ �˾�â [�̸��˻��Ͽ콺�湮]
				WINDOWMGR->MsgBox( MBI_HOUSESEACH_AREYOUSURE, MBT_YESNO, MsgBuf ) ;	//1890	"%s�� ������ �̵� �Ͻðڽ��ϱ�?"
				SetDisableState(TRUE) ;
			}
		}
		break;

		case HOUSE_SEARCH_SEARCHBTN :
			{
				char* name = "";
				name = m_pSearchNameEdit->GetEditText();
				int len = strlen(name);

				//090605 �Ͽ콺 �˻��� ��ȿ�����ʴ� ĳ�����̸� �˻� �߰� 
				BOOL bCheck = FALSE;

				if( len == 0 )
				{
					return;
				}
				else if( len < 4 )
				{
					bCheck = TRUE;
				}
				else if( len > MAX_NAME_LENGTH )
				{
					bCheck = TRUE;
				}
				
				if( FILTERTABLE->IsInvalidCharInclude((unsigned char*)name) )
				{
					bCheck = TRUE;	
				}

				if( !FILTERTABLE->IsUsableName(name) )
				{
					bCheck = TRUE;	
				}

				if( bCheck == TRUE )
				{
					//090605 pdy �Ͽ�¡ �ý��۸޼��� �Ͽ콺 �˻��� ����Ҽ����� �̸� [�Ͽ콺 �˻��湮]  
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 551 ) );		//551	"����� �� ���� �̸��Դϴ�."
					return;
				}

				char MsgBuf[128] = {0,};
				sprintf(MsgBuf,CHATMGR->GetChatMsg(1890), name); //1890	"%s�� ������ �̵� �Ͻðڽ��ϱ�?"

				//090527 pdy �Ͽ�¡ �˾�â [�̸��˻��Ͽ콺�湮]
				WINDOWMGR->MsgBox( MBI_HOUSESEACH_AREYOUSURE, MBT_YESNO, MsgBuf ) ;	//1890	"%s�� ������ �̵� �Ͻðڽ��ϱ�?"
				SetDisableState(TRUE) ;
			}
			break;
		//090618 pdy �Ͽ�¡ ����߰� �������� 
		case HOUSE_SEARCH_RANDOMBTN :
			{
				//090618 pdy �Ͽ�¡ �˾�â [�Ͽ콺��������]
				WINDOWMGR->MsgBox( MBI_VISIT_RANDOM_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1925) ) ;	//1890	"%s�� ������ �̵� �Ͻðڽ��ϱ�?"
				SetDisableState(TRUE) ;
			}
			break;

		// 091214 pdy �޺�����Ʈ�� Edit�ڽ��� ���ڰ� ��ġ�� ���� ����
		case HOUSE_SEARCH_VISITLIST:
			{
				if( we != WE_COMBOBOXSELECT )
					break;

				char name[128] = {0,};
				strcpy(name, m_pVisitList->GetComboText());
				m_pSearchNameEdit->SetEditText(name);
				m_pVisitList->DeleteComboText();
			}
			break;
	}
}


void cHouseSearchDlg::SetDisableState(BOOL val)
{
	if( val )
	{
		OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal) ;
	}
	else
	{
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
	}

	SetDisable(val) ;
}

void cHouseSearchDlg::LoadVisitList()
{
	FILE* fp;
	fp = fopen("./System/HouseVisitList.txt", "rt");
	if(! fp)
		return;

	char name[MAX_NAME_LENGTH] = {0,};

	while(0 == feof(fp))
	{
		fscanf(fp, "%s\n", name);
		stVisitName visit;
		strcpy(visit.name, name);
		m_VisitList.push_back(visit);
	}
	m_VisitList.reverse();
	fclose(fp);

	if(m_VisitList.size() > 10)
	{
		// ������ ����� �ʰ��ߴ�.
		// ��������� ����
		m_VisitList.clear();	
		_unlink("./System/HouseVisitList.txt");
		return;
	};

	std::list<stVisitName>::reverse_iterator rIter;
	int i;
	for(rIter=m_VisitList.rbegin(), i=0; rIter!=m_VisitList.rend(); rIter++, i++)
	{
		char* name = rIter->name;
		ITEM* item = m_pVisitList->GetItem(i);
		strcpy(item->string, name);
	}
}

void cHouseSearchDlg::AddVisitList()
{
	// 091124 NYJ �湮��� �߰�
	int num = m_VisitList.size();
	if(num > 9)
		m_VisitList.pop_front();

	stVisitName visit;
	strcpy(visit.name, GetName());
	
	// �ߺ��˻�
	std::list<stVisitName>::iterator iter;
	for(iter=m_VisitList.begin(); iter!=m_VisitList.end(); iter++)
	{
		if(0 == strcmp(visit.name, iter->name))
		{
			m_VisitList.erase(iter);
			break;
		}
	}

	// �ڿ� �߰�
	m_VisitList.push_back(visit);

	FILE* fp;
	fp = fopen("./System/HouseVisitList.txt", "wt");
	if(! fp)
		return;

	std::list<stVisitName>::reverse_iterator rIter;
	int i;
	for(rIter=m_VisitList.rbegin(), i=0; rIter!=m_VisitList.rend(); rIter++, i++)
	{
		ITEM* item = m_pVisitList->GetItem(i);
		strcpy(item->string, rIter->name);
		fprintf(fp, "%s\n", rIter->name);
	}
	fflush(fp);
	fclose(fp);
}