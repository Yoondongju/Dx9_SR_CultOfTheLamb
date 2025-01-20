#include "Boss_Eligor.h"
#include "GameInstance.h"

#include "Player.h"

#include "State_EligorIdle.h"
#include "State_EligorBurp.h"
#include "State_EilgorFA.h"
#include "State_EligorRoar.h"
#include "State_EligorLE.h"
#include "State_EligorTF.h"
#include "State_EligorUC.h"

#include "Effect.h"

#include "Player_Bullet.h"

CBoss_Eligor::CBoss_Eligor(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CMonster(pGraphic_Device)
{
}

CBoss_Eligor::CBoss_Eligor(const CBoss_Eligor& Prototype)
    :CMonster(Prototype)
{
}

HRESULT CBoss_Eligor::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBoss_Eligor::Initialize(void* pArg)
{
    m_eMonsterId = ELIGOS;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Animation()))
        return E_FAIL;

    if (FAILED(Ready_Fsm()))
        return E_FAIL;
    _float3 vScale = _float3{ 5.f,5.f,0.1f };
    m_vInitPosition = _float3{ 12.f,5.f,10.f };
    m_fAttackTimer = 0.f;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitPosition);
    m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);

    return S_OK;
}

void CBoss_Eligor::Priority_Update(_float fTimeDelta)
{
    if (m_tMonsterInfo.iHp <= 0.f)
        m_tMonsterInfo.iHp = 0.f;

    __super::SetUp_OnTerrain(m_pTransformCom, 0.5f);
}

void CBoss_Eligor::Update(_float fTimeDelta)
{
    if (m_isHit)
    {
        m_fHitEffectTime += fTimeDelta;
    }

    if (m_eCurState == TRANS_F
        || m_eCurState == UNCONVERTED)
    {
        _float3 vScale = { 10.f,10.f,0.1f };
        _float3 vPos = {};
        vPos.x = Get_Position().x;
        vPos.y = Get_Position().y - 4.5f;
        vPos.z = Get_Position().z;
        m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        
        m_pColliderCom->Set_IsActive(false);
    }

    Calculate_Flip();

    m_pFsmCom->Update(fTimeDelta);
}

void CBoss_Eligor::Late_Update(_float fTimeDelta)
{
    m_bAttackOnce = true;
    m_pAnimatorCom->Update(fTimeDelta);
    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBoss_Eligor::Render()
{
    m_pVIBufferCom->Filp_Texture(m_isFilp);

    if (FAILED(m_pTransformCom->Bind_Transform()))
        return E_FAIL;

    m_pAnimatorCom->Render(NULL);

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    _float4x4		WorldMatrix, ViewMatrix, ProjMatrix;

    WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());
    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
    m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

    m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
    m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
    m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4));

    m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
    m_pShaderCom->Set_Texture("g_Texture", m_pAnimatorCom->Get_CurrentTexture());

    if (m_isHit)
    {
        if (m_fHitEffectTime <= 0.1f)
        {
            m_pShaderCom->Begin_Shader(1);
        }
        else
        {
            m_isHit = false;
            m_fHitEffectTime = 0.f;
            m_pShaderCom->Begin_Shader(0);
        }
    }
    else
    {
        m_pShaderCom->Begin_Shader(0);
    }

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    m_pShaderCom->End_Shader();

    m_pColliderCom->Render_Collider();

    return S_OK;
}

