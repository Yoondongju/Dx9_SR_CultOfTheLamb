#include "..\Public\Level_Dungeon01.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Player.h"
#include "Monster.h"
#include "Camera.h"
#include "Chest.h"
#include "Door.h"

#include "VIBuffer_Terrain.h"
#include "Terrain.h"
#include "Road.h"

#include "Client_Operators.h"
#include "TarotNPC.h"
#include "PlayerHpUI.h"
#include <PressEtoPickCard.h>


#include "DungeonClearUI.h"

#include "Quest.h"

CLevel_Dungeon01::CLevel_Dungeon01(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device }
{
}

HRESULT CLevel_Dungeon01::Initialize()
{
	m_iStageIndex = 1;
	m_pGameInstance->Set_StageIndex(m_iStageIndex);
	m_bPlayerChangeOnce = true;
	m_bEnterOnce = true;

	static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_LevelChange(true);

	static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Player_State(CPlayer::CHANGE_LEVEL);
	static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_Animation(TEXT("IDLE"));

	m_pGameInstance->Find_Player()->Set_StageIndex(1);

	m_vPlayerInitPos = _float3(4.f, 2.5f, 1.5f);
	m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);

	if (FAILED(Load_Village_Map()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))		// �̹� �纻�� �ΰ��� ���� ī�޶� Desc���ÿ�
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;

	if (FAILED(Create_First()))
		return E_FAIL;

	CTransform* pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform"), m_iStageIndex));
	CVIBuffer_Terrain* pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer"), m_iStageIndex));

	static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Chagne_Terrain(pTerrainVIBuffer, pTerrainTransform);

	CGameInstance::Get_Instance()->PlayBGM(TEXT("BGM_Battle.wav"), g_fBgmVolume);

	return S_OK;
}

void CLevel_Dungeon01::Update(_float fTimeDelta)
{
	
	m_vWrapDistance = _float3(12.5f, 2.5f, 10.f) - static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_Position();
	m_fWrapDistance = D3DXVec3Length(&m_vWrapDistance);

	if(m_bPlayerSizeOnce)
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

		CQuest::Get_Instance()->Succeed_Quest(L"����1 �����ϱ�");
	}
	if (true == m_pPlayerHpUI[2]->GetActivate()
		&& m_bPlayerChangeOnce
		&& m_fTimeTo_CreateHp > 6.f)
	{
		static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::IDLE);
		m_bPlayerChangeOnce = false;
	}
	m_fTimeTo_CreateHp += fTimeDelta;

#pragma region �浹ó�� �з�
	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Attack")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Attack"), TEXT("Layer_Player"), LEVEL_DUNGEON1, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_MonsterBone")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_MonsterBone"), TEXT("Layer_Player"), LEVEL_DUNGEON1, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Monster")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Monster"), TEXT("Layer_Player"), LEVEL_DUNGEON1, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Boss")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Boss"), TEXT("Layer_Player"), LEVEL_DUNGEON1, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Item")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Item"), TEXT("Layer_Player"), LEVEL_DUNGEON1, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Object")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_BackGround_Object"), TEXT("Layer_Player"), LEVEL_DUNGEON1, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Monster"))
		&& nullptr != m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Player_Bullet")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Monster"), TEXT("Layer_Player_Bullet"), LEVEL_DUNGEON1, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Boss"))
		&& nullptr != m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Player_Bullet")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Boss"), TEXT("Layer_Player_Bullet"), LEVEL_DUNGEON1, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Terrain_Road")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Terrain_Road"), TEXT("Layer_Player"), LEVEL_DUNGEON1, LEVEL_STATIC);

