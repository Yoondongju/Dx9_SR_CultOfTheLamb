#include "State_Aim_Idle.h"
#include "Boss_Aim.h"

#include "GameInstance.h"

#include "Transform.h"
#include "Animator.h"

#include "TalkPanelUI.h"

CState_Aim_Idle::CState_Aim_Idle(class CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_Aim_Idle::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Aim_Idle::Start_State()
{
	m_pAim = dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner());
	

	m_pAim->Change_Animation(TEXT("Idle"), m_pAim->Get_CurrentFrame());

	return S_OK;
}


void CState_Aim_Idle::Update(_float fTimeDelta)
{
	CTalkPanelUI* pUI = dynamic_cast<CTalkPanelUI*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_LevelIndex(), L"Layer_TalkUI", 0));

	if(nullptr != pUI  && !pUI->GetActivate())
		dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner())->Change_State(CBoss_Aim::AIM_STATE::IDLE_TO_ATTACK);

}

void CState_Aim_Idle::End_State()
{
	m_pGameInstance->StopAll();
	m_pGameInstance->PlayBGM(TEXT("BGM_Aim.wav"), g_fBgmVolume);
}

CState_Aim_Idle* CState_Aim_Idle::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_Aim_Idle* pInstance = new CState_Aim_Idle(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Aim_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Aim_Idle::Free()
{
	__super::Free();


}
