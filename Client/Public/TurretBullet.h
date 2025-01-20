#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
class CFsm;
END

BEGIN(Client)

class CTurretBullet :
	public CGameObject
{
public:
	typedef struct
	{
		_float3 vPosition = {};
		_float3 vDir = {};

	}TURRETBullet_DESC;

private:
	CTurretBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTurretBullet(const CTurretBullet& Prototype);
	virtual ~CTurretBullet() = default;

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

	TURRETBullet_DESC	m_tTurretBulletDesc = {};

	_float			m_fDestoryTime = {};

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	static CTurretBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END