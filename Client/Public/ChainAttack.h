#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
class CFsm;
END

class CChainAttack :
    public CGameObject
{
private:
    CChainAttack(LPDIRECT3DDEVICE9 pGraphic_Device);
	CChainAttack(const CChainAttack& Prototype);
    virtual ~CChainAttack() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CAnimator* m_pAnimatorCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float m_fCreateDelayTime = { 0.f };

	_bool m_isDead = { false };
	_bool m_isChainSound = { false };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Animation();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	static CChainAttack* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

