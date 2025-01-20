#include "Shader.h"

CShader::CShader(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent { pGraphic_Device }
{
	m_eComponentType = SHADER;
}

CShader::CShader(const CShader& Prototype)
	: CComponent{ Prototype }
	, m_pEffect(Prototype.m_pEffect)
{

}

HRESULT CShader::Initialize_Prototype(const _tchar* pFilePath)
{	
	//hlsl 파일 불러오기
	if (FAILED(D3DXCreateEffectFromFile(m_pGraphic_Device, pFilePath, nullptr, nullptr, 0, nullptr, &m_pEffect, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShader::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CShader::Set_RawValue(D3DXHANDLE hParameter, const void* pData, _uint iLength)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetRawValue(hParameter, pData, 0, iLength);
}

HRESULT CShader::Set_Texture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetTexture(hParameter, pTexture);
}

HRESULT CShader::Set_Float(D3DXHANDLE hParameter, _float fAlpha)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return  m_pEffect->SetFloat(hParameter, fAlpha);
}

//그래픽 디바이스의 draw랑 draw end같은거임

//VIBuffer 바인드 전에 둘 것
void CShader::Begin_Shader(_uint iIndex)
{
	m_pEffect->Begin(nullptr, 0);
	m_pEffect->BeginPass(iIndex);
}

//VIBuffer 바인드 후에 둘 것
void CShader::End_Shader()
{
	m_pEffect->EndPass();
	m_pEffect->End();
}

CShader * CShader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath)
{
	CShader*		pInstance = new CShader(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CShader"));
		Safe_Release(pInstance);
	}

 	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CShader::Clone(void* pArg)
{
	CShader*		pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CShader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	if(!m_isCloned)
		Safe_Release(m_pEffect);
}
