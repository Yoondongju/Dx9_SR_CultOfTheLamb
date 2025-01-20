#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
class CFsm;
END

BEGIN(Client)
class CCommand_Circle :
    public CUI
{
public:
	enum MouseOnDir{RIGHT,LEFT,UP, DIREND};
private:
	CCommand_Circle(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCommand_Circle(const CCommand_Circle& Prototype);
	virtual ~CCommand_Circle() = default;

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

	RECT  m_rActionNameRect = {};
	RECT  m_rActionExplainRect = {};
	MouseOnDir m_eMouseDir = { DIREND };
private:
	HRESULT Ready_Components();
	_int  m_iBindNum = { 0 };

public:
	void Set_MouseOnDir(MouseOnDir _eDir) {
		m_eMouseDir = _eDir;
	}
	MouseOnDir Get_MouseOnDir() { return m_eMouseDir; }
public:
	static CCommand_Circle* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END