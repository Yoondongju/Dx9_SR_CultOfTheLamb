#include "State_EligorTF.h"
#include "Boss_Eligor.h"
#include "GameInstance.h"
#include "Effect.h"
#include <Camera.h>
#include <Player.h>

#include "Layer.h"

CState_EligorTF::CState_EligorTF(CFsm* pFsm)
    :CState(pFsm)
{
}

HRESULT CState_EligorTF::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_EligorTF::Start_State()
{
	m_pEligor = static_cast<CBoss_Eligor*>(m_pFsm->Get_Owner());
	dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to(m_pEligor,_float3(0.f,1.f,0.f));
	dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_In(1.6f);
	dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Player_State(CPlayer::CAMERA_ON);
	m_pEligor->Change_Animation(TEXT("Transform_Follower"));
	m_pEligor->Set_Eilgor_State(CBoss_Eligor::TRANS_F);
	m_pGameInstance->Play_Sound(TEXT("Elligos_Dead.wav"), SOUND_ELIGOR, 3.f);
	m_bDeadOnce = true;
    return S_OK;
}

void CState_EligorTF::Update(_float fTimeDelta)
{

	if (m_bDeadOnce)
	{
		CEffect::EFFECT_DESC Effect_Desc = {};
		Effect_Desc.vPos = m_pEligor->Get_Position();
		Effect_Desc.vPos.y = m_pEligor->Get_Position().y+1.f;
		Effect_Desc.vScale = _float3(5.f, 5.f, 0.1f);
		Effect_Desc.fEffectDura = 1.5f;
		Effect_Desc.strTextureTag = TEXT("Boss_Dead");
		Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
		Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
		m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

		m_bDeadOnce = false;
	}

	if (dynamic_cast<CAnimator*>(m_pEligor->Find_Component(TEXT("Com_Anim")))->Get_CurrentFrame() == 28)
	{
		if (m_pGameInstance->Check_IsPlaying(SOUND_EFFECT) == false)
		{
			m_pGameInstance->Play_Sound(TEXT("Follower_Ah6.wav"), SOUND_FOLLOWER, g_fEffectVolume);
		}
	}

	if (m_pEligor->IsEnd_CurrentAnimation())
	{
		dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_Out(1.6f);
		m_pEligor->Change_State(CBoss_Eligor::UNCONVERTED);
	}
}

void CState_EligorTF::End_State()
{
}

CState_EligorTF* CState_EligorTF::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_EligorTF* pInstance = new CState_EligorTF(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_EligorTF"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_EligorTF::Free()
{
	__super::Free();
}
