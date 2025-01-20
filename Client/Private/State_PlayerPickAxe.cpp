#include "State_PlayerPickAxe.h"
#include "Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"
#include "Rock_Particle.h"

CState_PlayerPickAxe::CState_PlayerPickAxe(CFsm* pFsm)
    :CState{ pFsm }
{
}

HRESULT CState_PlayerPickAxe::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;
    return S_OK;
}

HRESULT CState_PlayerPickAxe::Start_State()
{
    m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
    // Safe_AddRef(m_pPlayer);
    m_pPlayer->Change_Animation(TEXT("PickAxe"));
    m_pPlayer->Set_Player_State(CPlayer::PICKAXE);
    m_bIsParticle = true;
    return S_OK;
}

void CState_PlayerPickAxe::Update(_float fTimeDelta)
{
    if (m_pPlayer->Get_CurrentFrame() == 14 && !m_isCrash)
    {
        m_pGameInstance->Play_Sound(TEXT("Rock_Hit2.wav"), SOUND_PLAYER, g_fEffectVolume);
        m_isCrash = true;
        m_pPlayer->Get_InteractObject()->Set_IsShake(true);
        m_bIsParticle = true;

        if (!m_pPlayer->Get_InteractObject()->Crash_InterectObject())
        {
            m_pPlayer->Set_InteractObject(nullptr);
            m_pPlayer->Change_State(CPlayer::IDLE);
        }
        
    }
    else
    {
        m_isCrash = false;
    }


    if(m_pPlayer->Get_InteractObject()!=nullptr
        &&m_pPlayer->Get_InteractObject()->Get_IsShake())
    {

        if(m_bIsParticle)
        {
            CRock_Particle::ROCK_PARTICLE_DESC Rock_Info = {};
            CRock_Particle::ROCK_PARTICLE_DESC Rock_Info3 = {};

            Rock_Info.iLevelIndex = m_pGameInstance->Get_LevelIndex();
            Rock_Info.iStageIndex = m_pGameInstance->Get_StageIndex();

            Rock_Info3.iLevelIndex = m_pGameInstance->Get_LevelIndex();
            Rock_Info3.iStageIndex = m_pGameInstance->Get_StageIndex();


            // 물체가 오른쪽에 있음
            if (m_pPlayer->Get_isFilp())
            {
                Rock_Info.vPos.x = m_pPlayer->Get_Position().x + 0.5f;
                Rock_Info.vPos.y = m_pPlayer->Get_Position().y + rand() % 3 * 0.1f;
                Rock_Info.vPos.z = m_pPlayer->Get_Position().z + rand() % 2 * 0.1f;

                Rock_Info3.vPos.x = m_pPlayer->Get_Position().x + 1.f;
                Rock_Info3.vPos.y = m_pPlayer->Get_Position().y + rand() % 3 * 0.1f;
                Rock_Info3.vPos.z = m_pPlayer->Get_Position().z + rand() % 2 * 0.1f;

                Rock_Info.vDir = { -1.f,1.f,1.f };

                Rock_Info3.vDir = { 1.f,1.f,1.f };
            }

            else if (!m_pPlayer->Get_isFilp())
            {
                Rock_Info.vPos.x = m_pPlayer->Get_Position().x - 0.5f;
                Rock_Info.vPos.y = m_pPlayer->Get_Position().y + rand() % 3 * 0.1f;
                Rock_Info.vPos.z = m_pPlayer->Get_Position().z + rand() % 2 * 0.1f;

                Rock_Info3.vPos.x = m_pPlayer->Get_Position().x - 1.f;
                Rock_Info3.vPos.y = m_pPlayer->Get_Position().y + rand() % 3 * 0.1f;
                Rock_Info3.vPos.z = m_pPlayer->Get_Position().z + rand() % 2 * 0.1f;

                Rock_Info.vDir = { 1.f,1.f,1.f };

                Rock_Info3.vDir = { -1.f,1.f,1.f };
            }
            Rock_Info.fJumpPower = 0.2f;
            Rock_Info.iParticleNum = 1;
            Rock_Info.fMinY = 0.5f;
            Rock_Info.vSize = { 0.5f,0.5f,0.1f };
            Rock_Info.strTextureTag = TEXT("Prototype_Component_Texture_Rock_Particle_1");

            Rock_Info3.fJumpPower = 0.2f;
            Rock_Info3.iParticleNum = 1;
            Rock_Info.fMinY = 0.5f;
            Rock_Info3.vSize = { 0.5f,0.5f,0.1f };
            Rock_Info3.strTextureTag = TEXT("Prototype_Component_Texture_Rock_Particle_3");

            m_pGameInstance->Add_CloneObject_ToLayer(Rock_Info.iLevelIndex, TEXT("Layer_Particle_Rock"), TEXT("Prototype_GameObject_Rock_Particle"), &Rock_Info);

            m_pGameInstance->Add_CloneObject_ToLayer(Rock_Info3.iLevelIndex, TEXT("Layer_Particle_Rock"), TEXT("Prototype_GameObject_Rock_Particle"), &Rock_Info3);
        
            m_bIsParticle = false;
        }

    }
}

void CState_PlayerPickAxe::End_State()
{
}

CState_PlayerPickAxe* CState_PlayerPickAxe::Create(CFsm* pFsm, _uint iStateNum)
{
    CState_PlayerPickAxe* pInstance = new CState_PlayerPickAxe(pFsm);
    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_PlayerPickAxe"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CState_PlayerPickAxe::Free()
{
    __super::Free();
}
