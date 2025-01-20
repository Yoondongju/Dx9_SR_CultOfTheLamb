#include "TarotCard.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Player.h"
#include <TarotPanel.h>
#include "Quest.h"

CTarotCard::CTarotCard(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CUI{ pGraphic_Device }
{
}

CTarotCard::CTarotCard(const CTarotCard& Prototype)
    : CUI{ Prototype }
{
}

HRESULT CTarotCard::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTarotCard::Initialize(void* pArg)
{
    CARDTYPE* _eType = static_cast<CARDTYPE*>(pArg);
    if (_eType != nullptr)
        m_eType = *_eType;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fSizeX = 400.f;   //크기
    m_fSizeY = 400.f;
    
    m_fX = 0.f;
    m_fY = 0.f;

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.5f));

    D3DXMatrixIdentity(&m_ViewMatrix);
    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

    m_bActivate = false;
    m_bFirstActive = true;

    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_UI_CardExplainPanel"));
    CUI* pChild = dynamic_cast<CUI*>(pGameObj->Clone(pArg));
    if (pChild == nullptr)
        return E_FAIL;

    m_childUI_List.push_back(pChild);
    pChild->SetParentUI(this);
    
    return S_OK;
}

void CTarotCard::Priority_Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
    {
        pChild->Priority_Update(fTimeDelta);
    }
}

void CTarotCard::Update(_float fTimeDelta)
{
    if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
        return;

     m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.5f));

     if(m_bFinishExpand == false && m_bActivate)
     {
         Show();
         m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
         m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.5f));
     }
     else if(m_bFinishExpand == true && m_bActivate && m_bStartReduce == false)
     {
  
         MouseOnCheck();
         ShowSelected(); //마우스 ON에따른 이미지 변화

         Compute_MouseClick();
     }

    if (m_bStartReduce)
        Reduced(fTimeDelta);

    for (auto& pChild : m_childUI_List)
    {
        pChild->Update(fTimeDelta);
    }
}

void CTarotCard::Late_Update(_float fTimeDelta)
{
    m_bActivate = m_pParentUI->GetActivate();
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

    for (auto& pChild : m_childUI_List)
    {
        pChild->Late_Update(fTimeDelta);
    }
}

HRESULT CTarotCard::Render()
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
    for (auto& pChild : m_childUI_List)
    {
        pChild->Render();
    }
    return S_OK;
}

void CTarotCard::Show()
{
    if (m_bPlayCardShowSound == false)
    {
        m_pGameInstance->Play_Sound(TEXT("UI_TarotCardPull.wav"), SOUND_UI, g_fEffectVolume);
        m_bPlayCardShowSound = true;
    }
     m_iSizeChagneCount++;
     if (m_iSizeChagneCount < 8)
     {
         m_fSizeX += 2.f;
         m_fSizeY += 2.f;
     }
     else if (m_iSizeChagneCount < 16)
     {
         m_fSizeX -= 2.f;
         m_fSizeY -= 2.f;
     }
     else
         m_bFinishExpand = true;

}

void CTarotCard::Reduced(_float fTimeDelta)
{
    if (m_bPlayCardReduceSound == false && m_pGameInstance->Check_IsPlaying(SOUND_EFFECT)==true)
    {
        m_pGameInstance->Play_Sound(TEXT("UI_Tarot_Close.wav"), SOUND_EFFECT, g_fEffectVolume);
        m_bPlayCardReduceSound = true;
    }
    if (m_fSizeX > 20.f)
    {
        m_fSizeX -= 20.f;
        m_fSizeY -= 20.f;
        m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    }
    else
        m_pParentUI->SetActivate(false);
}

void CTarotCard::ShowSelected()
{
    if (m_bMouseOn)
    {
        if (m_bPlayCardMouseOnSound == false)
        {
            m_pGameInstance->Play_Sound(TEXT("UI_Change_Selection.wav"), SOUND_EFFECT, g_fEffectVolume);
            m_bPlayCardMouseOnSound = true;
        }
        m_fSizeX = 450.f;
        m_fSizeY = 450.f;
    }
    else
    {
        m_bPlayCardMouseOnSound = false;
        m_fSizeX = 400.f;
        m_fSizeY = 400.f;
    }
    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
}

void CTarotCard::Compute_MouseClick()
{
    // 마우스가 타로카드 위에 올라가있고 클릭했을때
    if (m_bMouseOn && m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
    {
        if (m_bPlayCardSelectSound == false)
        {
            m_pGameInstance->Play_Sound(TEXT("UI_Confirm_Selection.wav"), SOUND_EFFECT, g_fEffectVolume);
            m_bPlayCardSelectSound = true;
        }
        // 유아이 닫을라고 만들어둠
        // 플레이어에 있는 타로선택 bool 변수 true
        // 카드 타입 전달
        dynamic_cast<CTarotPanel*>(m_pParentUI)->Set_isClicked(true);
        dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Apply_Tarot(m_eType);



        if (m_eType == CTarotCard::BONUSHEART)
        {
            CUI::UI_DESC Desc = {};

            Desc.m_vPos = { g_iWinSizeX / 2.f + 10 , g_iWinSizeY / 2.f };
            Desc.m_vSize = { 150.f,150.f };
            Desc.m_iBindNum = 0;
            Desc.eUItype = UI_TYPE::UI_BAR;
            Desc.vTargetPos = { 250.f, 95.f };
            Desc.pReferenceOwner = m_pParentUI;         // 부모가 TaroPanel 이래

            if (FAILED(m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_PlayerHpUI"), sizeof(CUI::UI_DESC), &Desc)))
                MSG_BOX(L"Can't Create GreenPlayerHpUI");

        }


        CQuest::Get_Instance()->Succeed_Quest(L"타로 카드 선택하기");
    }
}

HRESULT CTarotCard::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    return S_OK;
}

HRESULT CTarotCard::Reset_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    return S_OK;
}

HRESULT CTarotCard::Ready_Components()
{
    switch (m_eType)
    {
    case Client::CTarotCard::ATTACKPOWERUP: 
        if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TarotCard_AttackPowerUp"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
            return E_FAIL;
        break;
    case Client::CTarotCard::BONUSHEART:
        if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TarotCard_BonusHeart"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
            return E_FAIL;
        break;
    case Client::CTarotCard::DOUBLEBULLET:
        if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TarotCard_DoubleBullet"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
            return E_FAIL;
        break;
    case Client::CTarotCard::HEAL:
        if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TarotCard_Heal"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
            return E_FAIL;
        break;
    case Client::CTarotCard::SPEEDUP:
        if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TarotCard_SpeedUp"),
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

CTarotCard* CTarotCard::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CTarotCard* pInstance = new CTarotCard(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTarotCard"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTarotCard::Clone(void* pArg)
{
    CTarotCard* pInstance = new CTarotCard(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CTarotCard"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTarotCard::Free()
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
