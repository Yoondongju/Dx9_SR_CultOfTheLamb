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

class CFrog final : public CMonster
{
public:
	enum FROG_STATE { IDLE, JUMP, ATTACK, HURT, STATE_END };

private:
	CFrog(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFrog(const CFrog& Prototype);
	virtual ~CFrog() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Change_State_Hurt() override;

private:
	virtual void OnCollisionEnter(CGameObject* pOther);
	virtual void OnCollisionStay(CGameObject* pOther);
	virtual void OnCollisionExit(CGameObject* pOther);

private:
	void State_Idle(_float fTimeDelta);
	void State_Jump(_float fTimeDelta);
	void State_Attack(_float fTimeDelta);
	void State_Hurt(_float fTimeDelta);

	HRESULT Create_Summon();

private:
	HRESULT Ready_Components();
	HRESULT Ready_Animation();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	_float m_fFindDistance = {};
	_float m_fAttackDsitance = {};
	_float m_fIdleWaitTime = {};

	_float3 m_vJumpDir = {};
	_float3* m_vSummonPosition = { nullptr };

	_bool m_isFindTarget = { false };
	_bool m_isSummon = { false };
	_bool m_isHit = { false };

	FROG_STATE m_ePrevState = { STATE_END };
	FROG_STATE m_eCurrentState = { STATE_END };

public:
	static CFrog* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END