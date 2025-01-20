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

class CMonologuePanelUI final : public CUI
{
public:
	enum PANEL_TYPE { NOBLOOD, DEAD, INVEN_AND_BUILD ,BUILDING, DUNGEON_CLEAR, PANEL_END};

private:
	CMonologuePanelUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonologuePanelUI(const CMonologuePanelUI& Prototype);
	virtual ~CMonologuePanelUI() = default;


public:
	_uint		Get_BindNum() { return m_iBindNum; }
	void		Set_PanelType(PANEL_TYPE _eType) { m_eMyType = _eType; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*		m_pTextureCom[PANEL_TYPE::PANEL_END] = {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };



	_float2				    m_vOffest = { 0.f, 0.f };
	_uint				    m_iBindNum = { 0 };

	_wstring				m_strText = {};
	RECT					m_rtText = {};


	_bool					m_bInCrease = false;


	PANEL_TYPE				m_eMyType = { PANEL_TYPE::PANEL_END };		// 항상 활성화 시킬때 타입정해서 주렴
	_float3					m_vColor = {1.f,1.f,1.f};


private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	virtual void MouseLBtnDown();

public:
	static CMonologuePanelUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END