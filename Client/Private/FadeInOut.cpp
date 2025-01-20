#include "FadeInOut.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"

#include "Camera.h"

CFadeInOut::CFadeInOut(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CUI{ pGraphic_Device }
{
}

CFadeInOut::CFadeInOut(const CFadeInOut& Prototype)
    : CUI{ Prototype }
{
}

HRESULT CFadeInOut::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFadeInOut::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fSizeX = g_iWinSizeX;   //크기
    m_fSizeY = g_iWinSizeY;

    m_fX = g_iWinSizeX * 0.5f;
    m_fY= g_iWinSizeY * 0.5f;

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

    D3DXMatrixIdentity(&m_ViewMatrix);
    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);


    m_fAlpha = 0.f;

    return S_OK;
}

void CFadeInOut::Priority_Update(_float fTimeDelta)
{

}

void CFadeInOut::Update(_float fTimeDelta)
{

    //if (m_bStart_FadeInOut)
    //    FadeInOut(fTimeDelta);


    if (m_bStart_FadeIn)
        FadeIn(fTimeDelta);

    if (m_bStart_FadeOut)
        FadeOut(fTimeDelta);

    if (m_bBlink)
        Blink(fTimeDelta);
}

void CFadeInOut::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CFadeInOut::Render()
{
    if (m_bFadeSet && !m_bBlink && !dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Get_IsEnding()
        && !dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Get_IsShowCrew())         // Fade In   Or  Fade Out 끝났으면 그리지마
        return S_OK;

    m_pVIBufferCom->Filp_Texture(m_isFilp);

    if (FAILED(m_pTransformCom->Bind_Transform()))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Texture()))
        return E_FAIL;

    m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    _float4x4		WorldMatrix;
    WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());


    m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
    m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&m_ViewMatrix, &m_ViewMatrix), sizeof(_float4x4));
    m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&m_ProjMatrix, &m_ProjMatrix), sizeof(_float4x4));

    m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(0));
    m_pShaderCom->Set_Float("g_fFadeAlpha", m_fAlpha);
    m_pShaderCom->Set_Float("g_fTotalColor", m_fColor);

    m_pShaderCom->Begin_Shader(m_iShaderNum);

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    m_pShaderCom->End_Shader();

    return S_OK;
}

HRESULT CFadeInOut::Ready_Components()
{
    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Black"),
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

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

void CFadeInOut::FadeOut(_float fTimeDelta)
{
    if (m_fAlpha > 0 && m_fAlpha <= 1.f)
    {
        m_fAlpha -= m_fFadeTime * fTimeDelta;
        
        m_bFadeSet = false;
    }       
    else
    {
        m_bStart_FadeOut = false;
        m_fAlpha = 0.f;

        m_bFadeSet = true;
    }
}

void CFadeInOut::Blink(_float fTimeDelta)
{
    m_fBlinkTime += fTimeDelta;

    if (m_fBlinkTime < 0.2f)
    {
        m_fColor = 1.f;
        m_fAlpha = 1.f;
    }
    else
    {
        m_fColor = 0.f;
        m_fAlpha = 0.f;
        m_fBlinkTime = 0.f;
        m_bBlink = false;
    }

}

void CFadeInOut::FadeIn(_float fTimeDelta)
{
    if (m_fAlpha < 1.f && m_fAlpha >= 0.f)
    {
        m_fAlpha += m_fFadeTime * fTimeDelta;

        m_bFadeSet = false;
    }
       
    else
    {
        m_bStart_FadeIn = false;
        m_fAlpha = 1.f;

        m_bFadeSet = true;
    }
}

CFadeInOut* CFadeInOut::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CFadeInOut* pInstance = new CFadeInOut(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFadeInOut"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFadeInOut::Clone(void* pArg)
{
    CFadeInOut* pInstance = new CFadeInOut(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CFadeInOut"));
        Safe_Release(pInstance);
    }

    return pInstance;
    return nullptr;
}

void CFadeInOut::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
