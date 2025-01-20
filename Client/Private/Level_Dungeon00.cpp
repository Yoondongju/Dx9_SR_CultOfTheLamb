
#include "..\Public\Level_Dungeon00.h"
#include "GameInstance.h"

#include "Layer.h"

#include "Player.h"
#include "Monster.h"
#include "Camera.h"
#include "Chest.h"

#include "VIBuffer_Terrain.h"
#include "Terrain.h"
#include "Road.h"
#include "TarotNPC.h"

#include "Client_Operators.h"

#include "PlayerHpUI.h"
#include "RatNPC.h"
#include <PressEtoPickCard.h>

#include "DungeonClearUI.h"
#include "Quest.h"

CLevel_Dungeon00::CLevel_Dungeon00(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device }
{
}

HRESULT CLevel_Dungeon00::Initialize()
{
	m_iStageIndex = 0;
	m_iPreStageIndex = 0;
	m_pGameInstance->Set_StageIndex(m_iStageIndex);
	m_bPlayerChangeOnce = true;
	m_bEnterOnce = true;

	if (FAILED(Load_Village_Map()))
		return E_FAIL;
 
	if (FAILED(Ready_Layer_Camera()))		// 이미 사본은 로고에서 만들어서 카메라 Desc세팅용
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject()))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;


	//if (FAILED(Create_Monster()))
	//	return E_FAIL;


	static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Player_State(CPlayer::CHANGE_LEVEL);
	static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_Animation(TEXT("IDLE"));

	m_vPlayerInitPos = _float3(7.5f, 2.5f, 5.5f);
	m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);
	(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->Set_Position_to_Player();

	CGameInstance::Get_Instance()->PlayBGM(TEXT("BGM_Battle.wav"), g_fBgmVolume);

	return S_OK;
}

void CLevel_Dungeon00::Update(_float fTimeDelta)
{
	m_vWrapDistance = _float3(12.5f, 2.5f, 10.f) - static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_Position();
	m_fWrapDistance = D3DXVec3Length(&m_vWrapDistance);

	m_fSizeTimer += fTimeDelta;
	if (m_fSizeTimer > 0.05f
		&& m_bPlayerSizeOnce)
	{
		_float3 vScale = static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_Transform()->Get_Scaled();

		if (vScale.x <= 5.f && vScale.y <= 5.f)
		{
			vScale.x += 0.25f;
			vScale.y += 0.25f;
			static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_Transform()->Scaling(vScale);
		}
		else
		{
			m_bPlayerSizeOnce = false;
		}

		m_fSizeTimer = 0.f;
	}

	//던전 시작 하트 이펙트
	if (m_fTimeTo_CreateHp > 1.f)
	{
		if (false == m_pPlayerHpUI[2]->GetActivate())
		{
			if (m_fTimeTo_CreateNextHp > 0.3f)
			{
				static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(1.f, 0.4f, 1);

				m_pPlayerHpUI[m_iHpNum]->SetActivate(true);
				m_pPlayerHpUI[m_iHpNum]->GetChildUI().back()->SetActivate(true);
				++m_iHpNum;

				m_fTimeTo_CreateNextHp = 0.f;

			}

			m_fTimeTo_CreateNextHp += fTimeDelta;
		}
	}

	if (true == m_pPlayerHpUI[2]->GetActivate()
		&& m_bPlayerChangeOnce
		&& m_fTimeTo_CreateHp > 6.f)
	{
		static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::IDLE);
		m_bPlayerChangeOnce = false;
	}
	m_fTimeTo_CreateHp += fTimeDelta;

#pragma region 충돌처리 분류
	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Attack")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Attack"), TEXT("Layer_Player"), LEVEL_DUNGEON0, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_MonsterBone")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_MonsterBone"), TEXT("Layer_Player"), LEVEL_DUNGEON0, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Boss")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Boss"), TEXT("Layer_Player"), LEVEL_DUNGEON0, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Boss"))
		&& nullptr != m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Player_Bullet")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Boss"), TEXT("Layer_Player_Bullet"), LEVEL_DUNGEON0, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Item")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Item"), TEXT("Layer_Player"), LEVEL_DUNGEON0, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Object")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_BackGround_Object"), TEXT("Layer_Player"), LEVEL_DUNGEON0, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Monster")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Monster"), TEXT("Layer_Player"), LEVEL_DUNGEON0, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Terrain_Road")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Terrain_Road"), TEXT("Layer_Player"), LEVEL_DUNGEON0, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Monster"))
		&& nullptr != m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Player_Bullet")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Monster"), TEXT("Layer_Player_Bullet"), LEVEL_DUNGEON0, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Boss"))
		&& nullptr != m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Player_Bullet")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Boss"), TEXT("Layer_Player_Bullet"), LEVEL_DUNGEON0, LEVEL_STATIC);

