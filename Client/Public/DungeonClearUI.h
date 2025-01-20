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

class CDungeonClearUI final : public CUI
{
public:
	enum DUNGEON_CLEARUI_TYPE { PANEL, TITLE, EXIT , ICON, ICON1 ,ICON2 ,ICON3 ,CLOCK,SKULL ,DUNGEON_CLEARUI_TYPE_END};		//  ´õ Ãß°¡ÇÏ·Å ..

	typedef struct tDungeonUI
	{
		CUI::UI_DESC				tDesc = {};
		DUNGEON_CLEARUI_TYPE		eType = {};
		
		_wstring					strDungeonName = L"";
		_wstring					strAdditionalName = L"";

		_float						fProceedingTime = {};

	}DUNGEON_CLEARUI_DESC;

private:
	CDungeonClearUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDungeonClearUI(const CDungeonClearUI& Prototype);
	virtual ~CDungeonClearUI() = default;


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
	CTexture*			   m_pTextureCom = { nullptr };
	CVIBuffer_Rect*		   m_pVIBufferCom = { nullptr };
	CShader*			   m_pShaderCom = { nullptr };


	_float2				   m_vOffest = { 0.f, 0.f };
	_uint				   m_iBindNum = { 0 };
	_uint				   m_fMouseOnTime = { 0 };
	_uint				   m_fMouseClickTime = { 0 };

	DUNGEON_CLEARUI_TYPE   m_eMyType = { DUNGEON_CLEARUI_TYPE_END };
	
	_wstring			   m_strDungeonName = L"";
	_wstring			   m_strAdditionalName = L"";


	_float				   m_fTime = { 0.f };
	_float				   m_fIconDelay = { 0.f };
	_float				   m_fProceedingTime = {};
	

	static _bool		   m_bExitActive;
	static _bool		   m_bExitClick;

	_float2				   m_vOriginSize = {};
	


private:
	HRESULT CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum = 0);

	virtual void MouseOn() override;
	virtual void MouseOut() override;
	virtual void MouseClick() override;

	void	RenderText(DUNGEON_CLEARUI_TYPE _eType);

private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

	HRESULT Ready_MySetting(void* pArg);


public:
	static CDungeonClearUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END