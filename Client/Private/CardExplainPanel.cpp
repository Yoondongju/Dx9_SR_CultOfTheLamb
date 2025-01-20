#include "CardExplainPanel.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include <TarotCard.h>
#include <TarotPanel.h>

CCardExplainPanel::CCardExplainPanel(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CUI{ pGraphic_Device }
{
}

CCardExplainPanel::CCardExplainPanel(const CCardExplainPanel& Prototype)
    : CUI{ Prototype }
{
}

HRESULT CCardExplainPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCardExplainPanel::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fSizeX = 250.f;   //크기
    m_fSizeY = 300.f;

    m_fX = 0.f;
    m_fY = 0.f;

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.8f));

    D3DXMatrixIdentity(&m_ViewMatrix);
    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

    m_bActivate = false;

    RECT TextBox = {};
    TextBox.left = (LONG)(m_fX - 80.f);
    TextBox.top = (LONG)(m_fY - 110.f);
    TextBox.right = (LONG)(m_fX + 80.f);
    TextBox.bottom = (LONG)(m_fY - 90.f);
    m_rCardName = TextBox;

    TextBox.left = (LONG)(m_fX - 90.f);
    TextBox.top = (LONG)(m_fY  + 30.f);
    TextBox.right = (LONG)(m_fX + 90.f);
    TextBox.bottom = (LONG)(m_fY + 110.f);
    m_rCardExplain = TextBox;

    return S_OK;
}

void CCardExplainPanel::Priority_Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
    {
        pChild->Priority_Update(fTimeDelta);
    }
}

void CCardExplainPanel::Update(_float fTimeDelta)
{
    if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
        return;

    if (dynamic_cast<CTarotPanel*>(m_pParentUI->GetParent())->Get_ExplainPanelOff())
        m_bActivate = false;

    if (dynamic_cast<CTarotPanel*>(m_pParentUI->GetParent())->Get_isClicked()==false &&dynamic_cast<CTarotCard*>(m_pParentUI)->Get_FinishExpand() == true)
    {
        m_bActivate = m_pParentUI->GetActivate();
        MoveRight(fTimeDelta);
    }
    else if(dynamic_cast<CTarotPanel*>(m_pParentUI->GetParent())->Get_isClicked() == false && dynamic_cast<CTarotCard*>(m_pParentUI)->Get_FinishExpand() == false)
    {
        m_fX = m_pParentUI->Get_fX() + 20.f;
        m_fY = m_pParentUI->Get_fY();
    }

    if (dynamic_cast<CTarotPanel*>(m_pParentUI->GetParent())->Get_isClicked())
    {
        m_bFinshSpread = false;
        MoveLeft(fTimeDelta);
    }

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.8f));

    m_rCardName.left = (LONG)(m_fX - 80.f);
    m_rCardName.top = (LONG)(m_fY - 110.f);
    m_rCardName.right = (LONG)(m_fX + 80.f);
    m_rCardName.bottom = (LONG)(m_fY - 90.f);
   
    m_rCardExplain.left = (LONG)(m_fX - 90.f);
    m_rCardExplain.top = (LONG)(m_fY + 30.f);
    m_rCardExplain.right = (LONG)(m_fX + 90.f);
    m_rCardExplain.bottom = (LONG)(m_fY + 110.f);

    for (auto& pChild : m_childUI_List)
    {
        pChild->Update(fTimeDelta);
    }

}

void CCardExplainPanel::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

    for (auto& pChild : m_childUI_List)
    {
        pChild->Late_Update(fTimeDelta);
    }
}

