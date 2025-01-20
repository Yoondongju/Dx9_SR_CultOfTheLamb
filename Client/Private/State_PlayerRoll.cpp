#include "State_PlayerRoll.h"
#include "Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"
#include "Effect.h"



CState_PlayerRoll::CState_PlayerRoll(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_PlayerRoll::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_PlayerRoll::Start_State()
{
	m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
	m_pGameInstance->Play_Sound(TEXT("Player_Roll2.wav"), SOUND_PLAYER, g_fEffectVolume);

	m_vMoveVec = Get_MoveVector();
	// 위
	if (m_vMoveVec.y < 0.f)
	{
		m_pPlayer->Change_Animation(TEXT("Roll_Up"));
		m_eCurState = ROLL_UP;
	}
	// 아래
	else if (m_vMoveVec.y > 0.f)
	{
		m_pPlayer->Change_Animation(TEXT("Roll_Down"));
		m_eCurState = ROLL_DOWN;
	}

	if (m_vMoveVec.x != 0.f)
	{
		m_pPlayer->Change_Animation(TEXT("Roll_Horizontal"));
		m_eCurState = ROLL_HOR;
	}

	if (m_vMoveVec.x == 0.f && m_vMoveVec.y == 0.f)
	{
		if (m_pPlayer->Get_Player_Anim()->Get_AnimationTag() == TEXT("IDLE_UP"))
		{
			m_pPlayer->Change_Animation(TEXT("Roll_Up"));
			m_eCurState = ROLL_UP;
			m_vMoveVec.y = -1.f;
		}
		else if (m_pPlayer->Get_isFilp())
		{
			m_pPlayer->Change_Animation(TEXT("Roll_Horizontal"));
			m_eCurState = ROLL_HOR;
			m_vMoveVec.x = 1.f;
		}
		else
		{
			m_pPlayer->Change_Animation(TEXT("Roll_Horizontal"));
			m_eCurState = ROLL_HOR;
			m_vMoveVec.x = -1.f;
		}
	}

	m_pPlayer->Set_Player_State(CPlayer::ROLL);

	m_fRollEndTimer = 0.f;
	m_bCheckState = true;

	return S_OK;
}

void CState_PlayerRoll::Update(_float fTimeDelta)
{
	m_fRollEndTimer += fTimeDelta;
	Roll(fTimeDelta);
	//Check_State(fTimeDelta);

	if (m_pPlayer->IsEnd_CurrentAnimation())
	{
		m_pPlayer->Change_State(CPlayer::IDLE);
	}

	else if (m_fRollEndTimer >= 1.f)
	{
		m_pPlayer->Change_State(CPlayer::IDLE);
		m_fRollEndTimer = 0.f;
	}
}

void CState_PlayerRoll::End_State()
{
}

_float2 CState_PlayerRoll::Get_MoveVector()
{
	_float2 vMoveVec = { 0.f,0.f };
	if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD)
		vMoveVec.y -= 1.f;
	if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD)
		vMoveVec.y += 1.f;
	if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD)
	{
		vMoveVec.x -= 1.f;
		m_pPlayer->Set_isFilp(false);
	}
	if (m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
	{
		vMoveVec.x += 1.f;
		m_pPlayer->Set_isFilp(true);
	}

	return vMoveVec;
}

void CState_PlayerRoll::Check_State(_float fTimeDelta)
{
	//Check_State_Init();


	Roll(fTimeDelta);


}

void CState_PlayerRoll::Check_State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Client::CState_PlayerRoll::ROLL_UP:
			Roll_Up_Init();
			break;
		case Client::CState_PlayerRoll::ROLL_DOWN:
			Roll_Down_Init();
			break;
		case Client::CState_PlayerRoll::ROLL_HOR:
			Roll_Horizontal_Init();
			break;
		case Client::CState_PlayerRoll::ROLL_DU:
			Roll_Diagonal_Up_Init();
			break;
		case Client::CState_PlayerRoll::ROLL_DD:
			Roll_Diagonal_Down_Init();
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CState_PlayerRoll::Roll(_float fTimeDelta)
{
	_float3 vDirRight = m_vMoveVec.x * m_pPlayer->Get_Player_Trans()->Get_State(CTransform::STATE_RIGHT);
	_float3 vDirLook = m_vMoveVec.y * -1.f * m_pPlayer->Get_Player_Trans()->Get_State(CTransform::STATE_LOOK);

	_float3 vDir = *D3DXVec3Normalize(&vDirRight, &vDirRight) + *D3DXVec3Normalize(&vDirLook, &vDirLook);

	_float3		vPosition = m_pPlayer->Get_Player_Trans()->Get_State(CTransform::STATE_POSITION);

	vPosition += *D3DXVec3Normalize(&vDir, &vDir) * m_pPlayer->Get_Player_Trans()->Get_SpeedPerSec() * 0.8f * fTimeDelta;

	m_pPlayer->Get_Player_Trans()->Set_State(CTransform::STATE_POSITION, vPosition);

}

