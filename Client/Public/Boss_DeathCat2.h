#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Rect;
class CFsm;
class CShader;
END

BEGIN(Client)

class CBoss_DeathCat2 final : public CMonster
{
public:
	enum DEATHCAT2_STATE { IDLE, APPEAR, ATTACK, HURT, STATE_END };

private:
	CBoss_DeathCat2(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoss_DeathCat2(const CBoss_DeathCat2& Prototype);
	virtual ~CBoss_DeathCat2() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float Get_FootPosY();
	_uint Get_CurrentState(); 
	virtual void Change_State_Hurt() {};

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
	class CBoss_Eye* m_pEyes[3] = { nullptr };


public:
	static CBoss_DeathCat2* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END