// 091116 ONS UiSettingManager
// ���ӷε� / ���̵��� ������ ����� UI�� ��ġ�� �����ϴ� Ŭ���� 
#pragma once
#define UISETTINGMGR USINGTON(cUiSettingManager)

struct stUiSettingState
{
	DWORD		dwID;
	BOOL		bSavePosition;
	BOOL		bSaveActivity;
};

// UI��ġ, Ȱ��ȭ �������� ����ü �߰�
struct stSettingProperty
{
	VECTOR2		vPos;
	DWORD		dwWidth;
	DWORD		dwHeight;
	BOOL		bActivity;
};

struct stUiSettingInfo
{
	DWORD				dwID;
	stSettingProperty	property;
};


class cUiSettingManager
{
	// 100304 ONS �ػ󵵺��濡 ���� ���̾�α� ��ġ ����
	enum eDispSector
	{
		eDispSector_1 = 0,
		eDispSector_2,
		eDispSector_3,
		eDispSector_4,
		eDispSector_None
	};

	RECT		m_screenRect[4];

	typedef std::vector<stUiSettingState>					UiSettingVector;
	UiSettingVector											m_UiSetting;
	
	typedef stdext::hash_map< DWORD, stSettingProperty >	UiSettingMap;
	UiSettingMap											m_UiSettingMap;

public:
	cUiSettingManager(void);
	~cUiSettingManager(void);

	void AddSettingInfo( stUiSettingInfo* pInfo );

	// ��ũ��Ʈ ���ϳ��� ���üӼ��� ��ȭ�� ������� ���������� �����Ѵ�.
	void ResetUiSettingInfo(void);

	// ���� ����� �Ǵ� ���̾�α� ����Ʈ�� �����Ѵ�.
	void SetSettingList( DWORD dwWindowID, BOOL bIsSavePos, BOOL bIsSaveActivity );

	// m_UiSettingList�� ����� ���°��� ���ؼ� ������ġ�� Ȱ��ȭ���°��� ���Ͽ� �����Ѵ�.
	void SaveSettingInfo(void);
	
	// ������ġ�� Ȱ��ȭ���°��� �ε��Ѵ�.(UiSetting.opt)
	void LoadSettingInfo(void);

	// ���̵��� m_UiSettingList�� ��ϵ� �������� ��ġ�� Ȱ��ȭ���¸� �����Ѵ�.
	void ApplySettingInfo(void);

	// ���̵� �� �α׾ƿ� ���� SettingInfo�� �����Ѵ�.
	void UpdateSettingInfo(void);

	const stSettingProperty* GetUiSettingInfo(DWORD dwWindowID);
	void SetUiPosition( DWORD dwWindowID, float fPosX, float fPosY );
	void SetUiCurActivity( DWORD dwWindowID, BOOL bCurActivity );

	// 100304 ONS �ػ󵵺��濡 ���� ���̾�α� ��ġ ����
	void SetScreenRect();
	int GetDisplaySector( const stSettingProperty* pProperty );
	VECTOR2 SetPositionRate( const stSettingProperty* pProperty, eDispSector Sector );
};

EXTERNGLOBALTON(cUiSettingManager)