void CState_PlayerRoll::Roll_Down(_float fTimeDelta)
{
	m_pPlayer->Get_Player_Trans()->Go_Backward(fTimeDelta * 0.8f);
}

void CState_PlayerRoll::Roll_Horizontal(_float fTimeDelta)
{
	if (m_pPlayer->Get_isFilp())
	{
		m_pPlayer->Get_Player_Trans()->Go_Right(fTimeDelta * 0.8f);
	}

	else if (!(m_pPlayer->Get_isFilp()))
	{
		m_pPlayer->Get_Player_Trans()->Go_Left(fTimeDelta * 0.8f);
	}
}

void CState_PlayerRoll::Roll_Diagonal_Up(_float fTimeDelta)
{
	_float3 vPos = m_pPlayer->Get_Player_Trans()->Get_State(CTransform::STATE_POSITION);
	m_vMoveVec = Get_MoveVector();

	if (D3DXVec2Length(&m_vMoveVec) != 0.f)
	{
		D3DXVec2Normalize(&m_vMoveVec, &m_vMoveVec);
		vPos.x += m_vMoveVec.x * fTimeDelta * 0.8f * 30.0f;
		vPos.z -= m_vMoveVec.y * fTimeDelta * 0.8f * 30.0f;
		m_pPlayer->Get_Player_Trans()->Set_State(CTransform::STATE_POSITION, vPos);
	}

	if (m_vMoveVec.y < 0.f)
	{
		m_eCurState = PLAYER_ROLL::ROLL_DU;
	}

	// 아래
	if (m_vMoveVec.y > 0.f)
	{
		if (m_vMoveVec.x == 0.f)
		{
			m_eCurState = PLAYER_ROLL::ROLL_DOWN;
		}

		else
		{
			m_eCurState = PLAYER_ROLL::ROLL_DD;
		}

	}
}

void CState_PlayerRoll::Roll_Diagonal_Down(_float fTimeDelta)
{
	_float3 vPos = m_pPlayer->Get_Player_Trans()->Get_State(CTransform::STATE_POSITION);
	m_vMoveVec = Get_MoveVector();

	if (D3DXVec2Length(&m_vMoveVec) != 0.f)
	{
		D3DXVec2Normalize(&m_vMoveVec, &m_vMoveVec);
		vPos.x += m_vMoveVec.x * fTimeDelta * 0.8f * 30.0f;
		vPos.z -= m_vMoveVec.y * fTimeDelta * 0.8f * 30.0f;
		m_pPlayer->Get_Player_Trans()->Set_State(CTransform::STATE_POSITION, vPos);
	}

	if (m_vMoveVec.y < 0.f)
	{
		m_eCurState = PLAYER_ROLL::ROLL_DU;
	}

	// 아래
	if (m_vMoveVec.y > 0.f)
	{
		m_eCurState = PLAYER_ROLL::ROLL_DD;

	}
}

void CState_PlayerRoll::Roll_Up_Init()
{
	m_pPlayer->Change_Animation(TEXT("Roll_Up"));
}

void CState_PlayerRoll::Roll_Down_Init()
{
	m_pPlayer->Change_Animation(TEXT("Roll_Down"));
}

void CState_PlayerRoll::Roll_Horizontal_Init()
{
	m_pPlayer->Change_Animation(TEXT("Roll_Horizontal"));
}

void CState_PlayerRoll::Roll_Diagonal_Up_Init()
{
	m_pPlayer->Change_Animation(TEXT("Roll_Horizontal"));
}

void CState_PlayerRoll::Roll_Diagonal_Down_Init()
{
	m_pPlayer->Change_Animation(TEXT("Roll_Horizontal"));
}

CState_PlayerRoll* CState_PlayerRoll::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_PlayerRoll* pInstance = new CState_PlayerRoll(pFsm);
	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_PlayerRoll"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_PlayerRoll::Free()
{
	__super::Free();
}
