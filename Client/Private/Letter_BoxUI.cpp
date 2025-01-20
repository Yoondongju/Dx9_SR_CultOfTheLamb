#include "Letter_BoxUI.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "AlphabetButton.h"

CLetter_BoxUI::CLetter_BoxUI(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CUI{ pGraphic_Device }
{
}

CLetter_BoxUI::CLetter_BoxUI(const CLetter_BoxUI& Prototype)
    : CUI{ Prototype }
{
}

HRESULT CLetter_BoxUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLetter_BoxUI::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fSizeX = g_iWinSizeX;   //크기
    m_fSizeY = 100.f;

    m_fX = g_iWinSizeX*0.5f;   //위치
    m_fY = g_iWinSizeY - m_fSizeY*0.5f;

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

    D3DXMatrixIdentity(&m_ViewMatrix);

    /* 직교투영행렬을 만들어준다. */
    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

    RECT TextBox = {};
    TextBox.left = (LONG)(190.f);
    TextBox.top = (LONG)(m_fY - 45.f);
    TextBox.right = (LONG)(280.f + 100.f);
    TextBox.bottom = (LONG)(m_fY) ;
    m_rText = TextBox;

    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Alphabet_Button_UI"));
    if (pGameObj != nullptr)
    {
        CAlphabetButton::AlphabetBtnType eType = CAlphabetButton::F;
        CUI* pChild = dynamic_cast<CUI*>(pGameObj->Clone(&eType));
        if (pChild != nullptr)
            m_childUI_List.push_back(pChild);

        pChild->SetParentUI(this);
        pChild->Set_fX(m_fX - 500.f);
        pChild->Set_fY(m_fY - 40.f);
    }

    m_bActivate = false;

    return S_OK;
}

void CLetter_BoxUI::Priority_Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
    {
        pChild->Priority_Update(fTimeDelta);
    }
}

void CLetter_BoxUI::Update(_float fTimeDelta)
{
    m_bActivate = m_pParentUI->GetActivate();

    for (auto& pChild : m_childUI_List)
    {
        pChild->Update(fTimeDelta);
    }
}

void CLetter_BoxUI::Late_Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
    {
        pChild->Late_Update(fTimeDelta);
    }
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CLetter_BoxUI::Render()
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

        m_pGameInstance->RenderText(L"뒤로", &m_rText, D3DCOLOR_XRGB(255, 255, 255), FONT_TYPE::SMALL);
    }

    for (auto& pChild : m_childUI_List)
    {
        pChild->Render();
    }

    return S_OK;
}

HRESULT CLetter_BoxUI::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    return S_OK;
}

HRESULT CLetter_BoxUI::Reset_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    return S_OK;
}

HRESULT CLetter_BoxUI::Ready_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(Client::LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_LetterBox_Bottom"),
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

CLetter_BoxUI* CLetter_BoxUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CLetter_BoxUI* pInstance = new CLetter_BoxUI(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLetter_BoxUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLetter_BoxUI::Clone(void* pArg)
{

    CLetter_BoxUI* pInstance = new CLetter_BoxUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CLetter_BoxUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLetter_BoxUI::Free()
{
    __super::Free();

    for (auto& pChild : m_childUI_List)
    {
        Safe_Release(pChild);
    }
    m_childUI_List.clear();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
