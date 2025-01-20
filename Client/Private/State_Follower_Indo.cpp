#include "State_Follower_Indo.h"
#include "GameInstance.h"
#include "Follower.h"
#include <PressEtoPickCard.h>
#include <Camera.h>
#include <Player.h>

CState_Follower_Indo::CState_Follower_Indo(CFsm* pFsm)
    :CState{ pFsm }
{
}

HRESULT CState_Follower_Indo::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;
    return S_OK;
}

HRESULT CState_Follower_Indo::Start_State()
{
    m_pFollower = dynamic_cast<CFollower*>(m_pFsm->Get_Owner());
    m_pFollower->Change_Animation(TEXT("INDO_LOOP"));
    static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to(m_pFollower,_float3(0.f,0.f,0.f), 40.f);
    static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_In(1.3f);

    return S_OK;
}

void CState_Follower_Indo::Update(_float fTimeDelta)
{
    if(m_bStopTimer == false)
        m_fTimer += fTimeDelta;

    if(m_fTimer > 2.f)
    {
        static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to_Player();
        static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_Out(1.3f);
        m_fTimer = 0.f;
        m_bStopTimer = true;
    }
      
    _float3 vDistance = m_pGameInstance->Find_Player()->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pFollower->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _float fDistance = D3DXVec3Length(&vDistance);

    if (fDistance < 3.f)
    {
     

        if(dynamic_cast<CAnimator*>(m_pFollower->Find_Component(TEXT("Com_Animation")))->Get_AnimationTag().compare(TEXT("INDO_END")) != 0)
        {
            m_pFollower->Change_Animation(TEXT("INDO_END"));
            dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to(m_pFollower);
            dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_In(1.4f);
            dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::SERMON);
        }
    }

    if (dynamic_cast<CAnimator*>(m_pFollower->Find_Component(TEXT("Com_Animation")))->Get_AnimationTag().compare(TEXT("INDO_END")) == 0
        && m_pFollower->IsEnd_CurrentAnimation())
    {
        dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to_Player();
        dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_Out(1.4f);
        m_pFollower->Set_isIndo(true) ;
        m_pFollower->Change_State(CFollower::IDLE);
    }

    Set_Sound();
}

void CState_Follower_Indo::End_State()
{
}

void CState_Follower_Indo::Set_Sound()
{
    if (dynamic_cast<CAnimator*>(m_pFollower->Find_Component(TEXT("Com_Animation")))->Get_AnimationTag().compare(TEXT("INDO_END")) == 0)
    {
        if(m_pGameInstance->Check_IsPlaying(SOUND_EFFECT) == false)
        {
            if (dynamic_cast<CAnimator*>(m_pFollower->Find_Component(TEXT("Com_Animation")))->Get_CurrentFrame() == 20)
                m_pGameInstance->Play_Sound(TEXT("Follower_Pop_in.wav"), SOUND_EFFECT, g_fEffectVolume);
        }

        if( m_pGameInstance->Check_IsPlaying(SOUND_FOLLOWER) == false)
        {
            if (dynamic_cast<CAnimator*>(m_pFollower->Find_Component(TEXT("Com_Animation")))->Get_CurrentFrame() == 38)
                m_pGameInstance->Play_Sound(TEXT("Follower_Finish_Convert.wav"), SOUND_FOLLOWER, g_fEffectVolume);
        }
    }
}

CState_Follower_Indo* CState_Follower_Indo::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_Follower_Indo* pInstance = new CState_Follower_Indo(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Follower_Indo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Follower_Indo::Free()
{
	__super::Free();
}
