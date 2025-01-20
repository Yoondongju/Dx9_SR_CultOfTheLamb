#include "PressEtoPickCard.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include <AlphabetButton.h>
#include <TarotNPC.h>

CPressEtoPickCard::CPressEtoPickCard(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CUI{ pGraphic_Device }
{
}

CPressEtoPickCard::CPressEtoPickCard(const CPressEtoPickCard& Prototype)
    : CUI{ Prototype }
{
}

HRESULT CPressEtoPickCard::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPressEtoPickCard::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fSizeX = 300.f;   //크기
    m_fSizeY = 80.0f;

    m_fX = g_iWinSizeX * 0.5f;   //위치
    m_fY = 590.f;

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
        -m_fY + g_iWinSizeY * 0.5f, 0.f));

    D3DXMatrixIdentity(&m_ViewMatrix);

    /* 직교투영행렬을 만들어준다. */
    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

    RECT TextBox = {};
    TextBox.left = (LONG)(m_fX - 40.f);
    TextBox.top = (LONG)(m_fY - 10.f);
    TextBox.right = (LONG)(m_fX + 300.f);
    TextBox.bottom = (LONG)(m_fY + 20.f);
    m_rText = TextBox;

    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Alphabet_Button_UI"));
    if (pGameObj != nullptr)
    {
        CAlphabetButton::AlphabetBtnType eType = CAlphabetButton::E;
        CUI* pChild = dynamic_cast<CUI*>(pGameObj->Clone(&eType));
        if (pChild != nullptr)
            m_childUI_List.push_back(pChild);

        pChild->SetParentUI(this);
        pChild->Set_fX(m_fX - 100.f);
        pChild->Set_fY(m_fY + 5.f);
    }


    BTNE_DESC* pDesc = static_cast<BTNE_DESC*>(pArg);

    m_BtnEDesc.iLevelIndex = pDesc->iLevelIndex;
    m_BtnEDesc.iType = pDesc->iType;

    m_bActivate = false;;

    return S_OK;
}

void CPressEtoPickCard::Priority_Update(_float fTimeDelta)
{
}

void CPressEtoPickCard::Update(_float fTimeDelta)
{
    if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
        return;

    if (dynamic_cast<CTarotNPC*>(m_pGameInstance->Find_Object(m_BtnEDesc.iLevelIndex, TEXT("Layer_TarotNPC"), 0)) == nullptr)
        return;
    else if (m_BtnEDesc.iType == 0)
        m_bActivate = dynamic_cast<CTarotNPC*>(m_pGameInstance->Find_Object(m_BtnEDesc.iLevelIndex, TEXT("Layer_TarotNPC"), 0))->Get_CanSelectCard();
    else if (m_BtnEDesc.iType == 1)
        m_bActivate = true;

    for (auto& pChild : m_childUI_List)
    {
        pChild->Update(fTimeDelta);
    }
}

void CPressEtoPickCard::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

    for (auto& pChild : m_childUI_List)
    {
        pChild->Late_Update(fTimeDelta);
    }
}

HRESULT CPressEtoPickCard::Render()
{
    m_pVIBufferCom->Filp_Texture(m_isFilp);

    if (m_bActivate)
    {
        if (FAILED(m_pTransformCom->Bind_Transform()))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_Texture(0)))
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

        switch (m_BtnEDesc.iType)
        {
        case 0:
            m_pGameInstance->RenderText(L"타로 카드 뽑기", &m_rText, D3DCOLOR_XRGB(255, 255, 255), FONT_TYPE::MEDIUM);
            break;
        case 1:
            m_pGameInstance->RenderText(L"데려가기", &m_rText, D3DCOLOR_XRGB(255, 255, 255), FONT_TYPE::MEDIUM);
            break;
        default:
            break;
        }
       
    }

    for (auto& pChild : m_childUI_List)
    {
        pChild->Render();
    }
    return S_OK;
}

HRESULT CPressEtoPickCard::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    return S_OK;
}

HRESULT CPressEtoPickCard::Reset_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    return S_OK;
}

HRESULT CPressEtoPickCard::Ready_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_SpeechBubble"),
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

CPressEtoPickCard* CPressEtoPickCard::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CPressEtoPickCard* pInstance = new CPressEtoPickCard(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CPressEtoPickCard"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPressEtoPickCard::Clone(void* pArg)
{
    CPressEtoPickCard* pInstance = new CPressEtoPickCard(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CPressEtoPickCard"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPressEtoPickCard::Free()
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
