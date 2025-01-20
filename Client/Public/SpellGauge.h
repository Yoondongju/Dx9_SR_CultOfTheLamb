#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CSpellGauge final : public CUI
{

private:
	CSpellGauge(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSpellGauge(const CSpellGauge& Prototype);
	virtual ~CSpellGauge() = default;


public:
	_uint		Get_BindNum() { return m_iBindNum; }

	void Set_GaugeRatio(_float _f) { m_fGaugeRatio = _f; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg, void** pOut = nullptr);
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };



	_uint				    m_iBindNum = { 0 };

	_float					m_fTime = { 0.f };

	_float					m_fGaugeRatio = {0.f};



private:
	HRESULT CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum = 0);
	


private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	virtual void MouseLBtnDown();

public:
	static CSpellGauge* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Clone_UI(void* pArg, void** pOut) override;
	virtual void Free() override;
};

END