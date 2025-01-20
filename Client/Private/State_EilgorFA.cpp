#include "State_EilgorFA.h"
#include "Boss_Eligor.h"
#include "GameInstance.h"

CState_EligorFA::CState_EligorFA(CFsm* pFsm)
	:CState(pFsm)
{
}

HRESULT CState_EligorFA::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_EligorFA::Start_State()
{
	m_pEligor = static_cast<CBoss_Eligor*>(m_pFsm->Get_Owner());
	Temp = m_pEligor->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	m_pEligor->Set_Eilgor_State(CBoss_Eligor::FLY_ATTACK);
	m_eCurState = ATTACK_CHARGE;
	m_fAngleSpeed = 8.f;
	m_fSoundTimer = 0.f;
	return S_OK;
}

void CState_EligorFA::Update(_float fTimeDelta)
{

	m_vCenterPos.x = m_pGameInstance->Find_Player()->Get_Position().x;
	m_vCenterPos.y = m_pGameInstance->Find_Player()->Get_Position().z;

	m_vDistance = m_pGameInstance->Find_Player()->Get_Position() - m_pEligor->Get_Position();
	m_fDistanceCheck = D3DXVec3Length(&m_vDistance);

	if (m_pEligor->Get_MonInfo().iHp <= 0.f)
	{
		m_pEligor->Change_State(CBoss_Eligor::TRANS_F);
	}

	Check_State(fTimeDelta);

}

void CState_EligorFA::End_State()
{
}

void CState_EligorFA::Check_State(_float fTimeDelta)
{
	Check_State_Init();

	switch (m_eCurState)
	{
	case Client::CState_EligorFA::ATTACK_CHARGE:
		Attack_Charge();
		break;
	case Client::CState_EligorFA::ATTACK:
		Attack(fTimeDelta);
		break;
	case Client::CState_EligorFA::ATTACK_STOP:
		Attack_Stop();
		break;
	}
}

void CState_EligorFA::Check_State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Client::CState_EligorFA::ATTACK_CHARGE:
			Attack_Charge_Init();
			break;
		case Client::CState_EligorFA::ATTACK:
			Attack_Init();
			break;
		case Client::CState_EligorFA::ATTACK_STOP:
			Attack_Stop_Init();
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CState_EligorFA::Attack_Charge()
{
	if (m_pEligor->IsEnd_CurrentAnimation())
		m_eCurState = ATTACK;
}

void CState_EligorFA::Attack(_float fTimeDelta)
{
	m_fSoundTimer += fTimeDelta;

	if (m_fSoundTimer > 0.2f)
	{
		m_pGameInstance->Play_SoundRepeat(TEXT("Elligos_Attack2.wav"), SOUND_ELIGOR, 1.f);
		m_fSoundTimer = 0.f;
	}

	if (m_fDistanceCheck > 2.5f)
	{
		_float3 vPlayerPos = m_pGameInstance->Find_Player()->Get_Position();
		_float3 vMonsterPos = m_pEligor->Get_Position();

		m_vDir = vPlayerPos - vMonsterPos;
		D3DXVec3Normalize(&m_vDir, &m_vDir);

		Temp.x += m_vDir.x * fTimeDelta * 10.f;
		Temp.z += m_vDir.z * fTimeDelta * 10.f;

		m_pEligor->Get_Transform()->Set_State(CTransform::STATE_POSITION, Temp);
	}

	else
	{
		m_fAngle += fTimeDelta* m_fAngleSpeed;
		_float3 vMonsterPos = m_pEligor->Get_Position();
		vMonsterPos.x = m_pGameInstance->Find_Player()->Get_Position().x + cos(m_fAngle);
		vMonsterPos.z = m_pGameInstance->Find_Player()->Get_Position().z - sin(m_fAngle);

		m_pEligor->Get_Transform()->Set_State(CTransform::STATE_POSITION, vMonsterPos);
	}

	m_fFlyTimer += fTimeDelta;
	if (m_fFlyTimer > 3.f)
	{
		m_eCurState = ATTACK_STOP;
		m_fFlyTimer = 0.f;
	}

}

void CState_EligorFA::Attack_Stop()
{
	if (m_pEligor->IsEnd_CurrentAnimation())
		m_pEligor->Change_State(CBoss_Eligor::IDLE);
}

void CState_EligorFA::Attack_Charge_Init()
{
	m_pGameInstance->Play_Sound(TEXT("Elligos_Charge.wav"), SOUND_ELIGOR, 1.f);
	m_pEligor->Change_Animation(TEXT("Attack_Charge"));
}

void CState_EligorFA::Attack_Init()
{
	m_pEligor->Change_Animation(TEXT("Attack"));
}

void CState_EligorFA::Attack_Stop_Init()
{
	m_pGameInstance->StopSound(SOUND_ELIGOR);
	m_pEligor->Change_Animation(TEXT("Attack_Stop"));
}

CState_EligorFA* CState_EligorFA::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_EligorFA* pInstance = new CState_EligorFA(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_EligorFA"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_EligorFA::Free()
{
	__super::Free();
}
