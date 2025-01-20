#include "Player_Bullet.h"
#include "GameInstance.h"

#include "Bullet_Particle.h"

CPlayer_Bullet::CPlayer_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CLandObject(pGraphic_Device)
{
}

CPlayer_Bullet::CPlayer_Bullet(const CPlayer_Bullet& Prototype)
    :CLandObject(Prototype)
{
}

HRESULT CPlayer_Bullet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer_Bullet::Initialize(void* pArg)
{

    PLAYER_BULLET* Player_Bullet = static_cast<PLAYER_BULLET*>(pArg);
    m_ePlayer_Bullet.vPos = Player_Bullet->vPos;
    m_ePlayer_Bullet.vDir = Player_Bullet->vDir;
    m_ePlayer_Bullet.fAtackPower = Player_Bullet->fAtackPower;
    m_ePlayer_Bullet.fDuration = Player_Bullet->fDuration;
    m_ePlayer_Bullet.fSpeed = Player_Bullet->fSpeed;

    if (FAILED(__super::Initialize(&Player_Bullet->tLandObj)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Scaling(_float3(1.5f, 1.5f, 0.1f));

    return S_OK;
}

void CPlayer_Bullet::Priority_Update(_float fTimeDelta)
{
    if (m_fBulletTimer >= m_ePlayer_Bullet.fDuration)
    {
        m_pGameInstance->Event_DestoryObject(this);

        m_fBulletTimer = 0.f;
    }
}

void CPlayer_Bullet::Update(_float fTimeDelta)
{
    m_fBulletTimer += fTimeDelta;
    m_ePlayer_Bullet.vPos.x += m_ePlayer_Bullet.vDir.x * m_ePlayer_Bullet.fSpeed * fTimeDelta;
    m_ePlayer_Bullet.vPos.z += m_ePlayer_Bullet.vDir.z * m_ePlayer_Bullet.fSpeed * fTimeDelta;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_ePlayer_Bullet.vPos);

}

void CPlayer_Bullet::Late_Update(_float fTimeDelta)
{
    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPlayer_Bullet::Render()
{
    _float4x4       fakeMatrix = m_pTransformCom->Get_WorldMatrix();
    _float3         Scale = m_pTransformCom->Get_Scaled();

    _float4x4       mViewMatrix = {};
    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &mViewMatrix);
    D3DXMatrixInverse(&mViewMatrix, nullptr, &mViewMatrix);

    _float3         vLookAt = *(_float3*)&mViewMatrix.m[3][0];

    _float3         vWorldPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    _float3         vLook = vLookAt - vWorldPosition;

    D3DXVec3Normalize(&vLook, &vLook);
    _float3         vUp = _float3(0.f, 1.f, 0.f);

    _float3         vRight = {};
    D3DXVec3Cross(&vRight, &vUp, &vLook);
    D3DXVec3Cross(&vUp, &vLook, &vRight);

    vRight *= Scale.x;
    vUp *= Scale.y;
    vLook *= Scale.z;

    memcpy(fakeMatrix.m[0], &vRight, sizeof(_float3));
    memcpy(fakeMatrix.m[1], &vUp, sizeof(_float3));
    memcpy(fakeMatrix.m[2], &vLook, sizeof(_float3));

    m_pGraphic_Device->SetTransform(D3DTS_WORLD, &fakeMatrix);

    if (FAILED(m_pTextureCom->Bind_Texture()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(SetUp_RenderState()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(Reset_RenderState()))
        return E_FAIL;


    m_pColliderCom->Render_Collider();
    return S_OK;
}

void CPlayer_Bullet::OnCollisionEnter(CGameObject* pOther)
{
    if(pOther->Get_LayerTag()==TEXT("Layer_Boss")
        ||pOther->Get_LayerTag()==TEXT("Layer_Monster"))
    {
        m_pGameInstance->Event_DestoryObject(this);


        CBullet_Particle::BULLET_PARTICLE_INFO Bullet_Particle_Info = {};
        Bullet_Particle_Info.iLevelIndex = m_pGameInstance->Get_LevelIndex();
        Bullet_Particle_Info.iStageIndex = m_pGameInstance->Get_StageIndex();
        Bullet_Particle_Info.vPos = Get_Position();
        Bullet_Particle_Info.fSpeed = 20.f;
        Bullet_Particle_Info.vDir = _float3((rand() % 5 + 1) * 0.1f, 0.f, (rand() % 5 + 1) * 0.1f);
        Bullet_Particle_Info.strTextureTag = TEXT("Prototype_Component_Texture_Player_Bullet");
        Bullet_Particle_Info.iParticleNum = 1;

        m_pGameInstance->Add_CloneObject_ToLayer(Bullet_Particle_Info.iLevelIndex, TEXT("Layer_Player_Bullet_Particle"), TEXT("Prototype_GameObject_Bullet_Particle"), &Bullet_Particle_Info);

        Bullet_Particle_Info.vDir = _float3((rand() % 5 + 1) * 0.1f, 0.f, (rand() % 5 + 1) * 0.1f);
        m_pGameInstance->Add_CloneObject_ToLayer(Bullet_Particle_Info.iLevelIndex, TEXT("Layer_Player_Bullet_Particle"), TEXT("Prototype_GameObject_Bullet_Particle"), &Bullet_Particle_Info);

        Bullet_Particle_Info.vDir = _float3((rand() % 5 + 1) * 0.1f, 0.f, (rand() % 5 + 1) * 0.1f);
        m_pGameInstance->Add_CloneObject_ToLayer(Bullet_Particle_Info.iLevelIndex, TEXT("Layer_Player_Bullet_Particle"), TEXT("Prototype_GameObject_Bullet_Particle"), &Bullet_Particle_Info);

        Bullet_Particle_Info.vDir = _float3(-(rand() % 5 + 1) * 0.1f, 0.f, (rand() % 5 + 1) * 0.1f);
        m_pGameInstance->Add_CloneObject_ToLayer(Bullet_Particle_Info.iLevelIndex, TEXT("Layer_Player_Bullet_Particle"), TEXT("Prototype_GameObject_Bullet_Particle"), &Bullet_Particle_Info);

        Bullet_Particle_Info.vDir = _float3(-(rand() % 5 + 1) * 0.1f, 0.f, -(rand() % 5 + 1) * 0.1f);
        m_pGameInstance->Add_CloneObject_ToLayer(Bullet_Particle_Info.iLevelIndex, TEXT("Layer_Player_Bullet_Particle"), TEXT("Prototype_GameObject_Bullet_Particle"), &Bullet_Particle_Info);

        Bullet_Particle_Info.vDir = _float3(-(rand() % 5 + 1) * 0.1f, 0.f, (rand() % 5 + 1) * 0.1f);
        m_pGameInstance->Add_CloneObject_ToLayer(Bullet_Particle_Info.iLevelIndex, TEXT("Layer_Player_Bullet_Particle"), TEXT("Prototype_GameObject_Bullet_Particle"), &Bullet_Particle_Info);

        Bullet_Particle_Info.vDir = _float3((rand() % 5 + 1) * 0.1f, 0.f, -(rand() % 5 + 1) * 0.1f);
        m_pGameInstance->Add_CloneObject_ToLayer(Bullet_Particle_Info.iLevelIndex, TEXT("Layer_Player_Bullet_Particle"), TEXT("Prototype_GameObject_Bullet_Particle"), &Bullet_Particle_Info);

        Bullet_Particle_Info.vDir = _float3(-(rand() % 5 + 1) * 0.1f, 0.f, (rand() % 5 + 1) * 0.1f);
        m_pGameInstance->Add_CloneObject_ToLayer(Bullet_Particle_Info.iLevelIndex, TEXT("Layer_Player_Bullet_Particle"), TEXT("Prototype_GameObject_Bullet_Particle"), &Bullet_Particle_Info);
    }
}

void CPlayer_Bullet::OnCollisionStay(CGameObject* pOther)
{
}

void CPlayer_Bullet::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CPlayer_Bullet::Ready_Components()
{
    // Texture Com
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Bullet"), TEXT("Com_Texture_Player_Bullet"),
        reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    // VIBuffer Com
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("VIBuffer_Com"),
        reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    // Collider Com
    CCollider_Cube::COLLIDER_CUBE_INFO ColliderDesc = {};
    ColliderDesc.fSizeX = 0.5f;
    ColliderDesc.fSizeY = 0.5f;
    ColliderDesc.fSizeZ = 0.5f;
    ColliderDesc.fOffSetX = 0.0f;
    ColliderDesc.fOffSetY = 0.0f;
    ColliderDesc.fOffSetZ = 0.0f;
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), TEXT("Collider_Com"),
        reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    // Transform Com
    CTransform::TRANSFORM_DESC TransformDesc = {};
    TransformDesc.fSpeedPerSec = 10.f;
    TransformDesc.fRotationPerSec = D3DXToRadian(60.f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Transform_Com"),
        reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer_Bullet::SetUp_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

    return S_OK;
}

HRESULT CPlayer_Bullet::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    return S_OK;
}

CPlayer_Bullet* CPlayer_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CPlayer_Bullet* pInstance = new CPlayer_Bullet(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CPlayer_Bullet"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer_Bullet::Clone(void* pArg)
{
    CPlayer_Bullet* pInstance = new CPlayer_Bullet(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CPlayer_Bullet"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_Bullet::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
