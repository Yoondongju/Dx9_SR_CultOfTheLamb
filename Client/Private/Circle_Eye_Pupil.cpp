#include "Circle_Eye_Pupil.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "CIrcle_Eye.h"

CCircle_Eye_Pupil::CCircle_Eye_Pupil(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CUI(pGraphic_Device)
{
}

CCircle_Eye_Pupil::CCircle_Eye_Pupil(const CCircle_Eye_Pupil& Prototype)
    :CUI(Prototype)
{
}

HRESULT CCircle_Eye_Pupil::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCircle_Eye_Pupil::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_bActivate = false;

    m_fSizeX = 40.f;   //크기
    m_fSizeY = 40.f;

    m_fX = 657.f;
    m_fY = 200.f;

    m_fOffsetX = 0.f;
    m_fOffsetY = 0.f;

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

    D3DXMatrixIdentity(&m_ViewMatrix);

    /* 직교투영행렬을 만들어준다. */
    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
    return S_OK;
}

void CCircle_Eye_Pupil::Priority_Update(_float fTimeDelta)
{
}

void CCircle_Eye_Pupil::Update(_float fTimeDelta)
{
    Move_Offset();

    m_fX = m_pParentUI->Get_fX() + m_fOffsetX;
    m_fY = m_pParentUI->Get_fY() + m_fOffsetY;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

    m_bActivate = dynamic_cast<CCircle_Eye*>(m_pParentUI)->Get_Pupil_Active();
}

void CCircle_Eye_Pupil::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CCircle_Eye_Pupil::Render()
{
    if (m_bActivate)
    {
        m_pVIBufferCom->Filp_Texture(m_isFilp);

        if (FAILED(m_pTransformCom->Bind_Transform()))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_Texture()))
            return E_FAIL;

        m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
        m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(SetUp_RenderState()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        if (FAILED(Reset_RenderState()))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CCircle_Eye_Pupil::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    return S_OK;
}

HRESULT CCircle_Eye_Pupil::Reset_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    return S_OK;
}

HRESULT CCircle_Eye_Pupil::Ready_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(Client::LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Eye_Pupil"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;


    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    /* For.Com_Transform */
    CTransform::TRANSFORM_DESC		TransformDesc{};

    TransformDesc.fSpeedPerSec = 10.0f;
    TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;
    return S_OK;
}

void CCircle_Eye_Pupil::Move_Offset()
{
    if (dynamic_cast<CCommand_Circle*> (m_pParentUI->GetParent()) == nullptr)
        return;

    CCommand_Circle::MouseOnDir _dir= dynamic_cast<CCommand_Circle*> (m_pParentUI->GetParent())->Get_MouseOnDir();
    switch (_dir)
    {
    case Client::CCommand_Circle::RIGHT:
        m_fOffsetX = 5.f;
        m_fOffsetY = 0.f;
        break;
    case Client::CCommand_Circle::LEFT:
        m_fOffsetX = -5.f;
        m_fOffsetY = 0.f;
        break;
    case Client::CCommand_Circle::UP:
        m_fOffsetX = 0.f;
        m_fOffsetY = -5.f;
        break;
    default:
        m_fOffsetX = 0.f;
        m_fOffsetY = 0.f;
        break;
    }
}

CCircle_Eye_Pupil* CCircle_Eye_Pupil::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CCircle_Eye_Pupil* pInstance = new CCircle_Eye_Pupil(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCircle_Eye_Pupil"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCircle_Eye_Pupil::Clone(void* pArg)
{
    CCircle_Eye_Pupil* pInstance = new CCircle_Eye_Pupil(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CCircle_Eye_Pupil"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCircle_Eye_Pupil::Free()
{
    __super::Free();;

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
