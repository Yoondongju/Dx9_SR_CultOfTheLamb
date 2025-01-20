#include "State_PlayerCard.h"
#include "Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

CState_PlayerCard::CState_PlayerCard(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_PlayerCard::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_PlayerCard::Start_State()
{
	m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
	// Safe_AddRef(m_pPlayer);
	m_eCurState = CARD_START;
	m_pPlayer->Set_Player_State(CPlayer::CARD);
    return S_OK;
}

void CState_PlayerCard::Update(_float fTimeDelta)
{
	Check_State();
}

void CState_PlayerCard::End_State()
{
}

void CState_PlayerCard::Check_State()
{
	Check_State_Init();
	switch (m_eCurState)
	{
	case Client::CState_PlayerCard::CARD_START:
		Card_Start();
		break;
	case Client::CState_PlayerCard::CARD_LOOP:
		Card_Loop();
		break;
	case Client::CState_PlayerCard::CARD_END:
		Card_End();
		break;
	}
}

void CState_PlayerCard::Check_State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Client::CState_PlayerCard::CARD_START:
			Card_Start_Init();
			break;
		case Client::CState_PlayerCard::CARD_LOOP:
			Card_Loop_Init();
			break;
		case Client::CState_PlayerCard::CARD_END:
			Card_End_Init();
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CState_PlayerCard::Card_Start()
{
	if (m_pPlayer->IsEnd_CurrentAnimation())
		m_eCurState = CARD_LOOP;
}

void CState_PlayerCard::Card_Loop()
{
	if (m_pPlayer->IsEnd_CurrentAnimation())
		m_eCurState = CARD_END;
}

void CState_PlayerCard::Card_End()
{
	if (m_pPlayer->IsEnd_CurrentAnimation())
		m_pPlayer->Change_State(CPlayer::IDLE);
}

void CState_PlayerCard::Card_Start_Init()
{
	m_pPlayer->Change_Animation(TEXT("Card_Start"));
}

void CState_PlayerCard::Card_Loop_Init()
{
	m_pPlayer->Change_Animation(TEXT("Card_Loop"));
}

void CState_PlayerCard::Card_End_Init()
{
	m_pPlayer->Change_Animation(TEXT("Card_End"));
}

CState_PlayerCard* CState_PlayerCard::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_PlayerCard* pInstance = new CState_PlayerCard(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_PlayerCard"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_PlayerCard::Free()
{
	__super::Free();
}
