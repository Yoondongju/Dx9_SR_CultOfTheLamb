#include "RigidBody.h"

#include "GameObject.h"
#include "Transform.h"

CRigidBody::CRigidBody(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent{ pGraphic_Device }
{
	m_eComponentType = RIGIDBODY;
}

CRigidBody::CRigidBody(const CRigidBody& Prototype)
	: CComponent{ Prototype }
{

}

HRESULT CRigidBody::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	return S_OK;
}

void CRigidBody::Update(_float fTimeDelta)
{	
	//�������� ���� ���� ���� ����
	//���� ���� �ӵ��� ����
	if (D3DXVec3Length(&m_vForce) != 0.f)
	{
		//���ӵ��� �̿��ؼ� �ӵ��� ������Ų��.
		m_vVelocity += m_vAccel;
	}

	Calculate_Tranform(fTimeDelta);

	//�� �ʱ�ȭ
	m_vForce = { 0.f, 0.f, 0.f };
}

void CRigidBody::Calculate_Tranform(_float fTimeDelta)
{
	Calculate_Friction();

	//�߷� ���� && !m_isGround
	if (m_isGravity)
	{
		Calculate_Gravity();
	}

	_float3 vCurrentPosition = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	
	//�ӵ� ����
	if (fabsf(m_vMaxVelocity.x) < fabsf(m_vVelocity.x))
	{
		m_vVelocity.x = (m_vVelocity.x / fabsf(m_vVelocity.x)) * fabsf(m_vMaxVelocity.x);
	}
	if (fabsf(m_vMaxVelocity.y) < fabsf(m_vVelocity.y))
	{
		m_vVelocity.y = (m_vVelocity.y / fabsf(m_vVelocity.y)) * fabsf(m_vMaxVelocity.y);
	}

	_float3 vNewPosition = vCurrentPosition + (m_vVelocity * fTimeDelta);

	m_pOwner->Get_Transform()->Set_State(CTransform::STATE_POSITION, vNewPosition);
}

void CRigidBody::Calculate_Gravity()
{
	_float3 vGravity = { 0.f, m_fGravityAccel * m_fGravityScale, 0.f };

	if (fabsf(vGravity.y) > m_fGravityLimit)
	{
		vGravity.y = m_fGravityLimit;
	}

	m_vVelocity -= vGravity;
}

void CRigidBody::Calculate_Friction()
{
		_float3 vFricitionDir = m_vVelocity;
		D3DXVec3Normalize(&vFricitionDir, &vFricitionDir);

		_float3 vFriction = -vFricitionDir * m_fFriction;

		//������ �� ū ���
		if (D3DXVec3Length(&vFricitionDir) >= D3DXVec3Length(&m_vVelocity))
		{
			m_vVelocity = { 0.f,0.f, 0.f };
		}
		else
		{
			m_vVelocity += vFriction;
		}
}

void CRigidBody::Add_Force(FORCEMODE eForceMode, _float3 vForce)
{
	m_vForce = vForce;

	switch (eForceMode)
	{
		//������ �߰����� ����(���Ը� ����)
	case Engine::CRigidBody::FORCE:
		//���ӵ��� = �� / ����
		m_vAccel += m_vForce / m_fMass;
		break;

		//������ �߰����� ����(���Ը� ���� �� ��)
	case Engine::CRigidBody::ACCELERATION:
		m_vAccel += m_vForce;
		break;

		//���������� �о��� ����(���Ը� ����)
	case Engine::CRigidBody::IMPLUSE:
		m_vVelocity += m_vForce / m_fMass;
		break;

		//���������� �о��� ����(���Ը� ���� �� ��)
	case Engine::CRigidBody::VELOCITYCHANGE:
		m_vVelocity += m_vForce;
		break;

	case Engine::CRigidBody::MODE_END:
		break;
	default:
		break;
	}
}

void CRigidBody::Add_Force_Direction(_float3 vDirection, _float3 vMagnitude, FORCEMODE eForceMode)
{
	D3DXVec3Normalize(&vDirection, &vDirection); // ���� ���͸� ����ȭ

	_float vForceX = vDirection.x * vMagnitude.x;    // ����� ũ�⸦ ���Ͽ� �� ���� ����
	_float vForceY = vDirection.y * vMagnitude.y;
	_float vForceZ = vDirection.z * vMagnitude.z;

	_float3 vForce = _float3(vForceX, vForceY, vForceZ);

	Add_Force(eForceMode, vForce); // ���� Add_Force �Լ� �̿�
}

CRigidBody* CRigidBody::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CRigidBody* pInstance = new CRigidBody(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRigidBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* ������ü�� ã�Ƽ� ������ü�� Clone�Լ��� ȣ���Ѵ�.  */
CComponent* CRigidBody::Clone(void* pArg)
{
	CRigidBody* pInstance = new CRigidBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CRigidBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRigidBody::Free()
{
	__super::Free();
}
