#include "State_HekatDie.h"
#include "Boss_Hekat.h"
#include "GameInstance.h"

#include "Effect.h"

CState_HekatDie::CState_HekatDie(CFsm* pFsm)
    :CState(pFsm)
{
}

HRESULT CState_HekatDie::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_HekatDie::Start_State()
{
    m_pHekat = static_cast<CBoss_Hekat*>(m_pFsm->Get_Owner());
    m_pHekat->Change_Animation(TEXT("Die"));
	m_pHekat->Set_Hekat_State(CBoss_Hekat::DIE);
	m_pGameInstance->Play_Sound(TEXT("Heket_Dead.wav"), SOUND_HEKAT, g_fEffectVolume);
	m_bDeadOnce = true;
    return S_OK;
}

void CState_HekatDie::Update(_float fTimeDelta)
{

	if (m_pHekat->Get_CurrentFrame()>=111.f
		&&m_bDeadOnce)
	{
		CEffect::EFFECT_DESC Effect_Desc = {};
		Effect_Desc.vPos = m_pHekat->Get_Position();
		Effect_Desc.vPos.y = m_pHekat->Get_Position().y+0.5f;
		Effect_Desc.vScale = _float3(10.f, 10.f, 0.1f);
		Effect_Desc.fEffectDura = 0.8f;
		Effect_Desc.strTextureTag = TEXT("After_Harvest");
		Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
		Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
		m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

		m_bDeadOnce = false;
	}

	if (m_pHekat->IsEnd_CurrentAnimation()
		&&!m_bDeadOnce)
	{
		m_pGameInstance->Event_DestoryObject(m_pHekat);
	}
}

void CState_HekatDie::End_State()
{
}

CState_HekatDie* CState_HekatDie::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_HekatDie* pInstance = new CState_HekatDie(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_HekatDie"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_HekatDie::Free()
{
	__super::Free();
}
