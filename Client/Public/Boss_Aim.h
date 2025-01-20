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

class CBoss_Aim final : public CMonster
{
public:
	enum AIM_STATE { IDLE, IDLE_TO_ATTACK, WALK, ATTACK1, ATTACK2, ATTACK3, DIE, AIM_STATE_END };

private:
	CBoss_Aim(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoss_Aim(const CBoss_Aim& Prototype);
	virtual ~CBoss_Aim() = default;

public:
	const _float3&		Get_Recognition_Range_ToAttack() { return m_vRecognition_Range_ToAttack; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float Get_FootPosY();
	virtual void Change_State_Hurt() {};


private:
	CShader* m_pShaderCom = { nullptr };

	_float3   m_vRecognition_Range_ToAttack = { };

	_float m_fHitEffectTime = { 0.f };

	_bool m_isHit = { false };

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
	static CBoss_Aim* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END