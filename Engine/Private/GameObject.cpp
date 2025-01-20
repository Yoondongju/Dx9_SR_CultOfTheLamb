#include "..\Public\GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device{ pGraphic_Device },
	m_pGameInstance{ CGameInstance::Get_Instance() },
	m_isCloned{ false },
	m_isDead{ false },
	m_isFilp{ false }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGraphic_Device);
}

CGameObject::CGameObject(const CGameObject& Prototype)
	: m_pGraphic_Device{ Prototype.m_pGraphic_Device },
	m_pGameInstance{ Prototype.m_pGameInstance },
	m_isCloned{ true },
	m_isDead{ false },
	m_isFilp{ false },
	m_iLevelIndex{ Prototype.m_iLevelIndex }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{
}

void CGameObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

CComponent * CGameObject::Find_Component(const _wstring & strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

CComponent* CGameObject::Find_Component(const COMPONENT_TYPE eComponentType)
{
	for (auto iter = m_Components.begin(); iter != m_Components.end(); ++iter)
	{
		if (eComponentType == iter->second->Get_ComponentType())
			return iter->second;
	}

	return nullptr;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const _wstring & strPrototypeTag, const _wstring & strComponentTag, CComponent** ppOut, void * pArg)
{
	if (nullptr != Find_Component(strComponentTag))
		return E_FAIL;



	CComponent*		pComponent = m_pGameInstance->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);
	
	return S_OK;
}

void CGameObject::Set_Position(_float3 vPosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

_float3 CGameObject::Get_Position()
{
	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

void CGameObject::Free()
{
	__super::Free();

	if (m_Components.size() > 0)
	{
		for (auto& Pair : m_Components)
			Safe_Release(Pair.second);

		m_Components.clear();
	}

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pGraphic_Device);
}
