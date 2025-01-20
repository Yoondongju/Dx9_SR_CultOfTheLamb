#include "Collider_Cube.h"

CCollider_Cube::CCollider_Cube(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CComponent{ pGraphic_Device }
{
	m_eComponentType = COLLIDER_CUBE;
}

CCollider_Cube::CCollider_Cube(const CCollider_Cube& Prototype)
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
	m_eComponentType = COLLIDER_CUBE;
}

void CCollider_Cube::Set_Collider_Cube_Scale(const COLLIDER_CUBE_INFO& ColliderInfo)
{
	m_ColliderInfo.fSizeX = ColliderInfo.fSizeX;
	m_ColliderInfo.fSizeY = ColliderInfo.fSizeY;
	m_ColliderInfo.fSizeZ = ColliderInfo.fSizeZ;

	m_ColliderInfo.fOffSetX = ColliderInfo.fOffSetX;
	m_ColliderInfo.fOffSetY = ColliderInfo.fOffSetY;
	m_ColliderInfo.fOffSetZ = ColliderInfo.fOffSetZ;


	VTXCUBE* pVertices = { nullptr };
	/* 정점버퍼의 내가 지정한 위치의 메모리 주소를 얻어올 수 있다. */
	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, (void**)&pVertices, 0);

	pVertices[0].vPosition = m_vColliderPoint[0] = _float3(-m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, -m_ColliderInfo.fSizeZ);
	pVertices[0].vTexcoord = pVertices[0].vPosition;

	pVertices[1].vPosition = m_vColliderPoint[1] = _float3(m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, -m_ColliderInfo.fSizeZ);
	pVertices[1].vTexcoord = pVertices[1].vPosition;

	pVertices[2].vPosition = m_vColliderPoint[2] = _float3(m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, -m_ColliderInfo.fSizeZ);
	pVertices[2].vTexcoord = pVertices[2].vPosition;

	pVertices[3].vPosition = m_vColliderPoint[3] = _float3(-m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, -m_ColliderInfo.fSizeZ);
	pVertices[3].vTexcoord = pVertices[3].vPosition;

	pVertices[4].vPosition = m_vColliderPoint[4] = _float3(-m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, m_ColliderInfo.fSizeZ);
	pVertices[4].vTexcoord = pVertices[4].vPosition;

	pVertices[5].vPosition = m_vColliderPoint[5] = _float3(m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, m_ColliderInfo.fSizeZ);
	pVertices[5].vTexcoord = pVertices[5].vPosition;

	pVertices[6].vPosition = m_vColliderPoint[6] = _float3(m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, m_ColliderInfo.fSizeZ);
	pVertices[6].vTexcoord = pVertices[6].vPosition;

	pVertices[7].vPosition = m_vColliderPoint[7] = _float3(-m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, m_ColliderInfo.fSizeZ);
	pVertices[7].vTexcoord = pVertices[7].vPosition;

	m_pVB->Unlock();
}

