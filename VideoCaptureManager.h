#pragma once

#include "bandicap.h"

#define VIDEOCAPTUREMGR		CVideoCaptureManager::GetInstance()

class cDialog;

// 091201 ShinJS --- ���� ��ȭ Class �߰�
class CVideoCaptureManager
{
	// �ݵ����Ʈ Library Class
	CBandiCaptureLibrary	 m_bandiCaptureLibrary;

public:
	CVideoCaptureManager();
	~CVideoCaptureManager();

	GETINSTANCE( CVideoCaptureManager );

	BOOL IsCapturing();

	BOOL CaptureStart();
	void CaptureStop();

	void Process();

	void SetCaptureOpt();
};