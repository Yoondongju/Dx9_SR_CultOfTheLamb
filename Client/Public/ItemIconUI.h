#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CItemIconUI final : public CUI
{
public:
	enum ITEMICON_TYPE { ACORN , BERRIES ,COIN, GOLDREFINED, GRASS , WOOD , LUMBER, STONE, ITEMICON_TYPE_END};

	struct ITEMICON_DESC
	{
		ITEMICON_TYPE eType = ITEMICON_TYPE::ITEMICON_TYPE_END;
		_float2		  vOffset = { 0.f,0.f };
	};



private:
	CItemIconUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CItemIconUI(const CItemIconUI& Prototype);
	virtual ~CItemIconUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*			 m_pTextureCom = { nullptr };
	CVIBuffer_Rect*		 m_pVIBufferCom = { nullptr };

	ITEMICON_DESC		 m_tMyDesc = {};

	_uint				 m_iBindNum = { 0 };


private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	virtual void MouseLBtnDown();

public:
	static CItemIconUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END