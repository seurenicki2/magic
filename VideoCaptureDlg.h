#pragma once
#include "./interface/cDialog.h"

class cComboBox;
class cCheckBox;

class CVideoCaptureDlg : public cDialog
{
	cComboBox* m_pSizeCombo;		// ������ ����
	cComboBox* m_pOptCombo;			// ȭ�� ����
	cCheckBox* m_pNoCursor;			// Ŀ�� ǥ�� ����
public:
	CVideoCaptureDlg();
	virtual ~CVideoCaptureDlg();

	// �ػ�
	enum eVideoCaptureSize
	{
		eVideoCaptureSize_Default,
		eVideoCaptureSize_500x375,
		eVideoCaptureSize_Max,
	};

	// ȭ��
	enum eVideoCaptureOpt
	{
		eVideoCaptureOpt_High,
		eVideoCaptureOpt_Low,
		eVideoCaptureOpt_Max,
	};

	void Linking();

	const char* GetTextVideoCaptureSize( eVideoCaptureSize eSize );			// �ػ󵵿� �ش��ϴ� InterfaceMsg ��ȯ
	const char* GetTextVideoCaptureOpt( eVideoCaptureOpt eOpt );			// ȭ���� �ش��ϴ� InterfaceMsg ��ȯ

	int GetSelecedSize();													// ���õ� �ػ� ��ȯ
	int GetSelecedOpt();													// ���õ� ȭ�� ��ȯ

	BOOL IsIncludeCursor();													// ��ȭ�� Ŀ�� ���� ���� ��ȯ
};