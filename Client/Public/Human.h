#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CFsm;
END

BEGIN(Client)

class CHuman final : public CMonster
{
public:
	enum HUMAN_STATE { IDLE, RUN, ATTACK, HURT, STATE_END };

private:
	CHuman(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHuman(const CHuman& Prototype);
	virtual ~CHuman() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Change_State_Hurt() override;

private:
	virtual void OnCollisionEnter(CGameObject* pOther);
	virtual void OnCollisionStay(CGameObject* pOther);
	virtual void OnCollisionExit(CGameObject* pOther);

private:
	void State_Idle(_float fTimeDelta);
	void State_Run(_float fTimeDelta);
	void State_Attack(_float fTimeDelta);
	virtual void State_Hurt(_float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Ready_Animation();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	_float m_fFindDistance = {};
	_float m_fAttackDsitance = {};
	_float m_fAttackTime = {};
	_float m_fDeadDelayTime = {};

	_float3 m_fAttackDir = {};

	_bool m_isFindTarget = { false };
	_bool m_isAttacking = { false };
	_bool m_isAttackEnd = { false };
	_bool m_isHit = { false };

	HUMAN_STATE m_ePrevState = { STATE_END };
	HUMAN_STATE m_eCurrentState = { STATE_END };

public:
	static CHuman* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END