#include "CIrcle_Eye.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"

CCircle_Eye::CCircle_Eye(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CUI(pGraphic_Device)
{
}

CCircle_Eye::CCircle_Eye(const CCircle_Eye& Prototype)
    :CUI(Prototype)
{
}

HRESULT CCircle_Eye::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCircle_Eye::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_bActivate = false;

    m_fSizeX = 150.f;   //크기
    m_fSizeY = 150.f;

    m_fOffsetX = 7.f;
    m_fOffsetY = 10.f;

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

    D3DXMatrixIdentity(&m_ViewMatrix);
    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

    //자식으로 눈동자
    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Pupil"));
    if (pGameObj != nullptr)
    {
        CUI* pChild = dynamic_cast<CUI*>(pGameObj->Clone(pArg));
        if (pChild != nullptr)
            m_childUI_List.push_back(pChild);

        pChild->SetParentUI(this);
      
    }
    m_iBindNum = 0;
    m_fTimer = 0.f;

    m_bPupilActive = false;
    return S_OK;
}

void CCircle_Eye::Priority_Update(_float fTimeDelta)
{
}

void CCircle_Eye::Update(_float fTimeDelta)
{
    m_bActivate = m_pParentUI->GetActivate();
    
    Move_Offset();

    m_fX = m_pParentUI->Get_fX() + m_fOffsetX;
    m_fY = m_pParentUI->Get_fY() + m_fOffsetY;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

    //시간에 따라 m_iBindnum 교체
    if (m_bActivate)
    {
        m_bPupilActive = true; 
        m_fTimer += fTimeDelta;
        if (m_fTimer > 3.f)
        {
            m_iBindNum = 1;
            m_bPupilActive = false;
        }
        if (m_fTimer > 3.5f)
        {
            m_iBindNum = 0;
            m_fTimer = 0.f;
            m_bPupilActive = true;
        }
    }
    else
    {
        m_iBindNum = 0;
        m_bPupilActive = false;
        m_fTimer = 0.f;
    }

    //이동 , 부모 위치 받아오기
    for (auto& pChild : m_childUI_List)
    {
        pChild->Set_fX(m_fX);
        pChild->Set_fY(m_fY);

        pChild->Update(fTimeDelta);
    }


}

void CCircle_Eye::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

    for (auto& pChild : m_childUI_List)
    {
        pChild->Late_Update(fTimeDelta);
    }
}

HRESULT CCircle_Eye::Render()
{
    if(m_bActivate)
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

        for (auto& pChild : m_childUI_List)
        {
            pChild->Render();
        }
    }
    return S_OK;
}

HRESULT CCircle_Eye::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    return S_OK;
}

HRESULT CCircle_Eye::Reset_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    return S_OK;
}

void CCircle_Eye::Move_Offset()
{
    if (dynamic_cast<CCommand_Circle*> (m_pParentUI) == nullptr)
        return;

    CCommand_Circle::MouseOnDir e_MouseDir = dynamic_cast<CCommand_Circle*> (m_pParentUI)->Get_MouseOnDir();

    switch (e_MouseDir)
    {
    case Client::CCommand_Circle::RIGHT:
        m_fOffsetX = 15.f;
        m_fOffsetY = 10.f;
        break;
    case Client::CCommand_Circle::LEFT:
        m_fOffsetX = -1.f;
        m_fOffsetY = 10.f;
        break;
    case Client::CCommand_Circle::UP:
        m_fOffsetX = 7.f;
        m_fOffsetY = 2.f;
        break;
    default:   
        m_fOffsetX = 7.f;
        m_fOffsetY = 10.f;
        break;
    }
}

HRESULT CCircle_Eye::Ready_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(Client::LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Eye"),
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

CCircle_Eye* CCircle_Eye::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CCircle_Eye* pInstance = new CCircle_Eye(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCircle_Eye"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCircle_Eye::Clone(void* pArg)
{
    CCircle_Eye* pInstance = new CCircle_Eye(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CCircle_Eye"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCircle_Eye::Free()
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
