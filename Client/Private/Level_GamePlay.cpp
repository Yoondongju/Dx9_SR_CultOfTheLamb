#include "..\Public\Level_GamePlay.h"

#include "GameInstance.h"
#include "Player.h"
#include "Monster.h"
#include "Camera.h"
#include "Chest.h"

#include "VIBuffer_Terrain.h"
#include "Terrain.h"
#include "Road.h"
#include "Door.h"
#include "UI.h"

#include "Client_Operators.h"
#include "Layer.h"
#include <BuildingObject.h>


std::istream& operator>>(std::istream& is, D3DXMATRIX& mat) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			is >> mat.m[i][j];
		}
	}
	return is;
}

CLevel_GamePlay::CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	m_iStageIndex = 0;
	//m_isLoaded[m_iStageIndex] = true;
	m_pGameInstance->Set_StageIndex(m_iStageIndex);
	m_isPrevDungeon = true;

	static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Position(_float3(31.5f, 2.5f, 38.5f));

	if (FAILED(Load_Village_Map()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))		// 이미 사본은 로고에서 만들어서 카메라 Desc세팅용
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject(0)))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject(1)))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;

	if (FAILED(Ready_Door()))
		return E_FAIL;

	if (FAILED(Init_Followers()))
		return E_FAIL;

	CTransform* pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform"), m_iStageIndex));
	CVIBuffer_Terrain* pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer"), m_iStageIndex));

	static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Chagne_Terrain(pTerrainVIBuffer, pTerrainTransform);

	CGameInstance::Get_Instance()->PlayBGM(TEXT("BGM_Villiage.wav"), g_fBgmVolume);


	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	if (m_iStageIndex == 0)
		m_vWrapDistance = _float3(31.5f, 2.5f, 38.5f) - static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_Position();
	else if (m_iStageIndex == 1)
		m_vWrapDistance = _float3(14.f, 2.5f, 11.f) - static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_Position();

	m_fWrapDistance = D3DXVec3Length(&m_vWrapDistance);

	Create_Follower();

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Follower")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Follower"), TEXT("Layer_Player"), LEVEL_GAMEPLAY, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Item")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Item"), TEXT("Layer_Player"), LEVEL_GAMEPLAY, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Object")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_BackGround_Object"), TEXT("Layer_Player"), LEVEL_GAMEPLAY, LEVEL_STATIC);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Object"))
		&& nullptr != m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Follower")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Follower"), TEXT("Layer_BackGround_Object"), LEVEL_STATIC, LEVEL_GAMEPLAY);

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Door")))
		m_pGameInstance->Collision_Cube(TEXT("Layer_Door"), TEXT("Layer_Player"), LEVEL_STATIC, LEVEL_STATIC);

	// 조건추가 원형 돌 안에 서있고 + 엔터를 누르면
	if (m_fWrapDistance < 5.f
		&& m_pGameInstance->Get_KeyState(KEY::ENTER) == KEY_STATE::TAP)
	{
		static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::WARP_OUT);
		static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Player_PreState(CPlayer::WARP_OUT);
		m_isWrap = false;

	}
	if (static_cast<CPlayer*>(m_pGameInstance->Find_Player())->IsEnd_CurrentAnimation()
		&& static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_Player_PreState() == CPlayer::WARP_OUT)
	{
		if (!m_isWrap)
		{
			if (m_iStageIndex == 1 || m_isPrevDungeon)
				Change_Stage(0);
			else if (m_iStageIndex == 0)
				Change_Stage(1);

			if (m_isPrevDungeon)
				m_isPrevDungeon = false;

			m_isWrap = true;
		}
	}

	if (static_cast<CPlayer*>(m_pGameInstance->Find_Player())->IsEnd_CurrentAnimation()
		&& static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_Player_PreState() == CPlayer::WARP_IN)
	{
		static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Player_State(CPlayer::IDLE);
		static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::IDLE);
		m_isWrap = false;
	}

}

