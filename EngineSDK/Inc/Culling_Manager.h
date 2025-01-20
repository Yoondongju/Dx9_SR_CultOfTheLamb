#pragma once
#include "Base.h"

BEGIN(Engine)

class CCulling_Manager final : public CBase
{
private:
	CCulling_Manager(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual ~CCulling_Manager() = default;

public:
	HRESULT Initialize();
	void Update();
	_bool Is_In_Sight(_float3 vPosition, _float fRadius);

private:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = { nullptr };
	_float3						m_Point[8];
	D3DXPLANE					m_Plane[6];

public:
	static CCulling_Manager* Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual void Free() override;

};

END