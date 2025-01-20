#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CShader;
END

BEGIN(Client)

class CLogoButtonUI final : public CUI
{
public:
	enum LOGO_BUTTONTYPE{ START, CREW, EXIT , BTN_END };

private:
	CLogoButtonUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLogoButtonUI(const CLogoButtonUI& Prototype);
	virtual ~CLogoButtonUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*			m_pTextureCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };

	LOGO_BUTTONTYPE		m_eCurType = LOGO_BUTTONTYPE::BTN_END;

	_int				m_iMouseOnTime = { 0 };

private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();


private:
	virtual void		 MouseLBtnDown();
	LOGO_BUTTONTYPE		 MouseOnCheck_LogoBindBox();


public:
	static CLogoButtonUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END