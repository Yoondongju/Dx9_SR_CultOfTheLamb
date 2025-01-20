#include "State_PlayerRun.h"
#include "Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"
#include "Effect.h"

CState_PlayerRun::CState_PlayerRun(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_PlayerRun::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_PlayerRun::Start_State()
{
	m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
	//Safe_AddRef(m_pPlayer);
	m_vMoveVec = Get_MoveVector();
	m_fSoundTimer = 0.f;

	// 위
	if (m_vMoveVec.y < 0.f)
	{
		if (m_vMoveVec.x == 0.f)
		{
			m_pPlayer->Change_Animation(TEXT("Run_Up"));
			m_eCurState = RUN_STATE::RUN_UP;
		}

		else
		{
			m_pPlayer->Change_Animation(TEXT("Run_Diagonal_Up"));
			m_eCurState = RUN_STATE::RUN_DU;
		}

	}

	// 아래
	if (m_vMoveVec.y > 0.f)
	{
		if (m_vMoveVec.x == 0.f)
		{
			m_pPlayer->Change_Animation(TEXT("Run_Down"));
			m_eCurState = RUN_STATE::RUN_DOWN;
		}

		else
		{
			m_pPlayer->Change_Animation(TEXT("Run_Diagonal_Down"));
			m_eCurState = RUN_STATE::RUN_DD;
		}

	}

	if (m_vMoveVec.y == 0.f)
	{
		m_pPlayer->Change_Animation(TEXT("Run_Horizontal"));
		m_eCurState = RUN_STATE::RUN_HOR;
	}
	m_pPlayer->Set_Player_State(CPlayer::RUN);
	m_vPlayerPos = m_pPlayer->Get_Position();
	return S_OK;
}

void CState_PlayerRun::Update(_float fTimeDelta)
{
	Check_Hit();

	m_fSoundTimer += fTimeDelta;
	if (m_fSoundTimer > 0.2f)
	{
		m_pGameInstance->Play_Sound(TEXT("Player_Walk2.wav"), SOUND_PLAYER, 0.5f);
		m_fSoundTimer = 0.f;
	}

	Check_Roll();
	Check_State(fTimeDelta);

}

// State가 끝나면
void CState_PlayerRun::End_State()
{
}

_float2 CState_PlayerRun::Get_MoveVector()
{
	_float2 vMoveVec = { 0.f,0.f };
	if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD)
		vMoveVec.y = -1.f;
	if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD)
		vMoveVec.y = 1.f;
	if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD)
	{
		vMoveVec.x = -1.f;
		m_pPlayer->Set_isFilp(false);
	}
	if (m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
	{
		vMoveVec.x = 1.f;
		m_pPlayer->Set_isFilp(true);
	}

	D3DXVec2Normalize(&vMoveVec, &vMoveVec);

	return vMoveVec;
}

void CState_PlayerRun::Check_Roll()
{
	if (m_pPlayer->Get_Roll_Timer() > CPlayer::fRollCoolTime)
	{
		if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::TAP)
		{
			m_pPlayer->Change_State(CPlayer::ROLL);
			m_pPlayer->Set_Roll_Timer();
		}
	}
}

void CState_PlayerRun::Check_Dead()
{
	if (m_pPlayer->Get_Dead())
		m_pPlayer->Change_State(CPlayer::DIE);
}

void CState_PlayerRun::Check_State(_float fTimeDelta)
{
	Check_State_Init();
	switch (m_eCurState)
	{
	case Client::CState_PlayerRun::RUN_HOR:
		Run_Horizontal(fTimeDelta);
		break;
	case Client::CState_PlayerRun::RUN_UP:
		Run_Up(fTimeDelta);
		break;
	case Client::CState_PlayerRun::RUN_DOWN:
		Run_Down(fTimeDelta);
		break;
	case Client::CState_PlayerRun::RUN_DU:
		Run_Diagonal_Up(fTimeDelta);
		break;
	case Client::CState_PlayerRun::RUN_DD:
		Run_Diagonal_Down(fTimeDelta);
		break;
	}
}

