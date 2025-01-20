#include "MouseClickUI.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include <Player.h>
#include "Layer.h"
#include <Follower.h>

CMouseClickUI::CMouseClickUI(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CUI{ pGraphic_Device }
{
}

CMouseClickUI::CMouseClickUI(const CMouseClickUI& Prototype)
    : CUI{ Prototype }
{
}

HRESULT CMouseClickUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMouseClickUI::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fSizeX = 400.f;   //크기
    m_fSizeY = 100.0f;

    m_fX = m_fSizeX * 0.5f + 400;   //위치
    m_fY = m_fSizeY * 0.5f + 500;

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

    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_MouseIcon"));
    if (pGameObj != nullptr)
    {
        CUI* pChild = dynamic_cast<CUI*>(pGameObj->Clone(pArg));
        if (pChild != nullptr)
            m_childUI_List.push_back(pChild);

        pChild->SetParentUI(this);
        pChild->Set_fX(m_fX - 100.f);
        pChild->Set_fY(m_fY + 5.f);
    }

    m_bActivate = false;

    return S_OK;
}

void CMouseClickUI::Priority_Update(_float fTimeDelta)
{
    
}

void CMouseClickUI::Update(_float fTimeDelta)
{
    if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
        return;

    if (dynamic_cast<CUI*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Follower_Command_UI"), 0))->GetActivate() == false)
    {
        m_bActivate = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_CanMouseClick();

        Calculate_Active();
    }
    else
        m_bActivate = false;

    for (auto& pChild : m_childUI_List)
    {
        pChild->Update(fTimeDelta);
    }
}

void CMouseClickUI::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

    for (auto& pChild : m_childUI_List)
    {
        pChild->Late_Update(fTimeDelta);
    }
}

HRESULT CMouseClickUI::Render()
{
    m_pVIBufferCom->Filp_Texture(m_isFilp);

    if(m_bActivate)
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

        m_pGameInstance->RenderText(L"대화 : 마우스 클릭", &m_rText, D3DCOLOR_XRGB(255, 255, 255), FONT_TYPE::MEDIUM);
    }

    for (auto& pChild : m_childUI_List)
    {
        pChild->Render();
    }
    return S_OK;
}

HRESULT CMouseClickUI::SetUp_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

    /* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 225);

    return S_OK;
}

HRESULT CMouseClickUI::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    return S_OK;
}

HRESULT CMouseClickUI::Ready_Components()
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

void CMouseClickUI::Calculate_Active()
{
    CLayer* pLayer = dynamic_cast<CLayer*>(m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Follower")));
    if (pLayer == nullptr)
        return;

    for (auto& pChild : pLayer->Get_ObjectList())
    {
        if (dynamic_cast<CFollower*>(pChild)->Get_Distance_with_Player() < m_fMinDistance && dynamic_cast<CFollower*>(pChild)->Get_isIndo() == true)
        {
            m_fMinDistance = dynamic_cast<CFollower*>(pChild)->Get_Distance_with_Player();
        }

    }

    if (m_fMinDistance < 3.f)
    {
       dynamic_cast<CPlayer*>( m_pGameInstance->Find_Player()) ->Set_CanMouseClick( true);
        m_fMinDistance = 10.f;
    }
    else
    {
        dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_CanMouseClick(false);
        m_fMinDistance = 10.f;
    }
}

CMouseClickUI* CMouseClickUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

    CMouseClickUI* pInstance = new CMouseClickUI(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMouseClickUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMouseClickUI::Clone(void* pArg)
{

    CMouseClickUI* pInstance = new CMouseClickUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CMouseClickUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMouseClickUI::Free()
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
