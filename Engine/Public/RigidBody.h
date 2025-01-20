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
    void Add_Force(FORCEMODE eForceMode, _float3 vForce);   // 힘을 주는 함수
    void Add_Force_Direction(_float3 vDirection, _float3 vMagnitude, FORCEMODE eForceMode); // 특정 방향으로 힘을 주는 함수



private:
    class CGameObject*      m_pOwner = { nullptr };

    _float                  m_fMass = { 1.f };             // 질량
    _float                  m_fGravityScale = { 1.f };     // 중력 크기
    _float                  m_fGravityAccel = { 9.8f };     // 중력 가속도
    _float                  m_fGravityLimit = { 10.f };     // 중력 최대 값
    _float                  m_fFriction = { 1.f };         // 마찰력


    _float3                 m_vForce = { 0.f,0.f,0.f };           // 가해지는 힘
    _float3                 m_vAccel = { 0.f,0.f,0.f };           // 가속도
    _float3                 m_vVelocity = { 0.f,0.f,0.f };        // 속도
    _float3                 m_vMaxVelocity = { 20.f,20.f,20.f };     // 최고 속도

    _bool                   m_isActive = { true };  // 활성화 되있어?
    _bool                   m_isGravity = { true }; // 중력 적용할거야?
    _bool                   m_isGround = { false }; // 땅에 붙어있어?
    _bool                   m_isKinematic = { false }; // true면 물리효과를 안 줄 거야

private:
    void Calculate_Tranform(_float fTimeDelta); // 최종 위치 계산
    void Calculate_Gravity(); // 중력 계산
    void Calculate_Friction(); // 마찰 계산

public:
	static CRigidBody* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END