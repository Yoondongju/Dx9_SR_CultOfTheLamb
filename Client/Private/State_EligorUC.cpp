#include "State_EligorUC.h"
#include "Boss_Eligor.h"
#include "GameInstance.h"
#include <Camera.h>
#include <Player.h>
#include <PressEtoPickCard.h>

#include "Quest.h"

CState_EligorUC::CState_EligorUC(CFsm* pFsm)
    :CState(pFsm)
{
}

HRESULT CState_EligorUC::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_EligorUC::Start_State()
{
	m_pEligor = static_cast<CBoss_Eligor*>(m_pFsm->Get_Owner());
	m_pEligor->Change_Animation(TEXT("Unconverted"));
	dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to_Player();
	m_pEligor->Set_Eilgor_State(CBoss_Eligor::UNCONVERTED);

	CPressEtoPickCard::BTNE_DESC B_Desc = {};
	B_Desc.iLevelIndex = (int)LEVEL_DUNGEON1;
	B_Desc.iType = 1;

	if (FAILED(m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON1, TEXT("Layer_ConvertUI"), TEXT("Prototype_GameObject_UI_PressEtoPickCard"), sizeof(CPressEtoPickCard::BTNE_DESC), &B_Desc)))
		MSG_BOX(L"Can't Create GuideUI");

	
	CQuest::Get_Instance()->Succeed_Quest(L"적을 신도로 영입하기");
    return S_OK;
}

void CState_EligorUC::Update(_float fTimeDelta)
{
	if(dynamic_cast<CAnimator*>(m_pEligor->Find_Component(TEXT("Com_Anim")))->Get_AnimationTag().compare(TEXT("Unconverted")) == 0)
		m_pGameInstance->Play_SoundRepeat(TEXT("Follower_Cry.wav"), SOUND_FOLLOWER, g_fEffectVolume);

	if (m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP && dynamic_cast<CAnimator*>(m_pEligor->Find_Component(TEXT("Com_Anim")))->Get_AnimationTag().compare(TEXT("Unconverted")) == 0)
	{
		m_pGameInstance->StopSound(SOUND_FOLLOWER);
	//	m_pGameInstance->Play_Sound(TEXT("Follower_Ah1.wav"), SOUND_FOLLOWER, 3.f);
		dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to(m_pEligor);
		dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_In(1.4f);
		dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Player_State(CPlayer::CAMERA_ON);

		m_pEligor->Change_Animation(TEXT("Converted"));

		CGameObject* pGameObj = m_pGameInstance->Find_Object(LEVEL_DUNGEON1, TEXT("Layer_ConvertUI"), 0);
		if (nullptr != pGameObj)
		{
			if (FAILED(m_pGameInstance->Event_DestoryObject(pGameObj)))
				MSG_BOX(L"Can't Delete ConvertUI");
		}
	}

	if (dynamic_cast<CAnimator*>(m_pEligor->Find_Component(TEXT("Com_Anim")))->Get_AnimationTag().compare(TEXT("Converted")) == 0)
	{
		
		if(dynamic_cast<CAnimator*>(m_pEligor->Find_Component(TEXT("Com_Anim"))) ->Get_CurrentFrame() == 3 )
		{
			if(m_pGameInstance->Check_IsPlaying(SOUND_FOLLOWER) == false)
				m_pGameInstance->Play_Sound(TEXT("Follower_Surprised.wav"), SOUND_FOLLOWER, g_fEffectVolume);
		}

		if(dynamic_cast<CAnimator*>(m_pEligor->Find_Component(TEXT("Com_Anim"))) ->Get_CurrentFrame() == 46 )
		{
			if (m_pGameInstance->Check_IsPlaying(SOUND_FOLLOWER) == false)
				m_pGameInstance->Play_Sound(TEXT("Follower_Ah5.wav"), SOUND_FOLLOWER, g_fEffectVolume);
		}

		if (m_pEligor->IsEnd_CurrentAnimation())
		{

			m_pGameInstance->StopSound(SOUND_FOLLOWER);
			dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_Out(1.3f);
			dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to_Player();
			dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Kill_Eligos(true);
			m_pGameInstance->Event_DestoryObject(m_pEligor);

		}
	}
}

void CState_EligorUC::End_State()
{
}

CState_EligorUC* CState_EligorUC::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_EligorUC* pInstance = new CState_EligorUC(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_EligorUC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_EligorUC::Free()
{
	__super::Free();
}
