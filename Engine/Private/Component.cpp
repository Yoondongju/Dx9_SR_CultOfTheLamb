#include "..\Public\Component.h"
#include "GameInstance.h"

CComponent::CComponent(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device { pGraphic_Device }
	, m_isCloned{ false }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGraphic_Device);
}

CComponent::CComponent(const CComponent & Prototype)
	: m_pGraphic_Device { Prototype.m_pGraphic_Device }
	, m_isCloned{ true }
	, m_pGameInstance { Prototype.m_pGameInstance }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void * pArg)
{
	return S_OK;
}


void CComponent::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pGraphic_Device);
}
