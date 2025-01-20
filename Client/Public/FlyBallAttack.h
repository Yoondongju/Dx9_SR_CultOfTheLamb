#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTexture;
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
class CShader;
class CFsm;
END

BEGIN(Client)

class CFlyBallAttack :
	public CBlendObject
{
public:
	enum FLYBALLTYPE { STRAGIHT_DGREE, STRAGIHT_DIR, CIRCLE, REFLECT, SHADOW, TYPE_END };
public:
	typedef struct
	{
		_float3 vPosition = {0.f, 0.f, 0.f};
		_float3 vCenterPosition = {0.f ,0.f ,0.f};
		_float3 vDir = { 0.f ,0.f ,0.f };
		_float fDelayTime = {};
		_float fDgree = {};
		FLYBALLTYPE eType = { TYPE_END };

		_uint iLevelIndex = {};
		_uint iStageIndex = {};

	}FLYBALL_DESC;

private:
	CFlyBallAttack(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFlyBallAttack(const CFlyBallAttack& Prototype);
	virtual ~CFlyBallAttack() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

	FLYBALL_DESC	m_tFlyBallDesc = {};

	_float			m_fDestoryTime = {};
	_float			m_fShadowCreateTime = {};
	_float			m_fAccTime = {};
	_float			m_fRadius = {};

	_bool			m_IsMove = { false };
	_bool			m_isDead = { false };

	_float			m_fTerrainX = {};
	_float			m_fTerrainZ = {};

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

	void Calculate_Reflect();

public:
	static CFlyBallAttack* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END