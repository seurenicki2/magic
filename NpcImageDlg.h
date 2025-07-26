#pragma once
#include ".\interface\cdialog.h"									// ���̾�α� �������̽� Ŭ���� ��� �ҷ�����.
#include "cImageSelf.h"												// �̹��� ���� Ŭ���� ��� �ҷ�����.

#include "cStatic.h"												// ����ƽ Ŭ���� ��� �ҷ�����.

enum NPC_EMOTION													// NPC ��� �̳� �ڵ�.
{
	EMOTION_NORMAL,													// �⺻ ���.
	EMOTION_HAPPY,													// �ູ ���.
	EMOTION_SURPRISE,												// ��� ���.
	EMOTION_UNHAPPY,												// ���� ���.
	EMOTION_ANGRY,													// ȭ�� ���.

	EMOTION_MAX,													// ��� �ִ��ġ.
};

struct NPC_IMAGE													// NPC �̹��� ����ü.
{
	WORD		idx;												// �ε���.
	cImageSelf	image[5];											// �̹��� �迭.
};

class cNpcImageDlg : public cDialog									// �⺻ ���̾�α׸� ��ӹ��� NPC �̹��� ���̾�α� Ŭ����.
{
	WORD	mNpcIdx;												// NPC �ε���.
	BYTE	mEmotion;												// ��ǹ�ȣ.
	DWORD	COLOR;													// ���� ����.
	DWORD	COLOR2;													// ���� ����.

	VECTOR2	SCALE;													// ������ ����.
	VECTOR2	POS;													// ��ġ ����.
	VECTOR2 BACK_SCALE;												// ��� ������ ����.
	VECTOR2 BACK_POS;												// ��� ��ġ ����.
	cImage	mImageBack;
	NPC_IMAGE mpCurImage;
	cStatic* m_pTopImage;
	cStatic* m_pBottomImage;
	SHORT_RECT mScreenRect;

public:
	cNpcImageDlg(void);												// ������ �Լ�.
	virtual ~cNpcImageDlg(void);
	void Linking();
	void LoadingImage(WORD indx);
	void SetActive(BOOL val);
	void SetNpc(WORD NpcUniqueIdx);
	void SetEmotion(BYTE emotion);
	virtual void Render();
};