HRESULT CLevel_GamePlay::Render()
{
	//SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera()
{
	CCamera::CAMERA_DESC		Desc{};

	Desc.vEye = _float3(0.f, 9.f, -12.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.fFovy = D3DXToRadian(55.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = D3DXToRadian(90.0f);

	CCamera* pCamera = static_cast<CCamera*>(m_pGameInstance->Find_Camera());

	(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->InitCamera_Desc(&Desc);
	(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->Set_Position_to_Player();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround()
{
	_uint iStageIndex = 0;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Prototype_GameObject_Terrain_Village"), &iStageIndex)))
		return E_FAIL;

	iStageIndex = 1;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Prototype_GameObject_Terrain_Village_Door"), &iStageIndex)))
		return E_FAIL;
	switch (m_iStageIndex)
	{
	case 0:
		
		break;

	case 1:
		
		break;

	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObjects()
{
	CLandObject::LANDOBJECT_DESC	LandDesc = {};

	LandDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
	LandDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
	LandDesc.iLevelIndex = LEVEL_GAMEPLAY;
	LandDesc.iStageIndex = m_pGameInstance->Get_StageIndex();

	(static_cast<CCamera*>(m_pGameInstance->Find_Camera()))->Set_Position_to_Player();

	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Follower(CFollower::FOLLOWER_INFO& FollowerInfo)
{

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Follower"), TEXT("Prototype_GameObject_Follower"), &FollowerInfo)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject(_uint iStageIndex)
{
	for (_uint i = 0; i < m_vecInit_CloneDesc[iStageIndex].size(); i++)
	{
		CLandObject::LANDOBJECT_DESC	Desc = {};

		Desc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform"), iStageIndex));
		Desc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer"), iStageIndex));

		Desc.iInitBindTexNum = m_vecInit_CloneDesc[iStageIndex][i].iBindTexNum;


		_float4x4 WorldMat = m_vecInit_CloneDesc[iStageIndex][i].vInitWorldMat;		// 가독성위해 한번 끊었음

		Desc.vInitPos = *((_float3*)&WorldMat.m[3][0]);
		Desc.vInitScale = _float3(D3DXVec3Length((_float3*)&WorldMat.m[0][0]),
			D3DXVec3Length((_float3*)&WorldMat.m[1][0]),
			D3DXVec3Length((_float3*)&WorldMat.m[2][0]));

		Desc.iPreModelType = m_vecInit_CloneDesc[iStageIndex][i].iModelType;
		Desc.iPreAdornType = m_vecInit_CloneDesc[iStageIndex][i].iAdornType;
		Desc.iPreLandType = m_vecInit_CloneDesc[iStageIndex][i].iLandType;
		Desc.iLevelIndex = m_vecInit_CloneDesc[iStageIndex][i].iLevelIndex;
		Desc.iStageIndex = m_vecInit_CloneDesc[iStageIndex][i].iStageIndex;

		if (m_vecInit_CloneDesc[iStageIndex][i].strPrototypeName == "Prototype_GameObject_DecoObject")
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Object"), TEXT("Prototype_GameObject_DecoObject"), &Desc)))
				return E_FAIL;
		}
		else if (m_vecInit_CloneDesc[iStageIndex][i].strPrototypeName == "Prototype_GameObject_InteractObject")
		{
			Desc.eInteractType = m_vecInit_CloneDesc[iStageIndex][i].eInteractType;

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Object"), TEXT("Prototype_GameObject_InteractObject"), &Desc)))
				return E_FAIL;
		}
	}


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_MouseClickUI"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Follower_Command_UI"), TEXT("Prototype_GameObject_Follower_Command_UI"))))
		return E_FAIL;



	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Door()
{
	if (!g_isLoaded[LEVEL_GAMEPLAY])
	{
		CLandObject::LANDOBJECT_DESC	LandDesc = {};

		LandDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
		LandDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
		LandDesc.iLevelIndex = LEVEL_GAMEPLAY;
		LandDesc.iStageIndex = 1;

		CDoor::DOOR_DESC tDoorDesc = {};

		tDoorDesc.eDoorId = CDoor::DOOR_DUNGEON1;
		tDoorDesc.vPosition = _float3(3.5f, 0.f, 20.3f);
		tDoorDesc.isOpen = true;
		tDoorDesc.tLandDesc = LandDesc;
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Door"), TEXT("Prototype_GameObject_Door"), &tDoorDesc);

		tDoorDesc.eDoorId = CDoor::DOOR_DUNGEON3;
		tDoorDesc.vPosition = _float3(13.5f, 0.f, 20.3f);
		tDoorDesc.isOpen = false;
		tDoorDesc.tLandDesc = LandDesc;
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Door"), TEXT("Prototype_GameObject_Door"), &tDoorDesc);

		tDoorDesc.eDoorId = CDoor::DOOR_DUNGEON2;
		tDoorDesc.vPosition = _float3(23.4f, 0.f, 20.3f);
		tDoorDesc.isOpen = false;
		tDoorDesc.tLandDesc = LandDesc;
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Door"), TEXT("Prototype_GameObject_Door"), &tDoorDesc);
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Init_Followers()
{
	list<CGameObject*> Followers;

	if (nullptr != m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Follower")))
	{
		Followers = m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Follower"))->Get_ObjectList();
	}
	else
	{
		return S_OK;
	}

	CLandObject::LANDOBJECT_DESC	LandDesc = {};

	LandDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
	LandDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));

	if (Followers.size() > 0)
	{
		for (auto& Follower : Followers)
		{
			dynamic_cast<CFollower*>(Follower)->Change_State(CFollower::FOLLOWER_STATE::IDLE);
		}

		CUI* pUI = dynamic_cast<CUI*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Follower_Command_UI"), 0));
		pUI->SetActivate(false);
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Village_Map()
{
	_char				iVerticesX[50] = {};
	_char				iVerticesZ[50] = {};
	_int				iTileNum = {};

	_uint				iObjectCount = {};
	string				strPrototypeName = {};
	_uint				iBindTexNum = {};
	_float4x4			WorldMat = {};

	ifstream fin;


#pragma region Village
	fin.open("../Bin/MapData/Map_Village.txt");

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

#pragma region Village_Door

	fin.open("../Bin/MapData/Map_Village_Door.txt");

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

	return S_OK;

}

HRESULT CLevel_GamePlay::Load_Village_Terrain(const _uint& iX, const _uint& iZ, const _uint& iTileNum, _int iStageIndex)
{
	if (!g_isLoaded[LEVEL_GAMEPLAY])
	{
		if (iStageIndex == 0)
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain_Village"),
				CVIBuffer_Terrain::Create(m_pGraphic_Device, iX, iZ))))
				return E_FAIL;

			/* For. Prototype_GameObject_Terrain */
			CTerrain* pTerrain = CTerrain::Create(m_pGraphic_Device);
			pTerrain->Set_BindTexNum(iTileNum);
			pTerrain->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Terrain_Village"));
			pTerrain->Set_LevelIndex(LEVEL_GAMEPLAY);

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Village"), pTerrain)))
				return E_FAIL;
		}

		if (iStageIndex == 1)
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain_Viilage_Door"),
				CVIBuffer_Terrain::Create(m_pGraphic_Device, iX, iZ))))
				return E_FAIL;

			/* For. Prototype_GameObject_Terrain */
			CTerrain* pTerrain = CTerrain::Create(m_pGraphic_Device);
			pTerrain->Set_BindTexNum(iTileNum);
			pTerrain->Set_VIBufferTag(TEXT("Prototype_Component_VIBuffer_Terrain_Viilage_Door"));
			pTerrain->Set_LevelIndex(LEVEL_GAMEPLAY);

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Village_Door"), pTerrain)))
				return E_FAIL;
		}
	}
	return S_OK;
}


HRESULT CLevel_GamePlay::Load_Village_Object(const string& strPrototypeName, _uint eInteractType, _uint& iModelType, _uint& iAdornType, _uint& iLandType, const _uint& iObjectTexNum, const _float4x4& vWorldMat, _uint iStageIndex)
{
	CLevel_Loading::INIT_CLONEDESC Desc = {};
	Desc.strPrototypeName = strPrototypeName;
	Desc.iBindTexNum = iObjectTexNum;
	Desc.vInitWorldMat = vWorldMat;
	Desc.eInteractType = (CLandObject::INTERACT_TYPE)eInteractType;

	Desc.iModelType = iModelType;
	Desc.iAdornType = iAdornType;
	Desc.iLandType = iLandType;
	Desc.iLevelIndex = LEVEL_GAMEPLAY;
	Desc.iStageIndex = iStageIndex;

	m_vecInit_CloneDesc[iStageIndex].push_back(Desc);

	return S_OK;
}

HRESULT CLevel_GamePlay::Change_Stage(_int iStageIndex)
{
	if (m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Monster")) == nullptr
		|| m_pGameInstance->Find_Layer(LEVEL_DUNGEON0, TEXT("Layer_Monster"))->Get_ObjectList().size() <= 0)
	{

		m_isChangeStage = true;
		m_pGameInstance->Set_StageIndex(iStageIndex);

		m_iStageIndex = iStageIndex;

		if (!m_isLoaded[m_iStageIndex])
		{
			m_isLoaded[m_iStageIndex] = true;

			//Ready_Layer_MapObject();
		}

		m_isChangeStage = false;

		m_pGameInstance->Find_Player()->Set_StageIndex(m_iStageIndex);

		static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::WARP_IN);
		static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Player_PreState(CPlayer::WARP_IN);

		//CTransform* pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform"), m_iStageIndex));
		//CVIBuffer_Terrain* pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer"), m_iStageIndex));

		//static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Chagne_Terrain(pTerrainVIBuffer, pTerrainTransform);

		if(iStageIndex == 1)
			static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Position(_float3(14.f, 2.5f, 4.f));
		else if(iStageIndex == 0)
			static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Position(_float3(31.5f, 2.5f, 38.5f));

		m_isWrap = false;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Create_Follower()
{
	CLandObject::LANDOBJECT_DESC	LandDesc = {};

	LandDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
	LandDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
	LandDesc.iLevelIndex = LEVEL_GAMEPLAY;
	LandDesc.iStageIndex = m_pGameInstance->Get_StageIndex();

	CFollower::FOLLOWER_INFO FollowerInfo = { };

	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_BuildingObject"));
	if (pLayer == nullptr)
		return E_FAIL;

	list<class CGameObject*>& ObjectList = pLayer->Get_ObjectList();

	if (ObjectList.empty())
		return S_OK;

	CBuildingObject* pBuildingObj = dynamic_cast<CBuildingObject*>(ObjectList.back());
	if (pBuildingObj == nullptr)
		return E_FAIL;

	if (ObjectList.size() == 1 && dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_FollowerCount() == 0 && pBuildingObj->IsLand() )
	{
		dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Add_FollowerCount();
		FollowerInfo.e_type = CFollower::RABBIT;
		FollowerInfo.iHunger = 100;
		FollowerInfo.tLandObjectDesc = LandDesc;
		FollowerInfo.fX = 44.3f;
		FollowerInfo.fZ = 30.3f;
		m_isWrap = true;
		if (FAILED(Ready_Layer_Follower(FollowerInfo)))
			return E_FAIL;
	}
	else if (ObjectList.size() == 2 && dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_FollowerCount() == 2 && pBuildingObj->IsLand())
	{
		dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Add_FollowerCount();
		FollowerInfo.e_type = CFollower::BEE;
		FollowerInfo.iHunger = 100;
		FollowerInfo.tLandObjectDesc = LandDesc;
		FollowerInfo.fX = 44.3f;
		FollowerInfo.fZ = 30.3f;
		m_isWrap = true;
		if (FAILED(Ready_Layer_Follower(FollowerInfo)))
			return E_FAIL;
	}
	else if (ObjectList.size() == 3 && dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_FollowerCount() == 3 && pBuildingObj->IsLand())
	{
		dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Add_FollowerCount();
		FollowerInfo.e_type = CFollower::DEER;
		FollowerInfo.iHunger = 100;
		FollowerInfo.tLandObjectDesc = LandDesc;
		FollowerInfo.fX = 44.3f;
		FollowerInfo.fZ = 30.3f;
		m_isWrap = true;
		if (FAILED(Ready_Layer_Follower(FollowerInfo)))
			return E_FAIL;
	}

	if (dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_Kill_Eligos())
	{
		dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Add_FollowerCount();
		dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Kill_Eligos(false);
		FollowerInfo.e_type = CFollower::ELIGOS;
		FollowerInfo.iHunger = 100;
		FollowerInfo.tLandObjectDesc = LandDesc;
		FollowerInfo.fX = 44.3f;
		FollowerInfo.fZ = 30.3f;
		m_isWrap = true;
		if (FAILED(Ready_Layer_Follower(FollowerInfo)))
			return E_FAIL;
	}
	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}
