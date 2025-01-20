#include "State_Aim_Attack2.h"
#include "Boss_Aim.h"

#include "GameInstance.h"

#include "Transform.h"
#include "Animator.h"

CState_Aim_Attack2::CState_Aim_Attack2(class CFsm* pFsm)
	: CState{ pFsm }
{
	srand((unsigned int)time(NULL));
}

HRESULT CState_Aim_Attack2::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Aim_Attack2::Start_State()
{
	m_pAim = dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner());
	

	m_pAim->Change_Animation(TEXT("Attack2"), 0);
	m_bAttackBegin_FilpCheck = false;

	return S_OK;
}


void CState_Aim_Attack2::Update(_float fTimeDelta)
{
	NextToDoing();

	_float3 vPlayerPos = m_pGameInstance->Find_Player()->Get_Position();


	if (m_fTime > 0.7f)
	{
		m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_ChainAttack"),
			sizeof(_float3), &vPlayerPos);

		m_fTime = 0.f;
	}

	


	m_fTime += fTimeDelta;

}

void CState_Aim_Attack2::NextToDoing()
{
	_float3 vAimPos = m_pAim->Get_Position();
	_float3 vPlayerPos = m_pGameInstance->Find_Player()->Get_Position();
	_float3 vDir = vPlayerPos - vAimPos;

	if (false == m_bAttackBegin_FilpCheck)
	{
		if (vDir.x < 0)
			m_pAim->Set_isFilp(true);
		else
			m_pAim->Set_isFilp(false);

		m_bAttackBegin_FilpCheck = true;
	}



	if (fabs(vPlayerPos.x - vAimPos.x) > 10.f ||
		fabs(vPlayerPos.z - vAimPos.z) > 10.f)
	{
		dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner())->Change_State(CBoss_Aim::AIM_STATE::WALK);
		return;
	}


	if (static_cast<CAnimator*>(m_pAim->Find_Component(COMPONENT_TYPE::ANIMATOR))->IsEnd_CurrentAnimation())
	{
		CBoss_Aim::AIM_STATE iAttackPatten = (CBoss_Aim::AIM_STATE)((rand() % 3) + 3);
		switch (iAttackPatten)
		{
		case Client::CBoss_Aim::ATTACK1:
			dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner())->Change_State(CBoss_Aim::AIM_STATE::ATTACK1);
			break;
		case Client::CBoss_Aim::ATTACK2:
			dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner())->Change_State(CBoss_Aim::AIM_STATE::ATTACK2);
			break;
		case Client::CBoss_Aim::ATTACK3:
			dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner())->Change_State(CBoss_Aim::AIM_STATE::ATTACK3);
			break;
		default:
			break;
		}
	}

}

void CState_Aim_Attack2::End_State()
{
}

CState_Aim_Attack2* CState_Aim_Attack2::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_Aim_Attack2* pInstance = new CState_Aim_Attack2(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Aim_Attack2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Aim_Attack2::Free()
{
	__super::Free();

	
}
