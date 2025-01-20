
#include "State_PlayerIdle.h"
#include "Player.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

CState_PlayerIdle::CState_PlayerIdle(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_PlayerIdle::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;
	return S_OK;
}

HRESULT CState_PlayerIdle::Start_State()
{
	m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());

	if(m_pGameInstance->Get_KeyState(KEY::W)==KEY_STATE::AWAY)
		m_pPlayer->Change_Animation(TEXT("IDLE_UP"));
	else
		m_pPlayer->Change_Animation(TEXT("IDLE"));

	m_pPlayer->Set_Player_State(CPlayer::IDLE);
	m_bDontAttack = true;
	return S_OK;
}

void CState_PlayerIdle::Update(_float fTimeDelta)
{
	m_pPlayer->Set_InteractEnd(false);

	Check_Attack();

	if(m_bDontAttack)
	{
		Check_Move();
	}
}

void CState_PlayerIdle::End_State()
{
}

void CState_PlayerIdle::Check_Attack()
{
	if (m_pGameInstance->Get_LevelIndex() != LEVELID::LEVEL_GAMEPLAY
		&&m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		m_pPlayer->Get_Player_Coll()->Set_IsActive(false);
		m_pPlayer->Change_State(CPlayer::ATTACK1);
		m_pPlayer->Set_Player_State(CPlayer::ATTACK1);
		m_bDontAttack = false;
	}

	else if (m_pGameInstance->Get_KeyState(KEY::RBUTTON) == KEY_STATE::HOLD)
	{
		if(m_pPlayer->Get_SermonGauge() >= 0.6f)
		{
			m_pPlayer->Change_State(CPlayer::SPELL);
			m_bDontAttack = false;
		}
	}	

}

void CState_PlayerIdle::Check_Move()
{
	if(m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD
		|| m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD
		|| m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD
		|| m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
	{
		m_pPlayer->Change_State(CPlayer::RUN);
		m_pPlayer->Set_Player_State(CPlayer::RUN);
	}
	
	if (m_pPlayer->Get_Roll_Timer() > CPlayer::fRollCoolTime)
	{
		if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::TAP)
		{
			m_pPlayer->Change_State(CPlayer::ROLL);
			m_pPlayer->Set_Roll_Timer();
		}
	}
}

void CState_PlayerIdle::Check_Dead()
{
	if (m_pPlayer->Get_Dead())
	{
		m_pPlayer->Change_State(CPlayer::DIE);
	}
}

void CState_PlayerIdle::Check_Hit()
{
	if (m_pPlayer->IsHit())
		m_pPlayer->Change_State(CPlayer::KNOCKBACK);
}

CState_PlayerIdle* CState_PlayerIdle::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_PlayerIdle* pInstance = new CState_PlayerIdle(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_PlayerIdle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_PlayerIdle::Free()
{
	__super::Free();

}
