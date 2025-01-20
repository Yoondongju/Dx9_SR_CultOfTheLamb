#include "State_Follower_Axe.h"
#include "Follower.h"
#include "GameInstance.h"
#include <Camera.h>
#include "InteractObject.h"
CState_Follower_Axe::CState_Follower_Axe(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_Follower_Axe::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;
	return S_OK;
}

HRESULT CState_Follower_Axe::Start_State()
{
	m_pFollower = dynamic_cast<CFollower*>(m_pFsm->Get_Owner());

	m_pFollower->Change_Animation(TEXT("AXE"));

	return S_OK;
}

void CState_Follower_Axe::Update(_float fTimeDelta)
{

	//CUI* pUI = dynamic_cast<CUI*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Follower_Command_UI"), 0));
	//if ((m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP) && pUI->GetActivate())
	//{
	//	pUI->SetActivate(false);
	//	dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_Out(1.5f);
	//	dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to_Player();
	//}

	if (m_pFollower->Get_CurrentFrame() == 25 && !m_isCrash)
	{
		m_isCrash = true;

		if (m_pFollower->Get_Interactive_Object() != nullptr)
		{
			m_pFollower->Get_Interactive_Object()->Set_IsShake(true);
			if (!m_pFollower->Get_Interactive_Object()->Crash_InterectObject())
			{
				m_pFollower->Set_Interactive_Object(nullptr);
				m_pFollower->Change_State(CFollower::WALK);
			}
		}
	}
	else if (m_pFollower->Get_CurrentFrame() == 43 && !m_isCrash)
	{
		m_isCrash = true;

		if (m_pFollower->Get_Interactive_Object() != nullptr)
		{
			m_pFollower->Get_Interactive_Object()->Set_IsShake(true);
			if (!m_pFollower->Get_Interactive_Object()->Crash_InterectObject())
			{
				m_pFollower->Set_Interactive_Object(nullptr);
				m_pFollower->Change_State(CFollower::WALK);
			}
		}
	}
	else
	{
		m_isCrash = false;
	}
}

void CState_Follower_Axe::End_State()
{
}

CState_Follower_Axe* CState_Follower_Axe::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_Follower_Axe* pInstance = new CState_Follower_Axe(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Follower_Axe"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Follower_Axe::Free()
{
	__super::Free();

}