#pragma endregion


	if (Clear_Dungeon())
	{
		if (!m_bExistClearUI && m_fClearUI_OpenTime > 2.f)
		{

			m_pGameInstance->StopAll();
			m_pGameInstance->Play_Sound(TEXT("BGM_DungeonClearUI.wav"), SOUND_BGM, g_fBgmVolume);
			Create_DungeonClearUI();
		}

		m_fClearUI_OpenTime += fTimeDelta;
	}
	else
	{
		m_fProceedingTime += fTimeDelta;	// ���� ���� �ð�
	}
	




	//�������� ��ȯ
	if (m_isChangeStage)
	{
		Change_Stage(m_iNextStageIndex);
	}



	if (m_fWrapDistance < 5.f
		&& m_pGameInstance->Get_KeyState(KEY::ENTER) == KEY_STATE::TAP)
	{
		if (m_iStageIndex == 3 
			&& m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Boss"))->Get_ObjectList().size() <= 0)
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
			for (auto& Door : m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Door"))->Get_ObjectList())
			{
				if (dynamic_cast<CDoor*>(Door)->Get_DoorId() == CDoor::DOOR_DUNGEON1)
				{
					dynamic_cast<CDoor*>(Door)->Set_DoorOpen(false);
				}
				else if (dynamic_cast<CDoor*>(Door)->Get_DoorId() == CDoor::DOOR_DUNGEON2)
				{
					dynamic_cast<CDoor*>(Door)->Set_DoorOpen(true);
				}
			}
			m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, LEVEL_GAMEPLAY));

			m_bWrapOnce = false;
		}
	}

	if (m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Monster")) == nullptr
		|| m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Monster"))->Get_ObjectList().size() <= 0)
	{
		if (!m_isActiveChest[0] && m_iStageIndex == 0)
		{
			m_isActiveChest[0] = true;

			if (FAILED(Ready_LandObjects()))
				return;
		}

		if (!m_isActiveChest[1] && m_iStageIndex == 2)
		{
			m_isActiveChest[1] = true;

			if (FAILED(Ready_LandObjects()))
				return;
		}
	}

}

HRESULT CLevel_Dungeon01::Render()
{
	//SetWindowText(g_hWnd, TEXT("�����÷��̷����Դϴ�."));
	return S_OK;
}


HRESULT CLevel_Dungeon01::Create_DungeonClearUI()
{
	CDungeonClearUI::DUNGEON_CLEARUI_DESC Desc = {};

	Desc.tDesc.m_vPos = { g_iWinSizeX / 2.f, g_iWinSizeY / 2.f };
	Desc.tDesc.m_vSize = { 1000.f, 600.f };
	Desc.tDesc.bActivate = true;
	Desc.strDungeonName = L"���ֹ��� ��";
	Desc.strAdditionalName = L"���� �뼭�ϴ�";
	Desc.fProceedingTime = m_fProceedingTime;
	Desc.eType = CDungeonClearUI::DUNGEON_CLEARUI_TYPE::PANEL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_UI"), TEXT("Prototype_GameObject_DungeonClearUI"), &Desc)))
		return E_FAIL;


	m_bExistClearUI = true;
	
	return S_OK;
}