void CState_PlayerRun::Check_State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Client::CState_PlayerRun::RUN_HOR:
			Run_Horizontal_Init();
			break;
		case Client::CState_PlayerRun::RUN_UP:
			Run_Up_Init();
			break;
		case Client::CState_PlayerRun::RUN_DOWN:
			Run_Down_Init();
			break;
		case Client::CState_PlayerRun::RUN_DU:
			Run_Diagonal_Up_Init();
			break;
		case Client::CState_PlayerRun::RUN_DD:
			Run_Diagonal_Down_Init();
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CState_PlayerRun::Run_Up(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::AWAY)
	{
		m_pGameInstance->StopSound(SOUND_PLAYER);
		m_pPlayer->Change_State(CPlayer::IDLE);
	}

	else if (m_pGameInstance->Get_LevelIndex() != LEVELID::LEVEL_GAMEPLAY
		&& m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		m_pPlayer->Change_State(CPlayer::ATTACK1);
	}

	m_pPlayer->Get_Player_Trans()->Go_Straight(fTimeDelta * 0.5f);
	m_vMoveVec = Get_MoveVector();
	if (m_vMoveVec.y < 0.f)
	{
		if (m_vMoveVec.x == 0.f)
		{
			m_eCurState = RUN_STATE::RUN_UP;
		}

		else
		{
			m_eCurState = RUN_STATE::RUN_DU;
		}

	}

	// 아래
	if (m_vMoveVec.y > 0.f)
	{
		if (m_vMoveVec.x == 0.f)
		{
			m_eCurState = RUN_STATE::RUN_DOWN;
		}

		else
		{
			m_eCurState = RUN_STATE::RUN_DD;
		}

	}

	if (m_vMoveVec.y == 0.f)
	{
		m_eCurState = RUN_STATE::RUN_HOR;
	}

}

void CState_PlayerRun::Run_Down(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::AWAY)
	{
		m_pGameInstance->StopSound(SOUND_PLAYER);
		m_pPlayer->Change_State(CPlayer::IDLE);
	}

	else if (m_pGameInstance->Get_LevelIndex() != LEVELID::LEVEL_GAMEPLAY
		&& m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		m_pPlayer->Change_State(CPlayer::ATTACK1);
	}

	m_pPlayer->Get_Player_Trans()->Go_Backward(fTimeDelta * 0.5f);
	m_vMoveVec = Get_MoveVector();
	if (m_vMoveVec.y < 0.f)
	{
		if (m_vMoveVec.x == 0.f)
		{
			m_eCurState = RUN_STATE::RUN_UP;
		}

		else
		{
			m_eCurState = RUN_STATE::RUN_DU;
		}

	}

	// 아래
	if (m_vMoveVec.y > 0.f)
	{
		if (m_vMoveVec.x == 0.f)
		{
			m_eCurState = RUN_STATE::RUN_DOWN;
		}

		else
		{
			m_eCurState = RUN_STATE::RUN_DD;
		}

	}

	if (m_vMoveVec.y == 0.f)
	{
		m_eCurState = RUN_STATE::RUN_HOR;
	}
}

void CState_PlayerRun::Run_Horizontal(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::AWAY
		|| m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::AWAY)
	{
		m_pGameInstance->StopSound(SOUND_PLAYER);
		m_pPlayer->Change_State(CPlayer::IDLE);
	}

	else if (m_pGameInstance->Get_LevelIndex() != LEVELID::LEVEL_GAMEPLAY
		&& m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		m_pPlayer->Change_State(CPlayer::ATTACK1);
	}

	if (m_pPlayer->Get_isFilp())
	{
		m_pPlayer->Get_Player_Trans()->Go_Right(fTimeDelta * 0.5f);
	}

	else if (!(m_pPlayer->Get_isFilp()))
	{
		m_pPlayer->Get_Player_Trans()->Go_Left(fTimeDelta * 0.5f);
	}
	m_vMoveVec = Get_MoveVector();
	if (m_vMoveVec.y < 0.f)
	{
		if (m_vMoveVec.x == 0.f)
		{
			m_eCurState = RUN_STATE::RUN_UP;
		}

		else
		{
			m_eCurState = RUN_STATE::RUN_DU;
		}

	}

	// 아래
	if (m_vMoveVec.y > 0.f)
	{
		if (m_vMoveVec.x == 0.f)
		{
			m_eCurState = RUN_STATE::RUN_DOWN;
		}

		else
		{
			m_eCurState = RUN_STATE::RUN_DD;
		}

	}

	if (m_vMoveVec.y == 0.f)
	{
		m_eCurState = RUN_STATE::RUN_HOR;
	}
}

