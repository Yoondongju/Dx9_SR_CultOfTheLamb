#include"Culling_Manager.h"

CCulling_Manager::CCulling_Manager(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device{ pGraphic_Device }
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CCulling_Manager::Initialize()
{
	//로컬 포지션 잡아주기
	m_Point[0] = _float3(-1.f, 1.f, 0.f);
	m_Point[1] = _float3(1.f, 1.f, 0.f);
	m_Point[2] = _float3(1.f, -1.f, 0.f);
	m_Point[3] = _float3(-1.f, -1.f, 0.f);
	m_Point[4] = _float3(-1.f, 1.f, 1.f);
	m_Point[5] = _float3(1.f, 1.f, 1.f);
	m_Point[6] = _float3(1.f, -1.f, 1.f);
	m_Point[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}


void CCulling_Manager::Update()
{
	//업데이트에서 카메라의 월드 좌표를 구한다
	//투영 상에서의 모든 좌표는 (-1,-1,0) ~ (1,1,1)
	m_Point[0] = _float3(-1.f, 1.f, 0.f);
	m_Point[1] = _float3(1.f, 1.f, 0.f); 
	m_Point[2] = _float3(1.f, -1.f, 0.f);
	m_Point[3] = _float3(-1.f, -1.f, 0.f);
	m_Point[4] = _float3(-1.f, 1.f, 1.f);
	m_Point[5] = _float3(1.f, 1.f, 1.f);
	m_Point[6] = _float3(1.f, -1.f, 1.f);
	m_Point[7] = _float3(-1.f, -1.f, 1.f);

	_float4x4 ProjMatrixInv{};
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrixInv);
	D3DXMatrixInverse(&ProjMatrixInv, nullptr, &ProjMatrixInv);

	_float4x4 ViewMatrixInv{};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrixInv);
	D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrixInv);

	for (_uint i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(&m_Point[i], &m_Point[i], &ProjMatrixInv);

	for (int i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(&m_Point[i], &m_Point[i], &ViewMatrixInv);
}

// 경계구 컬링
_bool CCulling_Manager::Is_In_Sight(_float3 vPosition, _float fRadius)
{
	//점들로 평면을 구성(평면의 방정식)
	D3DXPlaneFromPoints(&m_Plane[0], &m_Point[0], &m_Point[1], &m_Point[2]); // +x
	D3DXPlaneFromPoints(&m_Plane[1], &m_Point[1], &m_Point[5], &m_Point[6]); // -x
	D3DXPlaneFromPoints(&m_Plane[2], &m_Point[4], &m_Point[5], &m_Point[1]); // +y
	D3DXPlaneFromPoints(&m_Plane[3], &m_Point[4], &m_Point[0], &m_Point[3]); // -y
	D3DXPlaneFromPoints(&m_Plane[4], &m_Point[5], &m_Point[4], &m_Point[7]); // +z
	D3DXPlaneFromPoints(&m_Plane[5], &m_Point[3], &m_Point[2], &m_Point[6]); // -z


	for (int i = 0; i < 6; ++i)
	{
		// ax+by+cz = 0
		// 평면과 점 사이의 수직 거리
		_float distance = m_Plane[i].a * vPosition.x + m_Plane[i].b * vPosition.y + m_Plane[i].c * vPosition.z + m_Plane[i].d;

		//혹시 몰라서 더해줌
		if (distance > fRadius + 1.5f)
			return false;
	}

	return TRUE;
}

CCulling_Manager* CCulling_Manager::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCulling_Manager* pInstance = new CCulling_Manager(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CCulling_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCulling_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGraphic_Device);
}
