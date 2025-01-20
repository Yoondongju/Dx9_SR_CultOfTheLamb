#pragma once

#include "Component.h"

/* VIBuffer = Vertex + Index */
/* Vertex : �� */
/* Line : ���ΰ��� �̾ ������ �׸���. */
/* Triangle : �������� �̾ ���� �׸���. ������ */
/* Vertex -> Polygon -> Mesh(Model) */

/* Index : ������ �׸��� ������ ������ ���۴�. */

/* VIBuffer : ���� �����ϴ� ������ �ڽ�Ŭ������ �θ� �Ǵ� Ŭ������. */
/* �ڽ�Ŭ���� -> VIBuffer_Rect, VIBuffer_Cube, VIBuffer_Terrain */


BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer(const CVIBuffer& Prototype);
	virtual ~CVIBuffer() = default;


public:
	_float3*		Get_VerticesPos() { return m_pVerticesPos; }
	_uint			Get_NumVertices() { return m_iNumVertices; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render();

public:
	HRESULT Bind_Buffers(_int iStreamSource = 0);
	virtual _bool IsPick(class CTransform* pTransform, _float3* pOut = nullptr) = 0;
	virtual void Filp_Texture(_bool bFilp = false) {};

protected:
	class CGameInstance*			m_pGameInstance = { nullptr };

	/* dx�� �˾Ƽ� �Ҵ��س� ���� �迭�� ������ ��ǥ�ϴ� �İ�ü.  */
	LPDIRECT3DVERTEXBUFFER9			m_pVB = { nullptr };
	LPDIRECT3DINDEXBUFFER9			m_pIB = { nullptr };
	_uint							m_iVertexStride = { 0 };
	_uint							m_iNumVertices = { 0 };
	_ulong							m_dwFVF = {};
	_uint							m_iNumFaces = {};

	_uint							m_iIndexStride = { 0 };
	_uint							m_iNumIndices = { 0 };
	D3DFORMAT						m_eIndexFormat = {  };

	_float3*						m_pVerticesPos = { nullptr };

protected:
	HRESULT Ready_VertexBuffer();
	HRESULT Ready_IndexBuffer();

	

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END