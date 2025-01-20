#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
class CAnimator;
END

BEGIN(Client)

class CMortal_Fly final : public CGameObject
{
public:
	struct MORTAL_FLY
	{
		_float3 vPos = { 0.f,0.f,0.f };
		_float3 vInitPos = { 0.f,0.f,0.f };
		_float3 vDir = { 0.f,0.f,0.f };
		_float  fJumpPower = 0.f;
		_float  fSpeed = 0.f;
		_uint  iLevelIndex = 0;
		_uint  iStageIndex = 0;
	};

private:
	CMortal_Fly(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMortal_Fly(const CMortal_Fly& Prototype);
	virtual ~CMortal_Fly() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual void OnCollisionEnter(CGameObject* pOther);
	virtual void OnCollisionStay(CGameObject* pOther);
	virtual void OnCollisionExit(CGameObject* pOther);

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CAnimator* m_pAnimCom = nullptr;
	MORTAL_FLY m_eMortalFly = {};

public:
	static CMortal_Fly* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END




