#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider_Cube final : public CComponent
{
public:
	typedef struct
	{
		_float	fSizeX, fSizeY, fSizeZ;
		_float	fOffSetX, fOffSetY, fOffSetZ;
		_float4x4		WorldMatrix;
	}COLLIDER_CUBE_INFO;

private:
	CCollider_Cube(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider_Cube(const CCollider_Cube& Prototype);
	virtual ~CCollider_Cube() = default;

public:
	void Set_Collider_Cube_Scale(const COLLIDER_CUBE_INFO& ColliderInfo);

	_bool Get_IsActive() { return m_isActive; }
	void Set_IsActive(_bool isActive) { m_isActive = isActive; }

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg) override;
	HRESULT Update_Collider(_float4x4 WorldMatrix);
	HRESULT Render_Collider();

public:
	unordered_set<class CGameObject*>* GetCollisionSet() { return &m_ColliderObjects; }
	_bool IsCollision(CCollider_Cube* pTarget);

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
	COLLIDER_CUBE_INFO				m_ColliderInfo = {};
	_float3							m_vColliderPoint[8] = {};

	_bool							m_isActive = { true };

	unordered_set<class CGameObject*> m_ColliderObjects;

public:
	static CCollider_Cube* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END