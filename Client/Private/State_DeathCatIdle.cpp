
#include "State_DeathCatIdle.h"
#include "Boss_DeathCat.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

#include "Boss_Aim.h"
#include "TalkPanelUI.h"

CState_DeatCatIdle::CState_DeatCatIdle(class CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_DeatCatIdle::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_DeatCatIdle::Start_State()
{
	m_pDeathCat = dynamic_cast<CBoss_DeathCat*>(m_pFsm->Get_Owner());

	m_pDeathCat->Change_Animation(TEXT("IDLE"), m_pDeathCat->Get_CurrentFrame());

	m_iChoiceAttack = rand() % 2;

	return S_OK;
} 
 

void CState_DeatCatIdle::Update(_float fTimeDelta)
{
	CTalkPanelUI* pTalkUI{ nullptr };

	pTalkUI = dynamic_cast<CTalkPanelUI*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_TalkUI"), 0));

	if (m_bCheck_DeadAim && !m_isFightStart
		&& pTalkUI != nullptr && pTalkUI->Get_TalkEnd())
	{
		CGameInstance::Get_Instance()->StopAll();
		CGameInstance::Get_Instance()->PlayBGM(TEXT("BGM_DeathCat.wav"), g_fBgmVolume);
		m_isFightStart = true;
	}

	// 에임이 죽어서 등장함
	if (!m_bCheck_DeadAim)
	{
		
		CBoss_Aim* pAim = dynamic_cast<CBoss_Aim*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_LevelIndex(), L"Layer_Boss", 0));
		if (nullptr == pAim)
		{
			
			pTalkUI->SetActivate(true);

			vector<_wstring>& vecSpeech = pTalkUI->Get_vecSpeech((LEVELID)m_pGameInstance->Get_LevelIndex());

			pTalkUI->Set_OpponentName(L"기다리는 자");
			
			vecSpeech.push_back(L"에임이 쓰러지다니...\n네가 이 정도의 실력을 가지고 있을 줄은 몰랐다.\n하지만 이곳에서 너의 여정은 끝이다.");
			vecSpeech.push_back(L"나의 힘을 직접 경험하게 해주지.");
 			vecSpeech.push_back(L"이곳에서 너의 이야기는 끝나고,\n나의 지배는 계속될 것이다.");

			m_bCheck_DeadAim = true;
		}
	}
	
	if (m_isFightStart)
	{
		if (CBoss_DeathCat::HURT == m_pFsm->Get_PrevState())
		{
			dynamic_cast<CBoss_DeathCat*>(m_pFsm->Get_Owner())->Change_State(CBoss_DeathCat::DISAPPEAR);
		}
		else if(m_iChoiceAttack)
			dynamic_cast<CBoss_DeathCat*>(m_pFsm->Get_Owner())->Change_State(CBoss_DeathCat::CHAINATTACK);
		else
			dynamic_cast<CBoss_DeathCat*>(m_pFsm->Get_Owner())->Change_State(CBoss_DeathCat::FLYBALLATTACK);
	}


	
}

void CState_DeatCatIdle::End_State()
{
}

CState_DeatCatIdle* CState_DeatCatIdle::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_DeatCatIdle* pInstance = new CState_DeatCatIdle(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_DeatCatIdle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_DeatCatIdle::Free()
{
	__super::Free();
}
