#include "State_PlayerGS.h"
#include "Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

#include "Effect.h"

CState_PlayerGS::CState_PlayerGS(CFsm* pFsm)
    :CState{ pFsm }
{
}

HRESULT CState_PlayerGS::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_PlayerGS::Start_State()
{
    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player());
    m_pPlayer->Change_Animation(TEXT("Get_Sword"));
    m_pPlayer->Set_Player_State(CPlayer::GET_SWORD);
    m_pGameInstance->Play_Sound(TEXT("Player_Get_Sword.wav"), SOUND_PLAYER, g_fEffectVolume);

    m_bEffectOnce = true;
    return S_OK;
}

void CState_PlayerGS::Update(_float fTimeDelta)
{
    if (m_pPlayer->Get_CurrentFrame() >= 14
        && m_bEffectOnce)
    {
        CEffect::EFFECT_DESC Effect_Desc = {};
        Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
        Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
        Effect_Desc.vPos = m_pPlayer->Get_Position();
        Effect_Desc.vPos.x = m_pPlayer->Get_Position().x - 0.3f;
        Effect_Desc.vPos.y = m_pPlayer->Get_Position().y - 0.5f;
        Effect_Desc.fEffectDura = 1.2f;
        Effect_Desc.strTextureTag = TEXT("StrengthBuff");
        Effect_Desc.vScale = _float3(3.5f, 5.f, 0.1f);

        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
    
        m_bEffectOnce = false;
    }

    if (m_pPlayer->IsEnd_CurrentAnimation())
            m_pPlayer->Change_State(CPlayer::IDLE);
}

void CState_PlayerGS::End_State()
{
}

CState_PlayerGS* CState_PlayerGS::Create(CFsm* pFsm, _uint iStateNum)
{
    CState_PlayerGS* pInstance = new CState_PlayerGS(pFsm);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_PlayerGS"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_PlayerGS::Free()
{
    __super::Free();
}
