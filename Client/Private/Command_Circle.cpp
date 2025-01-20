#include "Command_Circle.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include <Follower_Circle_Button.h>

CCommand_Circle::CCommand_Circle(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CUI{ pGraphic_Device }
{
}

CCommand_Circle::CCommand_Circle(const CCommand_Circle& Prototype)
    : CUI{ Prototype }
{
}

HRESULT CCommand_Circle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCommand_Circle::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_bActivate = false;

    m_fSizeX =350.f;   //크기 

    m_fSizeY = 350.f;

    m_fX = m_fSizeX * 0.5f + 460;   //위치
    m_fY = m_fSizeY * 0.5f + 25.f;

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
        -m_fY + g_iWinSizeY * 0.5f, 0.f));

    D3DXMatrixIdentity(&m_ViewMatrix);

    /* 직교투영행렬을 만들어준다. */
    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

    RECT TextBox = {};
    TextBox.left = (LONG)(590.f);
    TextBox.top = (LONG)(146.f);
    TextBox.right = (LONG)(690.f);
    TextBox.bottom = (LONG)(180.f );
    m_rActionNameRect = TextBox;

    TextBox.left = (LONG)(570.f);
    TextBox.top = (LONG)(260.f);
    TextBox.right = (LONG)(710.f);
    TextBox.bottom = (LONG)(300.f);
    m_rActionExplainRect = TextBox;

    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Eye"));
    if (pGameObj != nullptr)
    {
        CUI* pChild = dynamic_cast<CUI*>(pGameObj->Clone(pArg));
        if (pChild != nullptr)
            m_childUI_List.push_back(pChild);
    }

    pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Follower_Circle_Button"));
    if (pGameObj != nullptr)
    {
        CFollower_Circle_Button::CircleBtnType eType = CFollower_Circle_Button::MINING;
        CUI* pChild = dynamic_cast<CUI*>(pGameObj->Clone(&eType));
        if (pChild != nullptr)
            m_childUI_List.push_back(pChild);

        eType = CFollower_Circle_Button::AXE;
        pChild = dynamic_cast<CUI*>(pGameObj->Clone(&eType));
        if (pChild != nullptr)
            m_childUI_List.push_back(pChild);

        eType = CFollower_Circle_Button::PRAY;
        pChild = dynamic_cast<CUI*>(pGameObj->Clone(&eType));
        if (pChild != nullptr)
            m_childUI_List.push_back(pChild);

    }

    for (auto& pChild : m_childUI_List)
    {
        pChild->SetParentUI(this);
        pChild->Set_fX(m_fX);
        pChild->Set_fY(m_fY);
    }
    
    return S_OK;
}

void CCommand_Circle::Priority_Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
    {
        pChild->Priority_Update(fTimeDelta);
    }
}

void CCommand_Circle::Update(_float fTimeDelta)
{
    m_bActivate = m_pParentUI->GetActivate();

    if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
        return;

    for (auto& pChild : m_childUI_List)
    {
         pChild->Update(fTimeDelta);

         if (pChild->IsMouseOn())
         {
             break;
         }

         m_eMouseDir = DIREND;
    }

    if(m_eMouseDir == RIGHT)
        m_iBindNum = 3;
    else if (m_eMouseDir == LEFT)
        m_iBindNum = 2;
    else if (m_eMouseDir == UP)
        m_iBindNum = 4;
    else
        m_iBindNum = 0;

}

void CCommand_Circle::Late_Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
    {
        pChild->Late_Update(fTimeDelta);
    }
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

}

HRESULT CCommand_Circle::Render()
{
    if( m_bActivate)
    {
        m_pVIBufferCom->Filp_Texture(m_isFilp);

        if (FAILED(m_pTransformCom->Bind_Transform()))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_Texture(m_iBindNum)))
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

        switch (m_eMouseDir)
        {
        case Client::CCommand_Circle::RIGHT:
            m_pGameInstance->RenderText(L"나무 베기", &m_rActionNameRect, D3DCOLOR_XRGB(255, 255, 255), Engine::MEDIUM);
            m_pGameInstance->RenderText(L"나무를 베어 목제를 \n   수집합니다.", &m_rActionExplainRect, D3DCOLOR_XRGB(255, 255, 255), Engine::SMALL);
            break;
        case Client::CCommand_Circle::LEFT:
            m_pGameInstance->RenderText(L" 돌 채광", &m_rActionNameRect, D3DCOLOR_XRGB(255, 255, 255), Engine::MEDIUM);
            m_pGameInstance->RenderText(L" 돌무더기를 치워 \n 돌을 수집합니다.", &m_rActionExplainRect, D3DCOLOR_XRGB(255, 255, 255), Engine::SMALL);
            break;
        case Client::CCommand_Circle::UP:
            m_pGameInstance->RenderText(L"   숭배", &m_rActionNameRect, D3DCOLOR_XRGB(255, 255, 255), Engine::MEDIUM);
            m_pGameInstance->RenderText(L"성지에서 숭배를 해 \n헌신을 생성합니다.", &m_rActionExplainRect, D3DCOLOR_XRGB(255, 255, 255), Engine::SMALL);
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

HRESULT CCommand_Circle::SetUp_RenderState()
{

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    return S_OK;
}

HRESULT CCommand_Circle::Reset_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    return S_OK;
}

HRESULT CCommand_Circle::Ready_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(Client::LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_BackGround"),
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

CCommand_Circle* CCommand_Circle::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CCommand_Circle* pInstance = new CCommand_Circle(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCommand_Circle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCommand_Circle::Clone(void* pArg)
{

    CCommand_Circle* pInstance = new CCommand_Circle(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CCommand_Circle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCommand_Circle::Free()
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
