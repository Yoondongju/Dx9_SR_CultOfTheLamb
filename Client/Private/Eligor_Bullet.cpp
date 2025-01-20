#include "Eligor_Bullet.h"
#include "GameInstance.h"

CEligor_Bullet::CEligor_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CLandObject(pGraphic_Device)
{
}

CEligor_Bullet::CEligor_Bullet(const CEligor_Bullet& Prototype)
    :CLandObject(Prototype)
{
}

HRESULT CEligor_Bullet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEligor_Bullet::Initialize(void* pArg)
{
    ELIGOR_BULLET* Eligor_Bullet = static_cast<ELIGOR_BULLET*>(pArg);
    m_eEligorBullet.vPos = Eligor_Bullet->vPos;
    m_eEligorBullet.vDir = Eligor_Bullet->vDir;
    m_eEligorBullet.vSpeed = Eligor_Bullet->vSpeed;

    if (FAILED(__super::Initialize(&Eligor_Bullet->tLandObj)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CEligor_Bullet::Priority_Update(_float fTimeDelta)
{
}

void CEligor_Bullet::Update(_float fTimeDelta)
{
    m_pColliderCom->Set_IsActive(true);

    if (Get_Position().x > 24.f
        || Get_Position().x < 0.f
        || Get_Position().z>18.f
        || Get_Position().z < 0.f)
        m_pGameInstance->Event_DestoryObject(this);

    m_eEligorBullet.vPos.x += m_eEligorBullet.vDir.x * fTimeDelta * m_eEligorBullet.vSpeed;
    m_eEligorBullet.vPos.z += m_eEligorBullet.vDir.z * fTimeDelta * m_eEligorBullet.vSpeed;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_eEligorBullet.vPos);

}

void CEligor_Bullet::Late_Update(_float fTimeDelta)
{
    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CEligor_Bullet::Render()
{
    _float4x4 fakeMatrix = m_pTransformCom->Get_WorldMatrix();
    _float3 Scale = m_pTransformCom->Get_Scaled();

    _float4x4 InvertView;
    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &InvertView);
    D3DXMatrixInverse(&InvertView, nullptr, &InvertView);
    _float3 vCameraPosition = (*(_float3*)&InvertView.m[3][0]);


    _float3 vWorldPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    _float3 vTempLook = vCameraPosition - vWorldPosition;
    D3DXVec3Normalize(&vTempLook, &vTempLook);
    _float3 vTempUp = { 0.f,1.f,0.f };
    _float3 vTempRight;

    D3DXVec3Cross(&vTempRight, &vTempUp, &vTempLook);
    D3DXVec3Cross(&vTempUp, &vTempLook, &vTempRight);

    vTempRight *= Scale.x;
    vTempUp *= Scale.y;
    vTempLook *= Scale.z;

    memcpy(fakeMatrix.m[0], &vTempRight, sizeof(_float3));
    memcpy(fakeMatrix.m[1], &vTempUp, sizeof(_float3));
    memcpy(fakeMatrix.m[2], &vTempLook, sizeof(_float3));

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

void CEligor_Bullet::OnCollisionEnter(CGameObject* pOther)
{
    if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
        m_pGameInstance->Event_DestoryObject(this);
}

void CEligor_Bullet::OnCollisionStay(CGameObject* pOther)
{
}

void CEligor_Bullet::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CEligor_Bullet::Ready_Components()
{
    // Texture Com
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Bullet"), TEXT("Com_Texture_Eligor_Bullet"),
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

HRESULT CEligor_Bullet::SetUp_RenderState()
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

HRESULT CEligor_Bullet::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    return S_OK;
}

CEligor_Bullet* CEligor_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CEligor_Bullet* pInstance = new CEligor_Bullet(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEligor_Bullet"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEligor_Bullet::Clone(void* pArg)
{
    CEligor_Bullet* pInstance = new CEligor_Bullet(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CEligor_Bullet"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEligor_Bullet::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
