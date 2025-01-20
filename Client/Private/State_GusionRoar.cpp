#include "State_GusionRoar.h"
#include "Boss_Gusion.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

#include "Camera.h"
#include "MonologueUI.h"
#include "Effect.h"

CState_GusionRoar::CState_GusionRoar(class CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_GusionRoar::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_GusionRoar::Start_State()
{
	m_pGusion = static_cast<CBoss_Gusion*>(m_pFsm->Get_Owner());

	m_pGusion->Change_Animation(TEXT("Roar"));
	m_pGusion->Set_Gusion_State(CBoss_Gusion::ROAR);
	m_pGameInstance->Play_Sound(TEXT("Gusion_Roar.wav"), SOUND_GUSION, 0.8f);
	static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to(m_pGusion);
	static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_In(1.8f);
	m_bCameraEnd = true;
	m_bEffectOnce = true;
	return S_OK;
}


void CState_GusionRoar::Update(_float fTimeDelta)
{
	
	if (m_pGusion->Get_CurrentFrame() >= 19.f
		&& m_pGusion->Get_CurrentFrame() <= 55.f)
	{
		static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(0.7f, 0.8f, 2);

		if (m_bEffectOnce)
		{
			CEffect::EFFECT_DESC Effect_Desc = {};
			Effect_Desc.fEffectDura = 1.f;
			Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
			Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
			Effect_Desc.strTextureTag = TEXT("HitEffect_Circle");
			Effect_Desc.vPos = m_pGusion->Get_Position();
			Effect_Desc.vPos.y = m_pGusion->Get_Position().y + 0.3f;
			Effect_Desc.vPos.z = m_pGusion->Get_Position().z - 0.1f;
			Effect_Desc.vScale = _float3(10.f, 10.f, 0.1f);
			m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
		
			m_bEffectOnce = false;
		}
	}

	if (m_pGusion->Get_CurrentFrame() >= 56.f
		&& m_bCameraEnd)
	{
		static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_Out(1.7f);
		static_cast<CCamera*> (m_pGameInstance->Find_Camera())->Target_to_Player();

		CMonologueUI::MONOLOGUE_DESC Desc = {};
		Desc.strText = TEXT("±¸½Ã¿Ë");
		Desc.rt = {
						(LONG)(g_iWinSizeX / 2.f - (Desc.strText.size() * 17.f)),
						(LONG)(g_iWinSizeY / 2.f),
						(LONG)(g_iWinSizeX / 2.f + 300.f),
						(LONG)(g_iWinSizeY / 2.f + 100.f)
		};

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonologueUI"), &Desc)))
			MSG_BOX(TEXT("Can't Create MonologueUI"));

		m_bCameraEnd = false;
	}

	if (m_pGusion->IsEnd_CurrentAnimation())
		m_pGusion->Change_State(CBoss_Gusion::IDLE);
}

void CState_GusionRoar::End_State()
{
}

CState_GusionRoar* CState_GusionRoar::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_GusionRoar* pInstance = new CState_GusionRoar(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_GusionRoar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_GusionRoar::Free()
{
	__super::Free();
}
