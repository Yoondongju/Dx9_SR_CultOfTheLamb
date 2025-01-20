#include "..\Public\VIBuffer_Rect.h"

#include "GameInstance.h"
#include "Key_Manager.h"
#include "Picking.h"

CVIBuffer_Rect::CVIBuffer_Rect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer { pGraphic_Device }
{
	m_eComponentType = VIBUFFER_RECT;
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect & Prototype)
	: CVIBuffer { Prototype }
{
	m_eComponentType = VIBUFFER_RECT;
}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
{
	m_iVertexStride = sizeof(VTXTEX);
	m_iNumVertices = 4;
	m_pVerticesPos = new _float3[m_iNumVertices];
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
	m_iNumFaces = 2;
	m_iIndexStride = 2;
	m_iNumIndices = 6;
	m_eIndexFormat = D3DFMT_INDEX16;

	/* ���� �ǵ��� �����ŭ�� ���� �迭������ �Ҵ��ߴ�. */
	if (FAILED(__super::Ready_VertexBuffer()))
		return E_FAIL;	

	VTXTEX*			pVertices = { nullptr };
	/* ���������� ���� ������ ��ġ�� �޸� �ּҸ� ���� �� �ִ�. */
	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, (void**)&pVertices, 0);

	pVertices[0].vPosition = m_pVerticesPos[0] =  _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = m_pVerticesPos[1] = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = m_pVerticesPos[2] = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = m_pVerticesPos[3] = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	m_pVB->Unlock();	

	if (FAILED(Ready_IndexBuffer()))
		return E_FAIL;

	_ushort*		pIndices = { nullptr };

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

HRESULT CVIBuffer_Rect::Initialize(void * pArg)
{
	return S_OK;
}

_bool CVIBuffer_Rect::IsPick(CTransform* pTransform, _float3* pOut)
{
	if (m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		m_pGameInstance->Change_To_LocalSpace(pTransform->Get_WorldMatrix_Inverse());

		if (m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[0], m_pVerticesPos[1], m_pVerticesPos[2], pOut))
			return true;

		if (m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[0], m_pVerticesPos[2], m_pVerticesPos[3], pOut))
			return true;
	}

	return false;
}

void CVIBuffer_Rect::Filp_Texture(_bool bFilp)
{
	if (bFilp)
	{
		// �ø� true�� UV ��ǥ�� �ݴ�� �ٲ㼭 �̹����� �Ųٷ� ������
		VTXTEX* pVertices = { nullptr };

		m_pVB->Lock(0, 0, (void**)&pVertices, 0);

		pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
		pVertices[0].vTexcoord = _float2(1.f, 0.f);

		pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
		pVertices[1].vTexcoord = _float2(0.f, 0.f);

		pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
		pVertices[2].vTexcoord = _float2(0.f, 1.f);

		pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
		pVertices[3].vTexcoord = _float2(1.f, 1.f);

		m_pVB->Unlock();

	}

	else
	{
		VTXTEX* pVertices = { nullptr };
		m_pVB->Lock(0, 0, (void**)&pVertices, 0);

		pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
		pVertices[0].vTexcoord = _float2(0.f, 0.f);

		pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
		pVertices[1].vTexcoord = _float2(1.f, 0.f);

		pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
		pVertices[2].vTexcoord = _float2(1.f, 1.f);

		pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
		pVertices[3].vTexcoord = _float2(0.f, 1.f);

		m_pVB->Unlock();
	}
}

CVIBuffer_Rect * CVIBuffer_Rect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CVIBuffer_Rect*		pInstance = new CVIBuffer_Rect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* ������ü�� ã�Ƽ� ������ü�� Clone�Լ��� ȣ���Ѵ�.  */
CComponent* CVIBuffer_Rect::Clone(void* pArg)
{
	CVIBuffer_Rect*		pInstance = new CVIBuffer_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect::Free()
{
	__super::Free();

}