#pragma endregion


	if (Clear_Dungeon())
	{
		if (!m_bExistClearUI && m_fClearUI_OpenTime > 2.f)
		{
			m_pGameInstance->StopAll();
			m_pGameInstance->Play_Sound(TEXT("BGM_DungeonClearUI.wav"), SOUND_BGM, g_fBgmVolume);
			Create_DungeonClearUI();
		}

		CQuest::Get_Instance()->Succeed_Quest(L"구시옹 무찌르기");
		m_fClearUI_OpenTime += fTimeDelta;
	}
	else
	{
		m_fProceedingTime += fTimeDelta;	// 던전 진행 시간
	}


	//스테이지 전환
	if (m_isChangeStage)
	{
		Change_Stage(m_iNextStageIndex);
	}


	if (m_fWrapDistance < 5.f
		&& m_pGameInstance->Get_KeyState(KEY::ENTER) == KEY_STATE::TAP)
	{
		if (m_iStageIndex == 4 && m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Boss"))->Get_ObjectList().size() <= 0)
		{
			static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::WARP_OUT);
			static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Player_PreState(CPlayer::WARP_OUT);
		}

	}
	if (static_cast<CPlayer*>(m_pGameInstance->Find_Player())->IsEnd_CurrentAnimation()
		&& static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_Player_PreState() == CPlayer::WARP_OUT)
	{
		if (m_bWrapOnce)
		{
			m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, LEVEL_GAMEPLAY));

			m_bWrapOnce = false;
		}
	}

	if (m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Monster")) == nullptr
		|| m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Monster"))->Get_ObjectList().size() <= 0)
	{
		if (!m_isActiveChest[0] && m_iStageIndex == 1)
		{
			m_isActiveChest[0] = true;

			if (FAILED(Ready_LandObjects()))
				return;
		}

		if (!m_isActiveChest[1] && m_iStageIndex == 3)
		{
			m_isActiveChest[1] = true;

			if (FAILED(Ready_LandObjects()))
				return;
		}
	}
}

HRESULT CLevel_Dungeon00::Render()
{
	//SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
	return S_OK;
}

