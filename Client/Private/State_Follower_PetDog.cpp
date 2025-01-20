#include "State_Follower_PetDog.h"
#include "GameInstance.h"
#include "Follower.h"

CState_Follower_PetDog::CState_Follower_PetDog(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_Follower_PetDog::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;
	return S_OK;
}

HRESULT CState_Follower_PetDog::Start_State()
{
	m_pFollower = dynamic_cast<CFollower*>(m_pFsm->Get_Owner());

	m_pFollower->Change_Animation(TEXT("PETDOG"));

	return S_OK;
}

void CState_Follower_PetDog::Update(_float fTimeDelta)
{

	if (m_pFollower->IsEnd_CurrentAnimation())
	{
		m_pFollower->Change_State(CFollower::WORSHIP);
	}
}

void CState_Follower_PetDog::End_State()
{
}

CState_Follower_PetDog* CState_Follower_PetDog::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_Follower_PetDog* pInstance = new CState_Follower_PetDog(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Follower_PetDog"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Follower_PetDog::Free()
{
	__super::Free();

}