void CState_PlayerRun::Run_Diagonal_Up(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::AWAY
		&& m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::AWAY)
	{
		m_pGameInstance->StopSound(SOUND_PLAYER);
		m_pPlayer->Change_State(CPlayer::IDLE);
	}

	if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::AWAY
		&& m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::AWAY)
	{
		m_pGameInstance->StopSound(SOUND_PLAYER);
		m_pPlayer->Change_State(CPlayer::IDLE);
	}

	else if (m_pGameInstance->Get_LevelIndex() != LEVELID::LEVEL_GAMEPLAY
		&& m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		m_pPlayer->Change_State(CPlayer::ATTACK1);
	}

	_float3 vPos = m_pPlayer->Get_Player_Trans()->Get_State(CTransform::STATE_POSITION);
	m_vMoveVec = Get_MoveVector();

	if (D3DXVec2Length(&m_vMoveVec) != 0.f)
	{
		D3DXVec2Normalize(&m_vMoveVec, &m_vMoveVec);
		vPos.x += m_vMoveVec.x * fTimeDelta * 0.5f * 30.0f;
		vPos.z -= m_vMoveVec.y * fTimeDelta * 0.5f * 30.0f;
		m_pPlayer->Get_Player_Trans()->Set_State(CTransform::STATE_POSITION, vPos);
	}

	if (m_vMoveVec.y < 0.f)
	{
		if (m_vMoveVec.x == 0.f)
		{
			m_eCurState = RUN_STATE::RUN_UP;
		}

		else
		{
			m_eCurState = RUN_STATE::RUN_DU;
		}

	}

	// 아래
	if (m_vMoveVec.y > 0.f)
	{
		if (m_vMoveVec.x == 0.f)
		{
			m_eCurState = RUN_STATE::RUN_DOWN;
		}

		else
		{
			m_eCurState = RUN_STATE::RUN_DD;
		}

	}

	if (m_vMoveVec.y == 0.f)
	{
		m_eCurState = RUN_STATE::RUN_HOR;
	}
}

void CState_PlayerRun::Run_Diagonal_Down(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::AWAY
		&& m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::AWAY)
	{
		m_pGameInstance->StopSound(SOUND_PLAYER);
		m_pPlayer->Change_State(CPlayer::IDLE);
	}

	if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::AWAY
		&& m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::AWAY)
	{
		m_pGameInstance->StopSound(SOUND_PLAYER);
		m_pPlayer->Change_State(CPlayer::IDLE);
	}

	else if (m_pGameInstance->Get_LevelIndex() != LEVELID::LEVEL_GAMEPLAY
		&& m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		m_pPlayer->Change_State(CPlayer::ATTACK1);
	}

	_float3 vPos = m_pPlayer->Get_Player_Trans()->Get_State(CTransform::STATE_POSITION);
	m_vMoveVec = Get_MoveVector();

	if (D3DXVec2Length(&m_vMoveVec) != 0.f)
	{
		D3DXVec2Normalize(&m_vMoveVec, &m_vMoveVec);
		vPos.x += m_vMoveVec.x * fTimeDelta * 15.f;
		vPos.z -= m_vMoveVec.y * fTimeDelta * 15.f;
		m_pPlayer->Get_Player_Trans()->Set_State(CTransform::STATE_POSITION, vPos);
	}

	if (m_vMoveVec.y < 0.f)
	{
		if (m_vMoveVec.x == 0.f)
		{
			m_eCurState = RUN_STATE::RUN_UP;
		}

		else
		{
			m_eCurState = RUN_STATE::RUN_DU;
		}

	}

	// 아래
	if (m_vMoveVec.y > 0.f)
	{
		if (m_vMoveVec.x == 0.f)
		{
			m_eCurState = RUN_STATE::RUN_DOWN;
		}

		else
		{
			m_eCurState = RUN_STATE::RUN_DD;
		}

	}

	if (m_vMoveVec.y == 0.f)
	{
		m_eCurState = RUN_STATE::RUN_HOR;
	}
}

void CState_PlayerRun::Run_Up_Init()
{
	m_pPlayer->Change_Animation(TEXT("Run_Up"));
}

void CState_PlayerRun::Run_Down_Init()
{
	m_pPlayer->Change_Animation(TEXT("Run_Down"));
}

void CState_PlayerRun::Run_Horizontal_Init()
{
	m_pPlayer->Change_Animation(TEXT("Run_Horizontal"));
}

void CState_PlayerRun::Run_Diagonal_Up_Init()
{
	m_pPlayer->Change_Animation(TEXT("Run_Diagonal_Up"));
}

void CState_PlayerRun::Run_Diagonal_Down_Init()
{
	m_pPlayer->Change_Animation(TEXT("Run_Diagonal_Down"));
}

void CState_PlayerRun::Check_Hit()
{
	if (m_pPlayer->IsHit())
		m_pPlayer->Change_State(CPlayer::KNOCKBACK);
}

CState_PlayerRun* CState_PlayerRun::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_PlayerRun* pInstance = new CState_PlayerRun(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_PlayerRun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_PlayerRun::Free()
{
	__super::Free();
}
