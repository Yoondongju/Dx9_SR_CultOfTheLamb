#include "Collider_Rect.h"

CCollider_Rect::CCollider_Rect(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CComponent{ pGraphic_Device }
{
	m_eComponentType = COLLIDER_RECT;
}

CCollider_Rect::CCollider_Rect(const CCollider_Rect& Prototype)
	:CComponent{ Prototype }
	, m_pVB{ Prototype.m_pVB }
	, m_pIB{ Prototype.m_pIB }
	, m_iVertexStride{ Prototype.m_iVertexStride }
	, m_iNumVertices{ Prototype.m_iNumVertices }
	, m_dwFVF{ Prototype.m_dwFVF }
	, m_iNumFaces{ Prototype.m_iNumFaces }
	, m_iIndexStride{ Prototype.m_iIndexStride }
	, m_iNumIndices{ Prototype.m_iNumIndices }
	, m_eIndexFormat{ Prototype.m_eIndexFormat }
{
	m_eComponentType = COLLIDER_RECT;
}

HRESULT CCollider_Rect::Initialize_Prototype()
{
	m_iVertexStride = sizeof(VTXTEX);
	m_iNumVertices = 4;
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
	m_iNumFaces = 2;
	m_iIndexStride = 2;
	m_iNumIndices = 6;
	m_eIndexFormat = D3DFMT_INDEX16;

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iVertexStride * m_iNumVertices,
		0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	VTXTEX* pVertices = { nullptr };
	/* 정점버퍼의 내가 지정한 위치의 메모리 주소를 얻어올 수 있다. */
	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, (void**)&pVertices, 0);

	pVertices[0].vPosition = m_vColliderPoint[0] = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = m_vColliderPoint[1] = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = m_vColliderPoint[2] = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = m_vColliderPoint[3] = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	m_pVB->Unlock();

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iIndexStride * m_iNumIndices, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	_ushort* pIndices = { nullptr };

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	m_pIB->Unlock();



	return S_OK;
}

HRESULT CCollider_Rect::Initialize(void* pArg)
{
	memcpy(&m_ColliderInfo, pArg, sizeof(COLLIDER_RECT_INFO));
	
	VTXTEX* pVertices = nullptr;

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iVertexStride * m_iNumVertices, 0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;


	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = m_vColliderPoint[0] = _float3(-m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, 0.f);
	pVertices[0].vTexcoord = _float2(0.0f, 0.f);

	pVertices[1].vPosition = m_vColliderPoint[1] = _float3(m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = m_vColliderPoint[2] = _float3(m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = m_vColliderPoint[3] = _float3(-m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	m_pVB->Unlock();
	
	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iIndexStride * m_iNumIndices, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	_ushort* pIndices = { nullptr };

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CCollider_Rect::Update_Collider(_float4x4 WorldMatrix)
{
	m_ColliderInfo.WorldMatrix = WorldMatrix;

	m_vColliderPoint[0] = _float3(-m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, 0.f);
	m_vColliderPoint[1] = _float3(m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, 0.f);
	m_vColliderPoint[2] = _float3(m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, 0.f);
	m_vColliderPoint[3] = _float3(-m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, 0.f);

	_float3 vOffset = *(_float3*)&(m_ColliderInfo.WorldMatrix.m[3][0]);
	vOffset.x += m_ColliderInfo.fOffSetX;
	vOffset.y += m_ColliderInfo.fOffSetY;

	m_ColliderInfo.WorldMatrix.m[3][0] = vOffset.x;
	m_ColliderInfo.WorldMatrix.m[3][1] = vOffset.y;

	for (size_t i = 0; i < 4; ++i)
	{
		D3DXVec3TransformCoord(&m_vColliderPoint[i], &m_vColliderPoint[i], &m_ColliderInfo.WorldMatrix);
	}

	return S_OK;
}

HRESULT CCollider_Rect::Render_Collider()
{
	m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_ColliderInfo.WorldMatrix);

	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iVertexStride);
	m_pGraphic_Device->SetFVF(m_dwFVF);
	m_pGraphic_Device->SetIndices(m_pIB);

	m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumFaces);

	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	return S_OK;
}

CCollider_Rect* CCollider_Rect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollider_Rect* pInstance = new CCollider_Rect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCollider_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CCollider_Rect::Clone(void* pArg)
{
	CCollider_Rect* pInstance = new CCollider_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CCollider_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Rect::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
