#include "State_PlayerWarp.h"
#include "Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

CState_PlayerWarp::CState_PlayerWarp(CFsm* pFsm)
    :CState{ pFsm }
{
}

HRESULT CState_PlayerWarp::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_PlayerWarp::Start_State()
{
    m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
    m_pPlayer->Change_Animation(TEXT("WarpOut"));
    m_eCurState = WARP_OUT;
    m_pPlayer->Set_Player_State(CPlayer::WARP);
    return S_OK;
}

void CState_PlayerWarp::Update(_float fTimeDelta)
{
    Check_State();
    
}

void CState_PlayerWarp::End_State()
{
}

void CState_PlayerWarp::Check_State()
{
    Check_State_Init();

    switch (m_eCurState)
    {
    case Client::CState_PlayerWarp::WARP_IN:
        Warp_In();
        break;
    case Client::CState_PlayerWarp::WARP_OUT:
        Warp_Out();
        break;
    }
}

void CState_PlayerWarp::Check_State_Init()
{
    if (m_eCurState != m_ePreState)
    {
        switch (m_eCurState)
        {
        case Client::CState_PlayerWarp::WARP_IN:
            Warp_In_Init();
            break;
        case Client::CState_PlayerWarp::WARP_OUT:
            Warp_Out_Init();
            break;
        }

        m_ePreState = m_eCurState;
    }
}

void CState_PlayerWarp::Warp_In()
{
    if (m_pPlayer->IsEnd_CurrentAnimation())
    {
        m_eCurState = WARP_OUT;
        m_pPlayer->Change_State(CPlayer::IDLE);
    }
}

void CState_PlayerWarp::Warp_Out()
{
    if (m_pPlayer->IsEnd_CurrentAnimation())
    {
        m_eCurState = WARP_IN;
    }
}

void CState_PlayerWarp::Warp_In_Init()
{
    m_pPlayer->Change_Animation(TEXT("WarpIn"));
}

void CState_PlayerWarp::Warp_Out_Init()
{
    m_pPlayer->Change_Animation(TEXT("WarpOut"));
}

CState_PlayerWarp* CState_PlayerWarp::Create(CFsm* pFsm, _uint iStateNum)
{
    CState_PlayerWarp* pInstance = new CState_PlayerWarp(pFsm);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_PlayerSpell"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_PlayerWarp::Free()
{
    __super::Free();
}
