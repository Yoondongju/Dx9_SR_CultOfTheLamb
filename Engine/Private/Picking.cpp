#include "..\Public\Picking.h"

#include "GameInstance.h"
#include "Transform.h"

CPicking::CPicking(HWND hWnd, LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device{ pGraphic_Device },
	m_hWnd{ hWnd }
{
	Safe_AddRef(m_pGraphic_Device);
}


HRESULT CPicking::Initialize()
{
	RECT rcWindow = {};

	GetClientRect(m_hWnd, &rcWindow);

	m_iWinCX = rcWindow.right - rcWindow.left;
	m_iWinCY = rcWindow.bottom - rcWindow.top;

	return S_OK;
}

void CPicking::Update()
{
	POINT			ptMouse = CGameInstance::Get_Instance()->Get_MosePos();

	_float3			vMousePos;

	// 투영 스페이스 마우스 좌표
	vMousePos.x = _float(ptMouse.x / (m_iWinCX * 0.5f) - 1);
	vMousePos.y = _float(ptMouse.y / (m_iWinCY * -0.5f) + 1);
	vMousePos.z = 0.f;

	// 뷰 스페이스 마우스 좌표
	_float4x4		ProjMatrixInv;
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrixInv);
	D3DXMatrixInverse(&ProjMatrixInv, nullptr, &ProjMatrixInv);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &ProjMatrixInv);

	// 뷰스페이스 상의 마우스 레이와 레이포스
	m_vRayPos = _float3(0.f, 0.f, 0.f);
	m_vRayDir = vMousePos - m_vRayPos;

	// 월드스페이스 상의 마우스 레이와 레이포스
	_float4x4		ViewMatrixInv;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrixInv);
	D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrixInv);
	D3DXVec3TransformCoord(&m_vRayPos, &m_vRayPos, &ViewMatrixInv);
	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &ViewMatrixInv);

	D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);
}

void CPicking::Change_To_LocalSpace(const _float4x4 WorldMatrixInverse)
{
	D3DXVec3TransformCoord(&m_vLocalRayPos, &m_vRayPos, &WorldMatrixInverse);
	D3DXVec3TransformNormal(&m_vLocalRayDir, &m_vRayDir, &WorldMatrixInverse);

	D3DXVec3Normalize(&m_vLocalRayDir, &m_vLocalRayDir);
}

_bool CPicking::isPicked_InWorldSpace(_float3& vPointA, _float3& vPointB, _float3& vPointC, _float3* pOut)
{
	_float		fU, fV, fDist;

	if (D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vRayPos, &m_vRayDir, &fU, &fV, &fDist))
	{
		*pOut = m_vRayPos + m_vRayDir * fDist;

		return true;
	}

	return false;
}

_bool CPicking::isPicked_InLocalSpace(_float3& vPointA, _float3& vPointB, _float3& vPointC, _float3* pOut)
{
	_float		fU, fV, fDist;

	if (D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vLocalRayPos, &m_vLocalRayDir, &fU, &fV, &fDist))
	{
		*pOut = m_vLocalRayPos + m_vLocalRayDir * fDist;

		return true;
	}

	return false;
}

CPicking* CPicking::Create(HWND hWnd, LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPicking* pInstance = new CPicking(hWnd, pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPicking"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking::Free()
{
	Safe_Release(m_pGraphic_Device);
}
