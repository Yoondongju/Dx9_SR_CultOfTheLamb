
#include "State_EyeDie.h"
#include "Boss_DeathCat2.h"
#include "Boss_Eye.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

CState_EyeDie::CState_EyeDie(class CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_EyeDie::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_EyeDie::Start_State()
{
	m_pEye = dynamic_cast<CBoss_Eye*>(m_pFsm->Get_Owner());

	m_pEye->Change_Animation(TEXT("Die"), 0);

	return S_OK;
}


void CState_EyeDie::Update(_float fTimeDelta)
{
	if (m_pEye->IsEnd_CurrentAnimation())
	{
		m_pGameInstance->Event_DestoryObject(m_pEye);
	}


}

void CState_EyeDie::End_State()
{
}

CState_EyeDie* CState_EyeDie::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_EyeDie* pInstance = new CState_EyeDie(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_EyeDie"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_EyeDie::Free()
{
	__super::Free();
}
