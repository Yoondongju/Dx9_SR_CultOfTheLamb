#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CAnimator;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBackGroundPlayer final : public CGameObject
{
private:
	CBackGroundPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBackGroundPlayer(const CBackGroundPlayer& Prototype);
	virtual ~CBackGroundPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CAnimator*		m_pAnimatorCom = { nullptr };

	_float			m_fZoomInTime = 0.f;
	
	_bool			m_bPlaySound = { true };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Animation();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	static CBackGroundPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END