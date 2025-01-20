#include "State_GusionJump.h"
#include "Boss_Gusion.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

CState_GusionJump::CState_GusionJump(CFsm* pFsm)
	:CState(pFsm)
{
}

HRESULT CState_GusionJump::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_GusionJump::Start_State()
{
	m_pGusion = static_cast<CBoss_Gusion*>(m_pFsm->Get_Owner());
	m_pGusion->Set_Gusion_State(CBoss_Gusion::JUMP);
	m_eCurState = JUMP_START;
	m_iJumpCount = 0;
	m_vDistance = { 0.f,0.f,0.f };
	m_fDistanceCheck = 0.f;
	return S_OK;
}

void CState_GusionJump::Update(_float fTimeDelta)
{
	if (m_pGusion->Get_MonInfo().iHp <= 0.f)
	{
		m_pGusion->Change_State(CBoss_Gusion::DIE);
	}

	m_vDir = m_pGameInstance->Find_Player()->Get_Position() - m_pGusion->Get_Position();
	D3DXVec3Normalize(&m_vDir, &m_vDir);

	Check_State(fTimeDelta);
}

void CState_GusionJump::End_State()
{
}

void CState_GusionJump::Check_State(_float fTimeDelta)
{
	Check_State_Init();

	switch (m_eCurState)
	{
	case Client::CState_GusionJump::JUMP_START:
		Jump_Start(fTimeDelta);
		break;
	case Client::CState_GusionJump::JUMP_END:
		Jump_End(fTimeDelta);
		break;
	}
}

void CState_GusionJump::Check_State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Client::CState_GusionJump::JUMP_START:
			Jump_Start_Init();
			break;
		case Client::CState_GusionJump::JUMP_END:
			Jump_End_Init();
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CState_GusionJump::Jump_Start(_float fTimeDelta)
{
	if(m_pGusion->Get_CurrentFrame()>=23)
	{
		m_pGusion->Get_Transform()->Move_Dir(fTimeDelta, m_vDir);
	}

	if (m_pGusion->IsEnd_CurrentAnimation())
		m_eCurState = JUMP_END;
}

void CState_GusionJump::Jump_End(_float fTimeDelta)
{
	_float3 vDistance = m_pGameInstance->Find_Player()->Get_Position() - m_pGusion->Get_Position();
	m_fDistanceCheck = D3DXVec3Length(&vDistance);

	if (m_pGusion->IsEnd_CurrentAnimation()
		|| m_iJumpCount >= 4
		|| m_fDistanceCheck <= 5.f)
		m_pGusion->Change_State(CBoss_Gusion::IDLE);

	else
		m_eCurState = JUMP_START;
}

void CState_GusionJump::Jump_Start_Init()
{
	m_pGameInstance->Play_Sound(TEXT("Gusion_Jump_Start.wav"), SOUND_GUSION, 2.f);
	m_pGusion->Change_Animation(TEXT("Jump_Start"));
}

void CState_GusionJump::Jump_End_Init()
{
	m_pGameInstance->Play_Sound(TEXT("Gusion_Jump_End.wav"), SOUND_GUSION, g_fEffectVolume);
	m_pGusion->Change_Animation(TEXT("Jump_End"));
}

CState_GusionJump* CState_GusionJump::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_GusionJump* pInstance = new CState_GusionJump(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_GusionJump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_GusionJump::Free()
{
	__super::Free();
}
