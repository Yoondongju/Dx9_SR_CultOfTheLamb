#include "State_PlayerAttack1.h"
#include "Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

#include "Effect.h"

const _float fCombo1to2Time = 0.2f;
const _float fAttackMove = 0.4f;
CState_PlayerAttack1::CState_PlayerAttack1(class CFsm* pFsm)
    :CState(pFsm)
{
}

HRESULT CState_PlayerAttack1::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;
    return S_OK;
}

HRESULT CState_PlayerAttack1::Start_State()
{
    m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
    // Safe_AddRef(m_pPlayer);
    m_pPlayer->Change_Animation(TEXT("Attack1"));
    m_pPlayer->Set_Player_State(CPlayer::ATTACK1);
    m_pGameInstance->Play_Sound(TEXT("Player_Attack.wav"), SOUND_PLAYER, g_fEffectVolume);
    
    Check_Mouse();

    if(m_vInterPos.x > m_pPlayer->Get_Position().x)
    {
        CCollider_Cube::COLLIDER_CUBE_INFO Info = {};
        Info.fSizeX = 0.3f;
        Info.fSizeY = 0.2f;
        Info.fSizeZ = 8.f;
        Info.fOffSetX = 0.8f;
        Info.fOffSetY = -0.25f;
        Info.fOffSetZ = 0.f;
        m_pPlayer->Get_Player_Coll()->Set_Collider_Cube_Scale(Info);
    }

    else if (m_vInterPos.x < m_pPlayer->Get_Position().x)
    {
        CCollider_Cube::COLLIDER_CUBE_INFO Info = {};
        Info.fSizeX = 0.3f;
        Info.fSizeY = 0.2f;
        Info.fSizeZ = 8.f;
        Info.fOffSetX = -0.8f;
        Info.fOffSetY = -0.25f;
        Info.fOffSetZ = 0.f;
        m_pPlayer->Get_Player_Coll()->Set_Collider_Cube_Scale(Info);
    }
    m_fComboTimer = 0.f;
    m_bAttackMove = true;
    m_bEffectOnce = true;
    return S_OK;
}

void CState_PlayerAttack1::Update(_float fTimeDelta)
{
    m_fComboTimer += fTimeDelta;

    m_pPlayer->Get_Player_Coll()->Set_IsActive(true);
    if (m_vInterPos.x > m_pPlayer->Get_Position().x
        &&m_bAttackMove)
    {
        m_pPlayer->Set_isFilp(true);
        vPlayer_Pos = m_pPlayer->Get_Position();
        vPlayer_Pos.x += fAttackMove;

        if (m_vInterPos.z > m_pPlayer->Get_Position().z)
        {
            vPlayer_Pos.z += fAttackMove;
        }

        else if (m_vInterPos.z < m_pPlayer->Get_Position().z)
        {
            vPlayer_Pos.z -= fAttackMove;
        }
        m_pPlayer->Set_Position(vPlayer_Pos);

        if (m_bEffectOnce)
        {
            CEffect::EFFECT_DESC Effect_Desc = {};
            Effect_Desc.fEffectDura = 2.f;
            Effect_Desc.vPos = m_pPlayer->Get_Position();
            Effect_Desc.vPos.x = m_pPlayer->Get_Position().x + 1.5f;
            Effect_Desc.vPos.y = m_pPlayer->Get_Position().y - 0.5f;
            Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
            Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
            Effect_Desc.vScale = _float3(3.f, 3.f, 0.1f);
            Effect_Desc.strTextureTag = TEXT("HitEffect_Rise");

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

            m_bEffectOnce = false;
        }

        m_bAttackMove = false;
    }

    else if (m_vInterPos.x < m_pPlayer->Get_Position().x
        &&m_bAttackMove)
    {
        m_pPlayer->Set_isFilp(false);
        _float3 vPlayer_Pos = m_pPlayer->Get_Position();
        vPlayer_Pos.x -= fAttackMove;

        if (m_vInterPos.z > m_pPlayer->Get_Position().z)
        {
            vPlayer_Pos.z += fAttackMove;
        }
        else if (m_vInterPos.z < m_pPlayer->Get_Position().z)
        {
            vPlayer_Pos.z -= fAttackMove;
        }

        m_pPlayer->Set_Position(vPlayer_Pos);
        if (m_bEffectOnce)
        {
            CEffect::EFFECT_DESC Effect_Desc = {};
            Effect_Desc.fEffectDura = 2.f;
            Effect_Desc.vPos = m_pPlayer->Get_Position();
            Effect_Desc.vPos.x = m_pPlayer->Get_Position().x - 1.5f;
            Effect_Desc.vPos.y = m_pPlayer->Get_Position().y - 0.5f;
            Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
            Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
            Effect_Desc.vScale = _float3(3.f, 3.f, 0.1f);
            Effect_Desc.strTextureTag = TEXT("HitEffect_Rise");

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

            m_bEffectOnce = false;
        }
        m_bAttackMove = false;
    }

    Check_Move();

    if (m_pPlayer->IsEnd_CurrentAnimation())
    {
        m_pPlayer->Change_State(CPlayer::IDLE);
        m_pPlayer->Get_Player_Coll()->Set_IsActive(true);
    }
    else if (m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP && m_fComboTimer >= fCombo1to2Time)
    {
        m_pPlayer->Change_State(CPlayer::ATTACK2);
        m_pPlayer->Get_Player_Coll()->Set_IsActive(false);
    }
}

