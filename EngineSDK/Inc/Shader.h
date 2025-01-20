#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(LPDIRECT3DDEVICE9 pGraphic_Device);
	CShader(const CShader& Prototype);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Set_RawValue(D3DXHANDLE hParameter, const void* pData, _uint iLength);
	HRESULT Set_Texture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture);
	HRESULT Set_Float(D3DXHANDLE hParameter, _float fAlpha);

	void Begin_Shader(_uint iIndex);
	void End_Shader();

private:
	LPD3DXEFFECT m_pEffect = { nullptr };

public:
	static CShader* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END