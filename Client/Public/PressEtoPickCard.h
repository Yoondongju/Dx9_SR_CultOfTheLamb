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
class CPressEtoPickCard :
    public CUI
{
public:
	typedef struct BTNEDesc
	{
		_uint iLevelIndex;
		_int  iType;

	}BTNE_DESC;
private:
	CPressEtoPickCard(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPressEtoPickCard(const CPressEtoPickCard& Prototype);
	virtual ~CPressEtoPickCard() = default;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

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

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

	RECT  m_rText = {};
private:
	HRESULT Ready_Components();
	BTNE_DESC m_BtnEDesc = {};
public:
	static CPressEtoPickCard* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END