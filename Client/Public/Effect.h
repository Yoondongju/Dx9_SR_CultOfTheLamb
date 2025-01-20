#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CAnimator;
END

BEGIN(Client)

class CEffect final : public CBlendObject
{
public:
	struct EFFECT_DESC
	{
		_float3 vPos = { 0.f,0.f,0.f };
		_float3 vScale = { 0.f,0.f,0.f };
		_float3 vSpeed = { 1.f,1.f,1.f };
		_float3 vDir = { 0.f,0.f,0.f };
		_float	fEffectDura = 0.f;
		_float  fDelayTimer = 0.f;
		_float  fJumpPower = 0.f;
		_bool	bIsFilp = false;
		_wstring strTextureTag = {};
		_uint   iLevelIndex = 0;
		_uint	iStageIndex = 0;
	};

private:
	CEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect(const CEffect& Prototype);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CAnimator*				m_pAnimCom = nullptr;

private:
	EFFECT_DESC				m_Effect_Desc = {};
	_float					m_fFrame = {};
	_float					m_fEffectTimer = 0.f;
	_float					m_fDelayTimer = 0.f;
	_bool					m_bEffectEnd = false;
	_bool					m_bEffectDelayEnd = true;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	static CEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END