#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CChainGround :
	public CGameObject
{
private:
	CChainGround(LPDIRECT3DDEVICE9 pGraphic_Device);
	CChainGround(const CChainGround& Prototype);
	virtual ~CChainGround() = default;

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

	_float3 m_vScale = { 0.f,0.f,0.f };

	_float m_fMaxSize{ 3.f };
	_float m_fMinSize{ 1.f };

	_float m_fDestoryTime = { 0.f };

	_bool m_isMaxsize = { false };
	_bool m_isMinSize = { false };

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	static CChainGround* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END