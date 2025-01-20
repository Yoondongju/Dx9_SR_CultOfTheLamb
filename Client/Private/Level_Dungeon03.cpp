
#include "..\Public\Level_Dungeon03.h"
#include "GameInstance.h"
#include "Player.h"
#include "Monster.h"
#include "Camera.h"
#include "Layer.h"

#include "VIBuffer_Terrain.h"
#include "Terrain.h"
#include "Road.h"

#include "Client_Operators.h"
#include "TarotNPC.h"
#include "PlayerHpUI.h"
#include "TalkPanelUI.h"

#include "Eye_Controller.h"

CLevel_Dungeon03::CLevel_Dungeon03(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device }
{
}

HRESULT CLevel_Dungeon03::Initialize()
{
	m_iStageIndex = 0;
	m_pGameInstance->Set_StageIndex(m_iStageIndex);
	m_bPlayerChangeOnce = true;
	m_bEnterOnce = true;

	static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_LevelChange(true);
	static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Player_State(CPlayer::CHANGE_LEVEL);
	static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_Animation(TEXT("IDLE"));

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

	CTransform* pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform"), m_iStageIndex));
	CVIBuffer_Terrain* pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer"), m_iStageIndex));

	static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Chagne_Terrain(pTerrainVIBuffer, pTerrainTransform);

	static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Position(_float3(15.f, 2.5f, 10.f));

	m_pEyeController = CEye_Controller::Create(m_pGraphic_Device, nullptr);

	CGameInstance::Get_Instance()->PlayBGM(TEXT("BGM_Dungeon3BeforStart.wav"), g_fBgmVolume);
	
	return S_OK;
}

void CLevel_Dungeon03::Update(_float fTimeDelta)
{
	if (m_bPlayerSizeOnce)
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
	}

	if (true == m_pPlayerHpUI[2]->GetActivate()
		&& m_bPlayerChangeOnce
		&& m_fTimeTo_CreateHp > 6.f)
	{
		static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::IDLE);
		m_bPlayerChangeOnce = false;
	}
	m_fTimeTo_CreateHp += fTimeDelta;

	if (m_fTimeTo_CreateHp > 8.f)
	{
		CGameObject* pGameObj = m_pGameInstance->Find_Object(m_pGameInstance->Get_LevelIndex(), L"Layer_Boss", 1);			// 0이 에임 1이 데스켓
		CCamera* pCam = static_cast<CCamera*>(m_pGameInstance->Find_Camera());

		static bool bCheck = false;			// 이것도 임시 나중에 상황에 맞게 바꿔야해

		if (!bCheck)
		{
			pCam->Target_to(pGameObj);
			pCam->Zoom_In(2.f);

			bCheck = true;
		}

		static bool bCheck2 = false;			// 이것도 임시 나중에 상황에 맞게 바꿔야해

		if (!bCheck2)
		{
			if (pCam->IsFinishZoomIn())
			{
				CTalkPanelUI::TALK_DESC Desc = {};
				Desc.UIDesc.m_vPos = { g_iWinSizeX / 2.f , g_iWinSizeY / 2.f - 200.f };
				Desc.UIDesc.m_vSize = { 600.f,300.f };
				Desc.UIDesc.m_iBindNum = 0;
				Desc.UIDesc.bActivate = true;
				Desc.strNameOpponent = L"기다리는 자";
				Desc.strLayerOpponent = L"Layer Boss";

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_TalkUI"), TEXT("Prototype_GameObject_TalkUI"), &Desc)))
					MSG_BOX(L"Can't Create TalkUI");

				m_fTimeTo_CreateHp = 0.f;

				bCheck2 = true;
			}
		}
	}

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON3, TEXT("Layer_Attack")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Attack"), TEXT("Layer_Player"), LEVEL_DUNGEON3, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON3, TEXT("Layer_MonsterBone")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_MonsterBone"), TEXT("Layer_Player"), LEVEL_DUNGEON3, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON3, TEXT("Layer_Boss")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Boss"), TEXT("Layer_Player"), LEVEL_DUNGEON3, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON3, TEXT("Layer_Monster")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Monster"), TEXT("Layer_Player"), LEVEL_DUNGEON3, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Object")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_BackGround_Object"), TEXT("Layer_Player"), LEVEL_DUNGEON3, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON3, TEXT("Layer_Monster"))
		&& nullptr != m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Player_Bullet")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Monster"), TEXT("Layer_Player_Bullet"), LEVEL_DUNGEON3, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON3, TEXT("Layer_Boss"))
		&& nullptr != m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Player_Bullet")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Boss"), TEXT("Layer_Player_Bullet"), LEVEL_DUNGEON3, LEVEL_STATIC);

	if (m_pGameInstance->Get_KeyState(KEY::ENTER) == KEY_STATE::TAP)
	{
		//m_pGameInstance->Play_Sound(TEXT("Build_Complete.wav"), SOUND_BGM, 1.f);
		/*if (m_iStageIndex < 3)
			Change_Stage(++m_iStageIndex);
		else
		{
			if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, LEVEL_GAMEPLAY))))
				return;
		}*/
	}

	m_pEyeController->Update(fTimeDelta);

}

