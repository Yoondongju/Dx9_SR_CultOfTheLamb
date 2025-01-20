#include "..\Public\VIBuffer_Terrain.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Picking.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer{ pGraphic_Device }
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
	: CVIBuffer{ Prototype }
	, m_iNumVerticesX{ Prototype.m_iNumVerticesX }
	, m_iNumVerticesZ{ Prototype.m_iNumVerticesZ }
	, m_fBorderTo_LeftX{ Prototype.m_fBorderTo_LeftX }
	, m_fBorderTo_RightX{ Prototype.m_fBorderTo_RightX }
	,m_fBorderTo_UpZ{ Prototype.m_fBorderTo_UpZ }
	,m_fBorderTo_DownZ{ Prototype.m_fBorderTo_DownZ }

{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ)
{
	m_iVertexStride = sizeof(VTXTERRAIN);
	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_pVerticesPos = new _float3[m_iNumVertices];

	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX2;
	m_iNumFaces = (iNumVerticesX - 1) * (iNumVerticesZ - 1) * 2;
	m_iIndexStride = 4;
	m_iNumIndices = m_iNumFaces * 3;
	m_eIndexFormat = D3DFMT_INDEX32;

	/* 내가 의도한 사이즈만큼의 정점 배열공간을 할당했다. */
	if (FAILED(__super::Ready_VertexBuffer()))
		return E_FAIL;

	VTXTERRAIN* pVertices = { nullptr };
	/* 정점버퍼의 내가 지정한 위치의 메모리 주소를 얻어올 수 있다. */
	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, (void**)&pVertices, 0);

	for (size_t i = 0; i < iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < iNumVerticesX; j++)
		{
			_uint		iIndex = (_uint)i * iNumVerticesX + (_uint)j;

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3((_float)j, 0.0f, (_float)i);
			pVertices[iIndex].vTexcoord1 = _float2((_float)j / (iNumVerticesX - 1.f) * 20.f , (_float)i / (iNumVerticesZ - 1.f) * 20.f);
			pVertices[iIndex].vTexcoord2 = _float2((_float)j / (iNumVerticesX - 1.f), (_float)i / (iNumVerticesZ - 1.f));

			if (i == 0 && j == 0) {
				m_fBorderTo_LeftX = m_pVerticesPos[iIndex].x;
				m_fBorderTo_DownZ = m_pVerticesPos[iIndex].z;
			}
			if (i == (iNumVerticesZ - 1) && j == (iNumVerticesX - 1)) {
				m_fBorderTo_RightX = m_pVerticesPos[iIndex].x;
				m_fBorderTo_UpZ = m_pVerticesPos[iIndex].z;
			}
		}
	}


	m_pVB->Unlock();

	if (FAILED(Ready_IndexBuffer()))
		return E_FAIL;

	_uint* pIndices = { nullptr };

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < iNumVerticesX - 1; j++)
		{
			_uint		iIndex = (_uint)i * iNumVerticesX + (_uint)j;

			_uint		iIndices[4] = {
				iIndex + iNumVerticesX, /* 왼쪽 위 정점의 인덱스 */
				iIndex + iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];
		}
	}

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath)
{
	_ulong	dwByte = {};

	HANDLE	hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	BITMAPFILEHEADER		fh{};
	BITMAPINFOHEADER		ih{};

	 
	 ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	_uint* pPixel = new _uint[m_iNumVerticesX * m_iNumVerticesZ];
	ZeroMemory(pPixel, sizeof(_uint) * m_iNumVerticesX * m_iNumVerticesZ);

	ReadFile(hFile, pPixel, sizeof(_uint) * m_iNumVerticesX * m_iNumVerticesZ, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iVertexStride = sizeof(VTXTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_pVerticesPos = new _float3[m_iNumVertices];
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
	m_iNumFaces = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iIndexStride = 4;
	m_iNumIndices = m_iNumFaces * 3;
	m_eIndexFormat = D3DFMT_INDEX32;

	/* 내가 의도한 사이즈만큼의 정점 배열공간을 할당했다. */
	if (FAILED(__super::Ready_VertexBuffer()))
		return E_FAIL;

	VTXTEX* pVertices = { nullptr };
	/* 정점버퍼의 내가 지정한 위치의 메모리 주소를 얻어올 수 있다. */
	m_pVB->Lock(0, /*m_m_iNumVertices * m_iVertexStride*/0, (void**)&pVertices, 0);

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = (_uint)i * m_iNumVerticesX + (_uint)j;

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3((_float)j, (pPixel[iIndex] & 0x000000ff) / 10.0f, (_float)i);
			pVertices[iIndex].vTexcoord = _float2((_float)j / (m_iNumVerticesX - 1.f) * 30.f, (_float)i / (m_iNumVerticesZ - 1.f) * 30.f);
		}
	}

	m_pVB->Unlock();

	if (FAILED(Ready_IndexBuffer()))
		return E_FAIL;

	_uint* pIndices = { nullptr };

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = (_uint)i * m_iNumVerticesX + (_uint)j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX, /* 왼쪽 위 정점의 인덱스 */
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];
		}
	}

	m_pIB->Unlock();

	Safe_Delete_Array(pPixel);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

