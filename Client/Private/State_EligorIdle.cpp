#include "State_EligorIdle.h"
#include "Boss_Eligor.h"
#include "GameInstance.h"

#include "Layer.h"

CState_EligorIdle::CState_EligorIdle(CFsm* pFsm)
	:CState(pFsm)
{
}

HRESULT CState_EligorIdle::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;
    return S_OK;
}

HRESULT CState_EligorIdle::Start_State()
{
	m_pEligor = static_cast<CBoss_Eligor*>(m_pFsm->Get_Owner());
	m_pEligor->Change_Animation(TEXT("IDLE"));
	m_pEligor->Set_Eilgor_State(CBoss_Eligor::IDLE);
	m_vDistance = m_pGameInstance->Find_Player()->Get_Position() - m_pEligor->Get_Position();
	m_fDistanceCheck = D3DXVec3Length(&m_vDistance);
	m_fAttackTimer = 0.f;

    return S_OK;
}

void CState_EligorIdle::Update(_float fTimeDelta)
{
	m_fAttackTimer += fTimeDelta;

	if (m_pEligor->Get_MonInfo().iHp <= 0.f)
	{
		m_pEligor->Change_State(CBoss_Eligor::TRANS_F);
	}

	if(m_fAttackTimer>1.5f)
	{

		if (m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Monster")) == nullptr
			|| m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Monster"))->Get_ObjectList().empty())
		{
			m_pEligor->Change_State(CBoss_Eligor::BURP);
		}
		
		else if (m_fDistanceCheck > 7.f)
		{
			m_pEligor->Change_State(CBoss_Eligor::FLY_ATTACK);
		}

		else
			m_pEligor->Change_State(CBoss_Eligor::LAY_EGG);

		m_fAttackTimer = 0.f;
	}

}

void CState_EligorIdle::End_State()
{
}

CState_EligorIdle* CState_EligorIdle::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_EligorIdle* pInstance = new CState_EligorIdle(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_EligorIdle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_EligorIdle::Free()
{
	__super::Free();
}
