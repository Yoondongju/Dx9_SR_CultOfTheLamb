#include "State_GusionIdle.h"
#include "Boss_Gusion.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

#include "Layer.h"

#include "Camera.h"

CState_GusionIdle::CState_GusionIdle(class CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_GusionIdle::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_GusionIdle::Start_State()
{
	m_pGusion = static_cast<CBoss_Gusion*>(m_pFsm->Get_Owner());

	m_pGusion->Change_Animation(TEXT("IDLE"));
	m_pGusion->Set_Gusion_State(CBoss_Gusion::IDLE);
	m_vDistance = { 0.f,0.f,0.f };
	m_fDistanceCheck = 0.f;
	m_fAttackTimer = 0.f;

	return S_OK;
}


void CState_GusionIdle::Update(_float fTimeDelta)
{
	m_fAttackTimer += fTimeDelta;

	_float3 vDistance = m_pGameInstance->Find_Player()->Get_Position() - m_pGusion->Get_Position();
	m_fDistanceCheck = D3DXVec3Length(&vDistance);

	if (m_pGusion->Get_MonInfo().iHp <= 0.f)
	{
		m_pGusion->Change_State(CBoss_Gusion::DIE);
	}
	
	if (m_fAttackTimer > 1.5f)
	{
		if (m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Monster")) == nullptr
			|| m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Monster"))->Get_ObjectList().size() <= 0)
		{
			m_pGusion->Change_State(CBoss_Gusion::BURP);
		}

		else if (m_fDistanceCheck > 7.f)
		{
			m_pGusion->Change_State(CBoss_Gusion::JUMP);
		}

		else
			m_pGusion->Change_State(CBoss_Gusion::LAY_EGG);

		m_fAttackTimer = 0.f;
	}

}

void CState_GusionIdle::End_State()
{
}

CState_GusionIdle* CState_GusionIdle::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_GusionIdle* pInstance = new CState_GusionIdle(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_GusionIdle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_GusionIdle::Free()
{
	__super::Free();
}