void CBoss_Eligor::OnCollisionEnter(CGameObject* pOther)
{
    if (m_eCurState != ROAR
        &&m_eCurState!=TRANS_F
        &&m_eCurState!=UNCONVERTED)

    {
        if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
        {
            if (static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK1
                || static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK2
                || static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK3)
            {
                m_isHit = true;
                if (pOther->Get_isFilp())
                {
                    if (Get_Position().x > pOther->Get_Position().x)
                    {
                        m_pGameInstance->Play_Sound(TEXT("Enemy_Hit1.wav"), SOUND_ELIGOR, g_fEffectVolume);
                        m_tMonsterInfo.iHp -= static_cast<CPlayer*>(pOther)->Get_Player_Info().iAttackPower;                        CEffect::EFFECT_DESC Effect_Desc = {};
                        Effect_Desc.vPos = Get_Position();
                        Effect_Desc.vPos.x = Get_Position().x - 1.5f;
                        Effect_Desc.vScale = _float3(20.0f, 20.0f, 0.1f);
                        Effect_Desc.fEffectDura = 0.4f;
                        Effect_Desc.strTextureTag = TEXT("Hit_Blood");
                        Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
                        Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
                        m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
                    }
                }

                else if (!pOther->Get_isFilp())
                {
                    if (Get_Position().x < pOther->Get_Position().x)
                    {
                        m_pGameInstance->Play_Sound(TEXT("Enemy_Hit1.wav"), SOUND_ELIGOR, g_fEffectVolume);
                        m_tMonsterInfo.iHp -= static_cast<CPlayer*>(pOther)->Get_Player_Info().iAttackPower;                        CEffect::EFFECT_DESC Effect_Desc = {};
                        Effect_Desc.vPos = Get_Position();
                        Effect_Desc.vPos.x = Get_Position().x + 1.5f;
                        Effect_Desc.vScale = _float3(20.0f, 20.0f, 0.1f);
                        Effect_Desc.fEffectDura = 0.4f;
                        Effect_Desc.strTextureTag = TEXT("Hit_Blood");
                        Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
                        Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
                        m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
                    }
                }
            }

        }

        if (pOther->Get_LayerTag() == TEXT("Layer_Player_Bullet"))
        {
            m_pGameInstance->Play_Sound(TEXT("Enemy_Hit1.wav"), SOUND_ELIGOR, g_fEffectVolume);
            m_tMonsterInfo.iHp -= static_cast<CPlayer_Bullet*>(pOther)->Get_Player_Bullet_Info().fAtackPower;
            m_isHit = true;
            if (Get_Position().x < pOther->Get_Position().x)
            {
                CEffect::EFFECT_DESC Effect_Desc = {};
                Effect_Desc.vPos = Get_Position();
                Effect_Desc.vPos.x = Get_Position().x + 1.f;
                Effect_Desc.vScale = _float3(4.0f, 4.0f, 0.1f);
                Effect_Desc.fEffectDura = 0.2f;
                Effect_Desc.strTextureTag = TEXT("HitEffect_Circle");
                Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
                Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

                m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
            }

            else if (Get_Position().x > pOther->Get_Position().x)
            {
                CEffect::EFFECT_DESC Effect_Desc = {};
                Effect_Desc.vPos = Get_Position();
                Effect_Desc.vPos.x = Get_Position().x - 1.f;
                Effect_Desc.vScale = _float3(4.0f, 4.0f, 0.1f);
                Effect_Desc.fEffectDura = 0.2f;
                Effect_Desc.strTextureTag = TEXT("HitEffect_Circle");
                Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
                Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

                m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
            }
        }
    }
}

void CBoss_Eligor::OnCollisionStay(CGameObject* pOther)
{
}

