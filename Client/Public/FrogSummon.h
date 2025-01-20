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

class CFrogSummon final : public CMonster
{
private:
	CFrogSummon(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFrogSummon(const CFrogSummon& Prototype);
	virtual ~CFrogSummon() = default;


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
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	static CFrogSummon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END