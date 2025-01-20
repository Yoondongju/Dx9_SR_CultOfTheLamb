#pragma once
#include "Base.h"
#include "fmod.h"
#include "fmod.hpp"
#include <io.h>

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

BEGIN(Engine)

class ENGINE_DLL CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();

public:
	void PlaySound(const TCHAR* pSoundKey, _uint eID, _float fVolume);
	void Play_SoundRepeat(const TCHAR* pSoundKey, _uint eID, _float fVolume);
	void PlayBGM(const TCHAR* pSoundKey, _float fVolume);
	void StopSound(_uint eID);
	void StopAll();
	void SetChannelVolume(_uint eID, _float fVolume);
	void SetPlayeSpeed(_uint eID, _float fSpeedRatio);
	void  Pause(_uint eID);
	_bool Check_IsPlaying(_uint eID);

private:
	void LoadSoundFile();

private:
	// ���� ���ҽ� ������ ���� ��ü 
	map<TCHAR*, FMOD::Sound*> m_Sounds;

	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	enum{ MAXCHANNEL = 100}; //�ӽ÷� ��Ƶ� ���߿� ��ġ��
	FMOD::Channel* m_pChannelArr[MAXCHANNEL];

	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	//FMOD::System* m_pSystem;
	FMOD::System* m_pSystem = { nullptr };

public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

END