HRESULT CLevel_Dungeon01::Ready_Layer_Camera()
{
	CCamera::CAMERA_DESC		Desc{};

	Desc.vEye = _float3(0.f, 9.f, -10.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.fFovy = D3DXToRadian(55.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = D3DXToRadian(90.0f);

	(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->InitCamera_Desc(&Desc);
	(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->Set_Position_to_Player();

	return S_OK;
}

HRESULT CLevel_Dungeon01::Ready_Layer_BackGround()
{
	CRoad::ROAD_DESC RoadDesc{};
	RoadDesc.iLevelIndex = LEVEL_DUNGEON1;


	RoadDesc.iStageIndex = 0;
	_uint iStageIndex = 0;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Prototype_GameObject_Terrain_Dungeon1_Stage0"), &iStageIndex)))
		return E_FAIL;

	// ������ �� Ÿ��
	RoadDesc.vPos = _float3(26.f, 0.1f, 5.f);
	RoadDesc.iWhereWeGo = 1;
	RoadDesc.eRoadDir = CRoad::RIGHT;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Terrain_Road"), TEXT("Prototype_GameObject_Road_Dungeon1"), &RoadDesc)))
		return E_FAIL;


	RoadDesc.iStageIndex = 1;
	iStageIndex = 1;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Prototype_GameObject_Terrain_Dungeon1_Stage1"), &iStageIndex)))
		return E_FAIL;

	// ���� �� ��������0
	RoadDesc.vPos = _float3(-28.f, 0.1f, 0.f);
	RoadDesc.iWhereWeGo = 0;
	RoadDesc.eRoadDir = CRoad::LEFT;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Terrain_Road"), TEXT("Prototype_GameObject_Road_Dungeon1"), &RoadDesc)))
		return E_FAIL;

	// �Ʒ� ��������2
	RoadDesc.vPos = _float3(8.f, 0.1f, 1.f);
	RoadDesc.iWhereWeGo = 2;
	RoadDesc.eRoadDir = CRoad::DOWN;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Terrain_Road"), TEXT("Prototype_GameObject_Road_Dungeon1"), &RoadDesc)))
		return E_FAIL;

	RoadDesc.iStageIndex = 2;
	iStageIndex = 2;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Prototype_GameObject_Terrain_Dungeon1_Stage2"), &iStageIndex)))
		return E_FAIL;

	// �� �� �������� 1
	RoadDesc.vPos = _float3(6.5f, 0.1f, 40.f);
	RoadDesc.iWhereWeGo = 1;
	RoadDesc.eRoadDir = CRoad::UP;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Terrain_Road"), TEXT("Prototype_GameObject_Road_Dungeon1"), &RoadDesc)))
		return E_FAIL;

	// �Ʒ� �� ����
	RoadDesc.vPos = _float3(6.5f, 0.1f, 1.f);
	RoadDesc.iWhereWeGo = 3;
	RoadDesc.eRoadDir = CRoad::DOWN;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Terrain_Road"), TEXT("Prototype_GameObject_Road_Dungeon1"), &RoadDesc)))
		return E_FAIL;


	RoadDesc.iStageIndex = 3;
	iStageIndex = 3;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Prototype_GameObject_Terrain_Dungeon1_Stage3"), &iStageIndex)))
		return E_FAIL;

	//// �� �� �������� 2
	//RoadDesc.vPos = _float3(10.f, 0.1f, 55.f);
	//RoadDesc.iWhereWeGo = 3;
	//RoadDesc.eRoadDir = CRoad::UP;
	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Terrain_Road"), TEXT("Prototype_GameObject_Road_Dungeon1"), &RoadDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Dungeon01::Ready_LandObjects()
{
	CLandObject::LANDOBJECT_DESC	LandDesc = {};

	LandDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform"), m_iStageIndex));
	LandDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer"), m_iStageIndex));
	LandDesc.iLevelIndex = LEVEL_DUNGEON1;
	LandDesc.iStageIndex = m_iStageIndex;

	if (m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Monster")) == nullptr
		|| m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Monster"))->Get_ObjectList().size() <= 0)
	{
		CChest::CHEST_DESC ChestDesc = {};
		
		if(m_iStageIndex == 0)
			ChestDesc.vIinitPos = _float3(13.f, 20.f, 5.f);
		else if(m_iStageIndex == 2)
			ChestDesc.vIinitPos = _float3(8.5f, 20.f, 5.f);
		ChestDesc.eChestId = CChest::GOLD;
		ChestDesc.tLandObjectDesc = LandDesc;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Chest"), TEXT("Prototype_GameObject_Chest"), &ChestDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Dungeon01::Ready_Layer_Monster(CLandObject::LANDOBJECT_DESC& LandObjectDesc)
{
	CMonster::MONSTER_INFO MonsterInfo = {};
	MonsterInfo.tLandObjectDesc = LandObjectDesc;

	if (0 == m_iStageIndex)
	{
		MonsterInfo.iHp = 100;
		MonsterInfo.iAttackPower = 2;

		MonsterInfo.vInitPosition = _float3(5.f, 0.f, 6.f);
		//����
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bat"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(5.f, 0.f, 3.f);
		//����
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bat"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(2.f, 0.f, 7.f);
		//����
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bat"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(10.f, 0.f, 9.f);
		//����
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bat"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(15.f, 0.f, 7.f);
		//����
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bat"), &MonsterInfo)))
			return E_FAIL;


		MonsterInfo.vInitPosition = _float3(13.f, 0.f, 3.f);
		//������
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;

	}
	else if (2 == m_iStageIndex)
	{
		MonsterInfo.iHp = 10;
		MonsterInfo.iAttackPower = 2;

		//������ 3 ���� 5
		MonsterInfo.vInitPosition = _float3(2.f, 0.f, 5.f);
		//����
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bat"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(3.f, 0.f, 9.f);
		//����
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bat"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(4.f, 0.f, 8.f);
		//����
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bat"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(15.f, 0.f, 3.f);
		//����
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bat"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(13.f, 0.f, 4.f);
		//����
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bat"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(15.f, 0.f, 5.f);
		//����
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bat"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(6.f, 0.f, 7.f);
		//������
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(13.f, 0.f, 9.f);
		//������
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.vInitPosition = _float3(16.f, 0.f, 3.f);
		//������
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Frog"), &MonsterInfo)))
			return E_FAIL;
	}
	if (3 == m_iStageIndex)
	{
		CMonster::MONSTER_INFO MonsterInfo_Eligor = {};
		MonsterInfo_Eligor.iHp = 50;
		MonsterInfo_Eligor.tLandObjectDesc = LandObjectDesc;

		//�������
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_Boss"), TEXT("Prototype_GameObject_Eligor"), &MonsterInfo_Eligor)))
			return E_FAIL;

	}

	return S_OK;
}

HRESULT CLevel_Dungeon01::Ready_Layer_NPC(CLandObject::LANDOBJECT_DESC& LandObjectDesc)
{
	CTarotNPC::TAROTNPC_INFO TarotDesc = {};
	TarotDesc.tLandObjectDesc = LandObjectDesc;
	TarotDesc.vPos = _float3(9.5f, 0.f, 6.f);

	if (1 == m_iStageIndex)
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_TarotNPC"), TEXT("Prototype_GameObject_NPC_TarotNPC"), &TarotDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Dungeon01::Ready_Layer_MapObject()
{
	for (_uint i = 0; i < m_vecInit_CloneDesc[m_iStageIndex].size(); i++)
	{
		CLandObject::LANDOBJECT_DESC	Desc = {};

		Desc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform"), m_iStageIndex));
		Desc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer"), m_iStageIndex));

		Desc.iInitBindTexNum = m_vecInit_CloneDesc[m_iStageIndex][i].iBindTexNum;


		_float4x4 WorldMat = m_vecInit_CloneDesc[m_iStageIndex][i].vInitWorldMat;		// ���������� �ѹ� ������

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
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Object"), TEXT("Prototype_GameObject_DecoObject"), &Desc)))
				return E_FAIL;
		}
		else if (m_vecInit_CloneDesc[m_iStageIndex][i].strPrototypeName == "Prototype_GameObject_InteractObject")
		{
			Desc.eInteractType = m_vecInit_CloneDesc[m_iStageIndex][i].eInteractType;

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Object"), TEXT("Prototype_GameObject_InteractObject"), &Desc)))
				return E_FAIL;
		}

	}

	return S_OK;
}

HRESULT CLevel_Dungeon01::Ready_Layer_UI()
{
	CUI::UI_DESC Desc = {};

	Desc.m_vPos = { g_iWinSizeX / 2.f - 160.f , g_iWinSizeY / 2.f };
	Desc.m_vSize = { 150.f,150.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::UI_BAR;
	Desc.vTargetPos = { 130.f, 95.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer_UI(LEVEL_DUNGEON1, TEXT("Layer_UI"), TEXT("Prototype_GameObject_PlayerHpUI"), &Desc, (void**)&m_pPlayerHpUI[0])))
		MSG_BOX(L"Can't Create PlayerHpUI");



	Desc.m_vPos = { g_iWinSizeX / 2.f + 10 , g_iWinSizeY / 2.f };
	Desc.m_vSize = { 150.f,150.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::UI_BAR;
	Desc.vTargetPos = { 170.f, 95.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer_UI(LEVEL_DUNGEON1, TEXT("Layer_UI"), TEXT("Prototype_GameObject_PlayerHpUI"), &Desc, (void**)&m_pPlayerHpUI[1])))
		MSG_BOX(L"Can't Create PlayerHpUI");



	Desc.m_vPos = { g_iWinSizeX / 2.f + 180, g_iWinSizeY / 2.f };
	Desc.m_vSize = { 150.f,150.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::UI_BAR;
	Desc.vTargetPos = { 210.f, 95.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer_UI(LEVEL_DUNGEON1, TEXT("Layer_UI"), TEXT("Prototype_GameObject_PlayerHpUI"), &Desc, (void**)&m_pPlayerHpUI[2])))
		MSG_BOX(L"Can't Create PlayerHpUI");



	// SermonUI Set
	Desc.m_vPos = { 60.f , 70.f };
	Desc.m_vSize = { 100.f,100.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::UI_NONE;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_SermonUI"), TEXT("Prototype_GameObject_SermonUI"), &Desc)))
		MSG_BOX(L"Can't Create SermonUI");


	Desc.m_vPos = { 110.f , 125.f };
	Desc.m_vSize = { 40.f, 40.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::UI_ICON;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_SermonUI"), TEXT("Prototype_GameObject_SermonUI"), &Desc)))
		MSG_BOX(L"Can't Create SermonUI");


	Desc.m_vPos = { 80.f , 155.f };
	Desc.m_vSize = { 40.f,40.f };
	Desc.m_iBindNum = 1;
	Desc.eUItype = UI_TYPE::UI_ICON;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_SermonUI"), TEXT("Prototype_GameObject_SermonUI"), &Desc)))
		MSG_BOX(L"Can't Create SermonUI");


	Desc.m_vPos = { 50.f , 185.f };
	Desc.m_vSize = { 40.f,40.f };
	Desc.m_iBindNum = 2;
	Desc.eUItype = UI_TYPE::UI_ICON;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_SermonUI"), TEXT("Prototype_GameObject_SermonUI"), &Desc)))
		MSG_BOX(L"Can't Create SermonUI");



	for (_uint i = 0; i < 3; i++)
	{
		Safe_AddRef(m_pPlayerHpUI[i]);
	}
	
	_uint iLevelIndex = (int)LEVEL_DUNGEON1;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_TarotUI"), TEXT("Prototype_GameObject_UI_TarotUI"), &iLevelIndex)))
		return E_FAIL;

	CPressEtoPickCard::BTNE_DESC B_Desc = {};
	B_Desc.iLevelIndex = (int)LEVEL_DUNGEON1;
	B_Desc.iType = 0;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON1, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_PressEtoPickCard"),&B_Desc)))
		return E_FAIL;

	return S_OK;
}

bool CLevel_Dungeon01::Clear_Dungeon()
{
	CLayer* pBossLayer = m_pGameInstance->Find_Layer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Boss"));

	if (nullptr != pBossLayer && 0 == pBossLayer->Get_ObjectList().size())
		return true;

	return false;
}



HRESULT CLevel_Dungeon01::Load_Village_Map()
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
	fin.open("../Bin/MapData/Dungeon1/Map_Dungeon1_0.txt");

	if (!fin.is_open())
	{
		MSG_BOX(TEXT("�� ������ �� �ҷ���"));
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

	fin.open("../Bin/MapData/Dungeon1/Map_Dungeon1_1.txt");

	if (!fin.is_open())
	{
		MSG_BOX(TEXT("�� ������ �� �ҷ���"));
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

	fin.open("../Bin/MapData/Dungeon1/Map_Dungeon1_2.txt");

	if (!fin.is_open())
	{
		MSG_BOX(TEXT("�� ������ �� �ҷ���"));
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

	fin.open("../Bin/MapData/Dungeon1/Map_Dungeon1_3.txt");

	if (!fin.is_open())
	{
		MSG_BOX(TEXT("�� ������ �� �ҷ���"));
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

	if (!g_isLoaded[LEVEL_DUNGEON1])
	{
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON1, TEXT("Prototype_Component_VIBuffer_Road_Dungeon1"),
			CVIBuffer_Terrain::Create(m_pGraphic_Device, 30, 5))))
			return E_FAIL;

		CRoad* pRoad = CRoad::Create(m_pGraphic_Device);
		pRoad->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Road_Dungeon1"));
		pRoad->Set_BindTexNum(iTileNum);
		pRoad->Set_LevelIndex(LEVEL_DUNGEON1);

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Road_Dungeon1"), pRoad)))
			return E_FAIL;
	}

	return S_OK;

}

HRESULT CLevel_Dungeon01::Load_Village_Terrain(const _uint& iX, const _uint& iZ, const _uint& iTileNum, _int iStageIndex)
{
	if (!g_isLoaded[LEVEL_DUNGEON1])
	{
		switch (iStageIndex)
		{
		case 0:
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON1, TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon1_Stage0"),
				CVIBuffer_Terrain::Create(m_pGraphic_Device, iX, iZ))))
				return E_FAIL;

			CTerrain* pTerrain = CTerrain::Create(m_pGraphic_Device);
			pTerrain->Set_BindTexNum(iTileNum);
			pTerrain->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon1_Stage0"));
			pTerrain->Set_LevelIndex(LEVEL_DUNGEON1);

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Dungeon1_Stage0"), pTerrain)))
				return E_FAIL;
		}
		break;

		case 1:
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON1, TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon1_Stage1"),
				CVIBuffer_Terrain::Create(m_pGraphic_Device, iX, iZ))))
				return E_FAIL;

			CTerrain* pTerrain = CTerrain::Create(m_pGraphic_Device);
			pTerrain->Set_BindTexNum(iTileNum);
			pTerrain->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon1_Stage1"));
			pTerrain->Set_LevelIndex(LEVEL_DUNGEON1);

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Dungeon1_Stage1"), pTerrain)))
				return E_FAIL;
		}
		break;

		case 2:
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON1, TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon1_Stage2"),
				CVIBuffer_Terrain::Create(m_pGraphic_Device, iX, iZ))))
				return E_FAIL;

			CTerrain* pTerrain = CTerrain::Create(m_pGraphic_Device);
			pTerrain->Set_BindTexNum(iTileNum);
			pTerrain->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon1_Stage2"));
			pTerrain->Set_LevelIndex(LEVEL_DUNGEON1);

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Dungeon1_Stage2"), pTerrain)))
				return E_FAIL;
		}
		break;

		case 3:
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON1, TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon1_Stage3"),
				CVIBuffer_Terrain::Create(m_pGraphic_Device, iX, iZ))))
				return E_FAIL;

			CTerrain* pTerrain = CTerrain::Create(m_pGraphic_Device);
			pTerrain->Set_BindTexNum(iTileNum);
			pTerrain->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon1_Stage3"));
			pTerrain->Set_LevelIndex(LEVEL_DUNGEON1);

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Dungeon1_Stage3"), pTerrain)))
				return E_FAIL;
		}
		break;

		default:
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLevel_Dungeon01::Load_Village_Object(const string& strPrototypeName, _uint eInteractType, _uint& iModelType, _uint& iAdornType, _uint& iLandType, const _uint& iObjectTexNum, const _float4x4& vWorldMat, _uint iStageIndex)
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

