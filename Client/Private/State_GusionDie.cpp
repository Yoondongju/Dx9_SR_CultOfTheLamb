#include "State_GusionDie.h"
#include "Boss_Gusion.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Effect.h"

CState_GusionDie::CState_GusionDie(class CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_GusionDie::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_GusionDie::Start_State()
{
	m_pGusion = static_cast<CBoss_Gusion*>(m_pFsm->Get_Owner());

	m_pGusion->Change_Animation(TEXT("Die"));
	m_pGusion->Set_Gusion_State(CBoss_Gusion::DIE);
	m_pGameInstance->Play_Sound(TEXT("Gusion_Dead.wav"), SOUND_GUSION, 0.8f);

	return S_OK;
}


void CState_GusionDie::Update(_float fTimeDelta)
{

	if (m_pGusion->Get_CurrentFrame() >= 22.f)
	{
		_float3 vPos = m_pGusion->Get_Position();
		vPos.y += 1.5f;
		m_pGusion->Set_Position(vPos);
	}

	if (m_pGusion->IsEnd_CurrentAnimation())
	{
		m_pGameInstance->Event_DestoryObject(m_pGusion);
	}
}

void CState_GusionDie::End_State()
{
}

CState_GusionDie* CState_GusionDie::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_GusionDie* pInstance = new CState_GusionDie(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_GusionDie"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_GusionDie::Free()
{
	__super::Free();
}
