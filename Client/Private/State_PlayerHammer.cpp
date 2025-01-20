#include "State_PlayerHammer.h"
#include "Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

CState_PlayerHammer::CState_PlayerHammer(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_PlayerHammer::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;
    return S_OK;
}

HRESULT CState_PlayerHammer::Start_State()
{
	m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
	// Safe_AddRef(m_pPlayer);
	m_pPlayer->Change_Animation(TEXT("Hammer"));
	m_pPlayer->Set_Player_State(CPlayer::GET_SWORD);
    return S_OK;
}

void CState_PlayerHammer::Update(_float fTimeDelta)
{
	m_fStateTimer += fTimeDelta;
	if (m_fStateTimer > 3.f)
	{
		m_pPlayer->Set_InteractEnd(true);
		m_pPlayer->Change_State(CPlayer::IDLE);
		m_fStateTimer = 0.f;
	}
}

void CState_PlayerHammer::End_State()
{
}

CState_PlayerHammer* CState_PlayerHammer::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_PlayerHammer* pInstance = new CState_PlayerHammer(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_PlayerHammer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_PlayerHammer::Free()
{
	__super::Free();
}
