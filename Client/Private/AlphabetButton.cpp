#include "AlphabetButton.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"

CAlphabetButton::CAlphabetButton(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CUI{ pGraphic_Device }
{
}

CAlphabetButton::CAlphabetButton(const CAlphabetButton& Prototype)
    : CUI{ Prototype }
{
}

HRESULT CAlphabetButton::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAlphabetButton::Initialize(void* pArg)
{
    AlphabetBtnType* _eType = static_cast<AlphabetBtnType*>(pArg);
    if (_eType != nullptr)
        m_eType = *_eType;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fSizeX = 40.f;   //Å©±â
    m_fSizeY = 40.f;

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

    D3DXMatrixIdentity(&m_ViewMatrix);
    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

    m_bActivate = false;

    return S_OK;
}

void CAlphabetButton::Priority_Update(_float fTimeDelta)
{
}

void CAlphabetButton::Update(_float fTimeDelta)
{
    if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
        return;

    m_bActivate = m_pParentUI->GetActivate();

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
}

void CAlphabetButton::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CAlphabetButton::Render()
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

HRESULT CAlphabetButton::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    return S_OK;
}

HRESULT CAlphabetButton::Reset_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    return S_OK;
}


HRESULT CAlphabetButton::Ready_Components()
{
    switch (m_eType)
    {
    case E:
        if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_InterAction_E"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
            return E_FAIL;
        break;
    case F:
        if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_InterAction_F"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
            return E_FAIL;
        break;
    }

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

CAlphabetButton* CAlphabetButton::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CAlphabetButton* pInstance = new CAlphabetButton(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAlphabetButton"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAlphabetButton::Clone(void* pArg)
{
    CAlphabetButton* pInstance = new CAlphabetButton(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CAlphabetButton"));
        Safe_Release(pInstance);
    }

    return pInstance;
    return nullptr;
}

void CAlphabetButton::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