HRESULT CCardExplainPanel::Render()
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
        if(m_bFinshSpread)
        {
            switch (dynamic_cast<CTarotCard*>(m_pParentUI)->Get_CardType())
            {
            case CTarotCard::ATTACKPOWERUP:
                m_pGameInstance->RenderText(L"광신자의 분노", &m_rCardName, D3DCOLOR_XRGB(255, 255, 255), Engine::MEDIUM);
                m_pGameInstance->RenderText(L"공격력이 상승합니다.", &m_rCardExplain, D3DCOLOR_XRGB(255, 255, 255), Engine::SMALL);
                break;
            case CTarotCard::BONUSHEART:
                m_pGameInstance->RenderText(L" 저승의 심장", &m_rCardName, D3DCOLOR_XRGB(255, 255, 255), Engine::MEDIUM);
                m_pGameInstance->RenderText(L"기존 체력보다 \n먼저 소멸하는 푸른 하트 \n한 개를 얻습니다.\n(일회성)", &m_rCardExplain, D3DCOLOR_XRGB(255, 255, 255), Engine::SMALL);
                break;
            case CTarotCard::DOUBLEBULLET:
                m_pGameInstance->RenderText(L" 심연의 분열", &m_rCardName, D3DCOLOR_XRGB(255, 255, 255), Engine::MEDIUM);
                m_pGameInstance->RenderText(L"탄환 개수가 3개로  \n늘어납니다.", &m_rCardExplain, D3DCOLOR_XRGB(255, 255, 255), Engine::SMALL);
                break;
            case CTarotCard::HEAL:
                m_pGameInstance->RenderText(L" 영혼의 재생", &m_rCardName, D3DCOLOR_XRGB(255, 255, 255), Engine::MEDIUM);
                m_pGameInstance->RenderText(L"체력이 전부 회복됩니다.", &m_rCardExplain, D3DCOLOR_XRGB(255, 255, 255), Engine::SMALL);
                break;
            case CTarotCard::SPEEDUP:
                m_pGameInstance->RenderText(L" 순간의 축복", &m_rCardName, D3DCOLOR_XRGB(255, 255, 255), Engine::MEDIUM);
                m_pGameInstance->RenderText(L"이동 속도가 상승합니다.", &m_rCardExplain, D3DCOLOR_XRGB(255, 255, 255), Engine::SMALL);
            default:
                break;
            }
        }
    }
    for (auto& pChild : m_childUI_List)
    {
        pChild->Render();
    }
    return S_OK;
}

void CCardExplainPanel::MoveRight(_float fTimeDelta)
{
    if (m_fX < m_pParentUI->Get_fX() + 234.5f)
    {
        m_fX += 800.f* fTimeDelta;
    }
    else
    {
        m_fX = m_pParentUI->Get_fX() + 234.5f;
        m_bFinshSpread = true;
    }
}

void CCardExplainPanel::MoveLeft(_float fTimeDelta)
{
    if (m_fX > m_pParentUI->Get_fX() + 10.f)
    {
        m_fX -= 800.f * fTimeDelta;
    }
    else
    {
        dynamic_cast<CTarotPanel*>(m_pParentUI->GetParent())->Set_ExplainPanelOff(true);
        dynamic_cast<CTarotCard*>(m_pParentUI)->Set_StartReduce(true);
    }

}
HRESULT CCardExplainPanel::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    return S_OK;
}

HRESULT CCardExplainPanel::Reset_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    return S_OK;
}

HRESULT CCardExplainPanel::Ready_Components()
{
    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ExpainBox"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    /* For.Com_Transform */
    CTransform::TRANSFORM_DESC		TransformDesc{};

    TransformDesc.fSpeedPerSec = 3.0f;
    TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    return S_OK;
}

CCardExplainPanel* CCardExplainPanel::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CCardExplainPanel* pInstance = new CCardExplainPanel(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCardExplainPanel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCardExplainPanel::Clone(void* pArg)
{
    CCardExplainPanel* pInstance = new CCardExplainPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CCardExplainPanel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCardExplainPanel::Free()
{
    __super::Free();

    if (m_childUI_List.size() > 0)
    {
        for (auto& pChild : m_childUI_List)
        {
            Safe_Release(pChild);
        }
        m_childUI_List.clear();
    }

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
