#include "State_PlayerSermon.h"
#include "Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

CState_PlayerSermon::CState_PlayerSermon(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_PlayerSermon::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;
	return S_OK;
}

HRESULT CState_PlayerSermon::Start_State()
{
	m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
	// Safe_AddRef(m_pPlayer);
	m_pPlayer->Change_Animation(TEXT("Sermon_Start"));
	m_eCurState = SERMON_START;
	m_pPlayer->Set_Player_State(CPlayer::SERMON);
	m_pGameInstance->Play_Sound(TEXT("Follower_Start_Indo.wav"), SOUND_PLAYER, g_fEffectVolume);
	return S_OK;
}

void CState_PlayerSermon::Update(_float fTimeDelta)
{
	Check_State();
}

void CState_PlayerSermon::End_State()
{
	m_pGameInstance->StopSound(SOUND_PLAYER);
}

void CState_PlayerSermon::Check_State()
{
	Check_State_Init();
	switch (m_eCurState)
	{
	case Client::CState_PlayerSermon::SERMON_START:
		Sermon_Start();
		break;
	case Client::CState_PlayerSermon::SERMON_END:
		Sermon_End();
		break;
	}
}

void CState_PlayerSermon::Check_State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Client::CState_PlayerSermon::SERMON_START:
			Sermon_Start_Init();
			break;
		case Client::CState_PlayerSermon::SERMON_END:
			Sermon_End_Init();
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CState_PlayerSermon::Sermon_Start()
{
	if (m_pPlayer->IsEnd_CurrentAnimation())
		m_eCurState = SERMON_END;
}

void CState_PlayerSermon::Sermon_End()
{
	if (m_pPlayer->IsEnd_CurrentAnimation())
		m_pPlayer->Change_State(CPlayer::IDLE);
}

void CState_PlayerSermon::Sermon_Start_Init()
{
	
	m_pPlayer->Change_Animation(TEXT("Sermon_Start"));
}

void CState_PlayerSermon::Sermon_End_Init()
{
	m_pPlayer->Change_Animation(TEXT("Sermon_End"));
}

CState_PlayerSermon* CState_PlayerSermon::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_PlayerSermon* pInstance = new CState_PlayerSermon(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_PlayerSermon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_PlayerSermon::Free()
{
	__super::Free();
}
