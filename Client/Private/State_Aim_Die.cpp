#include "State_Aim_Die.h"
#include "Boss_Aim.h"

#include "GameInstance.h"

#include "Transform.h"
#include "Animator.h"

#include "TalkPanelUI.h"

#include "Boss_DeathCat.h"
#include <Camera.h>

CState_Aim_Die::CState_Aim_Die(class CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_Aim_Die::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Aim_Die::Start_State()
{
	m_pAim = dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner());
	Safe_AddRef(m_pAim);

	m_pAim->Change_Animation(TEXT("Die"), 0);

	return S_OK;
}


void CState_Aim_Die::Update(_float fTimeDelta)
{
	if (!m_pAim->Get_Dead() && m_pAim->IsEnd_CurrentAnimation())
	{
		m_pGameInstance->Event_DestoryObject(m_pAim);

		CBoss_DeathCat* pDeathCat = dynamic_cast<CBoss_DeathCat*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_LevelIndex(), L"Layer_Boss", 1));
		pDeathCat->Set_IsActive(true);
		

		CCamera* pCam = static_cast<CCamera*>(m_pGameInstance->Find_Camera());
		pCam->Target_to(pDeathCat);
		pCam->Zoom_In(2.f);
	}
		

}

void CState_Aim_Die::End_State()
{
}

CState_Aim_Die* CState_Aim_Die::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_Aim_Die* pInstance = new CState_Aim_Die(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Aim_Die"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Aim_Die::Free()
{
	__super::Free();

	Safe_Release(m_pAim);
}
