
#include "State_PlayerDie.h"
#include "Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

CState_PlayerDie::CState_PlayerDie(CFsm* pFsm)
    :CState{ pFsm }
{
}

HRESULT CState_PlayerDie::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_PlayerDie::Start_State()
{
    m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
    m_pPlayer->Change_Animation(TEXT("Die"));
    m_pPlayer->Set_Player_State(CPlayer::DIE);
    m_pGameInstance->Play_Sound(TEXT("Player_Die.wav"), SOUND_PLAYER, g_fEffectVolume);
    return S_OK;
}

void CState_PlayerDie::Update(_float fTimeDelta)
{
    // 부활조건 만족시 부활 상태로 이동
    if (m_pGameInstance->Get_KeyState(KEY::P)==KEY_STATE::TAP)
    {
        m_pPlayer->Change_State(CPlayer::RESURRECT);

    }
}

void CState_PlayerDie::End_State()
{   
}

CState_PlayerDie* CState_PlayerDie::Create(CFsm* pFsm, _uint iStateNum)
{
    CState_PlayerDie* pInstance = new CState_PlayerDie(pFsm);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_PlayerDie"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_PlayerDie::Free()
{
    __super::Free();
}
