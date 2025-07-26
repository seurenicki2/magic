// SoundEffectUnit.cpp: implementation of the CSoundEffectUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SoundEffectUnit.h"
#include "SoundEffectUnitDesc.h"
#include "..\Audio\MHAudioManager.h"
#include "Sound.h"
#include "Object.h"

CSoundEffectUnit::CSoundEffectUnit()
{

}

CSoundEffectUnit::~CSoundEffectUnit()
{

}

void CSoundEffectUnit::Initialize(CEffectUnitDesc* pEffectUnitDesc,EFFECTPARAM* pEffectParam)
{
	m_dwSoundKey = 0 ;

	CEffectUnit::Initialize(pEffectUnitDesc,pEffectParam);
}
void CSoundEffectUnit::Release()
{
	CSoundEffectUnitDesc::ReleaseUnit(this);
}

void CSoundEffectUnit::DoOn(EFFECTPARAM* pParam,TARGETSET* pObject)
{
	CSoundEffectUnitDesc* pDesc = (CSoundEffectUnitDesc*)m_pEffectUnitDesc;

	m_dwSoundKey = DWORD(AUDIOMGR->Play(
		pDesc->m_SoundNum,
		pObject->pTarget->GetID()));
}

//090901 pdy ����Ʈ Hideó�� �߰�
void CSoundEffectUnit::SetHide( BOOL bHide )
{
	CSound* pSound = AUDIOMGR->GetSound( m_dwSoundKey ) ;

	if( pSound && pSound->IsPlaying() )
	{
		//���尡 Hide�Ǹ� ������ 0���� ����
		if( bHide )
			AUDIOMGR->SetVolume( pSound ,  0 ) ;
		else
			AUDIOMGR->SetVolume( pSound , AUDIOMGR->GetSoundVolume(pSound->GetEachSoundVolume()) ) ;
	}
}