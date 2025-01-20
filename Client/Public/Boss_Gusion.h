#pragma once
#include "Monster.h"

BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CShader;
class CCollider_Cube;
class CFsm;
END

BEGIN(Client)
class CBoss_Gusion final : public CMonster
{
public:
	enum GUSION_STATE
	{
		IDLE,
		ROAR,
		BURP,
		LAY_EGG,
		JUMP,
		DIE,

		GUSION_STATE_END
	};

private:
	CBoss_Gusion(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoss_Gusion(const CBoss_Gusion& Prototype);
	virtual ~CBoss_Gusion() = default;

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
	CTransform* Get_Transform()
	{
		return m_pTransformCom;
	}
	void		Set_Gusion_State(GUSION_STATE eState)
	{
		m_eCurState = eState;
	}
	GUSION_STATE Get_Gusion_State()
	{
		return m_eCurState;
	}

private:
	CShader* m_pShaderCom = { nullptr };

	_bool m_isHit = { false };

	_float m_fHitEffectTime = { 0.f };

	GUSION_STATE m_eCurState = GUSION_STATE_END;

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

public:
	static CBoss_Gusion* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
