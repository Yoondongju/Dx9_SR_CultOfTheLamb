#include "Hekat_Bullet.h"
#include "GameInstance.h"

CHekat_Bullet::CHekat_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CLandObject(pGraphic_Device)
{
}

CHekat_Bullet::CHekat_Bullet(const CHekat_Bullet& Prototype)
    :CLandObject(Prototype)
{
}

HRESULT CHekat_Bullet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHekat_Bullet::Initialize(void* pArg)
{
    HEKAT_BULLET* Hekat_Bullet = static_cast<HEKAT_BULLET*>(pArg);
    m_eHekatBullet.vPos = Hekat_Bullet->vPos;
    m_eHekatBullet.vDgree = Hekat_Bullet->vDgree;
    m_eHekatBullet.vSpeed = Hekat_Bullet->vSpeed;
   
    if (FAILED(__super::Initialize(&Hekat_Bullet->tLandObj)))
        return E_FAIL;
   
    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CHekat_Bullet::Priority_Update(_float fTimeDelta)
{
}

void CHekat_Bullet::Update(_float fTimeDelta)
{
    if (Get_Position().x > 24.f
        || Get_Position().x < 0.f
        || Get_Position().z>18.f
        || Get_Position().z < 0.f)
        m_pGameInstance->Event_DestoryObject(this);

    _float		fRadian = D3DXToRadian(m_eHekatBullet.vDgree);
    m_eHekatBullet.vPos.x += m_eHekatBullet.vSpeed * cosf(fRadian) * fTimeDelta;
    m_eHekatBullet.vPos.z += m_eHekatBullet.vSpeed * sinf(fRadian) * fTimeDelta;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_eHekatBullet.vPos);

    _float4x4 mViewMatrix = {};
    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &mViewMatrix);
    D3DXMatrixInverse(&mViewMatrix, nullptr, &mViewMatrix);
    m_pTransformCom->LookAt(*(_float3*)&mViewMatrix.m[3][0]);
}

void CHekat_Bullet::Late_Update(_float fTimeDelta)
{
    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CHekat_Bullet::Render()
{
    if (FAILED(m_pTransformCom->Bind_Transform()))
        return E_FAIL;

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

void CHekat_Bullet::OnCollisionEnter(CGameObject* pOther)
{
    if(pOther->Get_LayerTag()==TEXT("Layer_Player"))
        m_pGameInstance->Event_DestoryObject(this);
}

void CHekat_Bullet::OnCollisionStay(CGameObject* pOther)
{
}

void CHekat_Bullet::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CHekat_Bullet::Ready_Components()
{
    // Texture Com
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Bullet"), TEXT("Com_Texture_Hekat_Bullet"),
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

HRESULT CHekat_Bullet::SetUp_RenderState()
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

HRESULT CHekat_Bullet::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    return S_OK;
}

CHekat_Bullet* CHekat_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CHekat_Bullet* pInstance = new CHekat_Bullet(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CHekat_Bullet"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHekat_Bullet::Clone(void* pArg)
{
    CHekat_Bullet* pInstance = new CHekat_Bullet(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CHekat_Bullet"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHekat_Bullet::Free()
{
    __super::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
}
