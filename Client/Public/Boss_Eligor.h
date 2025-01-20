#pragma once
#include "Monster.h"

BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CShader;
class CFsm;
END

BEGIN(Client)
class CBoss_Eligor final : public CMonster
{

public:
	enum ELIGOR_STATE
	{
		IDLE,
		ROAR,
		FLY_ATTACK,
		BURP,
		LAY_EGG,
		TRANS_F,
		UNCONVERTED,
		ELIGOR_STATE_END
	};

private:
	CBoss_Eligor(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoss_Eligor(const CBoss_Eligor& Prototype);
	virtual ~CBoss_Eligor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Change_State_Hurt() {};

public:
	void Set_Eilgor_State(enum ELIGOR_STATE eState)
	{
		m_eCurState = eState;
	}
	ELIGOR_STATE Get_Eligor_State()
	{
		return m_eCurState;
	}
	_bool		 Get_Eilgor_IsHit()
	{
		return m_bAttack;
	}

public:
	class CTransform* Get_Transform()
	{
		return m_pTransformCom;
	}

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

	ELIGOR_STATE m_eCurState = ELIGOR_STATE_END;

	_float m_fHitEffectTime = { 0.f };

	_bool		 m_bAttack = false;
	_bool m_isHit = { false };

public:
	static CBoss_Eligor* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
