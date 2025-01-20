#include "State_PlayerWO.h"
#include "Player.h"
#include "GameInstance.h"

CState_PlayerWO::CState_PlayerWO(CFsm* pFsm)
    :CState(pFsm)
{
}

HRESULT CState_PlayerWO::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_PlayerWO::Start_State()
{
    m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
    m_pPlayer->Change_Animation(TEXT("WarpOut"));
    m_pPlayer->Set_Player_State(CPlayer::WARP_OUT);
    m_pGameInstance->Play_Sound(TEXT("Warp_Start.wav"), SOUND_GUSION, g_fEffectVolume);
    return S_OK;
}

void CState_PlayerWO::Update(_float fTimeDelta)
{
    if (m_pPlayer->IsEnd_CurrentAnimation())
        m_pPlayer->Change_State(CPlayer::IDLE);
}

void CState_PlayerWO::End_State()
{
}

CState_PlayerWO* CState_PlayerWO::Create(CFsm* pFsm, _uint iStateNum)
{
    CState_PlayerWO* pInstance = new CState_PlayerWO(pFsm);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_PlayerWO"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_PlayerWO::Free()
{
    __super::Free();
}
