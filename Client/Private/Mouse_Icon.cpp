#include "Mouse_Icon.h"

#include "GameInstance.h"

#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include <Player.h>


CMouse_Icon::CMouse_Icon(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CUI{ pGraphic_Device }
{
}

CMouse_Icon::CMouse_Icon(const CMouse_Icon& Prototype)
    : CUI{ Prototype }
{
}

HRESULT CMouse_Icon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMouse_Icon::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fSizeX = 60.f;   //크기
    m_fSizeY = 60.f;

    m_fX = 0;   //위치
    m_fY = 0;

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

    D3DXMatrixIdentity(&m_ViewMatrix);

    /* 직교투영행렬을 만들어준다. */
    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

    return S_OK;
}

void CMouse_Icon::Priority_Update(_float fTimeDelta)
{

}

void CMouse_Icon::Update(_float fTimeDelta)
{
    if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
        return;
    
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
}

void CMouse_Icon::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CMouse_Icon::Render()
{
    m_pVIBufferCom->Filp_Texture(m_isFilp);

    if (m_pParentUI->GetActivate())
    {
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

HRESULT CMouse_Icon::Ready_Components()
{

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(Client::LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Mouse_Icon"),
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


HRESULT CMouse_Icon::SetUp_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

    /* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 127);

    return S_OK;
}

HRESULT CMouse_Icon::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    return S_OK;
}


CMouse_Icon* CMouse_Icon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

    CMouse_Icon* pInstance = new CMouse_Icon(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMouse_Icon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMouse_Icon::Clone(void* pArg)
{

    CMouse_Icon* pInstance = new CMouse_Icon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CMouse_Icon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMouse_Icon::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
