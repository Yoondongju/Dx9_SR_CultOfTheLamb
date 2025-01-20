#include "State_Follower_Pray.h"
#include "GameInstance.h"
#include "Follower.h"
#include <Camera.h>

#include "Quest.h"

_uint CState_Follower_Pray::m_iPrayCount = {};

CState_Follower_Pray::CState_Follower_Pray(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_Follower_Pray::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;
    return S_OK;
}

HRESULT CState_Follower_Pray::Start_State()
{
	m_pFollower = dynamic_cast<CFollower*>(m_pFsm->Get_Owner());

	m_pFollower->Change_Animation(TEXT("PRAY"));
	

	++m_iPrayCount;

	if(m_iPrayCount >= 4)
		CQuest::Get_Instance()->Succeed_Quest(L"모든 신도 기도시키기");

    return S_OK;
}

void CState_Follower_Pray::Update(_float fTimeDelta)
{

	//CUI* pUI = dynamic_cast<CUI*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Follower_Command_UI"), 0));
	//if ((m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP) && pUI->GetActivate())
	//{
	//	pUI->SetActivate(false);
	//	dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_Out(1.5f);
	//	dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to_Player();
	//}
}

void CState_Follower_Pray::End_State()
{
}

CState_Follower_Pray* CState_Follower_Pray::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_Follower_Pray* pInstance = new CState_Follower_Pray(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Follower_Pray"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Follower_Pray::Free()
{
    __super::Free();

}
