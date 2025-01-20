#include "..\Public\VIBuffer_Rhombus.h"

#include "GameInstance.h"
#include "Key_Manager.h"
#include "Picking.h"

CVIBuffer_Rhombus::CVIBuffer_Rhombus(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer{ pGraphic_Device }
{
	m_eComponentType = VIBUFFER_RECT;
}

CVIBuffer_Rhombus::CVIBuffer_Rhombus(const CVIBuffer_Rhombus& Prototype)
	: CVIBuffer{ Prototype }
{
	m_eComponentType = VIBUFFER_RECT;
}

HRESULT CVIBuffer_Rhombus::Initialize_Prototype()
{
	m_iVertexStride = sizeof(VTXTEX);
	m_iNumVertices = 4;
	m_pVerticesPos = new _float3[m_iNumVertices];
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
	m_iNumFaces = 2;
	m_iIndexStride = 2;
	m_iNumIndices = 6;
	m_eIndexFormat = D3DFMT_INDEX16;

	/* 내가 의도한 사이즈만큼의 정점 배열공간을 할당했다. */
	if (FAILED(__super::Ready_VertexBuffer()))
		return E_FAIL;

	VTXTEX* pVertices = { nullptr };
	/* 정점버퍼의 내가 지정한 위치의 메모리 주소를 얻어올 수 있다. */
	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, (void**)&pVertices, 0);

	pVertices[0].vPosition = m_pVerticesPos[0] = _float3(0.f, 0.f, 0.5f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = m_pVerticesPos[1] = _float3(0.5f, 0.f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = m_pVerticesPos[2] = _float3(0.f, 0.f, -0.5f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = m_pVerticesPos[3] = _float3(-0.5f, 0.f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	m_pVB->Unlock();

	if (FAILED(Ready_IndexBuffer()))
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

HRESULT CVIBuffer_Rhombus::Initialize(void* pArg)
{
	return S_OK;
}

_bool CVIBuffer_Rhombus::IsPick(CTransform* pTransform, _float3* pOut)
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

void CVIBuffer_Rhombus::Filp_Texture(_bool bFilp)
{
	if (bFilp)
	{
		// 플립 true시 UV 좌표를 반대로 바꿔서 이미지가 거꾸로 찍히게
		VTXTEX* pVertices = { nullptr };

		m_pVB->Lock(0, 0, (void**)&pVertices, 0);

		pVertices[0].vPosition = _float3(-0.5f, 0.f, 0.5f);
		pVertices[0].vTexcoord = _float2(1.f, 0.f);

		pVertices[1].vPosition = _float3(0.5f, 0.f, 0.5f);
		pVertices[1].vTexcoord = _float2(0.f, 0.f);

		pVertices[2].vPosition = _float3(0.5f, 0.f, -0.5f);
		pVertices[2].vTexcoord = _float2(0.f, 1.f);

		pVertices[3].vPosition = _float3(-0.5f, 0.f, -0.5f);
		pVertices[3].vTexcoord = _float2(1.f, 1.f);

		m_pVB->Unlock();

	}

	else
	{
		VTXTEX* pVertices = { nullptr };
		m_pVB->Lock(0, 0, (void**)&pVertices, 0);


		pVertices[0].vPosition = _float3(-0.5f, 0.f, 0.5f);
		pVertices[0].vTexcoord = _float2(0.f, 0.f);

		pVertices[1].vPosition = _float3(0.5f, 0.f, 0.5f);
		pVertices[1].vTexcoord = _float2(1.f, 0.f);

		pVertices[2].vPosition = _float3(0.5f, 0.f, -0.5f);
		pVertices[2].vTexcoord = _float2(1.f, 1.f);

		pVertices[3].vPosition = _float3(-0.5f, 0.f, -0.5f);
		pVertices[3].vTexcoord = _float2(0.f, 1.f);

		m_pVB->Unlock();
	}
}

CVIBuffer_Rhombus* CVIBuffer_Rhombus::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CVIBuffer_Rhombus* pInstance = new CVIBuffer_Rhombus(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Rhombus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CVIBuffer_Rhombus::Clone(void* pArg)
{
	CVIBuffer_Rhombus* pInstance = new CVIBuffer_Rhombus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Rhombus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rhombus::Free()
{
	__super::Free();

}

