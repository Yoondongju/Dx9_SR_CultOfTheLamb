#include "State_HekatJump.h"
#include "Boss_Hekat.h"
#include "GameInstance.h"

CState_HekatJump::CState_HekatJump(CFsm* pFsm)
	:CState(pFsm)
{
}

HRESULT CState_HekatJump::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_HekatJump::Start_State()
{
	m_pHekat = static_cast<CBoss_Hekat*>(m_pFsm->Get_Owner());
	m_eCurState = JUMP_START;
	m_pHekat->Set_Hekat_State(CBoss_Hekat::JUMP);
	m_vDir = m_pGameInstance->Find_Player()->Get_Position() - m_pHekat->Get_Position();
	m_vPos = m_pHekat->Get_Position();
	D3DXVec3Normalize(&m_vDir, &m_vDir);
	return S_OK;
}

void CState_HekatJump::Update(_float fTimeDelta)
{
	Check_State(fTimeDelta);
}

void CState_HekatJump::End_State()
{
}

void CState_HekatJump::Check_State(_float fTimeDelta)
{
	Check_State_Init();

	switch (m_eCurState)
	{
	case Client::CState_HekatJump::JUMP_START:
		Jump_Start();
		break;
	case Client::CState_HekatJump::JUMP:
		Jump(fTimeDelta);
		break;
	case Client::CState_HekatJump::JUMP_END:
		Jump_End();
		break;
	}
}

void CState_HekatJump::Check_State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Client::CState_HekatJump::JUMP_START:
			Jump_Start_Init();
			break;
		case Client::CState_HekatJump::JUMP:
			Jump_Init();
			break;
		case Client::CState_HekatJump::JUMP_END:
			Jump_End_Init();
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CState_HekatJump::Jump_Start()
{
	if (m_pHekat->IsEnd_CurrentAnimation())
			m_eCurState = JUMP;
}

void CState_HekatJump::Jump(_float fTimeDelta)
{
	if (m_pHekat->Get_CurrentFrame() >= 7)
	{
		m_vPos.x += m_vDir.x* fTimeDelta*15.f;
		m_vPos.z += m_vDir.z* fTimeDelta*15.f;
		m_pHekat->Get_Transform()->Set_State(CTransform::STATE_POSITION, m_vPos);
	}

	if (m_pHekat->IsEnd_CurrentAnimation())
			m_eCurState = JUMP_END;
}

void CState_HekatJump::Jump_End()
{
	if (m_pHekat->IsEnd_CurrentAnimation())
			m_pHekat->Change_State(CBoss_Hekat::IDLE);
}

void CState_HekatJump::Jump_Start_Init()
{
	m_pHekat->Change_Animation(TEXT("Jump_Start"));
}

void CState_HekatJump::Jump_Init()
{
	m_pGameInstance->Play_Sound(TEXT("Gusion_Jump_Start.wav"), SOUND_HEKAT, g_fEffectVolume);
	m_pHekat->Change_Animation(TEXT("Jump"));
}

void CState_HekatJump::Jump_End_Init()
{
	m_pGameInstance->Play_Sound(TEXT("Gusion_Jump_End.wav"), SOUND_HEKAT, g_fEffectVolume);
	m_pHekat->Change_Animation(TEXT("Jump_End"));
}

CState_HekatJump* CState_HekatJump::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_HekatJump* pInstance = new CState_HekatJump(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_HekatJump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_HekatJump::Free()
{
	__super::Free();
}
