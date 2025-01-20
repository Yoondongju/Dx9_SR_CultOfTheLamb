#include "State_Aim_Walk.h"
#include "Boss_Aim.h"

#include "GameInstance.h"

#include "Transform.h"
#include "Animator.h"

CState_Aim_Walk::CState_Aim_Walk(class CFsm* pFsm)
	: CState{ pFsm }
{
}

HRESULT CState_Aim_Walk::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Aim_Walk::Start_State()
{
	m_pAim = dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner());
	


	CTransform* pAimTransform = static_cast<CTransform*>(m_pAim->Find_Component(COMPONENT_TYPE::TRANSFORM));
	_float3 vAimScale = pAimTransform->Get_Scaled();


	


	m_pAim->Change_Animation(TEXT("Walk"), 0);


	return S_OK;
}


void CState_Aim_Walk::Update(_float fTimeDelta)
{

	Track_ToPlayer(fTimeDelta);
	
}

void CState_Aim_Walk::Track_ToPlayer(_float fTimeDelta)
{
	_float3 vPlayerPos = m_pGameInstance->Find_Player()->Get_Position();
	_float3 vAimPos = m_pAim->Get_Position();

	

	if (fabs(vPlayerPos.x - vAimPos.x) <= 2.5f  &&
		fabs(vPlayerPos.z - vAimPos.z) <= 0.6f)
	{
		dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner())->Change_State(CBoss_Aim::AIM_STATE::ATTACK1);

		m_fTime = 0.f;
	}
	else
	{

		if(vPlayerPos.x > vAimPos.x)
			vPlayerPos.x -= 2.f;
		else if (vPlayerPos.x < vAimPos.x)
			vPlayerPos.x += 2.f;
		

		_float3 vDir = vPlayerPos - vAimPos;

		if (vDir.x < 0)
			m_pAim->Set_isFilp(true);
		else
			m_pAim->Set_isFilp(false);


		m_fTime += fTimeDelta;
		m_pAim->Get_Transform()->Move_Dir(m_fTime, vDir, 2.f);
	}
		


	
}




void CState_Aim_Walk::End_State()
{
	
}


CState_Aim_Walk* CState_Aim_Walk::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_Aim_Walk* pInstance = new CState_Aim_Walk(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Aim_Walk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Aim_Walk::Free()
{
	__super::Free();


}
