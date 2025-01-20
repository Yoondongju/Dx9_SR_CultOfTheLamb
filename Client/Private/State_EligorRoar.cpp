#include "State_EligorRoar.h"
#include "Boss_Eligor.h"
#include "GameInstance.h"

#include "Camera.h"
#include "MonologueUI.h"

#include "Effect.h"

CState_EligorRoar::CState_EligorRoar(CFsm* pFsm)
	:CState(pFsm)
{
}

HRESULT CState_EligorRoar::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_EligorRoar::Start_State()
{
	m_pEligor = static_cast<CBoss_Eligor*>(m_pFsm->Get_Owner());
	m_pEligor->Change_Animation(TEXT("Roar"));
	m_pEligor->Set_Eilgor_State(CBoss_Eligor::ROAR);
	m_pGameInstance->Play_Sound(TEXT("Elligos_Roar.wav"), SOUND_ELIGOR, 0.8f);
	static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to(m_pEligor);
	static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_In(2.f);
	m_bCameraEnd = true;
	m_bEffectOnce = true;

	return S_OK;
}

void CState_EligorRoar::Update(_float fTimeDelta)
{
	if (m_pEligor->Get_CurrentFrame() >= 39.f
		&& m_pEligor->Get_CurrentFrame() <= 82.f)
	{
		static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(0.7f, 0.8f, 2);

		if (m_bEffectOnce)
		{
			CEffect::EFFECT_DESC Effect_Desc = {};
			Effect_Desc.fEffectDura = 1.f;
			Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
			Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
			Effect_Desc.strTextureTag = TEXT("HitEffect_Circle");
			Effect_Desc.vPos = m_pEligor->Get_Position();
			Effect_Desc.vPos.y = m_pEligor->Get_Position().y + 0.3f;
			Effect_Desc.vPos.z = m_pEligor->Get_Position().z - 0.1f;
			Effect_Desc.vScale = _float3(10.f, 10.f, 0.1f);
			m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

			m_bEffectOnce = false;
		}

	}

	if (m_pEligor->Get_CurrentFrame() >= 83.f
		&& m_bCameraEnd)
	{
		static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_Out(1.9f);
		static_cast<CCamera*> (m_pGameInstance->Find_Camera())->Target_to_Player();

		CMonologueUI::MONOLOGUE_DESC Desc = {};
		Desc.strText = TEXT("¿¤¸®°í½º");
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

	if (m_pEligor->IsEnd_CurrentAnimation())
		m_pEligor->Change_State(CBoss_Eligor::IDLE);
}

void CState_EligorRoar::End_State()
{
}

CState_EligorRoar* CState_EligorRoar::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_EligorRoar* pInstance = new CState_EligorRoar(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_EligorRoar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_EligorRoar::Free()
{
	__super::Free();
}
