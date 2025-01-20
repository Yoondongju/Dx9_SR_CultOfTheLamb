
#include "State_Player_Tarot.h"
#include "Player.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

#include "Effect.h"

CState_Player_Tarot::CState_Player_Tarot(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_Player_Tarot::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;
	return S_OK;
}

HRESULT CState_Player_Tarot::Start_State()
{
	m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());

	m_pPlayer->Change_Animation(TEXT("Card_Start"));
	m_pPlayer->Set_Player_State(CPlayer::TAROT);
	m_bEffectOnce = true;
	return S_OK;
}

void CState_Player_Tarot::Update(_float fTimeDelta)
{
	_wstring AniTag = m_pPlayer->Get_Player_Anim()->Get_AnimationTag();

	if (AniTag.compare(TEXT("Card_Start")) == 0)
	{
		if(m_pPlayer->IsEnd_CurrentAnimation())
			m_pPlayer->Change_Animation(TEXT("Card_Loop"));
	}

	if (m_pPlayer->Get_PickCard() == true && AniTag.compare(TEXT("Card_End")) != 0)
	{
		m_pPlayer->Change_Animation(TEXT("Card_End"));
	}
	
	if (AniTag.compare(TEXT("Card_End")) == 0)
	{
		if (m_pPlayer->Get_CurrentFrame() >= 28
			&& m_bEffectOnce)
		{
			CEffect::EFFECT_DESC Effect_Desc = {};
			Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
			Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
			Effect_Desc.vPos = m_pPlayer->Get_Position();
			Effect_Desc.vPos.x = m_pPlayer->Get_Position().x - 0.3f;
			Effect_Desc.vPos.y = m_pPlayer->Get_Position().y - 0.5f;
			Effect_Desc.fEffectDura = 1.f;
			Effect_Desc.strTextureTag = TEXT("StrengthBuff");
			Effect_Desc.vScale = _float3(3.5f, 5.f, 0.1f);

			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

			m_bEffectOnce = false;
		}

		if (m_pPlayer->IsEnd_CurrentAnimation())
			m_pPlayer->Change_State(CPlayer::IDLE);
	}	
}

void CState_Player_Tarot::End_State()
{

}

CState_Player_Tarot* CState_Player_Tarot::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_Player_Tarot* pInstance = new CState_Player_Tarot(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Player_Tarot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Player_Tarot::Free()
{
	__super::Free();

}
