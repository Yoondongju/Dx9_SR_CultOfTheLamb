#include "State_PlayerGF.h"
#include "Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

CState_PlayerGF::CState_PlayerGF(CFsm* pFsm)
    :CState{ pFsm }
{
}

HRESULT CState_PlayerGF::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_PlayerGF::Start_State()
{
    m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
    // Safe_AddRef(m_pPlayer);
    m_pPlayer->Change_Animation(TEXT("Get_Follower"));
    m_pPlayer->Set_Player_State(CPlayer::GET_FOLLOWER);
    m_pGameInstance->Play_Sound(TEXT("Follower_Rescue.wav"), SOUND_PLAYER, g_fEffectVolume);
    return S_OK;
}

void CState_PlayerGF::Update(_float fTimeDelta)
{
    if (m_pPlayer->IsEnd_CurrentAnimation())
        m_pPlayer->Change_State(CPlayer::IDLE);
}

void CState_PlayerGF::End_State()
{
    m_pGameInstance->StopSound(SOUND_PLAYER);
}

CState_PlayerGF* CState_PlayerGF::Create(CFsm* pFsm, _uint iStateNum)
{
    CState_PlayerGF* pInstance = new CState_PlayerGF(pFsm);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_PlayerGF"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_PlayerGF::Free()
{
    __super::Free();
}
