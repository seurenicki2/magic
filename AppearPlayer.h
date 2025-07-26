#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: AppearPlayer.h
//	DESC		: ĳ������ ��� ó���� �����ϴ� Ŭ����.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//		PREPROCESSOR PART.
//-------------------------------------------------------------------------------------------------
enum GENDER_TYPE
{
	eMAN	= 0,
	eWOMAN,
} ;

// iros --- � FLAG�� �Ѱ���� �ұ� ;;;
enum CHANGE_APPEAR_FLAG
{
	eCHAPPEAR_NONE = 0x00,
	eCHAPPEAR_MOD = 0x01,
	eCHAPPEAR_PART  = 0x02,
	eCHAPPEAR_WEAPON  = 0x04,
	eCHAPPEAR_SCALE  = 0x08,
	eCHAPPEAR_EFFECT  = 0x10,
	eCHAPPEAR_OPTION  = 0x20,
	eCHAPPEAR_INIT = 0xFF,
};

//-------------------------------------------------------------------------------------------------
//		INCLUDE PART.
//-------------------------------------------------------------------------------------------------
class CEngineObject ;
class CPlayer ;

struct stFLAG_SHOWPART
{
	BYTE IsShowFace ;
	BYTE IsShowHelmet ;
	BYTE IsShowGlasses ;
	BYTE IsShowMask ;
	BYTE IsShowWeapon2 ;
	BYTE IsShowShield2 ;
} ;





//-------------------------------------------------------------------------------------------------
//		THE CLASS CAPPEARPLAYER
//-------------------------------------------------------------------------------------------------
class CAppearPlayer
{
private :
	stFLAG_SHOWPART			m_stShowFlag ;

	BOOL					m_IsForEffect ;
	BOOL					m_viewmode;

	CPlayer*				m_pPlayer ;
	CEngineObject*			m_pEngineObject ;

	CHARACTER_TOTALINFO*	m_pTotalInfo ;

private :
	void InitData() ;
	void InitShowFlag() ;

	BYTE InitMOD() ;
	BYTE InitHead() ;
	BYTE ChangePart() ;
	BYTE AttachPart() ;
	BYTE InitScale() ;
	BYTE InitPos() ;
	BYTE AppearWeapone() ;

	// 2009.02.20 nsoh �ڽ�Ƭ ���� ����� ��ǥ�� ó��
	BYTE InitHair();

public:
	CAppearPlayer(void) ;
	~CAppearPlayer(void) ;

	void SetForEffect(BOOL isForEffect)	;
	void SetPlayer( CPlayer* pPlayer ) ;
	void SetEngineObject( CEngineObject* pEngineObject)	;

	void SetBaseAppearPlayer();
	void RenewalAll();
	void Renewal(DWORD changeFlag);
	void Mviewmode(BYTE);
	//void Renewal();

	// 090530 ONS �ű����� ��������ۿ��� 2���� MOD���ϸ��� ��� �Լ�
	BOOL GetModFromChx(char* pFilename, char* RObj, char* LObj);
};
