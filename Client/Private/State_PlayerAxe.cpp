#include "State_PlayerAxe.h"
#include "Player.h"
#include "GameInstance.h"
#include "Transform.h"

CState_PlayerAxe::CState_PlayerAxe(CFsm* pFsm)
    :CState{ pFsm }
{
}

HRESULT CState_PlayerAxe::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;
    return S_OK;
}

HRESULT CState_PlayerAxe::Start_State()
{
    m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
    // Safe_AddRef(m_pPlayer);
    m_pPlayer->Change_Animation(TEXT("Axe"));
    m_pPlayer->Set_Player_State(CPlayer::AXE);
    return S_OK;
}

void CState_PlayerAxe::Update(_float fTimeDelta)
{
    if (m_pPlayer->Get_CurrentFrame() == 15 && !m_isCrash)
    {
        m_pGameInstance->Play_Sound(TEXT("Tree_Hit.wav"), SOUND_PLAYER, g_fEffectVolume);

        m_isCrash = true;
        m_pPlayer->Get_InteractObject()->Set_IsShake(true);

        if (!m_pPlayer->Get_InteractObject()->Crash_InterectObject())
        {
            m_pPlayer->Set_InteractObject(nullptr);
            m_pPlayer->Change_State(CPlayer::IDLE);
        }
    }
    else
    {
        m_isCrash = false;
    }

    m_fParticleTimer += fTimeDelta;

}

void CState_PlayerAxe::End_State()
{
}

CState_PlayerAxe* CState_PlayerAxe::Create(CFsm* pFsm, _uint iStateNum)
{
    CState_PlayerAxe* pInstance = new CState_PlayerAxe(pFsm);
    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_PlayerAxe"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CState_PlayerAxe::Free()
{
    __super::Free();
}
