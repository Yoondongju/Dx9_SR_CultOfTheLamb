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

class CTalkIcon final : public CUI
{
private:
	CTalkIcon(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTalkIcon(const CTalkIcon& Prototype);
	virtual ~CTalkIcon() = default;

public:
	void  Set_BindNum(_uint _i) { m_iBindNum = _i; }
	_uint Get_BindNum() { return m_iBindNum; }

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
	CShader* m_pShaderCom = { nullptr };


	_float2				m_vOffest = { 0.f,0.f };

	_uint				m_iBindNum = { 0 };

	_float				m_fRangeY = {};
	_float				m_fOriginY = {};			
	_int				m_iDir = { 1 };


private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	void	MoveToFloat(_float fDeltaTime);
	
public:
	static CTalkIcon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END