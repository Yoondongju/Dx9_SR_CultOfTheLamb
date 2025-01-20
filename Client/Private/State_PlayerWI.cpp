#include "State_PlayerWI.h"
#include "Player.h"
#include "GameInstance.h"

CState_PlayerWI::CState_PlayerWI(CFsm* pFsm)
    :CState(pFsm)
{
}

HRESULT CState_PlayerWI::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;
    return S_OK;
}

HRESULT CState_PlayerWI::Start_State()
{
    m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
    m_pPlayer->Change_Animation(TEXT("WarpIn"));
    m_pPlayer->Set_Player_State(CPlayer::WARP_IN);
    m_pGameInstance->Play_Sound(TEXT("Warp_Done.wav"), SOUND_GUSION, g_fEffectVolume);
    return S_OK;
}

void CState_PlayerWI::Update(_float fTimeDelta)
{
    if (m_pPlayer->IsEnd_CurrentAnimation())
            m_pPlayer->Change_State(CPlayer::IDLE);
}

void CState_PlayerWI::End_State()
{
}

CState_PlayerWI* CState_PlayerWI::Create(CFsm* pFsm, _uint iStateNum)
{
    CState_PlayerWI* pInstance = new CState_PlayerWI(pFsm);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_PlayerWI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_PlayerWI::Free()
{
    __super::Free();
}
