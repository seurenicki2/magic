// WeatherManager.cpp: implementation of the CWeatherManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WeatherManager.h"
#include "Hero.h"
#include "ObjectManager.h"
#include "MHfile.h"
#include "mhMap.h"
#include "StatusIconDlg.h"
#include "MHCamera.h"
#include "ChatManager.h"
#include "WeatherDialog.h"
#include "GameIn.h"
#include "./interface/cFont.h"
#include "NewWeatherDialog.h"
#include "interface/cPushupButton.h"
#include "./interface/cfont.h"

#ifdef _TESTCLIENT_
#include "TSToolManager.h"
#endif

GLOBALTON(CWeatherManager);

CWeatherManager::CWeatherManager()
{
	m_bEffectOn = FALSE;
	m_CurEffectWeatherHashCode = 0;
	m_wIntensity = 0;

	LoadScript();

	// ---- weather custom
	m_wTmpCode = 0;
	m_wTmpIntensity = 0;
	bIsWeather = TRUE;
}

CWeatherManager::~CWeatherManager()
{
	Release();
}

void CWeatherManager::Init()
{
	//aziz apply show weather
#ifdef _TW_LOCAL_
	LoadScript();
	//m_bDrawDebugInfo = FALSE;
#endif

#ifdef _GMTOOL_
	// GM-Tool �� ���̵��� ��ũ��Ʈ�� �ٽ� ������ �ֵ��� �Ѵ�.
	LoadScript();

	m_bDrawDebugInfo = FALSE;
#endif	

	// ---- weather custom : show window?
	EffectOff();
}

