#include "State_HekatTH.h"
#include "Boss_Hekat.h"
#include "GameInstance.h"

#include "Effect.h"

#include "Camera.h"

CState_HekatTH::CState_HekatTH(CFsm* pFsm)
	:CState(pFsm)
{
}

HRESULT CState_HekatTH::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_HekatTH::Start_State()
{
	m_pHekat = static_cast<CBoss_Hekat*>(m_pFsm->Get_Owner());
	m_pHekat->Change_Animation(TEXT("Transform_Human"));
	m_pHekat->Set_Hekat_State(CBoss_Hekat::TRANS_HUMAN);
	m_pGameInstance->Play_Sound(TEXT("Heket_Transform.wav"), SOUND_HEKAT, 0.8f);
	m_bEffect_Once = true;

	CEffect::EFFECT_DESC Effect_Desc = {};
	Effect_Desc.fEffectDura = 15.f;
	Effect_Desc.iLevelIndex = LEVEL_DUNGEON2;
	Effect_Desc.bIsFilp = false;
	Effect_Desc.fDelayTimer = 0.f;
	Effect_Desc.strTextureTag = TEXT("Enemy_Sucide");
	Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
	Effect_Desc.vScale = _float3(5.f, 5.f, 0.1f);
	Effect_Desc.vPos= _float3(12.f, 2.5f, 12.f);
	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

	Effect_Desc.bIsFilp = true;
	Effect_Desc.vPos = _float3(12.f, 2.5f, 5.f);
	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

	Effect_Desc.bIsFilp = true;
	Effect_Desc.vPos = _float3(7.5f, 2.5f, 10.f);
	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

	Effect_Desc.bIsFilp = true;
	Effect_Desc.vPos = _float3(8.f, 2.5f, 6.f);
	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

	Effect_Desc.bIsFilp = false;
	Effect_Desc.vPos = _float3(15.f, 2.5f, 6.f);
	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

	Effect_Desc.bIsFilp = false;
	Effect_Desc.vPos = _float3(15.5f, 2.5f, 10.f);
	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

	Effect_Desc.bIsFilp = true;
	Effect_Desc.vPos = _float3(4.f, 2.5f, 11.f);
	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
	
	Effect_Desc.bIsFilp = true;
	Effect_Desc.vPos = _float3(4.f, 2.5f, 3.f);
	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
	

	Effect_Desc.bIsFilp = false;
	Effect_Desc.vPos = _float3(19.5f, 2.5f, 3.f);
	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

	Effect_Desc.bIsFilp = false;
	Effect_Desc.vPos = _float3(19.f, 2.5f, 11.f);
	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

	m_fEffect_Delay_Timer = 0.f;
	m_fCameraTimer = 0.f;
	m_fZoom = 0.f;
	m_bCameraZoom = true;
	m_bCameraZoom2 = true;
	m_bCameraZoom3 = true;
	static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to(m_pHekat);

	return S_OK;
}

