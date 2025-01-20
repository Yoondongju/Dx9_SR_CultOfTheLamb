#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CMonologueUI final : public CUI
{
public:

	typedef struct tMonologueUI
	{
		RECT		 rt = {};
		_wstring     strText = {};

	}MONOLOGUE_DESC;




	
private:
	CMonologueUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonologueUI(const CMonologueUI& Prototype);
	virtual ~CMonologueUI() = default;


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
	CTexture*			m_pTextureCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };


	_float2				    m_vOffest = { 0.f, 0.f };
	_uint				    m_iBindNum = { 0 };

	_wstring				m_strText = {};
	RECT					m_rtText = {};

private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	virtual void MouseLBtnDown();

public:
	static CMonologueUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END