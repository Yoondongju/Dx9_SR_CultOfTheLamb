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

class CBat final : public CMonster
{
public:
	enum BAT_STATE { FLY, ATTACK, HURT, STATE_END };

private:
	CBat(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBat(const CBat& Prototype);
	virtual ~CBat() = default;


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
	void State_Fly(_float fTimeDelta);
	void State_Attack(_float fTimeDelta);
	void State_HURT(_float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Ready_Animation();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	_float m_fFindDistance = {};
	_float m_fAttackDsitance = {};
	_float m_fAttackStartTime = {};

	_float3 m_fAttackDir = {};

	_bool m_isFindTarget = { false };
	_bool m_isAttackCharging = { false };
	_bool m_isAttacking = { false };
	_bool m_isAttackEnd = { false };
	_bool m_isHit = { false };

	BAT_STATE m_ePrevState = { STATE_END };
	BAT_STATE m_eCurrentState = { STATE_END };

public:
	static CBat* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END