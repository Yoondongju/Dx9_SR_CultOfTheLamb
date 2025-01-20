#pragma once
#include "Client_Defines.h"
#include "UI.h"


BEGIN(Engine)
class CTransform;
class CVIBuffer_Rect;
class CTexture;
class CFsm;
END

BEGIN(Client)
class CCircle_Select :
    public CUI
{
private:
	CCircle_Select(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCircle_Select(const CCircle_Select& Prototype);
	virtual ~CCircle_Select() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Ready_Components();
private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	void  Turn_Select();

public:
	static CCircle_Select* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
