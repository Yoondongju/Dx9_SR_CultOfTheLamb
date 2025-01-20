#include "TarotPanel.h"
#include "GameInstance.h"
#include "TarotCard.h"
#include <Player.h>
CTarotPanel::CTarotPanel(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CUI(pGraphic_Device)
{
}

CTarotPanel::CTarotPanel(const CTarotPanel& Prototype)
    :CUI(Prototype)
{
}


HRESULT CTarotPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTarotPanel::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_bActivate = false;

    LEVELID* eLevel = (LEVELID*)pArg;
    iLevelID = *eLevel;

    //던전마다 나올 카드 지정
    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_UI_TarotCard"));
    switch (iLevelID)
    {
    case Client::LEVEL_DUNGEON0:
        if (pGameObj != nullptr)
        {
            CTarotCard::CARDTYPE eType = CTarotCard::ATTACKPOWERUP;
            CUI* pChild = dynamic_cast<CUI*>(pGameObj->Clone(&eType));
            if (pChild == nullptr)
                return E_FAIL;

            m_childUI_List.push_back(pChild);
            pChild->SetParentUI(this);
            pChild->Set_fX(g_iWinSizeX / 6.f);
            pChild->Set_fY(g_iWinSizeY * 0.5f);
        }
        pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_UI_TarotCard"));
        if (pGameObj != nullptr)
        {
            CTarotCard::CARDTYPE eType = CTarotCard::BONUSHEART;
            CUI* pChild = dynamic_cast<CUI*>(pGameObj->Clone(&eType));
            if (pChild == nullptr)
                return E_FAIL;

            m_childUI_List.push_back(pChild);
            pChild->SetParentUI(this);
            pChild->Set_fX(790.f);
            pChild->Set_fY(g_iWinSizeY * 0.5f);
        }
        break;
    case Client::LEVEL_DUNGEON1:
        pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_UI_TarotCard"));
        if (pGameObj != nullptr)
        {
            CTarotCard::CARDTYPE eType = CTarotCard::DOUBLEBULLET;
            CUI* pChild = dynamic_cast<CUI*>(pGameObj->Clone(&eType));
            if (pChild == nullptr)
                return E_FAIL;

            m_childUI_List.push_back(pChild);
            pChild->SetParentUI(this);
            pChild->Set_fX(g_iWinSizeX /6.f);
            pChild->Set_fY(g_iWinSizeY * 0.5f);
        }
        pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_UI_TarotCard"));
        if (pGameObj != nullptr)
        {
            CTarotCard::CARDTYPE eType = CTarotCard::HEAL;
            CUI* pChild = dynamic_cast<CUI*>(pGameObj->Clone(&eType));
            if (pChild == nullptr)
                return E_FAIL;

            m_childUI_List.push_back(pChild);
            pChild->SetParentUI(this);
            pChild->Set_fX(790.f);
            pChild->Set_fY( g_iWinSizeY *0.5f);
        }
        break;
    case Client::LEVEL_DUNGEON2:
        pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_UI_TarotCard"));
        if (pGameObj != nullptr)
        {
            CTarotCard::CARDTYPE eType = CTarotCard::ATTACKPOWERUP;
            CUI* pChild = dynamic_cast<CUI*>(pGameObj->Clone(&eType));
            if (pChild == nullptr)
                return E_FAIL;

            m_childUI_List.push_back(pChild);
            pChild->SetParentUI(this);
            pChild->Set_fX(g_iWinSizeX / 6.f);
            pChild->Set_fY(g_iWinSizeY * 0.5f);
        }
        pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_UI_TarotCard"));
        if (pGameObj != nullptr)
        {
            CTarotCard::CARDTYPE eType = CTarotCard::SPEEDUP;
            CUI* pChild = dynamic_cast<CUI*>(pGameObj->Clone(&eType));
            if (pChild == nullptr)
                return E_FAIL;

            m_childUI_List.push_back(pChild);
            pChild->SetParentUI(this);
            pChild->Set_fX(790.f);
            pChild->Set_fY(g_iWinSizeY * 0.5f);
        }
        break;
    }    

    return S_OK;
}

void CTarotPanel::Priority_Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
    {
        pChild->Priority_Update(fTimeDelta);
    }
}

void CTarotPanel::Update(_float fTimeDelta)
{
     

    for (auto& pChild : m_childUI_List)
    {
        pChild->Update(fTimeDelta);
    }

}

void CTarotPanel::Late_Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
    {
        pChild->Late_Update(fTimeDelta);
    }
}

HRESULT CTarotPanel::Render()
{
    for (auto& pChild : m_childUI_List)
    {
        pChild->Render();
    }
    return S_OK;
}

HRESULT CTarotPanel::Ready_Components()
{
    return S_OK;
}

CTarotPanel* CTarotPanel::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CTarotPanel* pInstance = new CTarotPanel(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTarotPanel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTarotPanel::Clone(void* pArg)
{
    CTarotPanel* pInstance = new CTarotPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CTarotPanel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTarotPanel::Free()
{
    __super::Free();

    for (auto& pChild : m_childUI_List)
    {
        Safe_Release(pChild);
    }
    m_childUI_List.clear();
}
