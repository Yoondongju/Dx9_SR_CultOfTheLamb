#include "Enemy_Suicide.h"

CEnemy_Suicide::CEnemy_Suicide(LPDIRECT3DDEVICE9 pGraphic_Device)
{
}

CEnemy_Suicide::CEnemy_Suicide(const CEnemy_Suicide& Prototype)
{
}

HRESULT CEnemy_Suicide::Initialize_Prototype()
{
    return E_NOTIMPL;
}

HRESULT CEnemy_Suicide::Initialize(void* pArg)
{
    return E_NOTIMPL;
}

void CEnemy_Suicide::Priority_Update(_float fTimeDelta)
{
}

void CEnemy_Suicide::Update(_float fTimeDelta)
{
}

void CEnemy_Suicide::Late_Update(_float fTimeDelta)
{
}

HRESULT CEnemy_Suicide::Render()
{
    return E_NOTIMPL;
}

HRESULT CEnemy_Suicide::Ready_Components()
{
    return E_NOTIMPL;
}

HRESULT CEnemy_Suicide::Ready_Animation()
{
    return E_NOTIMPL;
}

HRESULT CEnemy_Suicide::SetUp_RenderState()
{
    return E_NOTIMPL;
}

HRESULT CEnemy_Suicide::Reset_RenderState()
{
    return E_NOTIMPL;
}

CEnemy_Suicide* CEnemy_Suicide::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CEnemy_Suicide* pInstance = new CEnemy_Suicide(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CEnemy_Suicide"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEnemy_Suicide::Clone(void* pArg)
{
    CEnemy_Suicide* pInstance = new CEnemy_Suicide(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CEnemy_Suicide"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEnemy_Suicide::Free()
{
}
