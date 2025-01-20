#include "..\Public\VIBuffer.h"

#include "GameInstance.h"

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent { pGraphic_Device } ,
	m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

CVIBuffer::CVIBuffer(const CVIBuffer & Prototype)
	: CComponent{ Prototype }
	, m_pGameInstance{ Prototype.m_pGameInstance }
	, m_pVB { Prototype.m_pVB }
	, m_pIB { Prototype.m_pIB }
	, m_iVertexStride{ Prototype.m_iVertexStride }
	, m_iNumVertices{ Prototype.m_iNumVertices }
	, m_dwFVF{ Prototype.m_dwFVF }
	, m_iNumFaces{ Prototype.m_iNumFaces }
	, m_iIndexStride { Prototype.m_iIndexStride }
	, m_iNumIndices { Prototype.m_iNumIndices }
	, m_eIndexFormat { Prototype.m_eIndexFormat }
	, m_pVerticesPos{ Prototype.m_pVerticesPos }
{
	Safe_AddRef(m_pGameInstance);

	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	// m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_iNumFaces);
	m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumFaces);

	return S_OK;
}

HRESULT CVIBuffer::Bind_Buffers(_int iStreamSource)
{
	m_pGraphic_Device->SetStreamSource(iStreamSource, m_pVB, 0, m_iVertexStride);
	m_pGraphic_Device->SetFVF(m_dwFVF);
	m_pGraphic_Device->SetIndices(m_pIB);

	return S_OK;
}

HRESULT CVIBuffer::Ready_VertexBuffer()
{
	/* �����迭�� ������ ������ �� �ֵ��� �Լ��� ȣ���Ѵ�. */
	/* 1. Length : ���� �迭������ �� ����Ʈ�� �Ҵ��ұ��? (�����ϳ��� ����Ʈ ũ�� * ������ ����) */
	/* 2. Usage : �� ������ �Ӽ�(STATIC : 0, DYNAMIC: D3DUSAGE_DYNAMIC)�� �����ش޶�. */
	/* 3. fvf : D3DFVF �� ������ �̷��̷��� �������� �Ǿ��־�.������ �˷��ִ� �����̱� ������ ���� ������ ������ �������Ѵ�. */
	/* 4. MemoryPool : D3DPOOL_DEFAULT(VRam), D3DPOOL_MANAGED(VRam or Systemmem), D3DPOOL_SYSTEMMEM(Ram) */
	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iVertexStride * m_iNumVertices,
		0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;


	return S_OK;
}

HRESULT CVIBuffer::Ready_IndexBuffer()
{
	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iIndexStride * m_iNumIndices, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	return S_OK;
}



void CVIBuffer::Free()
{
	__super::Free();

	if (false == m_isCloned)
		Safe_Delete_Array(m_pVerticesPos);

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	Safe_Release(m_pGameInstance);
}
