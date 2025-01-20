#include "Follower_Circle_Button.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Command_Circle.h"
#include "Follower_Command_UI.h"
#include <Follower.h>

#include "Quest.h"

CFollower_Circle_Button::CFollower_Circle_Button(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CUI{ pGraphic_Device }
{
}

CFollower_Circle_Button::CFollower_Circle_Button(const CFollower_Circle_Button& Prototype)
    : CUI{ Prototype }
{
}

HRESULT CFollower_Circle_Button::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFollower_Circle_Button::Initialize(void* pArg)
{
    CircleBtnType* eType = static_cast<CircleBtnType*>(pArg);
    if (eType != nullptr)
        m_eType = *eType;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fSizeX = 60.f;   //크기
    m_fSizeY = 60.f;

    switch (m_eType)
    {
    case Client::CFollower_Circle_Button::MINING:
        m_fOffsetX = -120.f;
        m_fOffsetY = 0.f;
        break;
    case Client::CFollower_Circle_Button::PRAY:
        m_fOffsetX = 3.f;
        m_fOffsetY = -120.f;
        break;
    case Client::CFollower_Circle_Button::AXE:
        m_fOffsetX = +120.f;
        m_fOffsetY = 0.f;
        break;
    }
  
    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

    D3DXMatrixIdentity(&m_ViewMatrix);
    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

    m_bActivate = false;
    m_iBindNum = 0;

    return S_OK;
}

void CFollower_Circle_Button::Priority_Update(_float fTimeDelta)
{
}

void CFollower_Circle_Button::Update(_float fTimeDelta)
{
    if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
        return;


    m_fX = m_pParentUI->Get_fX() + m_fOffsetX;
    m_fY = m_pParentUI->Get_fY() + m_fOffsetY;

    if (m_bActivate)
        m_fActiveTimer += fTimeDelta;
    else
        m_fActiveTimer = 0.f;

    if(m_bActivate)
    {
        MouseOnCheck();
        Change_Image();
    }
}

void CFollower_Circle_Button::Late_Update(_float fTimeDelta)
{
    if (m_fActiveTimer > 0.5f)
        Compute_MouseClick();

    m_bActivate = m_pParentUI->GetActivate();

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CFollower_Circle_Button::Render()
{
    if (m_bActivate)
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
    }
    for (auto& pChild : m_childUI_List)
    {
        pChild->Render();
    }
    return S_OK;
}

HRESULT CFollower_Circle_Button::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    return S_OK;
}

HRESULT CFollower_Circle_Button::Reset_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    return S_OK;
}

HRESULT CFollower_Circle_Button::Ready_Components()
{
    switch (m_eType)
    {
    case Client::CFollower_Circle_Button::MINING:
        if (FAILED(__super::Add_Component(Client::LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Mining_Icon"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
            return E_FAIL;
        break;
    case Client::CFollower_Circle_Button::PRAY:
        if (FAILED(__super::Add_Component(Client::LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Pray_Icon"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
            return E_FAIL;
        break;
    case Client::CFollower_Circle_Button::AXE:
        if (FAILED(__super::Add_Component(Client::LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Axe_Icon"),
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

void CFollower_Circle_Button::Change_Image()
{
    if (m_bMouseOn)
    {
        m_iCulMousOnTime++;

        if(m_iCulMousOnTime == 1)
            m_pGameInstance->Play_Sound(TEXT("UI_Change_Selection.wav"), SOUND_UI, g_fEffectVolume);
        m_fSizeX = 100.f;   //크기
        m_fSizeY = 100.f;
        
        switch (m_eType)
        {
        case Client::CFollower_Circle_Button::MINING:
            dynamic_cast<CCommand_Circle*>(m_pParentUI)->Set_MouseOnDir(CCommand_Circle::LEFT);
            break;
        case Client::CFollower_Circle_Button::PRAY:
            m_iBindNum = 1;
            dynamic_cast<CCommand_Circle*>(m_pParentUI)->Set_MouseOnDir(CCommand_Circle::UP);
            break;
        case Client::CFollower_Circle_Button::AXE:
            dynamic_cast<CCommand_Circle*>(m_pParentUI)->Set_MouseOnDir(CCommand_Circle::RIGHT);
            break;

        }
    }
 
    if(!m_bMouseOn)
    {
        m_iCulMousOnTime = 0;
        m_fSizeX = 60.f;   //크기
        m_fSizeY = 60.f;
        if (m_eType == PRAY)
        {
            m_iBindNum = 0;
        }
    }
}

void CFollower_Circle_Button::Compute_MouseClick()
{
    if (m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP && m_bMouseOn)
    {
        m_pGameInstance->Play_Sound(TEXT("UI_Confirm_Selection.wav"), SOUND_UI, g_fEffectVolume);

        CUI* pUI = dynamic_cast<CUI*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Follower_Command_UI"), 0));
        if (pUI == nullptr)
            return;

        dynamic_cast<CFollower*>(dynamic_cast<CFollower_Command_UI*>(pUI)->Get_Follower())->Reset_Command();

        switch (m_eType)
        {
        case Client::CFollower_Circle_Button::MINING:
            dynamic_cast<CFollower*>(dynamic_cast<CFollower_Command_UI*>(pUI)->Get_Follower())->Set_IsMining(true);
            break;
        case Client::CFollower_Circle_Button::PRAY:
            dynamic_cast<CFollower*>(dynamic_cast<CFollower_Command_UI*>(pUI)->Get_Follower())->Set_IsPray(true);
            break;
        case Client::CFollower_Circle_Button::AXE:
            dynamic_cast<CFollower*>(dynamic_cast<CFollower_Command_UI*>(pUI)->Get_Follower())->Set_IsAxe(true);
            break;
        }

        CQuest::Get_Instance()->Succeed_Quest(L"신도 일 시키기");

        dynamic_cast<CFollower*>(dynamic_cast<CFollower_Command_UI*>(pUI)->Get_Follower())->Get_Fsm()->Change_State(CFollower::WALK);
    }
}

CFollower_Circle_Button* CFollower_Circle_Button::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CFollower_Circle_Button* pInstance = new CFollower_Circle_Button(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFollower_Circle_Button"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFollower_Circle_Button::Clone(void* pArg)
{
    CFollower_Circle_Button* pInstance = new CFollower_Circle_Button(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CFollower_Circle_Button"));
        Safe_Release(pInstance);
    }

    return pInstance;
    return nullptr;
}

void CFollower_Circle_Button::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
