#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype);
	virtual ~CVIBuffer_Terrain() = default;


public:
	_uint Get_NumVerticesX() { return m_iNumVerticesX; }
	_uint Get_NumVerticesZ() { return m_iNumVerticesZ; }

	_float		Get_Border_LeftPos() { return m_fBorderTo_LeftX; }
	_float		Get_Border_RightPos() { return m_fBorderTo_RightX; }

	_float		Get_Border_UpPos() { return m_fBorderTo_UpZ; }
	_float		Get_Border_DownPos() { return m_fBorderTo_DownZ; }

public:
	virtual HRESULT Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ);
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg) override;

public:
	_float Compute_Height(const _float3& vLocalPos);
	virtual _bool IsPick(class CTransform* pTransform, _float3* pOut = nullptr);
	void Setting_TerrainVertex(_uint iNumX, _uint iNumZ);
	
private:
	_uint			m_iNumVerticesX = {};
	_uint			m_iNumVerticesZ = {};


	_float			m_fBorderTo_LeftX = {};
	_float			m_fBorderTo_RightX = {};

	_float			m_fBorderTo_UpZ = {};
	_float			m_fBorderTo_DownZ = {};

public:
	static CVIBuffer_Terrain* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iNumVerticesX, _uint iNumVerticesZ);
	static CVIBuffer_Terrain* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pHeightMapFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END