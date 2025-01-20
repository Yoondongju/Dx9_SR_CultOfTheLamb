#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider_Rect final : public CComponent
{
public:
	typedef struct
	{
		_float	fSizeX, fSizeY;
		_float	fOffSetX, fOffSetY;
		_float4x4		WorldMatrix;
	}COLLIDER_RECT_INFO;

private:
	CCollider_Rect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider_Rect(const CCollider_Rect& Prototype);
	virtual ~CCollider_Rect() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg) override;
	HRESULT Update_Collider(_float4x4 WorldMatrix);
	HRESULT Render_Collider();

private:
	LPDIRECT3DVERTEXBUFFER9			m_pVB = { nullptr };
	LPDIRECT3DINDEXBUFFER9			m_pIB = { nullptr };
	_uint							m_iVertexStride = { 0 };
	_uint							m_iNumVertices = { 0 };
	_ulong							m_dwFVF = {};
	_uint							m_iNumFaces = {};

	_uint							m_iIndexStride = { 0 };
	_uint							m_iNumIndices = { 0 };
	D3DFORMAT						m_eIndexFormat = {  };

private:
	COLLIDER_RECT_INFO				m_ColliderInfo = {};

	_float3							m_vColliderPoint[4] = {};

public:
	static CCollider_Rect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END