void CBoss_Eligor::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CBoss_Eligor::Ready_Components()
{
    // Collider Component
    CCollider_Cube::COLLIDER_CUBE_INFO ColliderInfo = {};
    ColliderInfo.fSizeX = 0.3f;
    ColliderInfo.fSizeY = 0.3f;
    ColliderInfo.fSizeZ = 5.f;
    ColliderInfo.fOffSetX = 0.f;
    ColliderInfo.fOffSetY = 0.f;
    ColliderInfo.fOffSetZ = 0.f;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderInfo)))
        return E_FAIL;

    // CVIBuffer_Rect Component
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    // CTransform Component
    CTransform::TRANSFORM_DESC Transform_Desc = {};
    Transform_Desc.fSpeedPerSec = 10.0f;
    Transform_Desc.fRotationPerSec = D3DXToRadian(90.0f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &Transform_Desc)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Boss"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBoss_Eligor::Ready_Animation()
{
    CTexture* pTextureCom;

    // Anim Component
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animation"), TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimatorCom))))
        return E_FAIL;

    // IDLE
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_IDLE"), TEXT("Com_Texture_Boss_Eligor_Idle"), reinterpret_cast<CComponent**>(&pTextureCom))))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("IDLE"), pTextureCom, 0, 28, 0.03f, true);

    // Roar , Burp
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Roar"), TEXT("Com_Texture_Boss_Eligor_Roar"), reinterpret_cast<CComponent**>(&pTextureCom))))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("Roar"), pTextureCom, 0, 94, 0.03f, false);

    // Fly_Charge
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Fly_Charge"), TEXT("Com_Texture_Boss_Eligor_Fly_Charge"), reinterpret_cast<CComponent**>(&pTextureCom))))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("Attack_Charge"), pTextureCom, 0, 39, 0.03f, false);

    // Fly_Attack
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Fly_Attack"), TEXT("Com_Texture_Boss_Eligor_Fly_Attack"), reinterpret_cast<CComponent**>(&pTextureCom))))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("Attack"), pTextureCom, 0, 17, 0.03f, true);

    // Fly_Stop
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Fly_Stop"), TEXT("Com_Texture_Boss_Eligor_Fly_Stop"), reinterpret_cast<CComponent**>(&pTextureCom))))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("Attack_Stop"), pTextureCom, 0, 89, 0.03f, false);

    // Transform_Follower
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Transform_Follower"), TEXT("Com_Texture_Boss_Eligor_Transform_Follower"), reinterpret_cast<CComponent**>(&pTextureCom))))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("Transform_Follower"), pTextureCom, 0, 123, 0.03f, false);

    // Unconverted
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Unconverted"), TEXT("Com_Texture_Boss_Eligor_Unconverted"), reinterpret_cast<CComponent**>(&pTextureCom))))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("Unconverted"), pTextureCom, 0, 67, 0.03f, true);

    // Convert
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Convert"), TEXT("Com_Texture_Boss_Eligor_Convert"), reinterpret_cast<CComponent**>(&pTextureCom))))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("Converted"), pTextureCom, 0, 61, 0.04f, false);

    return S_OK;
}

HRESULT CBoss_Eligor::Ready_Fsm()
{
    // Fsm Component
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"), TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
        return E_FAIL;

    m_pFsmCom->Add_State(CState_EligorRoar::Create(m_pFsmCom, ROAR));
    m_pFsmCom->Add_State(CState_EligorIdle::Create(m_pFsmCom, IDLE));
    m_pFsmCom->Add_State(CState_EligorBurp::Create(m_pFsmCom, BURP));
    m_pFsmCom->Add_State(CState_EligorLE::Create(m_pFsmCom, LAY_EGG));
    m_pFsmCom->Add_State(CState_EligorFA::Create(m_pFsmCom, FLY_ATTACK));
    m_pFsmCom->Add_State(CState_EligorTF::Create(m_pFsmCom, TRANS_F));
    m_pFsmCom->Add_State(CState_EligorUC::Create(m_pFsmCom, UNCONVERTED));

    m_pFsmCom->Set_State(ROAR);
    return S_OK;
}

HRESULT CBoss_Eligor::SetUp_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 127);

    return S_OK;
}

HRESULT CBoss_Eligor::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    return S_OK;
}

CBoss_Eligor* CBoss_Eligor::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CBoss_Eligor* pInstance = new CBoss_Eligor(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBoss_Eligor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoss_Eligor::Clone(void* pArg)
{
    CBoss_Eligor* pInstance = new CBoss_Eligor(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CBoss_Eligor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_Eligor::Free()
{
    __super::Free();
    if (nullptr != m_pFsmCom)
        m_pFsmCom->Release_States();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pFsmCom);
    Safe_Release(m_pAnimatorCom);
    Safe_Release(m_pVIBufferCom);
}
