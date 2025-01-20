#include "State_Follower_Mining.h"
#include "GameInstance.h"
#include "Follower.h"
#include <Camera.h>
#include "InteractObject.h"

CState_Follower_Mining::CState_Follower_Mining(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_Follower_Mining::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;
    return S_OK;
}

HRESULT CState_Follower_Mining::Start_State()
{
	m_pFollower = dynamic_cast<CFollower*>(m_pFsm->Get_Owner());

	m_pFollower->Change_Animation(TEXT("MINING"));
	
    return S_OK;
}

void CState_Follower_Mining::Update(_float fTimeDelta)
{

	//CUI* pUI = dynamic_cast<CUI*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Follower_Command_UI"), 0));
	//if ((m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP) && pUI->GetActivate())
	//{
	//	pUI->SetActivate(false);
	//	dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_Out(1.5f);
	//	dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to_Player();
	//}

	if (m_pFollower->Get_CurrentFrame() == 18 && !m_isCrash)
	{
		m_isCrash = true;

		m_pFollower->Get_Interactive_Object()->Set_IsShake(true);
		if (!m_pFollower->Get_Interactive_Object()->Crash_InterectObject())
		{
			m_pFollower->Set_Interactive_Object(nullptr);
			m_pFollower->Change_State(CFollower::WALK);
		}
	}
	else
	{
		m_isCrash = false;
	}
}

void CState_Follower_Mining::End_State()
{
}

CState_Follower_Mining* CState_Follower_Mining::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_Follower_Mining* pInstance = new CState_Follower_Mining(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Follower_Mining"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Follower_Mining::Free()
{
    __super::Free();

}
