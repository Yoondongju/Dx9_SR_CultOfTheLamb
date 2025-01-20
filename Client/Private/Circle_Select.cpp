#include "Circle_Select.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Command_Circle.h"

CCircle_Select::CCircle_Select(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CUI{ pGraphic_Device }
{
}

CCircle_Select::CCircle_Select(const CCircle_Select& Prototype)
    : CUI{ Prototype }
{
}

HRESULT CCircle_Select::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCircle_Select::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_bActivate = false;

    m_fSizeX = 350.f;   //크기
    m_fSizeY = 350.f;

    m_fX = m_fSizeX * 0.5f + 450;   //위치
    m_fY = m_fSizeY * 0.5f + 40.f;

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
        -m_fY + g_iWinSizeY * 0.5f, 0.1f));

    D3DXMatrixIdentity(&m_ViewMatrix);

    /* 직교투영행렬을 만들어준다. */
    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 0.f);

    return S_OK;
}

void CCircle_Select::Priority_Update(_float fTimeDelta)
{
}

void CCircle_Select::Update(_float fTimeDelta)
{
    if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
        return;

    //if ((dynamic_cast<CCommand_Circle*>(m_pParentUI)->Get_MouseOnDir() != nullptr))
    //{
    //    if(*(dynamic_cast<CCommand_Circle*>(m_pParentUI)->Get_MouseOnDir() ) != CCommand_Circle::DIREND)
    //    {
    //        m_bActivate = true;
    //        Turn_Select();
    //    }
    //}
    //else
    //    m_bActivate = false; 

    if (dynamic_cast<CCommand_Circle*>(m_pParentUI)->Get_MouseOnDir() != CCommand_Circle::DIREND)
    {
        m_bActivate = true;
        Turn_Select();
    }
    else
      m_bActivate = false; 


}

void CCircle_Select::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CCircle_Select::Render()
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

HRESULT CCircle_Select::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    return S_OK;
}

HRESULT CCircle_Select::Reset_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    return S_OK;
}

void CCircle_Select::Turn_Select()
{
    switch (dynamic_cast<CCommand_Circle*>(m_pParentUI)->Get_MouseOnDir())
    {
    case CCommand_Circle::LEFT:
        m_pTransformCom->Rotation (_float3(0.0f,0.0f, 1.f),90.f);
        break;
    case CCommand_Circle::RIGHT:
          m_pTransformCom->Rotation(_float3(0.0f, 0.0f, 1.f), -90.f);
        break;
    case CCommand_Circle::UP:
        m_pTransformCom->Rotation(_float3(0.0f, 0.0f, 1.f), 180.f);
        break;
    case CCommand_Circle::DOWN:
        m_pTransformCom->Rotation(_float3(0.0f, 0.0f, 1.f),90.f);
        break;

    default:
        m_bActivate = false;
        break;
    }
}

HRESULT CCircle_Select::Ready_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(Client::LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_BackGround_Select"),
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

CCircle_Select* CCircle_Select::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CCircle_Select* pInstance = new CCircle_Select(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCircle_Select"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCircle_Select::Clone(void* pArg)
{
    CCircle_Select* pInstance = new CCircle_Select(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CCircle_Select"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCircle_Select::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