HRESULT CCollider_Cube::Initialize_Prototype()
{
	m_iVertexStride = sizeof(VTXCUBE);
	m_iNumVertices = 8;
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);
	m_iNumFaces = 12;
	m_iIndexStride = 2;
	m_iNumIndices = 36;
	m_eIndexFormat = D3DFMT_INDEX16;

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iVertexStride * m_iNumVertices,
		0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	VTXCUBE* pVertices = { nullptr };
	/* 정점버퍼의 내가 지정한 위치의 메모리 주소를 얻어올 수 있다. */
	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, (void**)&pVertices, 0);

	pVertices[0].vPosition = m_vColliderPoint[0] = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexcoord = pVertices[0].vPosition;

	pVertices[1].vPosition = m_vColliderPoint[1] = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTexcoord = pVertices[1].vPosition;

	pVertices[2].vPosition = m_vColliderPoint[2] = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexcoord = pVertices[2].vPosition;

	pVertices[3].vPosition = m_vColliderPoint[3] = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTexcoord = pVertices[3].vPosition;

	pVertices[4].vPosition = m_vColliderPoint[4] = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vTexcoord = pVertices[4].vPosition;

	pVertices[5].vPosition = m_vColliderPoint[5] = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vTexcoord = pVertices[5].vPosition;

	pVertices[6].vPosition = m_vColliderPoint[6] = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].vTexcoord = pVertices[6].vPosition;

	pVertices[7].vPosition = m_vColliderPoint[7] = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vTexcoord = pVertices[7].vPosition;
	
	m_pVB->Unlock();

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iIndexStride * m_iNumIndices, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	_ushort* pIndices = { nullptr };

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	/* +x */
	pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
	pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

	/* -x */
	pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
	pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;

	/* +y */
	pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;

	/* -y */
	pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;

	/* +z */
	pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
	pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;

	/* -z */
	pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CCollider_Cube::Initialize(void* pArg)
{
	memcpy(&m_ColliderInfo, pArg, sizeof(COLLIDER_CUBE_INFO));
	
	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iVertexStride * m_iNumVertices,
		0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	VTXCUBE* pVertices = { nullptr };
	/* 정점버퍼의 내가 지정한 위치의 메모리 주소를 얻어올 수 있다. */
	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, (void**)&pVertices, 0);

	pVertices[0].vPosition = m_vColliderPoint[0] = _float3(-m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, -m_ColliderInfo.fSizeZ);
	pVertices[0].vTexcoord = pVertices[0].vPosition;

	pVertices[1].vPosition = m_vColliderPoint[1] = _float3(m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, -m_ColliderInfo.fSizeZ);
	pVertices[1].vTexcoord = pVertices[1].vPosition;

	pVertices[2].vPosition = m_vColliderPoint[2] = _float3(m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, -m_ColliderInfo.fSizeZ);
	pVertices[2].vTexcoord = pVertices[2].vPosition;

	pVertices[3].vPosition = m_vColliderPoint[3] = _float3(-m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, -m_ColliderInfo.fSizeZ);
	pVertices[3].vTexcoord = pVertices[3].vPosition;

	pVertices[4].vPosition = m_vColliderPoint[4] = _float3(-m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, m_ColliderInfo.fSizeZ);
	pVertices[4].vTexcoord = pVertices[4].vPosition;

	pVertices[5].vPosition = m_vColliderPoint[5] = _float3(m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, m_ColliderInfo.fSizeZ);
	pVertices[5].vTexcoord = pVertices[5].vPosition;

	pVertices[6].vPosition = m_vColliderPoint[6] = _float3(m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, m_ColliderInfo.fSizeZ);
	pVertices[6].vTexcoord = pVertices[6].vPosition;

	pVertices[7].vPosition = m_vColliderPoint[7] = _float3(-m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, m_ColliderInfo.fSizeZ);
	pVertices[7].vTexcoord = pVertices[7].vPosition;

	m_pVB->Unlock();

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iIndexStride * m_iNumIndices, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	_ushort* pIndices = { nullptr };

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	/* +x */
	pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
	pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

	/* -x */
	pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
	pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;

	/* +y */
	pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;

	/* -y */
	pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;

	/* +z */
	pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
	pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;

	/* -z */
	pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CCollider_Cube::Update_Collider(_float4x4 WorldMatrix)
{
	m_ColliderInfo.WorldMatrix = WorldMatrix;

	m_vColliderPoint[0] = _float3(-m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, -m_ColliderInfo.fSizeZ);
	m_vColliderPoint[1] = _float3(m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, -m_ColliderInfo.fSizeZ);
	m_vColliderPoint[2] = _float3(m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, -m_ColliderInfo.fSizeZ);
	m_vColliderPoint[3] = _float3(-m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, -m_ColliderInfo.fSizeZ);
	m_vColliderPoint[4] = _float3(-m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, m_ColliderInfo.fSizeZ);
	m_vColliderPoint[5] = _float3(m_ColliderInfo.fSizeX, m_ColliderInfo.fSizeY, m_ColliderInfo.fSizeZ);
	m_vColliderPoint[6] = _float3(m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, m_ColliderInfo.fSizeZ);
	m_vColliderPoint[7] = _float3(-m_ColliderInfo.fSizeX, -m_ColliderInfo.fSizeY, m_ColliderInfo.fSizeZ);

	_float3 vecOffsetPos = *(_float3*)&(m_ColliderInfo.WorldMatrix.m[3][0]);
	vecOffsetPos.x += m_ColliderInfo.fOffSetX;
	vecOffsetPos.y += m_ColliderInfo.fOffSetY;
	vecOffsetPos.z += m_ColliderInfo.fOffSetZ;;

	m_ColliderInfo.WorldMatrix.m[3][0] = vecOffsetPos.x;
	m_ColliderInfo.WorldMatrix.m[3][1] = vecOffsetPos.y;
	m_ColliderInfo.WorldMatrix.m[3][2] = vecOffsetPos.z;

	for (int i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&m_vColliderPoint[i], &m_vColliderPoint[i], &m_ColliderInfo.WorldMatrix);
	}

	return S_OK;
}

