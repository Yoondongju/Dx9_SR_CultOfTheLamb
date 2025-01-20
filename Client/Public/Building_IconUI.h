#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBuilding_IconUI final : public CUI
{
public:
	enum ICON_TYPE { COIN, LUMBER, STONE, ICON_TYPEEND };

	typedef struct tIconDesc
	{
		ICON_TYPE		eType;
		_float2			vOffset;

	}ICON_DESC;



private:
	CBuilding_IconUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBuilding_IconUI(const CBuilding_IconUI& Prototype);
	virtual ~CBuilding_IconUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*		 m_pTextureCom = { nullptr };
	CVIBuffer_Rect*  m_pVIBufferCom = { nullptr };

	ICON_DESC		 m_tMyDesc = {};

private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	virtual void MouseLBtnDown();

public:
	static CBuilding_IconUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END