HRESULT CLevel_Dungeon03::Render()
{
	//SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
	return S_OK;
}

HRESULT CLevel_Dungeon03::Ready_Layer_Camera()
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

HRESULT CLevel_Dungeon03::Ready_Layer_BackGround()
{
	CRoad::ROAD_DESC RoadDesc{};
	RoadDesc.iStageIndex = m_iStageIndex;

	switch (m_iStageIndex)
	{
	case 0:
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Terrain"), TEXT("Prototype_GameObject_Terrain_Dungeon3_Stage0"), &m_iStageIndex)))
			return E_FAIL;

		break;

	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Dungeon03::Ready_LandObjects()
{
	CLandObject::LANDOBJECT_DESC	LandDesc = {};

	LandDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
	LandDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
	LandDesc.iLevelIndex = LEVEL_DUNGEON3;
	LandDesc.iStageIndex = m_pGameInstance->Get_StageIndex();

	if (FAILED(Ready_Layer_Monster(LandDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Dungeon03::Ready_Layer_Monster(CLandObject::LANDOBJECT_DESC& LandObjectDesc)
{
	CMonster::MONSTER_INFO MonsterInfo = {};
	MonsterInfo.tLandObjectDesc = LandObjectDesc;


	if (0 == m_iStageIndex)
	{
		// 임시로 Aim 박아 놧어요 //
	
		MonsterInfo.iHp = 100;
		MonsterInfo.iAttackPower = 2;
		MonsterInfo.vInitPosition = _float3(4.f, 0.f, 3.f);

		// Aim
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON3, TEXT("Layer_Boss"), TEXT("Prototype_GameObject_Aim"), &MonsterInfo)))
			return E_FAIL;

		MonsterInfo.iHp = 100;
		MonsterInfo.iAttackPower = 3;
		MonsterInfo.vInitPosition = _float3(7.f, 0.f, 15.f);

		//Death Cat
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON3, TEXT("Layer_Boss"), TEXT("Prototype_GameObject_DeathCat"), &MonsterInfo)))
			return E_FAIL;

	}

	return S_OK;
}

HRESULT CLevel_Dungeon03::Ready_Layer_Follower(CFollower::FOLLOWER_INFO& FollowerInfo)
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON3, TEXT("Layer_Follower"), TEXT("Prototype_GameObject_Follower"), &FollowerInfo)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Dungeon03::Ready_Layer_MapObject()
{
	for (_uint i = 0; i < m_vecInit_CloneDesc[m_iStageIndex].size(); i++)
	{
		CLandObject::LANDOBJECT_DESC	Desc = {};

		Desc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform"), m_iStageIndex));
		Desc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer"), m_iStageIndex));

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
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Object"), TEXT("Prototype_GameObject_DecoObject"), &Desc)))
				return E_FAIL;
		}
		else if (m_vecInit_CloneDesc[m_iStageIndex][i].strPrototypeName == "Prototype_GameObject_InteractObject")
		{
			Desc.eInteractType = m_vecInit_CloneDesc[m_iStageIndex][i].eInteractType;

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Object"), TEXT("Prototype_GameObject_InteractObject"), &Desc)))
				return E_FAIL;
		}

	}

	return S_OK;
}