void CState_PlayerAttack1::End_State()
{
}

void CState_PlayerAttack1::Check_Move()
{
    if (m_pPlayer->Get_Roll_Timer() > CPlayer::fRollCoolTime)
    {
        if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::TAP)
        {
            m_pPlayer->Change_State(CPlayer::ROLL);
            m_pPlayer->Set_Player_State(CPlayer::ROLL);
            m_pPlayer->Set_Roll_Timer();
        }
    }
}

void CState_PlayerAttack1::Check_Mouse()
{
    m_ptMousePos = m_pGameInstance->Get_MosePos();

    vMousePos.x = _float(m_ptMousePos.x / (g_iWinSizeX * 0.5f) - 1);
    vMousePos.y = _float(m_ptMousePos.y / (g_iWinSizeY * -0.5f) + 1);
    vMousePos.z = 0.f;

    _float4x4		ProjMatrixInv;
    m_pPlayer->Get_Device()->GetTransform(D3DTS_PROJECTION, &ProjMatrixInv);
    D3DXMatrixInverse(&ProjMatrixInv, nullptr, &ProjMatrixInv);
    D3DXVec3TransformCoord(&vMousePos, &vMousePos, &ProjMatrixInv);

    m_vRayPos = _float3(0.f, 0.f, 0.f);
    m_vRayDir = vMousePos - m_vRayPos;

    _float4x4		ViewMatrixInv;
    m_pPlayer->Get_Device()->GetTransform(D3DTS_VIEW, &ViewMatrixInv);
    D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrixInv);
    D3DXVec3TransformCoord(&m_vRayPos, &m_vRayPos, &ViewMatrixInv);
    D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &ViewMatrixInv);

    D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);

    float fDist = (m_pPlayer->Get_Position().y - m_vRayPos.y) / m_vRayDir.y;

    m_vInterPos.x = m_vRayPos.x + fDist * m_vRayDir.x;
    m_vInterPos.y = m_pPlayer->Get_Position().y;
    m_vInterPos.z = m_vRayPos.z + fDist * m_vRayDir.z;

}

CState_PlayerAttack1* CState_PlayerAttack1::Create(class CFsm* pFsm,_uint iStateNum)
{
    CState_PlayerAttack1* pInstance = new CState_PlayerAttack1(pFsm);
    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_PlayerAttack1"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CState_PlayerAttack1::Free()
{
    __super::Free();
}
