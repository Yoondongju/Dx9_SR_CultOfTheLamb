#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(LPDIRECT3DDEVICE9 pGraphic_Device);
	CComponent(const CComponent& Prototype);
	virtual ~CComponent() = default;

public:
	COMPONENT_TYPE Get_ComponentType() { return m_eComponentType; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

protected:
	class CGameInstance*		m_pGameInstance = { nullptr };
	LPDIRECT3DDEVICE9			m_pGraphic_Device = { nullptr };
	_bool						m_isCloned = { false };

	COMPONENT_TYPE				m_eComponentType = { COMPONENT_END };

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END