void CState_HekatTH::Update(_float fTimeDelta)
{
	m_fEffect_Delay_Timer += fTimeDelta;

	if (m_pHekat->Get_CurrentFrame() >= 100.f
		&& m_pHekat->Get_CurrentFrame() <= 149.f)
	{
		static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(0.01f, 0.1f, 1);
	}

	if(m_pHekat->Get_CurrentFrame()>=150.f
		&&m_pHekat->Get_CurrentFrame()<=208.f)
	{
		if (m_bCameraZoom)
		{

			static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_In(0.4f);
			m_bCameraZoom = false;
		}
			static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(0.1f, 0.3f, 1);

	}

	if (m_pHekat->Get_CurrentFrame() >= 209.f
		&& m_pHekat->Get_CurrentFrame() <= 341.f)
	{
		if (m_bCameraZoom2)
		{

			static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_In(0.8f);
			m_bCameraZoom2 = false;
		}
		static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(0.2f, 0.5f, 2);
	}


	if (m_pHekat->Get_CurrentFrame() >= 342.f)
	{
		if (m_bCameraZoom3)
		{

			static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_In(1.3f);
			m_bCameraZoom3 = false;
		}
		static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(0.5f, 0.8f, 3);
	}

	if (m_pHekat->IsEnd_CurrentAnimation())
		m_pHekat->Change_State(CBoss_Hekat::TRANS_FROG);

	if (m_fEffect_Delay_Timer > 1.5f)
	{
		m_fEffect_Timer += fTimeDelta;

		if(m_fEffect_Timer >0.2f)
		{
			CEffect::EFFECT_DESC Effect_Desc_Heart = {};
			Effect_Desc_Heart.fEffectDura = 2.f;
			Effect_Desc_Heart.iLevelIndex = LEVEL_DUNGEON2;
			Effect_Desc_Heart.bIsFilp = false;
			Effect_Desc_Heart.fJumpPower = 2.f;
			Effect_Desc_Heart.vDir = m_pHekat->Get_Position() - _float3(5.f, 5.f, 0.1f);
			D3DXVec3Normalize(&Effect_Desc_Heart.vDir, &Effect_Desc_Heart.vDir);
			Effect_Desc_Heart.strTextureTag = TEXT("Heart_Effect");
			Effect_Desc_Heart.iStageIndex = m_pGameInstance->Get_StageIndex();
			Effect_Desc_Heart.vScale = _float3(5.f, 5.f, 0.1f);
			/*Effect_Desc_Heart.vSpeed = _float3(2.f, 0.f, 2.f);
			Effect_Desc_Heart.vPos = _float3(12.f, 3.f, 12.f);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc_Heart);*/

			Effect_Desc_Heart.vPos = _float3(7.5f, 3.f, 10.f);
			Effect_Desc_Heart.vSpeed = _float3(2.5f, 0.f, 2.5f);
			Effect_Desc_Heart.fJumpPower = 2.f;
			Effect_Desc_Heart.vDir = m_pHekat->Get_Position() - _float3(7.5f, 3.f, 10.f);
			D3DXVec3Normalize(&Effect_Desc_Heart.vDir, &Effect_Desc_Heart.vDir);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc_Heart);

			Effect_Desc_Heart.vPos = _float3(8.f, 3.f, 6.f);
			Effect_Desc_Heart.vSpeed = _float3(2.5f, 0.f, 2.5f);
			Effect_Desc_Heart.fJumpPower = 2.f;
			Effect_Desc_Heart.vDir = m_pHekat->Get_Position() - _float3(8.f, 3.f, 6.f);
			D3DXVec3Normalize(&Effect_Desc_Heart.vDir, &Effect_Desc_Heart.vDir);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc_Heart);

			Effect_Desc_Heart.vPos = _float3(15.f, 3.f, 6.f);
			Effect_Desc_Heart.vSpeed = _float3(2.5f, 0.f, 2.5f);
			Effect_Desc_Heart.fJumpPower = 2.f;
			Effect_Desc_Heart.vDir = m_pHekat->Get_Position() - _float3(15.f, 3.f, 6.f);
			D3DXVec3Normalize(&Effect_Desc_Heart.vDir, &Effect_Desc_Heart.vDir);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc_Heart);

			Effect_Desc_Heart.vPos = _float3(15.5f, 3.f, 10.f);
			Effect_Desc_Heart.vSpeed = _float3(2.5f, 0.f, 2.5f);
			Effect_Desc_Heart.fJumpPower = 2.f;
			Effect_Desc_Heart.vDir = m_pHekat->Get_Position() - _float3(15.5f, 3.f, 10.f);
			D3DXVec3Normalize(&Effect_Desc_Heart.vDir, &Effect_Desc_Heart.vDir);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc_Heart);

			Effect_Desc_Heart.vPos = _float3(4.f, 3.f, 11.f);
			Effect_Desc_Heart.fEffectDura = 17.f;
			Effect_Desc_Heart.fJumpPower = 6.f;
			Effect_Desc_Heart.vSpeed = _float3(4.f, 0.f, 4.f);
			Effect_Desc_Heart.vDir = m_pHekat->Get_Position() - _float3(4.f, 3.f, 11.f);
			D3DXVec3Normalize(&Effect_Desc_Heart.vDir, &Effect_Desc_Heart.vDir);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc_Heart);

			Effect_Desc_Heart.vPos = _float3(4.f, 3.f, 3.f);
			Effect_Desc_Heart.fEffectDura = 17.f;
			Effect_Desc_Heart.fJumpPower = 6.f;
			Effect_Desc_Heart.vSpeed = _float3(4.f, 0.f, 4.f);
			Effect_Desc_Heart.vDir = m_pHekat->Get_Position() - _float3(4.f, 3.f, 3.f);
			D3DXVec3Normalize(&Effect_Desc_Heart.vDir, &Effect_Desc_Heart.vDir);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc_Heart);

			Effect_Desc_Heart.vPos = _float3(19.5f, 3.f, 3.f);
			Effect_Desc_Heart.fEffectDura = 17.f;
			Effect_Desc_Heart.fJumpPower = 6.f;
			Effect_Desc_Heart.vSpeed = _float3(4.f, 0.f, 4.f);
			Effect_Desc_Heart.vDir = m_pHekat->Get_Position() - _float3(19.5f, 3.f, 3.f);
			D3DXVec3Normalize(&Effect_Desc_Heart.vDir, &Effect_Desc_Heart.vDir);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc_Heart);

			Effect_Desc_Heart.vPos = _float3(19.f, 3.f, 11.f);
			Effect_Desc_Heart.fEffectDura = 17.f;
			Effect_Desc_Heart.fJumpPower = 6.f;
			Effect_Desc_Heart.vSpeed = _float3(4.f, 0.f, 4.f);
			Effect_Desc_Heart.vDir = m_pHekat->Get_Position() - _float3(19.f, 3.f, 11.f);
			D3DXVec3Normalize(&Effect_Desc_Heart.vDir, &Effect_Desc_Heart.vDir);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON2, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc_Heart);

			m_fEffect_Timer = 0.f;
		}
	
	}
}

void CState_HekatTH::End_State()
{
}

CState_HekatTH* CState_HekatTH::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_HekatTH* pInstance = new CState_HekatTH(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_HekatTH"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_HekatTH::Free()
{
	__super::Free();
}