HRESULT CLevel_Dungeon01::Start_StageChange(_int iNextStageIndex)
{
	if (m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Monster")) == nullptr
		|| m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Monster"))->Get_ObjectList().size() <= 0)
	{
		dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Start_FadeIn();
		m_iNextStageIndex = iNextStageIndex;
		m_isChangeStage = true;
	}
	return S_OK;
}

HRESULT CLevel_Dungeon01::Change_Stage(_int iStageIndex)
{
	if (!dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Get_IsFadeIn()
		|| !dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Get_IsFadeOut())
	{
		m_pGameInstance->Set_StageIndex(iStageIndex);

		m_iPreStageIndex = m_iStageIndex; 
		m_iStageIndex = iStageIndex;

		if (3 == m_iStageIndex)
		{
			CGameInstance::Get_Instance()->StopAll();
			CGameInstance::Get_Instance()->PlayBGM(TEXT("BGM_Eligos.wav"), g_fBgmVolume);
		}

		if (!m_isLoaded[m_iStageIndex])
		{
			m_isLoaded[m_iStageIndex] = true;

			//Ready_Layer_BackGround();
			Ready_Layer_MapObject();
			Create_First();
		}

		dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Start_FadeOut();

		m_pGameInstance->Find_Player()->Set_StageIndex(m_iStageIndex);

		CTransform* pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform"), m_iStageIndex));
		CVIBuffer_Terrain* pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer"), m_iStageIndex));

		static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Chagne_Terrain(pTerrainVIBuffer, pTerrainTransform);

		switch (m_iStageIndex)
		{
		case 0:
		{
			if (m_iPreStageIndex > m_iStageIndex)
			{
				m_vPlayerInitPos = _float3(25.5f, 2.5f, 6.f);
				m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);
			}
			static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Player_State(CPlayer::IDLE);
		}
		break;

		case 1:
		{
			if (m_iPreStageIndex > m_iStageIndex)
			{
				m_vPlayerInitPos = _float3(9.5f, 2.5f, 1.f);
				m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);
			}
			else
			{
				m_vPlayerInitPos = _float3(2.f, 2.5f, 1.5f);
				m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);
			}

			static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::IDLE);
		}
		break;

		case 2:
		{
			if (m_iPreStageIndex > m_iStageIndex)
			{
				m_vPlayerInitPos = _float3(8.f, 2.5f, 1.5f);
				m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);
			}
			else
			{
				m_vPlayerInitPos = _float3(8.5f, 2.5f, 8.5f);
				m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);
			}

			static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::IDLE);
		}
		break;

		case 3:
		{
			m_vPlayerInitPos = _float3(12.f, 2.5f, 1.5f);
			m_pGameInstance->Find_Player()->Set_Position(m_vPlayerInitPos);
			

			static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::IDLE);
		}
		break;

		}

		m_isChangeStage = false;
		m_iPreStageIndex = m_iStageIndex;
	}

	return S_OK;
}

HRESULT CLevel_Dungeon01::Create_First()
{
	CLandObject::LANDOBJECT_DESC	LandDesc = {};

	LandDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform"), m_iStageIndex));
	LandDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer"), m_iStageIndex));
	LandDesc.iLevelIndex = LEVEL_DUNGEON1;
	LandDesc.iStageIndex = m_iStageIndex;

	if (FAILED(Ready_Layer_Monster(LandDesc)))
		return E_FAIL;

	if (FAILED(Ready_Layer_NPC(LandDesc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Dungeon01* CLevel_Dungeon01::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Dungeon01* pInstance = new CLevel_Dungeon01(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Dungeon01"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Dungeon01::Free()
{
	__super::Free();

	for (_uint i = 0; i < 3; i++)
	{
		Safe_Release(m_pPlayerHpUI[i]);
	}

}
