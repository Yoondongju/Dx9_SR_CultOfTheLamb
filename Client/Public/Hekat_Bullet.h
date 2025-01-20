#pragma once
#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
END

BEGIN(Client)

class CHekat_Bullet final : public CLandObject
{
public:
	struct HEKAT_BULLET
	{
		_float3 vPos = {0.f,0.f,0.f};
		_float vDgree = 0.f;
		_float vSpeed = 0.f;
		LANDOBJECT_DESC tLandObj = {};
	};

private:
	CHekat_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHekat_Bullet(const CHekat_Bullet& Prototype);
	virtual ~CHekat_Bullet() = default;

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
	HEKAT_BULLET m_eHekatBullet = {};

public:
	static CHekat_Bullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END



