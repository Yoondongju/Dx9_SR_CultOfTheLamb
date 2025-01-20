#pragma once
#include "Client_Defines.h"
#include "UI.h"
BEGIN(Engine)
class CTransform;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CAlphabetButton :
    public CUI
{
public:
	enum AlphabetBtnType { F, E, ALPHABTNEND };
private:
	CAlphabetButton(LPDIRECT3DDEVICE9 pGraphic_Device);
	CAlphabetButton(const CAlphabetButton& Prototype);
	virtual ~CAlphabetButton() = default;

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
	HRESULT Ready_Components();

	AlphabetBtnType m_eType = {};


public:
	static CAlphabetButton* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
