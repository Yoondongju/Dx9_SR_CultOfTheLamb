
#include "State_PlayerKnockBack.h"
#include "Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"
#include <Camera.h>

CState_PlayerKnockBack::CState_PlayerKnockBack(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_PlayerKnockBack::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_PlayerKnockBack::Start_State()
{
	m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
	m_pPlayer->Change_Animation(TEXT("KnockBack"));
	m_pPlayer->Set_Player_PreState(CPlayer::KNOCKBACK);

	m_pGameInstance->Play_Sound(TEXT("Player_Hit.wav"), SOUND_PLAYER, 0.8f);

	static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(3.f, 3.f, 1);
	return S_OK;
}

void CState_PlayerKnockBack::Update(_float fTimeDelta)
{
	if (m_pPlayer->IsEnd_CurrentAnimation())
	{
		if (m_pPlayer->Get_Player_Info().iHp <= 0.f)
		{
			m_pPlayer->Change_State(CPlayer::DIE);
		}
		else 
		{
			m_pPlayer->Change_State(CPlayer::IDLE);
		}
	}

}

void CState_PlayerKnockBack::End_State()
{
	m_pPlayer->Set_Player_PreState(CPlayer::KNOCKBACK);
}

CState_PlayerKnockBack* CState_PlayerKnockBack::Create(CFsm* pFsm, _uint iStateNum)
{
    CState_PlayerKnockBack* pInstance = new CState_PlayerKnockBack(pFsm);
	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_PlayerKnockBack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_PlayerKnockBack::Free()
{
	__super::Free();
}
