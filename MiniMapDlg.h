#pragma once


#include "Interface/cDialog.h"
#include "MiniMapHeroIcon.h"
#include "cImageSelf.h"

class cStatic;
class cPushupButton;
class cChannelComboBox;

struct MINIMAPIMAGE
{
	cImageSelf	image;
	SIZE		size;
};

class CMiniMapDlg : public cDialog  
{
	cStatic* m_pMapName;
	// 080121 LYW --- MiniMapDlg : ä�ο� ����ƽ �߰�.
	cStatic* m_pPosXY;	

	cPushupButton* m_pLevelUpBTN;
	DWORD		   m_dwLevelUPBTNStartTime ;

	// 100614 ü�� ���� �޺��ڽ�
	cChannelComboBox*	m_pChannelComboBox;

	// 091210 ShinJS --- ��ȭ ��ư �߰�
	cButton*	m_pVideoCaptureStartBtn;
	cButton*	m_pVideoCaptureStopBtn;

	MAPTYPE m_MapNum;

	CMiniMapHeroIcon m_HeroIcon;	
	CYHHashTable<CMiniMapIcon> m_IconTable;
	CMemoryPoolTempl<CMiniMapIcon> m_IconPool;
	CMiniMapIconImage m_pIconImage[eMNMICON_MAX];
	// 090909 ShinJS --- ��ã��� Move Point ��¿� CMiniMapIcon
	CMiniMapIcon m_MovePoint;
	
	int m_CurMode;
	MINIMAPIMAGE m_MinimapImage[eMINIMAPMODE_MAX];

	// ���� 070522
	struct stMiniMap
	{
		// cImageSelf*�� ���� ����ϴ� �̹����� �����ؼ� �� ��� DLL���� Access violation �߻�. 
		// �̾� cImage�� ��������Ʈ �̹����� �����ϴ� ��� ���
		cImage mImage;
		cImageRect mRect;
		VECTOR2 mScale;
		
		cImageSize mViewport;
		VECTOR2 mTranslation;
	}
	m_MiniMap;

	cImage m_TooltipImage;

	void InitIconImage();

	void SetZoomRate(float zoomrate);

	void AddIcon( WORD Kind, DWORD dwObjectID, VECTOR3* pObjectPos, CObject* pObject, PARTY_MEMBER* pMember, CSHFarm* pFarm );
	void ReleaseMinniMapIcon(CMiniMapIcon* pIcon);
	void LoadMinimapImageInfo(MAPTYPE MapNum);

public:
	CMiniMapDlg();
	virtual ~CMiniMapDlg();
	virtual DWORD ActionEvent( CMouse* mouseInfo );
	void InitMiniMap(MAPTYPE MapNum);
	
	void Linking();
	virtual void SetActive( BOOL val );

	void AddMonsterIcon(CObject* pObject);
	void AddBossMonsterIcon(CObject* pObject);
	void AddStaticNpcIcon(CObject* pObject);

	void AddPartyMemberIcon(PARTY_MEMBER* pMember);
	void AddPartyMasterIcon(CObject* pObject);
	void AddQuestIcon( CObject* pTarget, int nKind );
	void AddHeroIcon(CObject* pObject);
	void AddFarmIcon(CSHFarm* pFarm);
	void RemoveIcon(DWORD ObjectID);

	void Process();
	void Render();

	void SetPartyIconTargetPos(DWORD PlayerID, int posX, int posZ);
	void SetPartyIconObjectPos(DWORD PlayerID, int posX, int posZ);
	
	int GetCurMode()	{ return m_CurMode; }
	
	BOOL CanActive();

	void UpdateQuestMark();

	// 090909 ShinJS --- ��ã�� �۾�
	void ClearMovePoint() { m_MovePoint.ShowQuestMark( eQM_NONE ); }		// Move Point ����(����»��·� ����)
	void SetMovePoint( VECTOR3* pDestPos );									// Move Point ����

	// 091209 ShinJS --- ��ã�� ����
	void Move_UsePath();

	// 091210 ShinJS --- ���� ��ȭ ��ư �߰�
	void ShowVideoCaptureStartBtn( BOOL bShow );

	// 100111 ONS �̴ϸ��� ǥ������ �ʴ� ������ ���θ� �˷��ش�.
	BOOL HaveMinimap() { return !m_MiniMap.mImage.IsNull(); }

	// 100610 ������ ��ư�� ��ȿ���� �˻�
	void ChangeLevelUpBTNState();

	// ������ ��ư Disable���� �˻�
	void ChangePushLevelUpBTN();

	// ������ ��ư Ȱ��ȭ�Ǹ� Alpha ����
	void ActiveLevelUpButton();
	
	void SetChanelComboBox( MSG_CHANNEL_INFO* pInfo );

	void RequestChannelInfo();

	void ChangeChannel();
};