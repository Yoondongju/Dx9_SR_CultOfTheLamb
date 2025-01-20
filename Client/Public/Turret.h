#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Rect;
class CFsm;
END

BEGIN(Client)

class CTurret final : public CMonster
{
public:
	enum Turret_STATE { IDLE, ATTACK, HURT, STATE_END };

private:
	CTurret(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTurret(const CTurret& Prototype);
	virtual ~CTurret() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Change_State_Hurt();

private:
	virtual void OnCollisionEnter(CGameObject* pOther);
	virtual void OnCollisionStay(CGameObject* pOther);
	virtual void OnCollisionExit(CGameObject* pOther);

private:
	void State_Idle(_float fTimeDelta);
	void State_Attack(_float fTimeDelta);
	void State_Hurt(_float fTimeDelta);

	void Create_Bullet();

private:
	HRESULT Ready_Components();
	HRESULT Ready_Animation();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	_float m_fFindDistance = {};

	_float m_fAttackDelayTime = {};

	_uint	m_iAttackCount = {};

	_float3 m_fAttackDir = {};

	_bool m_isFindTarget = { false };
	_bool m_isAttacking = { false };

	Turret_STATE m_ePrevState = { STATE_END };
	Turret_STATE m_eCurrentState = { STATE_END };

public:
	static CTurret* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END