#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Terrain;
class CShader;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
private:
	CTerrain(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTerrain(const CTerrain& Prototype);
	virtual ~CTerrain() = default;


public:
	void Set_BindTexNum(_uint _iTexNum) { m_iBindTexNum = _iTexNum; }
	void Set_VIBufferTag(_wstring strVIBufferTag) { m_strViBufferTag = strVIBufferTag; }

	_float Get_TerrainSizeX() { return m_fBorderRight.x; }
	_float Get_TerrainSizeZ() { return m_fBorderUp.z; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool Check_InTerrain(_float3 vPos);

private:
	CTexture*				m_pTextureCom = { nullptr };	
	CTexture*				m_pLightTextureCom = { nullptr };
	CTexture*				m_pCircleMapTextureCom = { nullptr };
	CVIBuffer_Terrain*		m_pVIBufferCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

	_uint					m_iBindTexNum = {};
	_wstring				m_strViBufferTag = {};

	_float3 m_fBorderLeft = _float3{ 0.f, 0.f, 0.f };
	_float3 m_fBorderRight = _float3{ 0.f, 0.f, 0.f };
	_float3 m_fBorderUp = _float3{ 0.f, 0.f, 0.f };
	_float3 m_fBorderDown = _float3{ 0.f, 0.f, 0.f };

	_float3 vGusionPos = { 0.f,0.f,0.f };
	_float3 vEligorPos = { 0.f,0.f,0.f };
	_float3 vHekatPos = { 0.f,0.f,0.f };

	class CBoss_Gusion* m_pGusion = nullptr;
	class CBoss_Eligor* m_pEligor = nullptr;
	class CBoss_Hekat* m_pHekat = nullptr;

private:
	HRESULT Ready_Components();
	HRESULT Ready_BuildingGrid();

	_float3 Compute_TerrainCenter();

public:
	static CTerrain* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END