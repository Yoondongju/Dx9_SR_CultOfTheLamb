#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CGuideUI final : public CUI
{

private:
	CGuideUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGuideUI(const CGuideUI& Prototype);
	virtual ~CGuideUI() = default;


public:
	_uint		Get_BindNum() { return m_iBindNum; }

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


	_float2				    m_vOffest = { 0.f, 0.f };
	_uint				    m_iBindNum = { 0 };

	UI_TYPE					m_eType = {};


private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	virtual void MouseLBtnDown();

public:
	static CGuideUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END