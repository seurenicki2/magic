// MHAudioManager.h: interface for the CMHAudioManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MHAUDIOMANAGER_H__1B6E7288_11D4_43DC_BD78_91228E0D1D11__INCLUDED_)
#define AFX_MHAUDIOMANAGER_H__1B6E7288_11D4_43DC_BD78_91228E0D1D11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SoundLib.h"

class CSound;
class CSoundItem;


#define AUDIOMGR	USINGTON(CMHAudioManager)

class CMHAudioManager  
{
	CYHHashTable<CSound> m_SoundTable;
	void			LoadList();

	float	m_MasterVolume;
	float	m_BGMVolume;
	float	m_SoundVolume;

	BOOL	m_bIsBGMPlaying;

	float	mLastBGMVolume;

//---KES : OPTION
	BOOL	m_bBGMSoundOn;
	BOOL	m_bEffectSoundOn;
	SNDIDX	m_idxLastBGM;
	// 091014 ShinJS BGM �ε巴�� �����ϱ�
	BOOL	m_bSmoothChangeBGM;			// �ε巴�� BGM �����ϱ� ���� ����
	SNDIDX	m_ChangeBGMIdx;				// ����� BGM
	BOOL	m_bChangedBGM;				// BGM ���� ����
	float	m_fCurChangeBGMVolume;		// ���� ������ BGM ���� �ۼ�Ʈ
	float	m_fSavedBGMVolume;			// ����� BGM ����
	typedef stdext::hash_map< SNDIDX, CSoundItem > SoundItemContainer;
	SoundItemContainer mSoundItemContainer;

	void ApplyVolumeChange();
	void ApplyBGMVolumeChange();

public:
	CMHAudioManager();
	virtual ~CMHAudioManager();
//	MAKESINGLETON(CMHAudioManager);

	BOOL			Initialize( HWND hWnd );
	void			Terminate();

	void			PlayBGM( SNDIDX idx );
	void			MuteBGM( BOOL val );
	void			StopBGM();
	CSound*			Play(SNDIDX, DWORD objectIndex);
	void			Stop(CSound*);
	void			StopAll();
	void			SetVolume( CSound* pSound, float val );	// 0~1 ����
	LPCTSTR			GetFileName(SNDIDX) const;
	void			SetListenerPosition(VECTOR3* pPos,float fAngleRad);

	void Process();

	void SetMasterVolume(float fVal);
	void SetBGMVolume(float fVal);
	void SetSoundVolume(float fVal);

	float GetBGMVolume();
	float GetSoundVolume(float fEachSoundVolume);

//---KES : OPTION
	void SetBGMSoundOn( BOOL bOn )		{ m_bBGMSoundOn = bOn; }
	void SetEffectSoundOn( BOOL bOn )	{ m_bEffectSoundOn = bOn; }
	void PlayLastBGM();
	CSound* GetSound( DWORD dwKey) { return m_SoundTable.GetData( dwKey ) ; } 
	void SmoothChangeBGM(SNDIDX idx);
};

EXTERNGLOBALTON(CMHAudioManager);
#endif // !defined(AFX_MHAUDIOMANAGER_H__1B6E7288_11D4_43DC_BD78_91228E0D1D11__INCLUDED_)