HRESULT CLevel_Dungeon00::Ready_Layer_Camera()
{
	CCamera::CAMERA_DESC		Desc{};

	Desc.vEye = _float3(0.f, 9.f, -12.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.fFovy = D3DXToRadian(55.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = D3DXToRadian(90.0f);

	(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->InitCamera_Desc(&Desc);
	//(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->Set_Position_to_Player();
	return S_OK;
}

HRESULT CLevel_Dungeon00::Ready_Layer_BackGround()
{
	CRoad::ROAD_DESC RoadDesc{};
	RoadDesc.iLevelIndex = LEVEL_DUNGEON0;
	RoadDesc.iStageIndex = m_iStageIndex;

	switch (m_iStageIndex)
	{
	case 0:
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Terrain"), TEXT("Prototype_GameObject_Terrain_Dungeon0_Stage0"), &m_iStageIndex)))
			return E_FAIL;

		// 오른쪽 길(1스테이지)
		RoadDesc.vPos = _float3(23.f, 0.1f, 4.5f);
		RoadDesc.iWhereWeGo = 1;
		RoadDesc.eRoadDir = CRoad::RIGHT;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Terrain_Road"), TEXT("Prototype_GameObject_Road_Dungeon0"), &RoadDesc)))
			return E_FAIL;

		break;

	case 1:
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Terrain"), TEXT("Prototype_GameObject_Terrain_Dungeon0_Stage1"), &m_iStageIndex)))
			return E_FAIL;

		// 왼쪽 길(시작)1
		RoadDesc.vPos = _float3(-28.f, 0.1f, 0.f);
		RoadDesc.iWhereWeGo = 0;
		RoadDesc.eRoadDir = CRoad::LEFT;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Terrain_Road"), TEXT("Prototype_GameObject_Road_Dungeon0"), &RoadDesc)))
			return E_FAIL;

		//오른쪽 길(타로)2
		RoadDesc.vPos = _float3(28.f, 0.1f, 0.f);
		RoadDesc.iWhereWeGo = 2;
		RoadDesc.eRoadDir = CRoad::RIGHT;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Terrain_Road"), TEXT("Prototype_GameObject_Road_Dungeon0"), &RoadDesc)))
			return E_FAIL;

		break;

	case 2:
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Terrain"), TEXT("Prototype_GameObject_Terrain_Dungeon0_Stage2"), &m_iStageIndex)))
			return E_FAIL;

		// 왼쪽 스테이지 1
		RoadDesc.vPos = _float3(-28.f, 0.1f, 0.f);
		RoadDesc.iWhereWeGo = 1;
		RoadDesc.eRoadDir = CRoad::LEFT;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Terrain_Road"), TEXT("Prototype_GameObject_Road_Dungeon0"), &RoadDesc)))
			return E_FAIL;

		// 위쪽 스테이지 3
		RoadDesc.vPos = _float3(15.f, 0.1f, 47.f);
		RoadDesc.iWhereWeGo = 3;
		RoadDesc.eRoadDir = CRoad::UP;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Terrain_Road"), TEXT("Prototype_GameObject_Road_Dungeon0"), &RoadDesc)))
			return E_FAIL;

		break;
	case 3:
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Terrain"), TEXT("Prototype_GameObject_Terrain_Dungeon0_Stage3"), &m_iStageIndex)))
			return E_FAIL;

		// 아래쪽 스테이지 2
		RoadDesc.vPos = _float3(10.f, 0.1f, 1.f);
		RoadDesc.iWhereWeGo = 2;
		RoadDesc.eRoadDir = CRoad::DOWN;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Terrain_Road"), TEXT("Prototype_GameObject_Road_Dungeon0"), &RoadDesc)))
			return E_FAIL;

		// 위쪽 보스 스테이지
		RoadDesc.vPos = _float3(0.f, 0.1f, 47.f);
		RoadDesc.iWhereWeGo = 4;
		RoadDesc.eRoadDir = CRoad::UP;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Terrain_Road"), TEXT("Prototype_GameObject_Road_Dungeon0"), &RoadDesc)))
			return E_FAIL;

		break;

	case 4:
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Terrain"), TEXT("Prototype_GameObject_Terrain_Dungeon0_Stage4"), &m_iStageIndex)))
			return E_FAIL;

		// 아래 길 못 감
		RoadDesc.vPos = _float3(9.f, 0.1f, 1.f);
		RoadDesc.iWhereWeGo = 3;
		RoadDesc.eRoadDir = CRoad::DOWN;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Terrain_Road"), TEXT("Prototype_GameObject_Road_Dungeon0"), &RoadDesc)))
			return E_FAIL;

		break;

	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Dungeon00::Ready_LandObjects()
{
	CLandObject::LANDOBJECT_DESC	LandDesc = {};

	LandDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
	LandDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
	LandDesc.iLevelIndex = LEVEL_DUNGEON0;
	LandDesc.iStageIndex = m_pGameInstance->Get_StageIndex();

	if (FAILED(Ready_Layer_Player(LandDesc)))
		return E_FAIL;

	if (m_iStageIndex != 0)
	{
		CChest::CHEST_DESC ChestDesc = {};

		if(m_iStageIndex == 1)
			ChestDesc.vIinitPos = _float3(13.f, 20.f, 10.f);
		else if (m_iStageIndex == 3)
		{
			ChestDesc.vIinitPos = _float3(10.f, 20.f, 10.f);
		}
		ChestDesc.eChestId = CChest::GOLD;
		ChestDesc.tLandObjectDesc = LandDesc;

		if (FAILED(m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON0, TEXT("Layer_Chest"), TEXT("Prototype_GameObject_Chest"), sizeof(CChest::CHEST_DESC), &ChestDesc)))
			return E_FAIL;
	}

	CRatNPC::Rat_INFO RattDesc = {};
	RattDesc.tLandObjectDesc = LandDesc;
	RattDesc.vPos = _float3(20.0f, 0.f, 5.f);
	if (0 == m_iStageIndex)
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_NPC"), TEXT("Prototype_GameObject_NPC_RatNPC"), &RattDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Dungeon00::Ready_Layer_Player(CLandObject::LANDOBJECT_DESC& LandObjectDesc)
{
	if (!g_isLoaded[LEVEL_DUNGEON0])
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &LandObjectDesc)))
			return E_FAIL;

		static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Position(_float3(3.5f, 2.5f, 7.5f));

		static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_InitState(CPlayer::IDLE);
	}
	return S_OK;
}

HRESULT CLevel_Dungeon00::Ready_Layer_Monster(CLandObject::LANDOBJECT_DESC& LandObjectDesc)
{
	CMonster::MONSTER_INFO MonsterInfo = {};

	MonsterInfo.tLandObjectDesc = LandObjectDesc;

	if (1 == m_iStageIndex)
	{
		MonsterInfo.iHp = 10;

		MonsterInfo.vInitPosition = _float3(14.f, 0.f, 8.f);
		//개구리
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(18.f, 0.f, 10.f);
		//개구리
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(14.f, 0.f, 12.f);
		//개구리
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(5.f, 0.f, 4.f);
		//개구리
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;

	}

	else if (3 == m_iStageIndex)
	{
		MonsterInfo.iHp = 10;
		MonsterInfo.iAttackPower = 2;

		MonsterInfo.vInitPosition = _float3(17.f, 0.f, 8.f);
		//개구리
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(11.f, 0.f, 10.f);
		//개구리
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(16.f, 0.f, 12.f);
		//개구리
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(6.f, 0.f, 4.f);
		//개구리
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(8.f, 0.f, 8.f);
		//개구리
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(3.f, 0.f, 10.f);
		//개구리
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;

	}

	if (4 == m_iStageIndex)
	{
		MonsterInfo.iHp = 10;
		MonsterInfo.vInitPosition = _float3(5.f, 2.5f, 12.f);
		//개구리
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(8.f, 2.5f, 6.f);
		//개구리
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;
		
		MonsterInfo.vInitPosition = _float3(3.5f, 2.5f, 3.f);
		//개구리
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;
		
		MonsterInfo.vInitPosition = _float3(18.5f, 2.5f, 3.f);
		//개구리
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;
		
		MonsterInfo.vInitPosition = _float3(17.5f, 2.5f, 12.f);
		//개구리
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.iHp = 20.f;
	
		//구시옹
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_Boss"), TEXT("Prototype_GameObject_Gusion"), &MonsterInfo)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Dungeon00::Ready_Layer_NPC(CLandObject::LANDOBJECT_DESC& LandObjectDesc)
{
	CTarotNPC::TAROTNPC_INFO TarotDesc = {};
	TarotDesc.tLandObjectDesc = LandObjectDesc;
	TarotDesc.vPos = _float3(9.5f, 0.f, 6.f);

	if (2 == m_iStageIndex)
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_TarotNPC"), TEXT("Prototype_GameObject_NPC_TarotNPC"), &TarotDesc)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CLevel_Dungeon00::Ready_Layer_MapObject()
{
	for (_uint i = 0; i < m_vecInit_CloneDesc[m_iStageIndex].size(); i++)
	{
		CLandObject::LANDOBJECT_DESC	Desc = {};

		Desc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform"), m_iStageIndex));
		Desc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer"), m_iStageIndex));
		Desc.iInitBindTexNum = m_vecInit_CloneDesc[m_iStageIndex][i].iBindTexNum;


		_float4x4 WorldMat = m_vecInit_CloneDesc[m_iStageIndex][i].vInitWorldMat;		// 가독성위해 한번 끊었음

		Desc.vInitPos = *((_float3*)&WorldMat.m[3][0]);
		Desc.vInitScale = _float3(D3DXVec3Length((_float3*)&WorldMat.m[0][0]),
			D3DXVec3Length((_float3*)&WorldMat.m[1][0]),
			D3DXVec3Length((_float3*)&WorldMat.m[2][0]));

		Desc.iPreModelType = m_vecInit_CloneDesc[m_iStageIndex][i].iModelType;
		Desc.iPreAdornType = m_vecInit_CloneDesc[m_iStageIndex][i].iAdornType;
		Desc.iPreLandType = m_vecInit_CloneDesc[m_iStageIndex][i].iLandType;
		Desc.iStageIndex = m_vecInit_CloneDesc[m_iStageIndex][i].iStageIndex;

		if (m_vecInit_CloneDesc[m_iStageIndex][i].strPrototypeName == "Prototype_GameObject_DecoObject")
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Object"), TEXT("Prototype_GameObject_DecoObject"), &Desc)))
				return E_FAIL;
		}
		else if (m_vecInit_CloneDesc[m_iStageIndex][i].strPrototypeName == "Prototype_GameObject_InteractObject")
		{
			Desc.eInteractType = m_vecInit_CloneDesc[m_iStageIndex][i].eInteractType;

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Object"), TEXT("Prototype_GameObject_InteractObject"), &Desc)))
				return E_FAIL;
		}

	}

	return S_OK;
}

HRESULT CLevel_Dungeon00::Ready_Layer_UI()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_QuestPanelUI"))))
		return E_FAIL;



	CUI::UI_DESC Desc = {};

	Desc.m_vPos = { g_iWinSizeX / 2.f - 160.f , g_iWinSizeY / 2.f };
	Desc.m_vSize = { 150.f,150.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::UI_BAR;
	Desc.vTargetPos = { 130.f, 95.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer_UI(LEVEL_DUNGEON0, TEXT("Layer_UI"), TEXT("Prototype_GameObject_PlayerHpUI"), &Desc ,(void**)&m_pPlayerHpUI[0])))
		MSG_BOX(L"Can't Create PlayerHpUI");



	Desc.m_vPos = { g_iWinSizeX / 2.f + 10 , g_iWinSizeY / 2.f };
	Desc.m_vSize = { 150.f,150.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::UI_BAR;
	Desc.vTargetPos = { 170.f, 95.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer_UI(LEVEL_DUNGEON0, TEXT("Layer_UI"), TEXT("Prototype_GameObject_PlayerHpUI") , &Desc, (void**)&m_pPlayerHpUI[1])))
		MSG_BOX(L"Can't Create PlayerHpUI");



	Desc.m_vPos = { g_iWinSizeX / 2.f + 180, g_iWinSizeY / 2.f };
	Desc.m_vSize = { 150.f,150.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::UI_BAR;
	Desc.vTargetPos = { 210.f, 95.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer_UI(LEVEL_DUNGEON0, TEXT("Layer_UI"), TEXT("Prototype_GameObject_PlayerHpUI"), &Desc, (void**)&m_pPlayerHpUI[2])))
		MSG_BOX(L"Can't Create PlayerHpUI");



	// SermonUI Set
	Desc.m_vPos = { 60.f , 70.f };
	Desc.m_vSize = { 100.f,100.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::UI_NONE;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_SermonUI"), TEXT("Prototype_GameObject_SermonUI"), &Desc)))
		MSG_BOX(L"Can't Create SermonUI");


	Desc.m_vPos = { 110.f , 125.f };
	Desc.m_vSize = { 40.f, 40.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::UI_ICON;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_SermonUI"), TEXT("Prototype_GameObject_SermonUI"), &Desc)))
		MSG_BOX(L"Can't Create SermonUI");


	Desc.m_vPos = { 80.f , 155.f };
	Desc.m_vSize = { 40.f,40.f };
	Desc.m_iBindNum = 1;
	Desc.eUItype = UI_TYPE::UI_ICON;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_SermonUI"), TEXT("Prototype_GameObject_SermonUI"), &Desc)))
		MSG_BOX(L"Can't Create SermonUI");


	Desc.m_vPos = { 50.f , 185.f };
	Desc.m_vSize = { 40.f,40.f };
	Desc.m_iBindNum = 2;
	Desc.eUItype = UI_TYPE::UI_ICON;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_SermonUI"), TEXT("Prototype_GameObject_SermonUI"), &Desc)))
		MSG_BOX(L"Can't Create SermonUI");

	 Desc = {};

	for (_uint i = 0; i < 3; i++)
	{
		Safe_AddRef(m_pPlayerHpUI[i]);
	}

	_uint iLevelIndex = (int)LEVEL_DUNGEON0;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_TarotUI"), TEXT("Prototype_GameObject_UI_TarotUI"), &iLevelIndex)))
		return E_FAIL;

	CPressEtoPickCard::BTNE_DESC B_Desc = {};
	B_Desc.iLevelIndex = (int)LEVEL_DUNGEON0;
	B_Desc.iType = 0;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_PressEtoPickCard"), &B_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Dungeon00::Start_StageChange(_int iNextStageIndex)
{
	if (m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Monster")) == nullptr
		|| m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Monster"))->Get_ObjectList().size() <= 0)
	{
		dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Start_FadeIn();
		m_iNextStageIndex = iNextStageIndex;
		m_isChangeStage = true;
	}
	return S_OK;
}

HRESULT CLevel_Dungeon00::Change_Stage(_int iStageIndex)
{
	if (!dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Get_IsFadeIn()
		|| !dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Get_IsFadeOut())
	{
		m_pGameInstance->Set_StageIndex(iStageIndex);

		m_iStageIndex = iStageIndex;

		if (3 == m_iStageIndex)
		{
			CGameInstance::Get_Instance()->StopAll();
			CGameInstance::Get_Instance()->PlayBGM(TEXT("BGM_Gusion.wav"), g_fBgmVolume);
		}

		if (!m_isLoaded[m_iStageIndex])
		{
			m_isLoaded[m_iStageIndex] = true;

			Ready_Layer_BackGround();
			Ready_Layer_MapObject();
			Create_First();
		}

		//아마 여기서 플레이어 위치 지정?
		m_pGameInstance->Find_Player()->Set_StageIndex(m_iStageIndex);

		dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Start_FadeOut();

		switch (m_iStageIndex)
		{
		case 0:
		{
			if (m_iPreStageIndex > m_iStageIndex)
			{
				m_vPlayerInitPos = _float3(21.5f, 2.5f, 5.5f);
				m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);
				(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->Set_Position_to_Player();
			}
			static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Player_State(CPlayer::IDLE);

		}
			break;
		case 1:
		{ 
			if (m_iPreStageIndex > m_iStageIndex)
			{
				m_vPlayerInitPos = _float3(27.5f, 2.5f, 1.f);
				m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);
				(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->Set_Position_to_Player();
			}
			else 
			{
				m_vPlayerInitPos = { 1.f,0.f,1.f };
				m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);
				(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->Set_Position_to_Player();
			}

			static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::IDLE);
		}
			break;
		case 2:
		{ 
			if (m_iPreStageIndex > m_iStageIndex)
			{
				m_vPlayerInitPos = _float3(17.f, 2.5f, 16.f);
				m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);
				(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->Set_Position_to_Player();
			}

			else 
			{
				m_vPlayerInitPos = { 1.f,0.f,1.f };
				m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);
				(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->Set_Position_to_Player();
			}

			static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::IDLE);
		}
			break;
		case 3:
		{
			if (m_iPreStageIndex > m_iStageIndex)
			{
				m_vPlayerInitPos = _float3(2.f, 2.5f, 16.f);
				m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);
				(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->Set_Position_to_Player();
			}

			else 
			{
				m_vPlayerInitPos = _float3(12.f, 2.5f, 1.f);
				m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);
				(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->Set_Position_to_Player();
			}

			static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::IDLE);
		}
			break;
		case 4:
		{
		
			m_vPlayerInitPos = _float3(11.f, 2.5f, 1.f);
			m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);
			(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->Set_Position_to_Player();
			static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::IDLE);

		}
			break;
		default:
			break;
		}

		m_isChangeStage = false;
		m_iPreStageIndex = m_iStageIndex;
	}


	return S_OK;
}

HRESULT CLevel_Dungeon00::Create_First()
{
	CLandObject::LANDOBJECT_DESC	LandDesc = {};

	LandDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform"), m_iStageIndex));
	LandDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer"), m_iStageIndex));
	LandDesc.iLevelIndex = LEVEL_DUNGEON0;
	LandDesc.iStageIndex = m_pGameInstance->Get_StageIndex();

	if (FAILED(Ready_Layer_Monster(LandDesc)))
		return E_FAIL;

	if (FAILED(Ready_Layer_NPC(LandDesc)))
		return E_FAIL;

	return S_OK;
}

bool CLevel_Dungeon00::Clear_Dungeon()
{
	CLayer* pBossLayer = m_pGameInstance->Find_Layer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Boss"));
	CLayer* pMonsterLayer = m_pGameInstance->Find_Layer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Monster"));

	if (nullptr != pBossLayer && 0 == pBossLayer->Get_ObjectList().size()
		&& nullptr != pMonsterLayer && 0 == pMonsterLayer->Get_ObjectList().size())
		return true;

	

	return false;
}

HRESULT CLevel_Dungeon00::Create_DungeonClearUI()
{
	CDungeonClearUI::DUNGEON_CLEARUI_DESC Desc = {};

	Desc.tDesc.m_vPos = { g_iWinSizeX / 2.f, g_iWinSizeY / 2.f };
	Desc.tDesc.m_vSize = { 1000.f, 600.f };
	Desc.tDesc.bActivate = true;
	Desc.strDungeonName = L"어둠의 심연";
	Desc.strAdditionalName = L"어린 양의 탄생";
	Desc.fProceedingTime = m_fProceedingTime;

	Desc.eType = CDungeonClearUI::DUNGEON_CLEARUI_TYPE::PANEL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON0, TEXT("Layer_DungeonClearUI"), TEXT("Prototype_GameObject_DungeonClearUI"), &Desc)))
		return E_FAIL;

	m_bExistClearUI = true;

	return S_OK;
}