_float CVIBuffer_Terrain::Compute_Height(const _float3& vLocalPos)
{
	/* 객체가 존재하는 네모 영역의 왼쪽 하단 인덱스를 구했다. */
	_uint		iIndex = _uint(vLocalPos.z) * m_iNumVerticesX + _uint(vLocalPos.x);

	if (iIndex < 0 || iIndex > _uint(vLocalPos.z) * m_iNumVerticesX + _uint(vLocalPos.x) 
		|| vLocalPos.z < 0.f || iIndex > 100000000)
		return 0.f;

	/* 왼쪽하단이넫ㄱ스 기준으로 네방향의 ㄱ꼭지점 인덷ㄱ스를 구하자.  */
	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX, /* 왼위*/
		iIndex + m_iNumVerticesX + 1, /* 오위 */
		iIndex + 1, /* 오아 */
		iIndex /* 왼아 */
	};

	_float		fWidth = vLocalPos.x - m_pVerticesPos[iIndices[0]].x;
	_float		fDepth = m_pVerticesPos[iIndices[0]].z - vLocalPos.z;

	D3DXPLANE		Plane{};

	/* 오른쪽 위 삼각형안에있어. */
	if (fWidth > fDepth)
	{
		D3DXPlaneFromPoints(&Plane, &m_pVerticesPos[iIndices[0]], &m_pVerticesPos[iIndices[1]], &m_pVerticesPos[iIndices[2]]);
	}
	/* 왼쪽 아래 삼각형안에있어. */
	else
	{
		D3DXPlaneFromPoints(&Plane, &m_pVerticesPos[iIndices[0]], &m_pVerticesPos[iIndices[2]], &m_pVerticesPos[iIndices[3]]);
	}

	/* ax + by + cz + d */
	/* y = (-ax - cz - d) / b */

	return (-Plane.a * vLocalPos.x - Plane.c * vLocalPos.z - Plane.d) / Plane.b;
}

_bool CVIBuffer_Terrain::IsPick(CTransform* pTransform, _float3* pOut)
{
	if (m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		m_pGameInstance->Change_To_LocalSpace(pTransform->Get_WorldMatrix_Inverse());

		_uint* pIndices = nullptr;

		m_pIB->Lock(0, 0, (void**)&pIndices, 0);

		for (_uint iIndex = 0; iIndex < m_iNumVertices; ++iIndex)
		{
			if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[pIndices[iIndex]], m_pVerticesPos[pIndices[iIndex+1]],
				m_pVerticesPos[pIndices[iIndex+2]], pOut))
			{
				D3DXVec3TransformCoord(pOut, pOut, &pTransform->Get_WorldMatrix());

				m_pIB->Unlock();
				return true;
			}
		}

		m_pIB->Unlock();
	}
		return false;
}

void CVIBuffer_Terrain::Setting_TerrainVertex(_uint iNumX, _uint iNumZ)
{
	VTXTEX* pVertices = { nullptr };
	/* 정점버퍼의 내가 지정한 위치의 메모리 주소를 얻어올 수 있다. */
	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, (void**)&pVertices, 0);

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = (_uint)i * m_iNumVerticesX + (_uint)j;

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3((_float)j, 0.0f, (_float)i);
			pVertices[iIndex].vTexcoord = _float2((_float)j / (m_iNumVerticesX - 1.f) * _float(iNumX), (_float)i / (m_iNumVerticesZ - 1.f) * _float(iNumZ));

			if (i == 0 && j == 0) {
				m_fBorderTo_LeftX = m_pVerticesPos[iIndex].x;
				m_fBorderTo_DownZ = m_pVerticesPos[iIndex].z;
			}
			if (i == (m_iNumVerticesZ - 1) && j == (m_iNumVerticesX - 1)) {
				m_fBorderTo_RightX = m_pVerticesPos[iIndex].x;
				m_fBorderTo_UpZ = m_pVerticesPos[iIndex].z;
			}
		}
	}

	m_pVB->Unlock();
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iNumVerticesX, _uint iNumVerticesZ)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(iNumVerticesX, iNumVerticesZ)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

}

