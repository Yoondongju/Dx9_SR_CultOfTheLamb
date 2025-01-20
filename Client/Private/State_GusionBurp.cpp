#include "State_GusionBurp.h"

#include "Boss_Gusion.h"
#include "Frog.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

CState_GusionBurp::CState_GusionBurp(class CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_GusionBurp::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_GusionBurp::Start_State()
{
	m_pGusion = static_cast<CBoss_Gusion*>(m_pFsm->Get_Owner());
	m_bFrogCreate = false;
	m_pGusion->Change_Animation(TEXT("Burp"));
	m_pGusion->Set_Gusion_State(CBoss_Gusion::BURP);
	m_pGameInstance->Play_Sound(TEXT("Gusion_Roar.wav"), SOUND_GUSION, g_fEffectVolume);
	return S_OK;
}


void CState_GusionBurp::Update(_float fTimeDelta)
{
	if (m_pGusion->Get_MonInfo().iHp <= 0.f)
	{
		m_pGusion->Change_State(CBoss_Gusion::DIE);
	}

	if (m_pGusion->Get_CurrentFrame() >= 22 && m_pGusion->Get_CurrentFrame() <= 38)
	{
		if(!m_bFrogCreate)
			Create_Frog();
	}

	if (m_pGusion->IsEnd_CurrentAnimation())
		m_pGusion->Change_State(CBoss_Gusion::IDLE);
}

void CState_GusionBurp::End_State()
{
}

HRESULT CState_GusionBurp::Create_Frog()
{
	m_bFrogCreate = true;
	CLandObject::LANDOBJECT_DESC Land_Desc = {};
	Land_Desc.pTerrainTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
	Land_Desc.pTerrainVIBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
	Land_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
	Land_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

	CMonster::MONSTER_INFO Monster_Info = {};
	Monster_Info.iHp = 3;
	Monster_Info.iAttackPower = 1;
	Monster_Info.vInitPosition = m_pGusion->Get_Position();
	Monster_Info.tLandObjectDesc = Land_Desc;

	for (size_t i = 0; i < 3; i++)
	{
		Monster_Info.vInitPosition.x = m_pGameInstance->Find_Player()->Get_Position().x+rand()%6;
		if (Monster_Info.vInitPosition.x >= 24.f
			|| Monster_Info.vInitPosition.x <= 0.f)
		{
			--i;
			continue;
		}
		Monster_Info.vInitPosition.z = m_pGameInstance->Find_Player()->Get_Position().z + rand() % 8;
		if (Monster_Info.vInitPosition.z >= 18.f
			|| Monster_Info.vInitPosition.z <= 0.f)
		{
			--i;
			continue;
		}
		Monster_Info.vInitPosition.y= m_pGusion->Get_Position().y;
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"),sizeof(CMonster::MONSTER_INFO), & Monster_Info);
	}

		for (size_t i = 0; i < 3; i++)
	{
			Monster_Info.vInitPosition.x = m_pGameInstance->Find_Player()->Get_Position().x - rand() % 6;
			if (Monster_Info.vInitPosition.x >= 24.f
				|| Monster_Info.vInitPosition.x <= 0.f)
			{
				--i;
				continue;
			}
			Monster_Info.vInitPosition.z = m_pGameInstance->Find_Player()->Get_Position().z - rand() % 8;
			if (Monster_Info.vInitPosition.z >= 18.f
				|| Monster_Info.vInitPosition.z <= 0.f)
			{
				--i;
				continue;
			}
		Monster_Info.vInitPosition.y= m_pGusion->Get_Position().y;
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);
	}

	return S_OK;
}

CState_GusionBurp* CState_GusionBurp::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_GusionBurp* pInstance = new CState_GusionBurp(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_GusionBurp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_GusionBurp::Free()
{
	__super::Free();
}
