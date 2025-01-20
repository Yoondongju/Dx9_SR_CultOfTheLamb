
#include "State_DeathCatDisappear.h"
#include "Boss_DeathCat.h"
#include "Layer.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

#include "Terrain.h"

CState_DeathCatDisappear::CState_DeathCatDisappear(class CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_DeathCatDisappear::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	CGameObject* pTerrain = m_pGameInstance->Find_Object(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Terrain"), 0);

	m_fTerrainX = dynamic_cast<CTerrain*>(pTerrain)->Get_TerrainSizeX() - 7.f;
	m_fTerrainZ = dynamic_cast<CTerrain*>(pTerrain)->Get_TerrainSizeZ() - 7.f;

	return S_OK;
}

HRESULT CState_DeathCatDisappear::Start_State()
{
	m_pDeathCat = dynamic_cast<CBoss_DeathCat*>(m_pFsm->Get_Owner());

	m_isDisappear = false;
	m_fDisappearTime = 0.f;

	m_pDeathCat->Change_Animation(TEXT("Disappear"));

	m_fTeleportPoint = _float3(_float(rand() % (_uint)m_fTerrainX + 4.f), 0.f, _float(rand() % (_uint)m_fTerrainZ + 4.f));

	if (CBoss_DeathCat::IDLE == m_pFsm->Get_PrevState())
    		m_fTeleportPoint = _float3((m_fTerrainX + 7.f) * 0.5f, 0.f, (m_fTerrainZ + 7.f) * 0.5f);

	return S_OK;
} 
 

void CState_DeathCatDisappear::Update(_float fTimeDelta)
{
	if (m_isDisappear)
		m_fDisappearTime += fTimeDelta;

	if (!m_isDisappear && m_pDeathCat->IsEnd_CurrentAnimation())
	{
		m_pDeathCat->Set_IsActive(false);
		m_pDeathCat->Off_ColliderActive();


		m_isDisappear = true;
	}

	if (m_pGameInstance->Find_Layer(LEVEL_DUNGEON3, TEXT("Layer_Boss"))->Get_ObjectList().size() <= 1)
	{
		if (m_isDisappear && m_fDisappearTime > 2.3f)
		{
			m_fDisappearTime = 0.f;
			m_isAppear = true;
			m_pDeathCat->Set_Position(m_fTeleportPoint);
			m_pDeathCat->Set_IsActive(true);
			m_pDeathCat->Change_Animation(TEXT("Appear"));
		}

		if (m_isAppear && m_pDeathCat->IsEnd_CurrentAnimation())
		{
			m_isDisappear = false;
			m_isAppear = false;
			m_fDisappearTime = 0.f;

			m_pDeathCat->On_ColliderActive();

			if (CBoss_DeathCat::IDLE == m_pFsm->Get_PrevState() 
				&& m_pDeathCat->Get_MonInfo().iHp <= m_pDeathCat->Get_MonInfo().iMaxHp * 0.5f)
			{
				m_pDeathCat->Change_State(CBoss_DeathCat::FLYBALLATTACK);
			}
			else
			{
				m_pDeathCat->Change_State(CBoss_DeathCat::IDLE);
			}
		}
	}
}

void CState_DeathCatDisappear::End_State()
{
	m_isDisappear = false;
	m_fDisappearTime = 0.f;
}

CState_DeathCatDisappear* CState_DeathCatDisappear::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_DeathCatDisappear* pInstance = new CState_DeathCatDisappear(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_DeathCatDisappear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_DeathCatDisappear::Free()
{
	__super::Free();
}
