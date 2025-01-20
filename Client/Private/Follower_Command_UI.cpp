#include "Follower_Command_UI.h"
#include "GameInstance.h"
#include <Player.h>
#include <Camera.h>

CFollower_Command_UI::CFollower_Command_UI(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CUI(pGraphic_Device)
{
}

CFollower_Command_UI::CFollower_Command_UI(const CFollower_Command_UI& Prototype)
    :CUI(Prototype)
{
}

HRESULT CFollower_Command_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFollower_Command_UI::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_bActivate = false;

    //Circle
    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Follower_Command_Circle"));
    if (pGameObj != nullptr)
    {
        CUI* pChild = dynamic_cast<CUI*>(pGameObj->Clone(pArg));
        if (pChild != nullptr)
            m_childUI_List.push_back(pChild);

        pChild->SetParentUI(this);
    }

    //LetterBox
   pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Letter_BoxUI"));
    if (pGameObj != nullptr)
    {
        CUI* pChild = dynamic_cast<CUI*>(pGameObj->Clone(pArg));
        if (pChild != nullptr)
            m_childUI_List.push_back(pChild);

        pChild->SetParentUI(this);
    }

    return S_OK;
}

void CFollower_Command_UI::Priority_Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
    {
        pChild->Priority_Update(fTimeDelta);
    }
}

void CFollower_Command_UI::Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
    {
        pChild->Update(fTimeDelta);
    }
}

void CFollower_Command_UI::Late_Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
    {
        pChild->Late_Update(fTimeDelta);
    }
}

HRESULT CFollower_Command_UI::Render()
{
    for (auto& pChild : m_childUI_List)
    {
        pChild->Render();
    }
    return S_OK;
}


HRESULT CFollower_Command_UI::Ready_Components()
{
    return S_OK;
}

CFollower_Command_UI* CFollower_Command_UI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CFollower_Command_UI* pInstance = new CFollower_Command_UI(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFollower_Command_UI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFollower_Command_UI::Clone(void* pArg)
{
    CFollower_Command_UI* pInstance = new CFollower_Command_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CFollower_Command_UI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFollower_Command_UI::Free()
{
    __super::Free();

    for (auto& pChild : m_childUI_List)
    {
        Safe_Release(pChild);
    }
    m_childUI_List.clear();
}