HRESULT CLevel_Dungeon03::Ready_Layer_UI()
{
	CUI::UI_DESC Desc = {};

	Desc.m_vPos = { g_iWinSizeX / 2.f - 160.f , g_iWinSizeY / 2.f };
	Desc.m_vSize = { 150.f,150.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::UI_BAR;
	Desc.vTargetPos = { 130.f, 95.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer_UI(LEVEL_DUNGEON3, TEXT("Layer_UI"), TEXT("Prototype_GameObject_PlayerHpUI"), &Desc, (void**)&m_pPlayerHpUI[0])))
		MSG_BOX(L"Can't Create PlayerHpUI");



	Desc.m_vPos = { g_iWinSizeX / 2.f + 10 , g_iWinSizeY / 2.f };
	Desc.m_vSize = { 150.f,150.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::UI_BAR;
	Desc.vTargetPos = { 170.f, 95.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer_UI(LEVEL_DUNGEON3, TEXT("Layer_UI"), TEXT("Prototype_GameObject_PlayerHpUI"), &Desc, (void**)&m_pPlayerHpUI[1])))
		MSG_BOX(L"Can't Create PlayerHpUI");



	Desc.m_vPos = { g_iWinSizeX / 2.f + 180, g_iWinSizeY / 2.f };
	Desc.m_vSize = { 150.f,150.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::UI_BAR;
	Desc.vTargetPos = { 210.f, 95.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer_UI(LEVEL_DUNGEON3, TEXT("Layer_UI"), TEXT("Prototype_GameObject_PlayerHpUI"), &Desc, (void**)&m_pPlayerHpUI[2])))
		MSG_BOX(L"Can't Create PlayerHpUI");



	// SermonUI Set
	Desc.m_vPos = { 60.f , 70.f };
	Desc.m_vSize = { 100.f,100.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::UI_NONE;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON3, TEXT("Layer_SermonUI"), TEXT("Prototype_GameObject_SermonUI"), &Desc)))
		MSG_BOX(L"Can't Create SermonUI");


	Desc.m_vPos = { 110.f , 125.f };
	Desc.m_vSize = { 40.f, 40.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::UI_ICON;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON3, TEXT("Layer_SermonUI"), TEXT("Prototype_GameObject_SermonUI"), &Desc)))
		MSG_BOX(L"Can't Create SermonUI");


	Desc.m_vPos = { 80.f , 155.f };
	Desc.m_vSize = { 40.f,40.f };
	Desc.m_iBindNum = 1;
	Desc.eUItype = UI_TYPE::UI_ICON;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON3, TEXT("Layer_SermonUI"), TEXT("Prototype_GameObject_SermonUI"), &Desc)))
		MSG_BOX(L"Can't Create SermonUI");


	Desc.m_vPos = { 50.f , 185.f };
	Desc.m_vSize = { 40.f,40.f };
	Desc.m_iBindNum = 2;
	Desc.eUItype = UI_TYPE::UI_ICON;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON3, TEXT("Layer_SermonUI"), TEXT("Prototype_GameObject_SermonUI"), &Desc)))
		MSG_BOX(L"Can't Create SermonUI");



	for (_uint i = 0; i < 3; i++)
	{
		Safe_AddRef(m_pPlayerHpUI[i]);
	}

	return S_OK;
}

CEye_Controller* CLevel_Dungeon03::Get_EyeController()
{
	return m_pEyeController;
}




HRESULT CLevel_Dungeon03::Load_Village_Map()
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
	fin.open("../Bin/MapData/Dungeon3/Map_Dungeon3_0.txt");

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

	return S_OK;

}

