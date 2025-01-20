#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CAnimator;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBackGround_Crown final : public CUI
{
private:
	CBackGround_Crown(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBackGround_Crown(const CBackGround_Crown& Prototype);
	virtual ~CBackGround_Crown() = default;

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
	CAnimator* m_pAnimatorCom = { nullptr };

	_float			m_fZoomInTime = 0.f;


private:
	HRESULT Ready_Components();
	HRESULT Ready_Animation();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	static CBackGround_Crown* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END