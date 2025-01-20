#include "State_EligorBurp.h"
#include "GameInstance.h"

#include "Boss_Eligor.h"


CState_EligorBurp::CState_EligorBurp(CFsm* pFsm)
	:CState(pFsm)
{
}

HRESULT CState_EligorBurp::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;
	return S_OK;
}

HRESULT CState_EligorBurp::Start_State()
{
	m_pEligor = static_cast<CBoss_Eligor*>(m_pFsm->Get_Owner());
	m_pEligor->Change_Animation(TEXT("Roar"));
	m_pEligor->Set_Eilgor_State(CBoss_Eligor::BURP);
	m_pGameInstance->Play_Sound(TEXT("Elligos_Roar.wav"), SOUND_ELIGOR, g_fEffectVolume);
	m_bBatHere = false;

	return S_OK;
}

void CState_EligorBurp::Update(_float fTimeDelta)
{
	if (m_pEligor->Get_MonInfo().iHp <= 0.f)
	{
		m_pEligor->Change_State(CBoss_Eligor::TRANS_F);
	}


	else if (!m_bBatHere)
	{
		Create_Bat();
	}

	if (m_pEligor->IsEnd_CurrentAnimation())
		m_pEligor->Change_State(CBoss_Eligor::IDLE);
}

void CState_EligorBurp::End_State()
{
}

void CState_EligorBurp::Create_Bat()
{
	m_bBatHere = true;

	CLandObject::LANDOBJECT_DESC Land_Desc = {};
	Land_Desc.pTerrainTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
	Land_Desc.pTerrainVIBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
	Land_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
	Land_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

	for (size_t i = 0; i < 3; i++)
	{
		CMonster::MONSTER_INFO Monster_Info = {};
		Monster_Info.iHp = 5;
		Monster_Info.iAttackPower = 3;
		if(m_pGameInstance->Find_Player()->Get_isFilp())
		{
			Monster_Info.vInitPosition.x = m_pGameInstance->Find_Player()->Get_Position().x - rand() % 6 + 2.f;
		}
		else if (!m_pGameInstance->Find_Player()->Get_isFilp())
		{
			Monster_Info.vInitPosition.x = m_pGameInstance->Find_Player()->Get_Position().x + rand() % 6 + 2.f;
		}
		if (Monster_Info.vInitPosition.x > 22.f
			|| Monster_Info.vInitPosition.x < 0.f)
		{
			--i;
			continue;
		}
		Monster_Info.vInitPosition.z = m_pGameInstance->Find_Player()->Get_Position().z + rand() % 8+3.f;
		if (Monster_Info.vInitPosition.z > 15.f
			|| Monster_Info.vInitPosition.z < 0.f)
		{
			--i;
			continue;
		}
		Monster_Info.vInitPosition.y = m_pEligor->Get_Position().y;
		Monster_Info.tLandObjectDesc = Land_Desc;
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bat"),sizeof(CMonster::MONSTER_INFO), & Monster_Info);
	}

	for (size_t i = 0; i < 3; i++)
	{
		CMonster::MONSTER_INFO Monster_Info = {};
		Monster_Info.iHp = 5;
		Monster_Info.iAttackPower = 3;
		if (m_pGameInstance->Find_Player()->Get_isFilp())
		{
			Monster_Info.vInitPosition.x = m_pGameInstance->Find_Player()->Get_Position().x - rand() % 6 + 2.f;
		}
		else if (!m_pGameInstance->Find_Player()->Get_isFilp())
		{
			Monster_Info.vInitPosition.x = m_pGameInstance->Find_Player()->Get_Position().x + rand() % 6 + 2.f;
		}
		if (Monster_Info.vInitPosition.x >= 22.f
			|| Monster_Info.vInitPosition.x <= 0.f)
		{
			--i;
			continue;
		}
		Monster_Info.vInitPosition.z = m_pGameInstance->Find_Player()->Get_Position().z + rand() % 8 - 5.f;
		if (Monster_Info.vInitPosition.z >= 15.f
			|| Monster_Info.vInitPosition.z <= 0.f)
		{
			--i;
			continue;
		}
		Monster_Info.vInitPosition.y = m_pEligor->Get_Position().y;
		Monster_Info.tLandObjectDesc = Land_Desc;
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bat"),sizeof(CMonster::MONSTER_INFO), &Monster_Info);
	}
}

CState_EligorBurp* CState_EligorBurp::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_EligorBurp* pInstance = new CState_EligorBurp(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_EligorBurp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_EligorBurp::Free()
{
	__super::Free();
}
