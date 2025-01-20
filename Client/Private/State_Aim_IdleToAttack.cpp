#include "State_Aim_IdleToAttack.h"
#include "Boss_Aim.h"

#include "GameInstance.h"

#include "Transform.h"
#include "Animator.h"

CState_Aim_IdleToAttack::CState_Aim_IdleToAttack(class CFsm* pFsm)
	: CState{ pFsm }
{
}

HRESULT CState_Aim_IdleToAttack::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Aim_IdleToAttack::Start_State()
{
	m_pAim = dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner());


	m_pAim->Change_Animation(TEXT("IdleToAttack"), 0);


	return S_OK;
}


void CState_Aim_IdleToAttack::Update(_float fTimeDelta)
{
	if(m_pAim->IsEnd_CurrentAnimation())
		dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner())->Change_State(CBoss_Aim::AIM_STATE::WALK);
}


void CState_Aim_IdleToAttack::End_State()
{
	
}


CState_Aim_IdleToAttack* CState_Aim_IdleToAttack::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_Aim_IdleToAttack* pInstance = new CState_Aim_IdleToAttack(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Aim_IdleToAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Aim_IdleToAttack::Free()
{
	__super::Free();

}
