#include "Mortal_Fly.h"
#include "GameInstance.h"

CMortal_Fly::CMortal_Fly(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CGameObject(pGraphic_Device)
{
}

CMortal_Fly::CMortal_Fly(const CMortal_Fly& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT CMortal_Fly::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMortal_Fly::Initialize(void* pArg)
{
    MORTAL_FLY* Mortal_Fly = static_cast<MORTAL_FLY*>(pArg);
    m_eMortalFly.vInitPos = Mortal_Fly->vInitPos;
    m_eMortalFly.vPos = Mortal_Fly->vPos;
    m_eMortalFly.fJumpPower = Mortal_Fly->fJumpPower;
    m_eMortalFly.fSpeed = Mortal_Fly->fSpeed;
    m_eMortalFly.vDir = Mortal_Fly->vDir;
    m_eMortalFly.iLevelIndex = Mortal_Fly->iLevelIndex;
    m_eMortalFly.iStageIndex = Mortal_Fly->iStageIndex;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    _float3 vScale = { 8.f,8.f,0.1f };
    m_pTransformCom->Scaling(vScale);
    m_pAnimCom->Change_Animation(TEXT("Mortal_Fly"));

    return S_OK;
}

void CMortal_Fly::Priority_Update(_float fTimeDelta)
{
}

void CMortal_Fly::Update(_float fTimeDelta)
{
    m_eMortalFly.fJumpPower -= fTimeDelta * 5.f;
    m_eMortalFly.vPos.x += m_eMortalFly.vDir.x * fTimeDelta * m_eMortalFly.fSpeed;
    m_eMortalFly.vPos.y += m_eMortalFly.fJumpPower * fTimeDelta * m_eMortalFly.fSpeed;
    m_eMortalFly.vPos.z += m_eMortalFly.vDir.z * fTimeDelta * m_eMortalFly.fSpeed;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_eMortalFly.vPos);

       if (m_eMortalFly.vPos.y <= m_eMortalFly.vInitPos.y-2.5f)
       {
           m_pGameInstance->Event_DestoryObject(this);
       }
}

void CMortal_Fly::Late_Update(_float fTimeDelta)
{
    m_pAnimCom->Update(fTimeDelta);
    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMortal_Fly::Render()
{
    if (FAILED(m_pTransformCom->Bind_Transform()))
        return E_FAIL;

    m_pAnimCom->Render(NULL);


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

void CMortal_Fly::OnCollisionEnter(CGameObject* pOther)
{
    if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
        m_pGameInstance->Event_DestoryObject(this);
}

void CMortal_Fly::OnCollisionStay(CGameObject* pOther)
{
}

void CMortal_Fly::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CMortal_Fly::Ready_Components()
{
    // Anim Component
    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
        TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimCom), nullptr)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Mortal_Fly"),
        TEXT("Com_Texture_Mortal_Fly"),
        reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    m_pAnimCom->Add_Animation(TEXT("Mortal_Fly"), m_pTextureCom, 0, 18, 0.03f, true);

    // VIBuffer Com
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("VIBuffer_Com"),
        reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    // Collider Com
    CCollider_Cube::COLLIDER_CUBE_INFO ColliderDesc = {};
    ColliderDesc.fSizeX = 0.3f;
    ColliderDesc.fSizeY = 0.2f;
    ColliderDesc.fSizeZ = 1.f;
    ColliderDesc.fOffSetX = 0.0f;
    ColliderDesc.fOffSetY = 1.0f;
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

HRESULT CMortal_Fly::SetUp_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

    return S_OK;
}

HRESULT CMortal_Fly::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    return S_OK;
}

CMortal_Fly* CMortal_Fly::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CMortal_Fly* pInstance = new CMortal_Fly(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMortal_Fly"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMortal_Fly::Clone(void* pArg)
{
    CMortal_Fly* pInstance = new CMortal_Fly(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CMortal_Fly"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMortal_Fly::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pAnimCom);
}