HRESULT CCollider_Cube::Render_Collider()
{
	if (!g_DebugMode)
		return S_OK;

	m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_ColliderInfo.WorldMatrix);

	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iVertexStride);
	m_pGraphic_Device->SetFVF(m_dwFVF);
	m_pGraphic_Device->SetIndices(m_pIB);

	m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumFaces);

	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	return S_OK;
}

_bool CCollider_Cube::IsCollision(CCollider_Cube* pTarget)
{
	CCollider_Cube* pTargetCollider = pTarget;

	if (nullptr == pTargetCollider)
		return false;

	//부딪힌 스스로의 point
	_float3		vSelfMin = m_vColliderPoint[3];	//모두 -인 점을 가져오기 위함
	_float3		vSelfMax = m_vColliderPoint[5]; //모두 +인 점을 가져오기 위함

	if (vSelfMin.x > vSelfMax.x)
	{
		_float vTemp = vSelfMin.x;
		vSelfMin.x = vSelfMax.x;
		vSelfMax.x = vTemp;
	}

	if (vSelfMin.z > vSelfMax.z)
	{
		_float vTemp = vSelfMin.z;
		vSelfMin.z = vSelfMax.z;
		vSelfMax.z = vTemp;
	}

	_float3		vSelfCenter = (vSelfMin + vSelfMax) * 0.5f; // 중점을 가져오기 위함

	//부딪힌 대상의 point
	_float3		vTargetMin = pTarget->m_vColliderPoint[3];	//모두 -인 점을 가져오기 위함
	_float3		vTargetMax = pTarget->m_vColliderPoint[5]; //모두 +인 점을 가져오기 위함
	_float3		vTargetCenter = (vTargetMin + vTargetMax) * 0.5f; // 중점을 가져오기 위함

	//AABB충돌 기법 쓸거임
	//자신의 최대점 < 타겟의 최소점이면 충돌이 아님 -> 요걸로 비교
	//자신의 최소점 > 타겟의 최대점이면 충돌이 아님
	if (min(vSelfMax.x, vTargetMax.x) < max(vSelfMin.x, vTargetMin.x))
		return false;

	//if(min(vSelfMax.y, vTargetMax.y) < max(vSelfMin.y, vTargetMin.y))
	//	return false;

	if (min(vSelfMax.z, vTargetMax.z) < max(vSelfMin.z, vTargetMin.z))
		return false;


	// 충돌간의 거리를 계산함
	//혹시 사용할거면 나중에 따로 빼기
	_float3 vDistance = _float3(fabsf((min(vSelfMax.x, vTargetMax.x) - max(vSelfMin.x, vTargetMin.x))),
		fabsf((min(vSelfMax.y, vTargetMax.y) - max(vSelfMin.y, vTargetMin.y))),
		fabsf((min(vSelfMax.z, vTargetMax.z) - max(vSelfMin.z, vTargetMin.z))));

	return true;
}

CCollider_Cube* CCollider_Cube::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollider_Cube* pInstance = new CCollider_Cube(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCollider_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CCollider_Cube::Clone(void* pArg)
{
	CCollider_Cube* pInstance = new CCollider_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CCollider_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Cube::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
