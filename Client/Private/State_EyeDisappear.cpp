
#include "State_EyeDisappear.h"
#include "Boss_Eye.h"

#include "Terrain.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"
#include "Effect.h"

#include "Level_Dungeon03.h"
#include "Eye_Controller.h"

CState_EyeDisappear::CState_EyeDisappear(class CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_EyeDisappear::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	CGameObject* pTerrain = m_pGameInstance->Find_Object(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Terrain"), 0);

	m_fTerrainX = dynamic_cast<CTerrain*>(pTerrain)->Get_TerrainSizeX() - 7.f;
	m_fTerrainZ = dynamic_cast<CTerrain*>(pTerrain)->Get_TerrainSizeZ() - 7.f;

	return S_OK;
}

HRESULT CState_EyeDisappear::Start_State()
{
	m_pEye = dynamic_cast<CBoss_Eye*>(m_pFsm->Get_Owner());

	m_isDisappear = false;
	m_isAppear = false;
	m_fMoveTime = 0.f;
	m_fDustDelayTime = 0.f;
	//무적 상태 주기

	m_pEye->Change_Animation(TEXT("Disappear"), 0);

	return S_OK;
}


void CState_EyeDisappear::Update(_float fTimeDelta)
{
	if (!m_isDisappear && m_pEye->IsEnd_CurrentAnimation())
	{
		m_isDisappear = true;
		m_pEye->Set_IsHide(true);

		if (rand() % 2)
		{
			m_vMoveDir.x = -_float(rand() % 10 + 1);
		}
		else
		{
			m_vMoveDir.x = _float(rand() % 10 + 1);
		}

		if (rand() % 2)
		{
			m_vMoveDir.z = _float(rand() % 10 + 1);
		}
		else
		{
			m_vMoveDir.z = -_float(rand() % 10 + 1);
		}
	}

	if (m_isDisappear)
	{
		m_fMoveTime += fTimeDelta;

		if (!m_isFristDie 
			&& dynamic_cast<CLevel_Dungeon03*>(m_pGameInstance->Get_CurLevel())->Get_EyeController()->Get_IsFirsDie())
		{
			if (!m_isAppear && m_fMoveTime >= 11.f)
			{
				m_isAppear = true;
				m_pEye->Set_IsHide(false);
				m_pEye->Change_Animation(TEXT("Appear"), 0);
			}

			if (!m_isAppear)
			{
				m_fDustDelayTime += fTimeDelta;

				if (m_pEye->Get_Position().x > m_fTerrainX
					|| m_pEye->Get_Position().x < 4.f)
				{
					m_vMoveDir.x = -m_vMoveDir.x;
				}

				if (m_pEye->Get_Position().z > m_fTerrainZ
					|| m_pEye->Get_Position().z < 4.f)
				{
					m_vMoveDir.z = -m_vMoveDir.z;
				}

				m_pEye->Move_Dir(fTimeDelta, m_vMoveDir, 3.f);
			}

			if (m_isAppear && m_pEye->IsEnd_CurrentAnimation())
			{
				m_pEye->Change_State(CBoss_Eye::IDLE);
			}
		}
		else
		{
			if (!m_isAppear && m_fMoveTime >= 3.f)
			{
				m_isAppear = true;
				m_pEye->Set_IsHide(false);
				m_pEye->Change_Animation(TEXT("Appear"), 0);
			}

			if (!m_isAppear)
			{
				m_fDustDelayTime += fTimeDelta;

				if (m_pEye->Get_Position().x > m_fTerrainX
					|| m_pEye->Get_Position().x < 4.f)
				{
					m_vMoveDir.x = -m_vMoveDir.x;
				}

				if (m_pEye->Get_Position().z > m_fTerrainZ
					|| m_pEye->Get_Position().z < 4.f)
				{
					m_vMoveDir.z = -m_vMoveDir.z;
				}

				if (m_fDustDelayTime > 0.7f)
				{
					CEffect::EFFECT_DESC Effect_Desc = {};
					Effect_Desc.vPos = m_pEye->Get_Position();
					Effect_Desc.vPos.y = m_pEye->Get_Position().y - 2.f;
					Effect_Desc.vScale = _float3(2.0f, 2.0f, 1.f);
					Effect_Desc.fEffectDura = 0.4f;
					Effect_Desc.strTextureTag = TEXT("WalkDust");
					Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
					Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
					m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
					m_fDustDelayTime = 0.f;
				}
				m_pEye->Move_Dir(fTimeDelta, m_vMoveDir, 3.f);
			}

			if (m_isAppear && m_pEye->IsEnd_CurrentAnimation())
			{
				m_pEye->Change_State(CBoss_Eye::IDLE);
			}
		}

	}
}

void CState_EyeDisappear::End_State()
{
} 


CState_EyeDisappear* CState_EyeDisappear::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_EyeDisappear* pInstance = new CState_EyeDisappear(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_DeatCatIdle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_EyeDisappear::Free()
{
	__super::Free();
}
