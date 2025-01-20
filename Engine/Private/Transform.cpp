#include "..\Public\Transform.h"

#include "GameInstance.h"
#include "GameObject.h"

_float4x4 CTransform::m_Camera_ProjMatrix = {};

CTransform::CTransform(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent{ pGraphic_Device }
{
	m_eComponentType = TRANSFORM;
}

CTransform::CTransform(const CTransform& Prototype)
	: CComponent{ Prototype }
	, m_WorldMatrix{ Prototype.m_WorldMatrix }
{
	m_eComponentType = TRANSFORM;
}

_float2 CTransform::Get_ViewportPos()
{
	_float3 vWorldPos = Get_State(STATE::STATE_POSITION);

	CTransform* pCameraTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Camera()->Find_Component(TRANSFORM));

	_float4x4 ViewMat = pCameraTransform->Get_WorldMatrix_Inverse();
	
		
	D3DXVec3TransformCoord(&vWorldPos, &vWorldPos, &ViewMat);
	D3DXVec3TransformCoord(&vWorldPos, &vWorldPos, &m_Camera_ProjMatrix);


	D3DVIEWPORT9 viewport;
	m_pGraphic_Device->GetViewport(&viewport);

	D3DXVECTOR3 screenPos;
	screenPos.x = ((vWorldPos.x + 1.0f) / 2.0f) * viewport.Width;
	screenPos.y = ((1.0f - vWorldPos.y) / 2.0f) * viewport.Height;
	screenPos.z = vWorldPos.z;

	return _float2(screenPos.x ,screenPos.y);
}

HRESULT CTransform::Initialize_Prototype()
{
	D3DXMatrixIdentity(&m_WorldMatrix);

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;

	return S_OK;
}

HRESULT CTransform::Bind_Transform()
{
	return m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
}

void CTransform::Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	_float3		vRight = Get_State(STATE_RIGHT);
	Set_State(STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * fScaleX);

	_float3		vUp = Get_State(STATE_UP);
	Set_State(STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * fScaleY);

	_float3		vLook = Get_State(STATE_LOOK);
	Set_State(STATE_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * fScaleZ);

}

void CTransform::Scaling(_float3 vScale)
{
	_float3		vRight = Get_State(STATE_RIGHT);
	Set_State(STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * vScale.x);

	_float3		vUp = Get_State(STATE_UP);
	Set_State(STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * vScale.y);

	_float3		vLook = Get_State(STATE_LOOK);
	Set_State(STATE_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * vScale.z);
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vLook = Get_State(STATE_LOOK);

	vPosition += *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vLook = Get_State(STATE_LOOK);

	vPosition -= *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vRight = Get_State(STATE_RIGHT);

	vPosition += *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vRight = Get_State(STATE_RIGHT);

	vPosition -= *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Straight_BuildingObj()
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vLook = Get_State(STATE_LOOK);

	
	vPosition.z += 1.9f + 0.3f;		// 그리드 한칸의 크기만큼
	
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward_BuildingObj()
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vLook = Get_State(STATE_LOOK);

	vPosition.z -= 1.9f + 0.3f;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right_BuildingObj()
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vRight = Get_State(STATE_RIGHT);

	vPosition.x += 1.9f + 0.3f;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left_BuildingObj()
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vRight = Get_State(STATE_RIGHT);

	vPosition.x -= 1.9f + 0.3f;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Turn(const _float3& vAxis, _float fTimeDelta)
{
	_float3		vRight = Get_State(STATE_RIGHT);
	_float3		vUp = Get_State(STATE_UP);
	_float3		vLook = Get_State(STATE_LOOK);

	_float4x4		RotationMatrix{};

	D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, m_fRotationPerSec * fTimeDelta);

	D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrix);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrix);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::ZTurn(const _float3& vAxis, _float m_fRotationAngle)
{
	_float3		vRight = Get_State(CTransform::STATE_RIGHT);
	_float3		vUp = Get_State(CTransform::STATE_UP);
	_float3		vLook = Get_State(CTransform::STATE_LOOK);

	_float4x4		RotationMatrix{};

	D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, m_fRotationAngle);

	D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrix);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrix);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}

void CTransform::Rotation(const _float3& vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	_float3		vRight = _float3(vScaled.x, 0.f, 0.f);
	_float3		vUp = _float3(0.f, vScaled.y, 0.f);
	_float3		vLook = _float3(0.f, 0.f, vScaled.z);

	_float4x4		RotationMatrix{};

	D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, fRadian);

	D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrix);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrix);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::LookAt(const _float3& vAt)
{
	_float3		vScaled = Get_Scaled();

	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vLook = vAt - vPosition;
	_float3		vRight{}, vUp{};

	D3DXVec3Cross(&vRight, &_float3(0.f, 1.f, 0.f), &vLook);
	D3DXVec3Cross(&vUp, &vLook, &vRight);

	Set_State(STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * vScaled.x);
	Set_State(STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * vScaled.y);
	Set_State(STATE_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * vScaled.z);
}

void CTransform::Change_SpeedPerSec(_float& fSpeedPerSec)
{
	m_fSpeedPerSec = fSpeedPerSec;
}

void CTransform::Change_RotationPerSec(_float& fRotationPerSec)
{
	m_fRotationPerSec = fRotationPerSec;
}

void CTransform::Move_Dir(_float fTimeDelta, _float3 vDir, _float fSpeedPerSec)
{
	//우리는 y축 움직임 없음
	_float3		vPosition = Get_State(STATE_POSITION);
	D3DXVec3Normalize(&vDir, &vDir);

	if(0.f != fSpeedPerSec)
	{
		vPosition.x += vDir.x * fSpeedPerSec * fTimeDelta;
		vPosition.z += vDir.z * fSpeedPerSec * fTimeDelta;
	}
	else
	{
		vPosition.x += vDir.x * m_fSpeedPerSec * fTimeDelta;
		vPosition.z += vDir.z * m_fSpeedPerSec * fTimeDelta;
	}


	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Move_DirY(_float fTimeDelta, _float3 vDir, _float fSpeedPerSec)
{
	_float3		vPosition = Get_State(STATE_POSITION);

	D3DXVec3Normalize(&vDir, &vDir);

	if (0.f != fSpeedPerSec)
	{
		vPosition.x += vDir.x * fSpeedPerSec * fTimeDelta;
		vPosition.y += vDir.y * fSpeedPerSec * fTimeDelta;
		vPosition.z += vDir.z * fSpeedPerSec * fTimeDelta;
	}
	else
	{
		vPosition.x += vDir.x * m_fSpeedPerSec * fTimeDelta;
		vPosition.y += vDir.y * m_fSpeedPerSec * fTimeDelta;
		vPosition.z += vDir.z * m_fSpeedPerSec * fTimeDelta;
	}


	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Move_Dgree(_float fTimeDelta, _float fDgree, _float fSpeedPerSec)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float		fRadian = D3DXToRadian(fDgree);

	if (0.f != fSpeedPerSec)
	{
		vPosition.x += fSpeedPerSec * cosf(fRadian) * fTimeDelta;
		vPosition.z += fSpeedPerSec * sinf(fRadian) * fTimeDelta;
	}
	else
	{
		vPosition.x += m_fSpeedPerSec * cosf(fRadian) * fTimeDelta;
		vPosition.z += m_fSpeedPerSec * sinf(fRadian) * fTimeDelta;
	}

	Set_State(STATE_POSITION, vPosition);
}

CTransform* CTransform::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTransform* pInstance = new CTransform(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

}
