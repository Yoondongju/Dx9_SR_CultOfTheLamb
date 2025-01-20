#include "State_HekatRoar.h"
#include "Boss_Hekat.h"
#include "GameInstance.h"

#include "MonsterBone.h"

#include "Frog.h"
#include"Bat.h"
#include "Human.h"
#include "Turret.h"

CState_HekatRoar::CState_HekatRoar(CFsm* pFsm)
	:CState(pFsm)
{
}

HRESULT CState_HekatRoar::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_HekatRoar::Start_State()
{
	m_pHekat = static_cast<CBoss_Hekat*>(m_pFsm->Get_Owner());
	m_pHekat->Change_Animation(TEXT("Roar"));
	m_pHekat->Set_Hekat_State(CBoss_Hekat::ROAR);
	m_pGameInstance->Play_Sound(TEXT("Heket_Attack0.wav"), SOUND_HEKAT, g_fEffectVolume);
	m_bMonsterNot = true;

	return S_OK;
}

void CState_HekatRoar::Update(_float fTimeDelta)
{
	m_iMonsterNumber = rand() % 4;
	if (m_bMonsterNot)
	{
		switch (m_iMonsterNumber)
		{
		case 0:
			Create_Frog();
			break;
		case 1:
			Create_Bat();
			break;
		case 2:
			Create_Human();
			break;
		case 3:
			Create_Turret();
			break;
		}

		m_bMonsterNot = false;
	}

	if (m_pHekat->IsEnd_CurrentAnimation())
		m_pHekat->Change_State(CBoss_Hekat::IDLE);
}

void CState_HekatRoar::End_State()
{
}

HRESULT CState_HekatRoar::Create_Frog()
{
	CLandObject::LANDOBJECT_DESC Land_Desc = {};
	Land_Desc.pTerrainTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON2, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
	Land_Desc.pTerrainVIBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON2, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
	Land_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
	Land_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

	CMonster::MONSTER_INFO Monster_Info = {};
	Monster_Info.iHp = 3;
	Monster_Info.iAttackPower = 1;
	Monster_Info.vInitPosition = m_pHekat->Get_Position();
	Monster_Info.tLandObjectDesc = Land_Desc;

	for (size_t i = 0; i < 3; i++)
	{
		Monster_Info.vInitPosition.x = m_pGameInstance->Find_Player()->Get_Position().x + rand() % 6;
		if (Monster_Info.vInitPosition.x >= 20.f
			|| Monster_Info.vInitPosition.x <= 0.f)
		{
			--i;
			continue;
		}
		Monster_Info.vInitPosition.z = m_pGameInstance->Find_Player()->Get_Position().z + rand() % 8;
		if (Monster_Info.vInitPosition.z >= 15.f
			|| Monster_Info.vInitPosition.z <= 0.f)
		{
			--i;
			continue;
		}
		Monster_Info.vInitPosition.y = m_pHekat->Get_Position().y;
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);
	}

	for (size_t i = 0; i < 3; i++)
	{
		Monster_Info.vInitPosition.x = m_pGameInstance->Find_Player()->Get_Position().x - rand() % 6;
		if (Monster_Info.vInitPosition.x >= 20.f
			|| Monster_Info.vInitPosition.x <= 0.f)
		{
			--i;
			continue;
		}
		Monster_Info.vInitPosition.z = m_pGameInstance->Find_Player()->Get_Position().z - rand() % 8;
		if (Monster_Info.vInitPosition.z >= 15.f
			|| Monster_Info.vInitPosition.z <= 0.f)
		{
			--i;
			continue;
		}
		Monster_Info.vInitPosition.y = m_pHekat->Get_Position().y;
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);
	}


	return S_OK;
}

HRESULT CState_HekatRoar::Create_Bat()
{
	CLandObject::LANDOBJECT_DESC Land_Desc = {};
	Land_Desc.pTerrainTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON2, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
	Land_Desc.pTerrainVIBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON2, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
	Land_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
	Land_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

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
		if (Monster_Info.vInitPosition.x > 22.f
			|| Monster_Info.vInitPosition.x < 0.f)
		{
			--i;
			continue;
		}
		Monster_Info.vInitPosition.z = m_pGameInstance->Find_Player()->Get_Position().z + rand() % 8 + 3.f;
		if (Monster_Info.vInitPosition.z > 15.f
			|| Monster_Info.vInitPosition.z < 0.f)
		{
			--i;
			continue;
		}
		Monster_Info.vInitPosition.y = m_pHekat->Get_Position().y;
		Monster_Info.tLandObjectDesc = Land_Desc;
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bat"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);
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
		Monster_Info.vInitPosition.y = m_pHekat->Get_Position().y;
		Monster_Info.tLandObjectDesc = Land_Desc;
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bat"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);


	}

	return S_OK;
}

HRESULT CState_HekatRoar::Create_Human()
{
	CLandObject::LANDOBJECT_DESC Land_Desc = {};
	Land_Desc.pTerrainTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON2, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
	Land_Desc.pTerrainVIBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON2, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
	Land_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
	Land_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

	for (size_t i = 0; i < 8; i++)
	{
		CMonster::MONSTER_INFO Monster_Info = {};
		Monster_Info.iHp = 5;
		Monster_Info.iAttackPower = 3;
		Monster_Info.tLandObjectDesc = Land_Desc;
		Monster_Info.vInitPosition = _float3(8.f, 2.5f, 6.f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Human"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);

		Monster_Info.vInitPosition = _float3(12.f, 2.5f, 12.f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Human"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);

		Monster_Info.vInitPosition = _float3(12.f, 2.5f, 5.f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Human"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);

		Monster_Info.vInitPosition = _float3(7.5f, 2.5f, 10.f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Human"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);

		Monster_Info.vInitPosition = _float3(15.5f, 2.5f, 10.f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Human"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);

		Monster_Info.vInitPosition = _float3(16.f, 2.5f, 5.f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Human"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);

		Monster_Info.vInitPosition = _float3(7.5f, 2.5f, 10.f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Human"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);


	}

	return S_OK;
}

HRESULT CState_HekatRoar::Create_Turret()
{
	CLandObject::LANDOBJECT_DESC Land_Desc = {};
	Land_Desc.pTerrainTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON2, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
	Land_Desc.pTerrainVIBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON2, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
	Land_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
	Land_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

	for (size_t i = 0; i < 4; i++)
	{
		CMonster::MONSTER_INFO Monster_Info = {};
		Monster_Info.iHp = 5;
		Monster_Info.iAttackPower = 3;
		Monster_Info.tLandObjectDesc = Land_Desc;
		Monster_Info.vInitPosition = _float3(3.f, 2.5f, 13.5f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Turret"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);

		Monster_Info.vInitPosition = _float3(19.5f, 2.5f, 13.5f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Turret"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);

		Monster_Info.vInitPosition = _float3(19.5f, 2.5f, 2.f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Turret"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);

		Monster_Info.vInitPosition = _float3(3.f, 2.5f, 2.f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Turret"), sizeof(CMonster::MONSTER_INFO), &Monster_Info);

	}

	return S_OK;
}


CState_HekatRoar* CState_HekatRoar::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_HekatRoar* pInstance = new CState_HekatRoar(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_HekatRoar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_HekatRoar::Free()
{
	__super::Free();
}