HRESULT CLevel_Dungeon00::Load_Village_Map()
{
	_char				iVerticesX[50] = {};
	_char				iVerticesZ[50] = {};
	_int				iTileNum = {};

	_uint				iObjectCount = {};
	string				strPrototypeName = {};
	_uint				iBindTexNum = {};
	_float4x4			WorldMat = {};

	ifstream fin;


#pragma region Stage00
	fin.open("../Bin/MapData/Dungeon0/Map_Dungeon0_0.txt");

	if (!fin.is_open())
	{
		MSG_BOX(TEXT("맵 데이터 못 불러옴"));
		return E_FAIL;
	}

	fin.getline(iVerticesX, 50);
	fin.getline(iVerticesZ, 50);
	fin >> iTileNum;

	if (FAILED(Load_Village_Terrain(atoi(iVerticesX), atoi(iVerticesZ), iTileNum, 0)))
		return E_FAIL;



	fin >> iObjectCount;
	m_vecInit_CloneDesc[0].reserve(iObjectCount);

	fin >> fixed >> setprecision(8);
	for (_uint i = 0; i < iObjectCount; i++)
	{
		string szName;
		string szLayerName;

		_uint eInteractType = { CLandObject::INTERACT_TYPE::INTERACTTYPE_END };

		int iBindNum;

		_float4x4 vWorldMat;
		D3DXMatrixIdentity(&vWorldMat);


		fin >> szName;
		fin >> szLayerName;

		if (szName == "Prototype_GameObject_InteractObject")
		{
			fin >> eInteractType;
		}
		else
			fin >> eInteractType;


		_uint iModelType = {};
		_uint iAdornType = {};
		_uint iLandType = {};

		fin >> iModelType;
		fin >> iAdornType;
		fin >> iLandType;

		fin >> iBindNum;
		fin >> vWorldMat;

		if (FAILED(Load_Village_Object(szName, eInteractType, iModelType, iAdornType, iLandType, iBindNum, vWorldMat, 0)))
			return E_FAIL;

	}

	fin.close();

#pragma endregion

#pragma region Stage01

	fin.open("../Bin/MapData/Dungeon0/Map_Dungeon0_1.txt");

	if (!fin.is_open())
	{
		MSG_BOX(TEXT("맵 데이터 못 불러옴"));
		return E_FAIL;
	}

	fin.getline(iVerticesX, 50);
	fin.getline(iVerticesZ, 50);
	fin >> iTileNum;

	if (FAILED(Load_Village_Terrain(atoi(iVerticesX), atoi(iVerticesZ), iTileNum, 1)))
		return E_FAIL;

	fin >> iObjectCount;
	m_vecInit_CloneDesc[0].reserve(iObjectCount);

	fin >> fixed >> setprecision(8);
	for (_uint i = 0; i < iObjectCount; i++)
	{
		string szName;
		string szLayerName;

		_uint eInteractType = { CLandObject::INTERACT_TYPE::INTERACTTYPE_END };

		int iBindNum;

		_float4x4 vWorldMat;
		D3DXMatrixIdentity(&vWorldMat);


		fin >> szName;
		fin >> szLayerName;

		if (szName == "Prototype_GameObject_InteractObject")
		{
			fin >> eInteractType;
		}
		else
			fin >> eInteractType;


		_uint iModelType = {};
		_uint iAdornType = {};
		_uint iLandType = {};

		fin >> iModelType;
		fin >> iAdornType;
		fin >> iLandType;

		fin >> iBindNum;
		fin >> vWorldMat;

		if (FAILED(Load_Village_Object(szName, eInteractType, iModelType, iAdornType, iLandType, iBindNum, vWorldMat, 1)))
			return E_FAIL;

	}

	fin.close();

#pragma endregion

#pragma region Stage02

	fin.open("../Bin/MapData/Dungeon0/Map_Dungeon0_2.txt");

	if (!fin.is_open())
	{
		MSG_BOX(TEXT("맵 데이터 못 불러옴"));
		return E_FAIL;
	}

	fin.getline(iVerticesX, 50);
	fin.getline(iVerticesZ, 50);
	fin >> iTileNum;

	if (FAILED(Load_Village_Terrain(atoi(iVerticesX), atoi(iVerticesZ), iTileNum, 2)))
		return E_FAIL;

	fin >> iObjectCount;
	m_vecInit_CloneDesc[0].reserve(iObjectCount);

	fin >> fixed >> setprecision(8);
	for (_uint i = 0; i < iObjectCount; i++)
	{
		string szName;
		string szLayerName;

		_uint eInteractType = { CLandObject::INTERACT_TYPE::INTERACTTYPE_END };

		int iBindNum;

		_float4x4 vWorldMat;
		D3DXMatrixIdentity(&vWorldMat);


		fin >> szName;
		fin >> szLayerName;

		if (szName == "Prototype_GameObject_InteractObject")
		{
			fin >> eInteractType;
		}
		else
			fin >> eInteractType;


		_uint iModelType = {};
		_uint iAdornType = {};
		_uint iLandType = {};

		fin >> iModelType;
		fin >> iAdornType;
		fin >> iLandType;

		fin >> iBindNum;
		fin >> vWorldMat;

		if (FAILED(Load_Village_Object(szName, eInteractType, iModelType, iAdornType, iLandType, iBindNum, vWorldMat, 2)))
			return E_FAIL;

	}

	fin.close();

#pragma endregion

#pragma region Stage03

	fin.open("../Bin/MapData/Dungeon0/Map_Dungeon0_3.txt");

	if (!fin.is_open())
	{
		MSG_BOX(TEXT("맵 데이터 못 불러옴"));
		return E_FAIL;
	}

	fin.getline(iVerticesX, 50);
	fin.getline(iVerticesZ, 50);
	fin >> iTileNum;

	if (FAILED(Load_Village_Terrain(atoi(iVerticesX), atoi(iVerticesZ), iTileNum, 3)))
		return E_FAIL;

	fin >> iObjectCount;
	m_vecInit_CloneDesc[0].reserve(iObjectCount);

	fin >> fixed >> setprecision(8);
	for (_uint i = 0; i < iObjectCount; i++)
	{
		string szName;
		string szLayerName;

		_uint eInteractType = { CLandObject::INTERACT_TYPE::INTERACTTYPE_END };

		int iBindNum;

		_float4x4 vWorldMat;
		D3DXMatrixIdentity(&vWorldMat);


		fin >> szName;
		fin >> szLayerName;

		if (szName == "Prototype_GameObject_InteractObject")
		{
			fin >> eInteractType;
		}
		else
			fin >> eInteractType;


		_uint iModelType = {};
		_uint iAdornType = {};
		_uint iLandType = {};

		fin >> iModelType;
		fin >> iAdornType;
		fin >> iLandType;

		fin >> iBindNum;
		fin >> vWorldMat;

		if (FAILED(Load_Village_Object(szName, eInteractType, iModelType, iAdornType, iLandType, iBindNum, vWorldMat, 3)))
			return E_FAIL;

	}

	fin.close();

#pragma endregion

#pragma region Stage04

	fin.open("../Bin/MapData/Dungeon0/Map_Dungeon0_4.txt");

	if (!fin.is_open())
	{
		MSG_BOX(TEXT("맵 데이터 못 불러옴"));
		return E_FAIL;
	}

	fin.getline(iVerticesX, 50);
	fin.getline(iVerticesZ, 50);
	fin >> iTileNum;

	if (FAILED(Load_Village_Terrain(atoi(iVerticesX), atoi(iVerticesZ), iTileNum, 4)))
		return E_FAIL;

	fin >> iObjectCount;
	m_vecInit_CloneDesc[0].reserve(iObjectCount);

	fin >> fixed >> setprecision(8);
	for (_uint i = 0; i < iObjectCount; i++)
	{
		string szName;
		string szLayerName;

		_uint eInteractType = { CLandObject::INTERACT_TYPE::INTERACTTYPE_END };

		int iBindNum;

		_float4x4 vWorldMat;
		D3DXMatrixIdentity(&vWorldMat);


		fin >> szName;
		fin >> szLayerName;

		if (szName == "Prototype_GameObject_InteractObject")
		{
			fin >> eInteractType;
		}
		else
			fin >> eInteractType;


		_uint iModelType = {};
		_uint iAdornType = {};
		_uint iLandType = {};

		fin >> iModelType;
		fin >> iAdornType;
		fin >> iLandType;

		fin >> iBindNum;
		fin >> vWorldMat;

		if (FAILED(Load_Village_Object(szName, eInteractType, iModelType, iAdornType, iLandType, iBindNum, vWorldMat, 4)))
			return E_FAIL;

	}

	fin.close();

#pragma endregion

	if (!g_isLoaded[LEVEL_DUNGEON0])
	{
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON0, TEXT("Prototype_Component_VIBuffer_Road_Dungeon0"),
			CVIBuffer_Terrain::Create(m_pGraphic_Device, 30, 5))))
			return E_FAIL;

		CRoad* pRoad = CRoad::Create(m_pGraphic_Device);
		pRoad->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Road_Dungeon0"));
		pRoad->Set_BindTexNum(iTileNum);
		pRoad->Set_LevelIndex(LEVEL_DUNGEON0);

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Road_Dungeon0"), pRoad)))
			return E_FAIL;
	}
	return S_OK;

}