void CWeatherManager::LoadScript()
{
	Release();

	enum Block
	{
		BlockNone,
		BlockWeather,
	}
	blockType = BlockNone;
	
	DWORD dwOpenCnt = 0;
	BOOL bIsComment = FALSE;

	DWORD dwCurWeatherHashCode = 0;

	CMHFile file;
	file.Init( "./System/Resource/WeatherEffect.bin", "rb" );
	while( ! file.IsEOF() )
	{
		char txt[ MAX_PATH ] = {0,};
		file.GetLine( txt, MAX_PATH );

		int txtLen = _tcslen( txt );

		// �߰� �ּ� ����
		for( int i=0 ; i<txtLen-1 ; ++i )
		{
			if( txt[i] == '/' && txt[i+1] == '/' )
			{
				txt[i] = 0;
				break;
			}
			else if( txt[i] == '/' && txt[i+1] == '*' )
			{
				txt[i] = 0;
				bIsComment = TRUE;
			}
			else if( txt[i] == '*' && txt[i+1] == '/' )
			{
				txt[i] = ' ';
				txt[i+1] = ' ';
				bIsComment = FALSE;
			}
			else if( bIsComment )
			{
				txt[i] = ' ';
			}
		}

		char buff[ MAX_PATH ] = {0,};
		SafeStrCpy( buff, txt, MAX_PATH );
		const char* delimit = " \n\t=,~()";
		const char* markBegin = "{";
		const char* markEnd = "}";
		char* token = strtok( buff, delimit );

		if( ! token )
		{
			continue;
		}
		else if( bIsComment )
		{
			continue;
		}
		else if( ! stricmp( "Weather", token ) )
		{
			blockType = BlockWeather;
			token = strtok( 0, delimit );
			if( !token )		break;
			dwCurWeatherHashCode = GetHashCodeFromTxt( token );
			stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
			SafeStrCpy( data.hashCodeTxt, token, sizeof(data.hashCodeTxt) );
			data.dwHashCode = dwCurWeatherHashCode;
		}
		else if( ! strnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++dwOpenCnt;
		}
		else if( ! strnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			--dwOpenCnt;

			switch( blockType )
			{
			case BlockWeather:
				{
					blockType = BlockNone;
					dwCurWeatherHashCode = 0;
				}
				break;
			}
		}

		switch( blockType )
		{
		case BlockWeather:
			{
				if( stricmp( token, "distance" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;
					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.fRenderValidDist = float( atof( token ) );
				}
				else if( stricmp( token, "createTime" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;
					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.fCreateTime = float( atof( token ) );
				}
				else if( stricmp( token, "renderTime" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;
					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.fRenderTime = float( atof( token ) );
				}
				else if( stricmp( token, "deleteTime" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;
					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.fDeleteTime = float( atof( token ) );
				}
				else if( stricmp( token, "file" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;
					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					SafeStrCpy( data.effectFileName, token, MAX_PATH );
				}
				else if( stricmp( token, "maxEffectCnt" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;
					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.wMaxObjectCnt = WORD( atoi( token ) );
				}
				else if( stricmp( token, "rotate" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;

					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];

					if( stricmp( token, "Random" ) == 0 )
					{
						data.nRotate = eWeatherEffectRotateType_Random;
					}
					else if( stricmp( token, "Player" ) == 0 )
					{
						data.nRotate = eWeatherEffectRotateType_PlayerLook;
					}
					else if( stricmp( token, "Camera" ) == 0 )
					{
						data.nRotate = eWeatherEffectRotateType_CameraLook;
					}
					else
					{
						data.nRotate = atoi( token );
					}
				}
				else if( stricmp( token, "createPosition" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;

					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];

					if( stricmp( token, "Camera" ) == 0 )
					{
						data.eCreatePosType = eWeatherEffectCreatePosType_Camera;
					}
					else if( stricmp( token, "Player" ) == 0 )
					{
						data.eCreatePosType = eWeatherEffectCreatePosType_Player;
					}
				}
				else if( stricmp( token, "createDistance" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;

					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.fCreateDist = float( atof( token ) );
				}
				else if( stricmp( token, "createRange" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;

					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.fCreateRangeMin = float( atof( token ) );

					token = strtok( 0, delimit );
					if( !token )		break;
					data.fCreateRangeMax = float( atof( token ) );
				}
				else if( stricmp( token, "scaleRange" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;

					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.fScaleMin = float( atof( token ) );

					token = strtok( 0, delimit );
					if( !token )		break;
					data.fScaleMax = float( atof( token ) );
				}
				else if( stricmp( token, "createDelay" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;

					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.dwCreateDelay = DWORD( atoi( token ) );
				}

				// ---- weather custom : add "component_id"
				else if( stricmp( token, "component_id" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;

					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.wCID = WORD( atoi( token ) ) ;
				}
				// ----
			}
			break;
		}
	}

#ifdef _GMTOOL_
	if( dwOpenCnt )
	{
		MessageBox( NULL, "{} Error! Check WeatherEffect.bin", NULL, NULL );
	}
#endif

	// ���� ������ ������ ��� �����Ų��.
	if( m_CurEffectWeatherHashCode )
	{
		EffectOn( m_CurEffectWeatherHashCode, m_wIntensity );
	}
}

void CWeatherManager::Release()
{ 
	WeatherEffectDataMap::iterator iterData;
	for( iterData = m_WeatherEffectDataMap.begin() ; iterData != m_WeatherEffectDataMap.end() ; ++iterData )
	{
		stWeatherEffectData& data = iterData->second;
		
		std::map<int, stWeatherEffectObject>::iterator iterObject;
		for( iterObject = data.objectMap.begin() ; iterObject != data.objectMap.end() ; ++iterObject )
		{
			stWeatherEffectObject& object = iterObject->second;
			object.effecObj.Release();
		}

		data.objectMap.clear();
	}

	m_WeatherEffectDataMap.clear();

	m_CreateEffectQue.clear();
	m_RenderEffectQue.clear();
	m_DeleteEffectQue.clear();
}

void CWeatherManager::Process()
{
	MHCAMERADESC* pCameraDesc = CAMERA->GetCameraDesc();
	VECTOR3 vecCheckPos = {0,};

	WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( m_CurEffectWeatherHashCode );
	if( iterData != m_WeatherEffectDataMap.end() )
	{
		stWeatherEffectData& data = iterData->second;

		switch( data.eCreatePosType )
		{
		case eWeatherEffectCreatePosType_Camera:
			{
				vecCheckPos = pCameraDesc->m_CameraPos + (pCameraDesc->m_CameraDir * data.fCreateDist);
			}
			break;
		case eWeatherEffectCreatePosType_Player:
			{
				if( !HERO )
					break;

				MATRIX4 matRot;
				SetRotationXMatrix( &matRot, HERO->GetAngle() );
				VECTOR3 vecDir = {0,};
				vecDir.z = 1.f;
				TransformVector3_VPTR2( &vecDir, &vecDir, &matRot, 1 );

				VECTOR3 vecHeroPos;
				HERO->GetPosition( &vecHeroPos );
				vecCheckPos = vecHeroPos + (vecDir * data.fCreateDist);
			}
			break;
		}
	}


	WeatherRenderStateQue::iterator iterQue = m_CreateEffectQue.begin();
	while( iterQue != m_CreateEffectQue.end() )
	{
		WeatherObjectKey& dataKey = *iterQue;
		WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( dataKey.first );
		if( iterData == m_WeatherEffectDataMap.end() )
		{
			++iterQue;
			continue;
		}

		stWeatherEffectData& data = iterData->second;
		stWeatherEffectObject& object = data.objectMap[ dataKey.second ];

		// �ð� ����� ���Ͽ� Alpha �� ����
		float fAlpha = (gCurTime - object.dwTime) / data.fCreateTime;

		// Create -> Render ���·� ����
		if( fAlpha > 1.f )
		{
			object.eState = eWeatherEffectRenderState_Render;
			object.dwTime = DWORD( gCurTime + data.fRenderTime );
			object.effecObj.SetAlpha( 1.f );

			// Render Que�� ����
			m_RenderEffectQue.push_back( dataKey );

			// Create Que ����
			iterQue = m_CreateEffectQue.erase( iterQue );

			continue;
		}

		// ����� Alpha ����
		object.effecObj.SetAlpha( fAlpha );

		++iterQue;
	}

	iterQue = m_RenderEffectQue.begin();
	while( iterQue != m_RenderEffectQue.end() )
	{
		WeatherObjectKey& dataKey = *iterQue;
		WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( dataKey.first );
		if( iterData == m_WeatherEffectDataMap.end() )
		{
			++iterQue;
			continue;
		}

		stWeatherEffectData& data = iterData->second;
		stWeatherEffectObject& object = data.objectMap[ dataKey.second ];

		float fDist = CalcDistance( &vecCheckPos, &object.vecPos );

		// Render -> Delete ���·� ����
		if( gCurTime > object.dwTime ||
			fDist > data.fRenderValidDist )
		{
			object.eState = eWeatherEffectRenderState_Delete;
			object.dwTime = gCurTime;

			// Delete Que�� ����
			m_DeleteEffectQue.push_back( dataKey );

			// Render Que ����
			iterQue = m_RenderEffectQue.erase( iterQue );

			continue;
		}

		++iterQue;
	}

	iterQue = m_DeleteEffectQue.begin();
	while( iterQue != m_DeleteEffectQue.end() )
	{
		WeatherObjectKey& dataKey = *iterQue;
		WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( dataKey.first );
		if( iterData == m_WeatherEffectDataMap.end() )
		{
			++iterQue;
			continue;
		}

		stWeatherEffectData& data = iterData->second;
		stWeatherEffectObject& object = data.objectMap[ dataKey.second ];

		// �ð� ����� ���Ͽ� Alpha �� ����
		float fAlpha = 1.f - (gCurTime - object.dwTime) / data.fDeleteTime;

		// Delete -> None ���·� ����
		if( fAlpha < 0.f )
		{
			object.eState = eWeatherEffectRenderState_None;
			object.effecObj.Hide();

			// UsableIndex �߰�
			data.usableIndex.push_back( dataKey.second );

			// Delete Que ����
			iterQue = m_DeleteEffectQue.erase( iterQue );

			continue;
		}

		// ����� Alpha ����
		object.effecObj.SetAlpha( fAlpha );

		++iterQue;
	}

	if( ! m_bEffectOn || iterData == m_WeatherEffectDataMap.end() )
		return;

	// Effect �߰�
	AddCreateStateEffect( iterData->second, vecCheckPos );
}

void CWeatherManager::AddCreateStateEffect( stWeatherEffectData& data, VECTOR3& vecCheckPos )
{
	int nRenderEffectCnt = m_CreateEffectQue.size() + m_RenderEffectQue.size();

	if( data.usableIndex.empty() ||
		data.dwValidCreateTime > gCurTime ||
		nRenderEffectCnt > m_wIntensity )
		return;

	int nUsableIndex = data.usableIndex.back();
	data.usableIndex.pop_back();

	stWeatherEffectObject& object = data.objectMap[ nUsableIndex ];

	// ũ�� ó��(�Ҽ��� ���ڸ����� ��ȿ�ϰ� ���)
	VECTOR3 vecScale = {1.0f, 1.0f, 1.0f};
	if( data.fScaleMin < data.fScaleMax )
	{
		float fRandRange = data.fScaleMax - data.fScaleMin;
		float fScale = (rand() % (int)(fRandRange*100))/100.f + data.fScaleMin;
		vecScale.x = vecScale.y = vecScale.z = fScale;
	}

	// ȸ�� ó��
	float fRad = 0.f;
	switch( data.nRotate )
	{
		// Random
	case eWeatherEffectRotateType_Random:
		{
			fRad = DEGTORAD( rand() % 360 );
		}
		break;

		// Player(HERO)�� �ٶ󺸴� ����
	case eWeatherEffectRotateType_PlayerLook:
		{
			if( HERO )
				fRad = HERO->GetAngle();
		}
		break;

		// Camera �� �ٶ󺸴� ����
	case eWeatherEffectRotateType_CameraLook:
		{
			MHCAMERADESC* pCameraDesc = CAMERA->GetCameraDesc();
			fRad = pCameraDesc->m_AngleY.ToRad() + PI;
		}
		break;

		// ���� ������ ���
	default:
		{
			fRad = DEGTORAD( data.nRotate );
		}
		break;
	}

	// ��ġ ó��
	object.vecPos = vecCheckPos;
	if( data.fCreateRangeMin || data.fCreateRangeMax )
	{
		VECTOR3 vecDir={0,};
        vecDir.x = 1.f - ((float)(rand()%2000)/1000.f);
		vecDir.y = 1.f - ((float)(rand()%2000)/1000.f);
		vecDir.z = 1.f - ((float)(rand()%2000)/1000.f);
		Normalize( &vecDir, &vecDir );

		float fRandRange = data.fCreateRangeMax - data.fCreateRangeMin;
		float fScale = (rand() % (int)(fRandRange*100))/100.f + data.fCreateRangeMax;
		vecDir = vecDir * fScale;

		object.vecPos = object.vecPos + vecDir;
	}

	object.eState = eWeatherEffectRenderState_Create;
	object.dwTime = gCurTime;
	object.effecObj.Show();
	object.effecObj.SetAlpha( 0.f );
	object.effecObj.SetEngineToCurmotion();
	object.effecObj.SetEngObjPosition( &object.vecPos );
	object.effecObj.SetEngObjAngle( fRad );
	object.effecObj.SetScale( &vecScale );

	data.dwValidCreateTime = gCurTime + data.dwCreateDelay;

	m_CreateEffectQue.push_back( std::make_pair( m_CurEffectWeatherHashCode, nUsableIndex ) );
}

BOOL CWeatherManager::EffectOn( LPCTSTR weatherTxt, WORD wIntensity )
{
	DWORD dwHashCode = GetHashCodeFromTxt( weatherTxt );

	return EffectOn( dwHashCode, wIntensity );
/*	
	//aziz test
	WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( dwHashCode );
	if( iterData == m_WeatherEffectDataMap.end() )
		return FALSE;

	m_CurEffectWeatherHashCode = dwHashCode;
	m_bEffectOn = TRUE;
	m_wIntensity = wIntensity;

	// Engine Object�� �������� ���� ���
	stWeatherEffectData& data = iterData->second;
	if( data.wMaxObjectCnt > 0 && data.objectMap.size() == 0 )
	{
		for( int i=0 ; i < (int)data.wMaxObjectCnt ; ++i )
		{
			data.usableIndex.push_back( i );

			stWeatherEffectObject& object = data.objectMap[i];
			if( object.effecObj.Init( data.effectFileName, NULL, eEngineObjectType_GameSystemObject, (WORD)0, (WORD)-1 ) )
			{
				object.effecObj.SetRepeat( TRUE );
			}
		}
	}
	return TRUE;
	*/
}

BOOL CWeatherManager::EffectOn( DWORD dwWeatherHashCode, WORD wIntensity )
{
	WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( dwWeatherHashCode );
	if( iterData == m_WeatherEffectDataMap.end() )
		return FALSE;
	
	// ---- weather custom : if option is off, only save data for future.
	if( !bIsWeather ){
		m_wTmpCode = dwWeatherHashCode;
		m_wTmpIntensity = wIntensity;
		return FALSE;
	}

	m_CurEffectWeatherHashCode = dwWeatherHashCode;
	m_bEffectOn = TRUE;
	m_wIntensity = wIntensity;

	// Engine Object�� �������� ���� ���
	stWeatherEffectData& data = iterData->second;
	if( data.wMaxObjectCnt > 0 && data.objectMap.size() == 0 )
	{
		for( int i=0 ; i < (int)data.wMaxObjectCnt ; ++i )
		{
			data.usableIndex.push_back( i );

			stWeatherEffectObject& object = data.objectMap[i];
			if( object.effecObj.Init( data.effectFileName, NULL, eEngineObjectType_GameSystemObject, (WORD)0, (WORD)-1 ) )
			{
				object.effecObj.SetRepeat( TRUE );
			}
		}
	}

#ifdef _GMTOOL_
	if( wIntensity > data.wMaxObjectCnt )
	{
		MessageBox( NULL, "Weather Effect Intensity must be lower than MaxObjectCount!", NULL, NULL );
	}
#endif
	// ---- wasd custom : saving data from map server
	m_wTmpCode = m_CurEffectWeatherHashCode;
	m_wTmpIntensity = m_wIntensity;
#ifdef _FORMOON_
	if( GAMEIN->getWeatherDlg() ){
		GAMEIN->getWeatherDlg()->SetWeather( data.wCID );
	}
#else
	if( GAMEIN->GetNewWeatherDialog() ){
		GAMEIN->GetNewWeatherDialog()->SetWeather( data.wCID );
	}
#endif
	//

	return TRUE;
}

void CWeatherManager::EffectOff()
{
	m_CurEffectWeatherHashCode = 0;
	m_bEffectOn = FALSE;

	// ---- weather custom : clear temporary data
	m_wTmpCode = 0;
	m_wTmpIntensity = 0;
#ifdef _FORMOON_
	if( GAMEIN->getWeatherDlg() ){
		GAMEIN->getWeatherDlg()->SetWeather(eWS_Clear);
	}
#else
	if( GAMEIN->GetNewWeatherDialog() ){
		GAMEIN->GetNewWeatherDialog()->SetWeather(eWS_Clear);
	}
#endif
}

void CWeatherManager::SetEffectCreateTime( LPCTSTR weatherTxt, float fTime )
{
	DWORD dwHashCode = GetHashCodeFromTxt( weatherTxt );
	SetEffectCreateTime( dwHashCode, fTime );
}
void CWeatherManager::SetEffectRenderTime( LPCTSTR weatherTxt, float fTime )
{
	DWORD dwHashCode = GetHashCodeFromTxt( weatherTxt );
	SetEffectRenderTime( dwHashCode, fTime );
}
void CWeatherManager::SetEffectDeleteTime( LPCTSTR weatherTxt, float fTime )
{
	DWORD dwHashCode = GetHashCodeFromTxt( weatherTxt );
	SetEffectDeleteTime( dwHashCode, fTime );
}

void CWeatherManager::SetEffectCreateTime( DWORD dwWeatherHashCode, float fTime )
{
	WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( m_CurEffectWeatherHashCode );
	if( iterData == m_WeatherEffectDataMap.end() )
		return;

	stWeatherEffectData& data = iterData->second;
	data.fCreateTime = fTime;
}
void CWeatherManager::SetEffectRenderTime( DWORD dwWeatherHashCode, float fTime )
{
	WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( m_CurEffectWeatherHashCode );
	if( iterData == m_WeatherEffectDataMap.end() )
		return;

	stWeatherEffectData& data = iterData->second;
	data.fRenderTime = fTime;
}
void CWeatherManager::SetEffectDeleteTime( DWORD dwWeatherHashCode, float fTime )
{
	WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( m_CurEffectWeatherHashCode );
	if( iterData == m_WeatherEffectDataMap.end() )
		return;

	stWeatherEffectData& data = iterData->second;
	data.fDeleteTime = fTime;
}


void CWeatherManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
#ifndef _FORMOON_
	switch( Protocol )
	{
	case MP_WEATHER_ACK:
	case MP_WEATHER_CHANGETONEWWEATHER:
		{
			// Casting Message			
			MSG_WEATHER_ACK* msg = (MSG_WEATHER_ACK*)pMsg;
	
			// Check Character Map Equal with Server send.
			if (MAP->GetMapNum() != msg->mapnum) return;

			char weathertypename[MAXIMUM_WEATHERNAME];
			SafeStrCpy( weathertypename , msg->weathertype, MAXIMUM_WEATHERNAME);
			int output = strcmp(weathertypename,"CLEAR");
			if (strcmp(msg->weathertype,"CLEAR") == 0)
			{
				// Effect Off
				 EffectOff();
				 // ---- wasd custom : map server sent clear weather, temporary code must clear.
				 m_wTmpCode = 0;
				//aziz Show Weather notice
//#if _GMTOOL_
			CHATMGR->AddMsg( CTC_TOGM, "Weather Effect Off : %s[%d]", msg->weathertype, 0 );
//#endif
			}
			else
			{
				// Show Effect
				EffectOn(msg->weathertype , WORD(msg->intensity));
//#if _GMTOOL_
			CHATMGR->AddMsg( CTC_TOGM, "Weather Effect On : %s[%d]", msg->weathertype, msg->intensity );
//#endif
			}
		}
	}
#endif
}

void CWeatherManager::ShowCurrentWeatherEffect()
{
#ifndef _FORMOON_
	// Send Message to Map Server 
	MSG_DWORD msg;
	msg.Category = MP_WEATHER;
	msg.Protocol = MP_WEATHER_SYN;
	msg.dwData = HEROID;
	NETWORK->Send( &msg, sizeof( msg ) );
#endif
}


#if defined(_GMTOOL_) || defined(_TESTCLIENT_)
#include "GMToolManager.h"
void CWeatherManager::RenderDebugState()
{
	if( !m_bDrawDebugInfo )
		return;

	// ���� Weather ������ ǥ���Ѵ�.
	char txt[ 512 ]={0,};
	RECT rc;
	rc.left = 220;
	rc.right = 1040;

	int nRow = 5;
	rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
	sprintf( txt, "--------------------------------------------" );
	CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

	rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
	sprintf( txt, "               Weather status information" );
	CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

	rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
	sprintf( txt, "--------------------------------------------" );
	CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

	WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( m_CurEffectWeatherHashCode );
	if( iterData != m_WeatherEffectDataMap.end() )
	{
		stWeatherEffectData& data = iterData->second;

		rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
		sprintf( txt, "Currently set weather : %s", data.hashCodeTxt );
		CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

		rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
		sprintf( txt, "Currently set strength : %d", m_wIntensity );
		CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

		rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
		sprintf( txt, "Number of remaining objects available : %d\n", data.usableIndex.size() );
		CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

		rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
		sprintf( txt, "Create State Object Count : %d\n", m_CreateEffectQue.size() );
		CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

		rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
		sprintf( txt, "Render State Object Count : %d\n", m_RenderEffectQue.size() );
		CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

		rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
		sprintf( txt, "Delete State Object Count : %d\n", m_DeleteEffectQue.size() );
		CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));
	}

	rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
	sprintf( txt, "--------------------------------------------" );
	CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));
}
#endif

#ifdef _GMTOOL_
void CWeatherManager::AddGMToolMenu()
{
	WeatherEffectDataMap::iterator iterData;
	for( iterData = m_WeatherEffectDataMap.begin() ; iterData != m_WeatherEffectDataMap.end() ; ++iterData )
	{
		stWeatherEffectData& data = iterData->second;
		GMTOOLMGR->AddWeatherType( data.hashCodeTxt );
	}
}
#elif defined _TESTCLIENT_
void CWeatherManager::AddTSToolMenu()
{
	WeatherEffectDataMap::iterator iterData;
	for( iterData = m_WeatherEffectDataMap.begin() ; iterData != m_WeatherEffectDataMap.end() ; ++iterData )
	{
		stWeatherEffectData& data = iterData->second;
		TSTOOLMGR->AddWeatherType( data.hashCodeTxt );
	}
}
#endif

// ---- weather custom

void CWeatherManager::EffectOff_custom(){
	// original EffectOff() is used in many places. use custom one for option.
	// all temporary data is not cleared.
	m_CurEffectWeatherHashCode = 0;
	m_bEffectOn = FALSE;
}

void CWeatherManager::set_weather(BOOL enable){
	// this function only called in apply setting from optionmanager.cpp
#ifdef _FORMOON_
	if( GAMEIN->getWeatherDlg() ){
		GAMEIN->getWeatherDlg()->SetActive(TRUE);
	}
#else
	if( GAMEIN->GetNewWeatherDialog() ){
		GAMEIN->GetNewWeatherDialog()->SetActive(TRUE);
	}
#endif
	
	if( bIsWeather == enable ){ return ; }
	bIsWeather = enable;
	if( enable ){
		if( !m_bEffectOn && ( m_wTmpCode != 0 ) ){
			EffectOn( m_wTmpCode, m_wTmpIntensity);
		}
		
	} 
	else {
		//m_wTmpCode = m_CurEffectWeatherHashCode;
		//m_wTmpIntensity = m_wIntensity;
		EffectOff_custom();
	}
}

void CWeatherManager::pushupbtn_onoffweather( LONG lId, void * p, DWORD we)
{
	cPushupButton * pshbtn = NULL;
#ifdef _FORMOON_
	if( GAMEIN->getWeatherDlg() ){
		pshbtn = (cPushupButton *)(GAMEIN->getWeatherDlg()->GetWindowForID( lId ));
	}
#else
	if( GAMEIN->GetNewWeatherDialog() ){
		pshbtn = (cPushupButton *)(GAMEIN->GetNewWeatherDialog()->GetWindowForID( lId ));
	}
#endif

	if( pshbtn ){
		if( pshbtn->IsPushed() ){
			set_weather( FALSE );
		}
		else {
			set_weather( TRUE );
		}
	}
}