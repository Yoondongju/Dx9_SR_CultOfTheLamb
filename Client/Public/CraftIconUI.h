#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CCraftIconUI final : public CUI
{
public:
	enum CRAFTICON_TYPE { SHRINE, TEMPLE , UPGRADE_SHRINE , CRAFT_TYPE_END };

	struct CRAFTICON_DESC
	{
		CRAFTICON_TYPE eType = CRAFTICON_TYPE::CRAFT_TYPE_END;
		_float2		  vInitPos = { 0.f,0.f };

	};


private:
	CCraftIconUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCraftIconUI(const CCraftIconUI& Prototype);
	virtual ~CCraftIconUI() = default;


public:
	void			Set_CraftIcon_Type(CRAFTICON_TYPE _eType) { m_eMyType = _eType; }
	CRAFTICON_TYPE  Get_CraftIcon_Type() { return m_eMyType; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg, void** pOut = nullptr);
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*			 m_pTextureCom = { nullptr };
	CVIBuffer_Rect*		 m_pVIBufferCom = { nullptr };

	_uint				 m_iBindNum = { 0 };

	CRAFTICON_TYPE		 m_eMyType = {};


private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	virtual void MouseLBtnDown();



public:
	static CCraftIconUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Clone_UI(void* pArg, void** pOut) override;

	virtual void Free() override;
};

END