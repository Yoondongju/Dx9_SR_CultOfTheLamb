#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_GridRect;
END


BEGIN(Client)

class CGrid final : public CBlendObject
{
public:
	typedef struct tGridDesc
	{
		CGameObject*	pOwner = { nullptr };
		_uint			iBindNum = {};

		_float3			vPos = {};
		_float3			vScale = {};

	}GRID_DESC;



private:
	CGrid(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGrid(const CGrid& Prototype);
	virtual ~CGrid() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	GRID_DESC				m_tMyDesc = {};

	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_GridRect*		m_pVIBufferCom = { nullptr };

	
	bool					m_bActivate = { false };
	_float3					m_vInitOffset = {};



private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();


public:
	static CGrid* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};

END