HRESULT CLevel_Dungeon03::Load_Village_Terrain(const _uint& iX, const _uint& iZ, const _uint& iTileNum, _int iStageIndex)
{
	if (!g_isLoaded[LEVEL_DUNGEON3])
	{
		switch (iStageIndex)
		{
		case 0:
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon3_Stage0"),
				CVIBuffer_Terrain::Create(m_pGraphic_Device, iX, iZ))))
				return E_FAIL;

			CTerrain* pTerrain = CTerrain::Create(m_pGraphic_Device);
			pTerrain->Set_BindTexNum(iTileNum);
			pTerrain->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon3_Stage0"));
			pTerrain->Set_LevelIndex(LEVEL_DUNGEON3);

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Dungeon3_Stage0"), pTerrain)))
				return E_FAIL;
		}
		break;

		case 1:
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon3_Stage1"),
				CVIBuffer_Terrain::Create(m_pGraphic_Device, iX, iZ))))
				return E_FAIL;

			CTerrain* pTerrain = CTerrain::Create(m_pGraphic_Device);
			pTerrain->Set_BindTexNum(iTileNum);
			pTerrain->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon3_Stage1"));
			pTerrain->Set_LevelIndex(LEVEL_DUNGEON3);

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Dungeon3_Stage1"), pTerrain)))
				return E_FAIL;
		}
		break;

		case 2:
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon3_Stage2"),
				CVIBuffer_Terrain::Create(m_pGraphic_Device, iX, iZ))))
				return E_FAIL;

			CTerrain* pTerrain = CTerrain::Create(m_pGraphic_Device);
			pTerrain->Set_BindTexNum(iTileNum);
			pTerrain->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon3_Stage2"));
			pTerrain->Set_LevelIndex(LEVEL_DUNGEON3);

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Dungeon3_Stage2"), pTerrain)))
				return E_FAIL;
		}
		break;

		case 3:
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon3_Stage3"),
				CVIBuffer_Terrain::Create(m_pGraphic_Device, iX, iZ))))
				return E_FAIL;

			CTerrain* pTerrain = CTerrain::Create(m_pGraphic_Device);
			pTerrain->Set_BindTexNum(iTileNum);
			pTerrain->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Terrain_Dungeon3_Stage3"));
			pTerrain->Set_LevelIndex(LEVEL_DUNGEON3);

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Dungeon3_Stage3"), pTerrain)))
				return E_FAIL;
		}
		break;

		default:
			return E_FAIL;
		}
	}

	return S_OK;
}


HRESULT CLevel_Dungeon03::Load_Village_Object(const string& strPrototypeName, _uint eInteractType, _uint& iModelType, _uint& iAdornType, _uint& iLandType, const _uint& iObjectTexNum, const _float4x4& vWorldMat, _uint iStageIndex)
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

HRESULT CLevel_Dungeon03::Change_Stage(_int iStageIndex)
{
	/*m_isChangeStage = true;
	m_pGameInstance->Set_StageIndex(iStageIndex);

	m_iStageIndex = iStageIndex;

	if (!m_isLoaded[m_iStageIndex])
	{
		m_isLoaded[m_iStageIndex] = true;

		Ready_Layer_BackGround();
		Ready_Layer_MapObject();
		Create_First();
	}

	m_isChangeStage = false;

	m_pGameInstance->Find_Player()->Set_StageIndex(m_iStageIndex);
	static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Player_State(CPlayer::CHANGE_LEVEL);*/

	return S_OK;
}

HRESULT CLevel_Dungeon03::Create_First()
{
	CLandObject::LANDOBJECT_DESC	LandDesc = {};

	LandDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform"), m_iStageIndex));
	LandDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer"), m_iStageIndex));
	LandDesc.iLevelIndex = LEVEL_DUNGEON3;
	LandDesc.iStageIndex = m_pGameInstance->Get_StageIndex();

	if (FAILED(Ready_Layer_Monster(LandDesc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Dungeon03* CLevel_Dungeon03::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Dungeon03* pInstance = new CLevel_Dungeon03(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Dungeon03"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Dungeon03::Free()
{
	__super::Free();


	for (_uint i = 0; i < 3; i++)
	{
		Safe_Release(m_pPlayerHpUI[i]);
	}

	Safe_Release(m_pEyeController);

}
