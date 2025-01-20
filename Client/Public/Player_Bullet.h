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

class CPlayer_Bullet final : public CLandObject
{
public:
	struct PLAYER_BULLET
	{
		_float3 vPos = { 0.f,0.f,0.f };
		_float3 vDir = { 0.f,0.f,0.f };
		_float	fAtackPower = 8.f;
		_float  fDuration = 0.f;
		_float  fSpeed = 0.f;
		LANDOBJECT_DESC tLandObj = {};
	};

private:
	CPlayer_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayer_Bullet(const CPlayer_Bullet& Prototype);
	virtual ~CPlayer_Bullet() = default;

public:
	PLAYER_BULLET Get_Player_Bullet_Info() { return m_ePlayer_Bullet; }

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
	PLAYER_BULLET	m_ePlayer_Bullet = {};
	_float			m_fBulletTimer = 0.f;

public:
	static CPlayer_Bullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


