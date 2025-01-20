#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rhomb final : public CVIBuffer
{
private:
	CVIBuffer_Rhomb(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Rhomb(const CVIBuffer_Rhomb& Prototype);
	virtual ~CVIBuffer_Rhomb() = default;

public:
	_float3* Get_pVerticesPos() { return m_pVerticesPos; }		// 무조건 4개 겟지

	
public:
	virtual _bool IsPick(class CTransform* pTransform, _float3* pOut = nullptr);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;



public:
	static CVIBuffer_Rhomb* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

