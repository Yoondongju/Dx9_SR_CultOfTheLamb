#include "State_HekatTFollower.h"
#include "Boss_Hekat.h"
#include "GameInstance.h"
#include "Effect.h"

CState_HekatTFollower::CState_HekatTFollower(CFsm* pFsm)
	:CState(pFsm)
{
}

HRESULT CState_HekatTFollower::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_HekatTFollower::Start_State()
{
	m_pHekat = static_cast<CBoss_Hekat*>(m_pFsm->Get_Owner());
	m_pHekat->Change_Animation(TEXT("Transform_Follower"));
	m_pHekat->Set_Hekat_State(CBoss_Hekat::TRANS_FOLLOWER);
	m_bDeadOnce = true;
	return S_OK;
}

void CState_HekatTFollower::Update(_float fTimeDelta)
{
	if (m_bDeadOnce)
	{
		CEffect::EFFECT_DESC Effect_Desc = {};
		Effect_Desc.vPos = m_pHekat->Get_Position();
		Effect_Desc.vPos.y = m_pHekat->Get_Position().y + 1.f;
		Effect_Desc.vScale = _float3(5.f, 5.f, 0.1f);
		Effect_Desc.fEffectDura = 3.f;
		Effect_Desc.strTextureTag = TEXT("Boss_Dead");
		Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
		Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
		m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

		m_bDeadOnce = false;
	}


	if (m_pHekat->IsEnd_CurrentAnimation())
			m_pHekat->Change_State(CBoss_Hekat::UNCONVERTED);
}

void CState_HekatTFollower::End_State()
{
}

CState_HekatTFollower* CState_HekatTFollower::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_HekatTFollower* pInstance = new CState_HekatTFollower(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_HekatTFollower"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_HekatTFollower::Free()
{
	__super::Free();
}

