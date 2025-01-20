#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRigidBody final : public CComponent
{
public:
    enum FORCEMODE { FORCE, ACCELERATION, IMPLUSE, VELOCITYCHANGE, MODE_END };

private:
	CRigidBody(LPDIRECT3DDEVICE9 pGraphic_Device);
	CRigidBody(const CRigidBody& Prototype);
	virtual ~CRigidBody() = default;

public:
    class CGameObject* GetOwner() { return m_pOwner; }
    void SetOwner(class CGameObject* pOwner) { m_pOwner = pOwner; }

    _float              Get_Mass() { return m_fMass; }
    void                Set_Mass(_float fMass) { m_fMass = fMass; }

    _float3             Get_Force() { return m_vForce; }

    _float3             Get_Velocity() { return m_vVelocity; }
    void                Set_Velocity(_float3 _vVelocity) { m_vVelocity = _vVelocity; }

    void                AddVelocity(_float3 _vAddV) { m_vVelocity += _vAddV; }

    _bool               Get_IsGravity() { return m_isGravity; }
    void                Set_IsGravity(bool isGravity) { 
        m_isGravity = isGravity;  
        if (!m_isGravity && m_vVelocity.y < 0)  
            m_vVelocity.y = 0.f; 
    }

    void                Set_VelocityLimit(_float3 vMaxVelocity) { m_vMaxVelocity = vMaxVelocity; }

    void                Set_Friction(_float fFriction) { m_fFriction = fFriction; }

    void                Set_GravityAccel(_float fGravityAccel) { m_fGravityAccel = fGravityAccel; }
    void                Set_GravityLimit(_float fGravityLimit) { m_fGravityLimit = fGravityLimit; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
    void Update(_float fTimeDelta);

public:
    void Add_Force(FORCEMODE eForceMode, _float3 vForce);   // ���� �ִ� �Լ�
    void Add_Force_Direction(_float3 vDirection, _float3 vMagnitude, FORCEMODE eForceMode); // Ư�� �������� ���� �ִ� �Լ�



private:
    class CGameObject*      m_pOwner = { nullptr };

    _float                  m_fMass = { 1.f };             // ����
    _float                  m_fGravityScale = { 1.f };     // �߷� ũ��
    _float                  m_fGravityAccel = { 9.8f };     // �߷� ���ӵ�
    _float                  m_fGravityLimit = { 10.f };     // �߷� �ִ� ��
    _float                  m_fFriction = { 1.f };         // ������


    _float3                 m_vForce = { 0.f,0.f,0.f };           // �������� ��
    _float3                 m_vAccel = { 0.f,0.f,0.f };           // ���ӵ�
    _float3                 m_vVelocity = { 0.f,0.f,0.f };        // �ӵ�
    _float3                 m_vMaxVelocity = { 20.f,20.f,20.f };     // �ְ� �ӵ�

    _bool                   m_isActive = { true };  // Ȱ��ȭ ���־�?
    _bool                   m_isGravity = { true }; // �߷� �����Ұž�?
    _bool                   m_isGround = { false }; // ���� �پ��־�?
    _bool                   m_isKinematic = { false }; // true�� ����ȿ���� �� �� �ž�

private:
    void Calculate_Tranform(_float fTimeDelta); // ���� ��ġ ���
    void Calculate_Gravity(); // �߷� ���
    void Calculate_Friction(); // ���� ���

public:
	static CRigidBody* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END