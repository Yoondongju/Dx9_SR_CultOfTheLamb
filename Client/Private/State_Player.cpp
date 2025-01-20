#include "State_Player.h"
#include "Player.h"
#include "GameInstance.h"
#include "Animator.h"

CState_Player::CState_Player(CFsm* pFsm)
    :CState(pFsm)
{
}

HRESULT CState_Player::Initialize(_uint iStateNum)
{
    m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
    //Safe_AddRef(m_pPlayer);
    m_iStateNum = iStateNum;
    return S_OK;
}

void CState_Player::Free()
{
    __super::Free();

   // Safe_Release(m_pPlayer);
}
