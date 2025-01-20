
#include "State_PlayerResurrect.h"
#include "Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

CState_PlayerResurrect::CState_PlayerResurrect(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_PlayerResurrect::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_PlayerResurrect::Start_State()
{
	m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
	m_pPlayer->Change_Animation(TEXT("Resurrect"));
	m_pPlayer->Set_Player_State(CPlayer::RESURRECT);
	m_pGameInstance->Play_Sound(TEXT("Player_Resurrect.wav"), SOUND_PLAYER, g_fEffectVolume);
    return S_OK;
}

void CState_PlayerResurrect::Update(_float fTimeDelta)
{
	if (m_pPlayer->IsEnd_CurrentAnimation())
	{
		m_pPlayer->Set_Player_Info();
		m_pPlayer->Change_State(CPlayer::IDLE);
	}
}

void CState_PlayerResurrect::End_State()
{
	m_pPlayer->Set_Player_PreState(CPlayer::RESURRECT);
}

CState_PlayerResurrect* CState_PlayerResurrect::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_PlayerResurrect* pInstance = new CState_PlayerResurrect(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_PlayerResurrect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_PlayerResurrect::Free()
{
	__super::Free();
}
