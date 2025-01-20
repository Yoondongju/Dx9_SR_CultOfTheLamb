#include "State_Follower_Idle.h"
#include "GameInstance.h"
#include "Follower.h"

CState_Follower_Idle::CState_Follower_Idle(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_Follower_Idle::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;
    return S_OK;
}

HRESULT CState_Follower_Idle::Start_State()
{
	m_pFollower = dynamic_cast<CFollower*>(m_pFsm->Get_Owner());

	m_pFollower->Change_Animation(TEXT("IDLE"));

    return S_OK;
}

void CState_Follower_Idle::Update(_float fTimeDelta)
{
	//CUI* pUI = dynamic_cast<CUI*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Follower_Command_UI"), 0));
	//if(pUI != nullptr && pUI->GetActivate())
	//{
	//	m_pFollower->Change_State(CFollower::WORSHIP);
	//}
}

void CState_Follower_Idle::End_State()
{
}

CState_Follower_Idle* CState_Follower_Idle::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_Follower_Idle* pInstance = new CState_Follower_Idle(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Follower_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Follower_Idle::Free()
{
    __super::Free();

}
