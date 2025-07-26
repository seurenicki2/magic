// WeatherManager.h: interface for the CWeatherManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEATHERMANAGER_H__5EB34DB4_AE8E_4654_B1FB_2A4A701DAFB1__INCLUDED_)
#define AFX_WEATHERMANAGER_H__5EB34DB4_AE8E_4654_B1FB_2A4A701DAFB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ENGINE\EngineEffect.h"

#define WEATHERMGR	USINGTON(CWeatherManager)

class CWeatherManager
{
	enum eWeatherEffectRenderState{
		eWeatherEffectRenderState_None,
		eWeatherEffectRenderState_Render,		// Effect Render ����
		eWeatherEffectRenderState_Create,		// Effect Create ����, ���� �ѷ������� ����
		eWeatherEffectRenderState_Delete,		// Effect Delete ����, ���� ������� ����
	};

	// Effect ������ Rotate Type, 0 �̻��� ������ ������ ����
	enum eWeatherEffectRotateType{
		eWeatherEffectRotateType_Random = -1,
		eWeatherEffectRotateType_PlayerLook = -2,
		eWeatherEffectRotateType_CameraLook = -3,
	};

	// Effect ������ �Ÿ� Type,
	enum eWeatherEffectCreatePosType{
		eWeatherEffectCreatePosType_Camera,
		eWeatherEffectCreatePosType_Player,
	};

	struct stWeatherEffectObject{
		stWeatherEffectObject()
		{
			vecPos.x = vecPos.y = vecPos.z = 0.0f;
			eState = eWeatherEffectRenderState_None;
			dwTime = 0;
		}

		VECTOR3 vecPos;							// Effect�� ��ġ
		eWeatherEffectRenderState eState;		// ���� ����
		DWORD dwTime;							// ���� ���� �ð�
		CEngineEffect effecObj;
	};

	struct stWeatherEffectData{

		stWeatherEffectData()
		{
			dwHashCode = 0;
			ZeroMemory( hashCodeTxt, MAX_PATH );
			ZeroMemory( effectFileName, MAX_PATH );
			wMaxObjectCnt = 0;
			fCreateTime = 200.f;
			fRenderTime = 5000.f;
			fDeleteTime = 1000.f;
			fRenderValidDist = 0.f;
			nRotate = 0;
			eCreatePosType = eWeatherEffectCreatePosType_Camera;
			fCreateDist = 0;
			fCreateRangeMin = fCreateRangeMax = 0.f;
			fScaleMin = fScaleMax = 0.f;
			dwValidCreateTime = 0;
			dwCreateDelay = 0;
			bBillboard = FALSE;

			// ---- weather custom 
			wCID = 0;
		}

		std::map<int, stWeatherEffectObject> objectMap;
		std::deque<int> usableIndex;

		DWORD dwHashCode;
		char hashCodeTxt[MAX_PATH];

		char effectFileName[MAX_PATH];	// Effect File Name
		WORD wMaxObjectCnt;				// EngineObject ����

		float fCreateTime;				// Create State Render Time
		float fRenderTime;				// Render State Render Time
		float fDeleteTime;				// Delete State Render Time

		float fRenderValidDist;			// Render ��ȿ �Ÿ�
		int nRotate;					// Rotation ����

		eWeatherEffectCreatePosType eCreatePosType;			// ī�޶��� ��ǥ�� �˻���ġ�� ������ ������ �Ǵ�
		float fCreateDist;				// �˻���ġ�κ��� ������ ����� �Ÿ�
		float fCreateRangeMin;			// ���� ����
		float fCreateRangeMax;

		float fScaleMin;				// Scale ����
		float fScaleMax;

		DWORD dwValidCreateTime;		// ���� ���� �ð�
		DWORD dwCreateDelay;			// ���� Delay

		BOOL bBillboard;

		// ---- weather custom 
		WORD wCID;
	};


	typedef std::pair<DWORD, int> WeatherObjectKey;
	typedef std::deque<WeatherObjectKey> WeatherRenderStateQue;

	WeatherRenderStateQue m_CreateEffectQue;
	WeatherRenderStateQue m_RenderEffectQue;
	WeatherRenderStateQue m_DeleteEffectQue;

	typedef std::map<DWORD, stWeatherEffectData> WeatherEffectDataMap;
	WeatherEffectDataMap m_WeatherEffectDataMap;

	BOOL m_bEffectOn;						// Effect On/Off ����
	DWORD m_CurEffectWeatherHashCode;		// ���� Effect ���� ����
    WORD m_wIntensity;						// ����

	void LoadScript();
	void AddCreateStateEffect( stWeatherEffectData& data, VECTOR3& vecCheckPos );

public:
	CWeatherManager();
	virtual ~CWeatherManager();
	void Init();
	void Release();
	void Process();
	BOOL EffectOn( LPCTSTR weatherTxt, WORD wIntensity=0 );
	BOOL EffectOn( DWORD dwWeatherHashCode, WORD wIntensity=0 );
	void EffectOff();
	void SetEffectCreateTime( LPCTSTR weatherTxt, float fTime );
	void SetEffectRenderTime( LPCTSTR weatherTxt, float fTime );
	void SetEffectDeleteTime( LPCTSTR weatherTxt, float fTime );
	void SetEffectCreateTime( DWORD dwWeatherHashCode, float fTime );
	void SetEffectRenderTime( DWORD dwWeatherHashCode, float fTime );
	void SetEffectDeleteTime( DWORD dwWeatherHashCode, float fTime );

// Network
	void NetworkMsgParse(BYTE Protocol, void* pMsg);
	void ShowCurrentWeatherEffect();

//#if defined(_GMTOOL_) || defined(_TESTCLIENT_)
protected:
	BOOL m_bDrawDebugInfo;
public:
	void ShowDebugState( BOOL bShow ) { m_bDrawDebugInfo = bShow; }
	BOOL IsShowDebugState() const { return m_bDrawDebugInfo; }
	void RenderDebugState();
//#endif
#ifdef _GMTOOL_
	void AddGMToolMenu();
#elif defined(_TESTCLIENT_)
	void AddTSToolMenu();
#endif
// ---- weather custom
private:
	DWORD m_wTmpCode;
	WORD m_wTmpIntensity;
	BOOL bIsWeather;
public:
	void set_weather(BOOL enable);
	void EffectOff_custom();
	void pushupbtn_onoffweather( LONG lId, void * p, DWORD we);
//
};

EXTERNGLOBALTON(CWeatherManager);

#endif // !defined(AFX_WEATHERMANAGER_H__5EB34DB4_AE8E_4654_B1FB_2A4A701DAFB1__INCLUDED_)
