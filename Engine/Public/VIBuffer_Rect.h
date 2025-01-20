#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect final : public CVIBuffer
{
private:
	CVIBuffer_Rect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Rect(const CVIBuffer_Rect& Prototype);
	virtual ~CVIBuffer_Rect() = default;

public:
	_float3* Get_pVerticesPos() { return m_pVerticesPos; }		// 무조건 4개 겟지


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool IsPick(class CTransform* pTransform, _float3* pOut = nullptr);

public:
	virtual void Filp_Texture(_bool bFilp = false);

public:
	static CVIBuffer_Rect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END