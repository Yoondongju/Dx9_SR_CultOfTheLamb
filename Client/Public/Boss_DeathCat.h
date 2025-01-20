#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CShader;
class CFsm;
END

BEGIN(Client)

class CBoss_DeathCat final : public CMonster
{
public:
	enum DEATHCAT_STATE { IDLE, CHAINATTACK, FLYBALLATTACK, DISAPPEAR, HURT, STATE_END };

private:
	CBoss_DeathCat(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoss_DeathCat(const CBoss_DeathCat& Prototype);
	virtual ~CBoss_DeathCat() = default;


public:
	void	Set_IsActive(_bool isActive) { m_isActive = isActive; }
	_bool	Get_IsActive() { return m_isActive; }

	_bool Get_IsPhase2() { return m_isPhase2; }
	void Set_IsPhase2(_bool isPahse2) { m_isPhase2 = isPahse2; }

	_bool Get_IsAllDie() { return m_isAllDie; }
	void Set_IsAllDie(_bool isAllDie) { m_isAllDie = isAllDie; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Change_State_Hurt() {};
	_uint Get_CurrentState();

	void On_ColliderActive();
	void Off_ColliderActive();

private:
	virtual void OnCollisionEnter(CGameObject* pOther);
	virtual void OnCollisionStay(CGameObject* pOther);
	virtual void OnCollisionExit(CGameObject* pOther);

private:
	HRESULT Ready_Components();
	HRESULT Ready_Animation();
	HRESULT Ready_Fsm();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	CShader* m_pShaderCom = { nullptr };

	_bool m_isActive = { true };
	_bool m_isHit = { false };
	_bool m_isPhase2 = { false };
	_bool m_isZeroHp = { false };
	_bool m_isAllDie = { false };

	_float m_fHitEffectTime = { 0.f };


public:
	static CBoss_DeathCat* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END