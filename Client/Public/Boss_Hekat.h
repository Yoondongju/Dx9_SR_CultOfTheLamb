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
class CBoss_Hekat final : public CMonster
{
public:
	enum HEKAT_STATE
	{
		IDLE,
		TRANS_HUMAN,
		TRANS_FROG,
		JUMP,
		ROAR,
		THROW_BOMB,
		LAY_EGG,
		DIE,
		TRANS_FOLLOWER,
		UNCONVERTED,
		
		HEKAT_STATE_END
	};
private:
	CBoss_Hekat(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoss_Hekat(const CBoss_Hekat& Prototype);
	virtual ~CBoss_Hekat() = default;

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
	class CTransform* Get_Transform()
	{
		return m_pTransformCom;
	}
	HEKAT_STATE Get_Hekat_State()
	{
		return m_eCurState;
	}
	void Set_Hekat_State(HEKAT_STATE eState)
	{
		m_eCurState = eState;
	}

private:
	CShader* m_pShaderCom = { nullptr };

	_bool m_isHit = { false };

	_float m_fHitEffectTime = { 0.f };

	HEKAT_STATE m_eCurState = HEKAT_STATE_END;

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
	static CBoss_Hekat* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
