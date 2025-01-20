#pragma once

#include "Client_Defines.h"
#include "UI.h"

#include "Item.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CShader;
END

BEGIN(Client)

class CItemGetUI final : public CUI
{
public:
	typedef struct tItemGet
	{
		UI_DESC			eUIDesc = { };
		CItem::ITEM_ID  eItemId = { };

		_uint			iLevelIndex = {};
		
		_int			iAddedItem_Count = {};
		_int			iNeedItem_Count = {};

	}ITEM_GETUI_DESC;



private:
	CItemGetUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CItemGetUI(const CItemGetUI& Prototype);
	virtual ~CItemGetUI() = default;


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
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };


	_float2				   m_vOffest = { 0.f, 0.f };

	_uint				   m_iBindNum = { 0 };


	_float				   m_fTargetX = { 0.f };
	_float				   m_fTime = { 0.f };


	_float				   m_fAlpha = { 0.7f };
	bool				   m_bInitSet = false;
	
	_int				   m_iNeedItemCount = {};
	_int				   m_iAddedItem_Count = {};

private:
	HRESULT CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum = 0);

private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();


private:
	virtual void MouseLBtnDown();

public:
	static CItemGetUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END