#include "State_HekatIdle.h"
#include "Boss_Hekat.h"
#include "GameInstance.h"

#include "Layer.h"

#include "Camera.h"

CState_HekatIdle::CState_HekatIdle(CFsm* pFsm)
	:CState(pFsm)
{
}

HRESULT CState_HekatIdle::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_HekatIdle::Start_State()
{
	m_pHekat = static_cast<CBoss_Hekat*>(m_pFsm->Get_Owner());
	m_pHekat->Change_Animation(TEXT("IDLE"));
	m_pHekat->Set_Hekat_State(CBoss_Hekat::IDLE);
	m_fAttackTimer = 0.f;
	return S_OK;
}

void CState_HekatIdle::Update(_float fTimeDelta)
{

	if (m_pHekat->Get_MonInfo().iHp <= 0.f)
	{
		m_pHekat->Change_State(CBoss_Hekat::DIE);
	}

	m_iBulletChoice = rand() % 2;
	m_fAttackTimer += fTimeDelta;
	m_vDistance = m_pGameInstance->Find_Player()->Get_Position() - m_pHekat->Get_Position();
	m_fDistance = D3DXVec3Length(&m_vDistance);

	if(m_fAttackTimer>1.5f)
	{
		if (m_pGameInstance->Find_Layer(LEVELID::LEVEL_DUNGEON2, TEXT("Layer_Monster")) == nullptr
			|| m_pGameInstance->Find_Layer(LEVELID::LEVEL_DUNGEON2, TEXT("Layer_Monster"))->Get_ObjectList().size() <= 0)
		{
			m_pHekat->Change_State(CBoss_Hekat::ROAR);
		}

		else if (m_fDistance > 8.f)
		{
			m_pHekat->Change_State(CBoss_Hekat::JUMP);
		}

		else
		{
			switch (m_iBulletChoice)
			{
			case 0:
				m_pHekat->Change_State(CBoss_Hekat::THROW_BOMB);
				break;
			case 1:
				m_pHekat->Change_State(CBoss_Hekat::LAY_EGG);
				break;
			}
		}

		m_fAttackTimer = 0.f;
	}
}

void CState_HekatIdle::End_State()
{
}

CState_HekatIdle* CState_HekatIdle::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_HekatIdle* pInstance = new CState_HekatIdle(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_HekatIdle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_HekatIdle::Free()
{
	__super::Free();
}