HRESULT CLevel_Dungeon00::Load_Village_Terrain(const _uint& iX, const _uint& iZ, const _uint& iTileNum, _int iStageIndex)
{
	if (!g_isLoaded[LEVEL_DUNGEON0])
	{
		switch (iStageIndex)
		{
		case 0:
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON0, TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon0_Stage0"),
				CVIBuffer_Terrain::Create(m_pGraphic_Device, iX, iZ))))
				return E_FAIL;

			CTerrain* pTerrain = CTerrain::Create(m_pGraphic_Device);
			pTerrain->Set_BindTexNum(iTileNum);
			pTerrain->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon0_Stage0"));
			pTerrain->Set_LevelIndex(LEVEL_DUNGEON0);

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Dungeon0_Stage0"), pTerrain)))
				return E_FAIL;
		}
		break;

		case 1:
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON0, TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon0_Stage1"),
				CVIBuffer_Terrain::Create(m_pGraphic_Device, iX, iZ))))
				return E_FAIL;

			CTerrain* pTerrain = CTerrain::Create(m_pGraphic_Device);
			pTerrain->Set_BindTexNum(iTileNum);
			pTerrain->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon0_Stage1"));
			pTerrain->Set_LevelIndex(LEVEL_DUNGEON0);

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Dungeon0_Stage1"), pTerrain)))
				return E_FAIL;
		}
		break;

		case 2:
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON0, TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon0_Stage2"),
				CVIBuffer_Terrain::Create(m_pGraphic_Device, iX, iZ))))
				return E_FAIL;

			CTerrain* pTerrain = CTerrain::Create(m_pGraphic_Device);
			pTerrain->Set_BindTexNum(iTileNum);
			pTerrain->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon0_Stage2"));
			pTerrain->Set_LevelIndex(LEVEL_DUNGEON0);

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Dungeon0_Stage2"), pTerrain)))
				return E_FAIL;
		}
		break;

		case 3:
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON0, TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon0_Stage3"),
				CVIBuffer_Terrain::Create(m_pGraphic_Device, iX, iZ))))
				return E_FAIL;

			CTerrain* pTerrain = CTerrain::Create(m_pGraphic_Device);
			pTerrain->Set_BindTexNum(iTileNum);
			pTerrain->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon0_Stage3"));
			pTerrain->Set_LevelIndex(LEVEL_DUNGEON0);

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Dungeon0_Stage3"), pTerrain)))
				return E_FAIL;
		}
		break;

		case 4:
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON0, TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon0_Stage4"),
				CVIBuffer_Terrain::Create(m_pGraphic_Device, iX, iZ))))
				return E_FAIL;

			CTerrain* pTerrain = CTerrain::Create(m_pGraphic_Device);
			pTerrain->Set_BindTexNum(iTileNum);
			pTerrain->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon0_Stage4"));
			pTerrain->Set_LevelIndex(LEVEL_DUNGEON0);

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Dungeon0_Stage4"), pTerrain)))
				return E_FAIL;
		}
		break;

		default:
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLevel_Dungeon00::Load_Village_Object(const string& strPrototypeName, _uint eInteractType, _uint& iModelType, _uint& iAdornType, _uint& iLandType, const _uint& iObjectTexNum, const _float4x4& vWorldMat, _uint iStageIndex)
{
	CLevel_Loading::INIT_CLONEDESC Desc = {};
	Desc.strPrototypeName = strPrototypeName;
	Desc.iBindTexNum = iObjectTexNum;
	Desc.vInitWorldMat = vWorldMat;
	Desc.eInteractType = (CLandObject::INTERACT_TYPE)eInteractType;

	Desc.iModelType = iModelType;
	Desc.iAdornType = iAdornType;
	Desc.iLandType = iLandType;
	Desc.iStageIndex = iStageIndex;

	m_vecInit_CloneDesc[iStageIndex].push_back(Desc);

	return S_OK;
}



CLevel_Dungeon00* CLevel_Dungeon00::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Dungeon00* pInstance = new CLevel_Dungeon00(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Dungeon00"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Dungeon00::Free()
{
	__super::Free();

	for (_uint i = 0; i < 3; i++)
	{
		Safe_Release(m_pPlayerHpUI[i]);
	}
}
