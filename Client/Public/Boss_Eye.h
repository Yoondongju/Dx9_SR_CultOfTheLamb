#pragma once

#include "Client_Defines.h"
#include "Monster.h"
//#include "Boss_DeathCat2.h"

BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Rect;
class CFsm;
class CShader;
END

BEGIN(Client)

class CBoss_Eye final : public CMonster
{
public:
	enum EYE_STATE { IDLE, DISAPPEAR, ATTACK, DIE, STATE_END };
	enum EYE_TYPE { LEFT, MIDDLE ,RIGHT , EYE_TYPE_END };


	typedef struct tEye
	{
		MONSTER_INFO eMonInfo = {  };
		EYE_TYPE     eEyeType = {};

	}EYE_DESC;

public:

private:
	CBoss_Eye(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoss_Eye(const CBoss_Eye& Prototype);
	virtual ~CBoss_Eye() = default;

public:
	_bool	Get_IsHide() { return m_isHide; }
	void	Set_IsHide(_bool isHide) { m_isHide = isHide; }

	EYE_TYPE Get_EyeType() { return m_eMyType; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
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

public:
	_float Get_FootPosY();
	void Move_Dir(_float fTimeDelta, _float3 vDir, _float fSpeedPerSec = 0.f);

private:
	CShader* m_pShaderCom = { nullptr };

	_float m_fMoveTime = {};
	_float m_fHitEffectTime = { 0.f };

	_bool m_isHide = { false };
	_bool m_isHit = { false };
	_bool m_isEndDieAnimation = { false };

	EYE_TYPE	m_eMyType = { EYE_TYPE_END };

public:
	static CBoss_Eye* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END