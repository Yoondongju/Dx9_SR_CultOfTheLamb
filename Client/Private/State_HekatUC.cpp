#include "State_HekatUC.h"
#include "Boss_Hekat.h"
#include "GameInstance.h"

CState_HekatUC::CState_HekatUC(CFsm* pFsm)
	:CState(pFsm)
{
}

HRESULT CState_HekatUC::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_HekatUC::Start_State()
{
	m_pHekat = static_cast<CBoss_Hekat*>(m_pFsm->Get_Owner());
	m_pHekat->Change_Animation(TEXT("Unconverted"));
	m_pHekat->Set_Hekat_State(CBoss_Hekat::UNCONVERTED);
	return S_OK;
}

void CState_HekatUC::Update(_float fTimeDelta)
{
	
}

void CState_HekatUC::End_State()
{
}

CState_HekatUC* CState_HekatUC::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_HekatUC* pInstance = new CState_HekatUC(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_HekatUC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_HekatUC::Free()
{
	__super::Free();
}

