#ifndef _NPCNOTICEDLG_
#define _NPCNOTICEDLG_

#include "cListDialogEx.h"

class cNpcNoticeDlg : public cDialog
{
	cListDialogEx*	m_pListDlg;						// NPC ��� ����Ʈ ���̾�α�.

public:
	cNpcNoticeDlg();
	virtual ~cNpcNoticeDlg();

	void Linking();

	BOOL OpenDialog( DWORD dwNpcId, WORD npcChxNum );

	virtual void SetActive( BOOL val );
};

#endif