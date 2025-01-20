#include "State_HekatTF.h"
#include "Boss_Hekat.h"
#include "GameInstance.h"

#include "MonsterBone.h"

#include "Camera.h"
#include "MonologueUI.h"

CState_HekatTF::CState_HekatTF(CFsm* pFsm)
	:CState(pFsm)
{
}

HRESULT CState_HekatTF::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_HekatTF::Start_State()
{
	m_pHekat = static_cast<CBoss_Hekat*>(m_pFsm->Get_Owner());
	m_pHekat->Change_Animation(TEXT("Transform_Frog"));
	m_pHekat->Set_Hekat_State(CBoss_Hekat::TRANS_FROG);
	m_bEffectOnce = true;
	m_bNameOnce = true;
	return S_OK;
}

void CState_HekatTF::Update(_float fTimeDelta)
{
	if (m_bEffectOnce
		&& m_pHekat->Get_CurrentFrame() >= 71.f)
	{
		CLandObject::LANDOBJECT_DESC Land_Desc = {};
		Land_Desc.pTerrainTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON2, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform"), 3));
		Land_Desc.pTerrainVIBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON2, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer"), 3));
		Land_Desc.iLevelIndex = LEVEL_DUNGEON2;
		Land_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

		CMonsterBone::BONE_DESC Monster_Bone = {};
		Monster_Bone.eBoneType = CMonsterBone::HUMAN;
		Monster_Bone.tLandDesc = Land_Desc;
		// 10
		Monster_Bone.vInitPos = _float3(8.f, 2.5f, 6.f);
		Monster_Bone.vFlyDir = _float3(-1.f, 0.f, -1.f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_MonsterBone"), TEXT("Prototype_GameObject_MonsterBone"), sizeof(CMonsterBone::BONE_DESC), &Monster_Bone);

		// 12
		Monster_Bone.vInitPos = _float3(12.f, 2.5f, 12.f);
		Monster_Bone.vFlyDir = _float3(0.f, 0.f, 0.7f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_MonsterBone"), TEXT("Prototype_GameObject_MonsterBone"), sizeof(CMonsterBone::BONE_DESC), &Monster_Bone);

		// 6
		Monster_Bone.vInitPos = _float3(12.f, 2.5f, 5.f); 
		Monster_Bone.vFlyDir = _float3(0.f, 0.f, -0.7f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_MonsterBone"), TEXT("Prototype_GameObject_MonsterBone"), sizeof(CMonsterBone::BONE_DESC), &Monster_Bone);

		// 8
		Monster_Bone.vInitPos = _float3(7.5f, 2.5f, 10.f);
		Monster_Bone.vFlyDir = _float3(-1.f, 0.f, 1.f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_MonsterBone"), TEXT("Prototype_GameObject_MonsterBone"), sizeof(CMonsterBone::BONE_DESC), &Monster_Bone);

		// 1
		Monster_Bone.vInitPos = _float3(15.5f, 2.5f, 10.f);
		Monster_Bone.vFlyDir = _float3(1.f, 0.f, 1.f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_MonsterBone"), TEXT("Prototype_GameObject_MonsterBone"), sizeof(CMonsterBone::BONE_DESC), &Monster_Bone);

		// 5
		Monster_Bone.vInitPos = _float3(15.5f, 2.5f, 5.f);
		Monster_Bone.vFlyDir = _float3(0.7f, 0.f, -0.7f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_MonsterBone"), TEXT("Prototype_GameObject_MonsterBone"), sizeof(CMonsterBone::BONE_DESC), &Monster_Bone);

		Monster_Bone.vInitPos = _float3(16.f, 2.5f, 8.5f);
		Monster_Bone.vFlyDir = _float3(2.f, 0.f, 0.f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_MonsterBone"), TEXT("Prototype_GameObject_MonsterBone"), sizeof(CMonsterBone::BONE_DESC), &Monster_Bone);

		Monster_Bone.vInitPos = _float3(7.5f, 2.5f, 8.5f);
		Monster_Bone.vFlyDir = _float3(-2.f, 0.f, 0.f);
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_MonsterBone"), TEXT("Prototype_GameObject_MonsterBone"), sizeof(CMonsterBone::BONE_DESC), &Monster_Bone);
		
		static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_Out(1.2f);
		static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(0.7f, 0.8f, 5);

		m_bEffectOnce = false;
	
	}

	if (m_pHekat->Get_CurrentFrame() >= 75.f
		&& m_bNameOnce)
	{
		CMonologueUI::MONOLOGUE_DESC Desc = {};
		Desc.strText = TEXT("ÇìÄÏ");
		Desc.rt = {
						(LONG)(g_iWinSizeX / 2.f - (Desc.strText.size() * 25.f)),
						(LONG)(g_iWinSizeY / 2.f),
						(LONG)(g_iWinSizeX / 2.f + 300.f),
						(LONG)(g_iWinSizeY / 2.f + 100.f)
		};

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonologueUI"), &Desc)))
			MSG_BOX(TEXT("Can't Create MonologueUI"));

		m_bNameOnce = false;
	}

	if (m_pHekat->IsEnd_CurrentAnimation())
	{
		static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to_Player();
		m_pHekat->Change_State(CBoss_Hekat::IDLE);
	}
}

void CState_HekatTF::End_State()
{
}

CState_HekatTF* CState_HekatTF::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_HekatTF* pInstance = new CState_HekatTF(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_HekatTF"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_HekatTF::Free()
{
	__super::Free();
}

