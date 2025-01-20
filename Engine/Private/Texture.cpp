#include "..\Public\Texture.h"

CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent { pGraphic_Device }
{
	m_eComponentType = TEXTURE;
}

CTexture::CTexture(const CTexture & Prototype)
	: CComponent{ Prototype }
	, m_iNumTextures{ Prototype.m_iNumTextures }
	, m_Textures{ Prototype.m_Textures }
{
	for (auto& pTexture : m_Textures)
		Safe_AddRef(pTexture);

	m_eComponentType = TEXTURE;
}

HRESULT CTexture::Initialize_Prototype(TYPE eTextureType, const _wstring & strTextureFilePath, _uint iNumTextures)
{	
	m_iNumTextures = iNumTextures;

	m_Textures.reserve(iNumTextures);

	for (size_t i = 0; i < iNumTextures; i++)
	{
		IDirect3DBaseTexture9*		pTexture = { nullptr };

		_tchar		szTextureFilePath[MAX_PATH] = TEXT("");

		wsprintf(szTextureFilePath, strTextureFilePath.c_str(), i);

		HRESULT hr = eTextureType == TYPE_TEXTURE2D
			? D3DXCreateTextureFromFile(m_pGraphic_Device, szTextureFilePath, reinterpret_cast<LPDIRECT3DTEXTURE9*>(&pTexture))
			: D3DXCreateCubeTextureFromFile(m_pGraphic_Device, szTextureFilePath, reinterpret_cast<LPDIRECT3DCUBETEXTURE9*>(&pTexture));

		if (FAILED(hr))
		{
			if (hr == D3DERR_OUTOFVIDEOMEMORY || hr == E_OUTOFMEMORY)
			{
				// 메모리 부족으로 인한 에러 처리
				MSG_BOX(TEXT("텍스쳐 올릴 메모리 부족"));
				return E_FAIL;
			}
			return E_FAIL;
		}

		m_Textures.emplace_back(pTexture);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_Texture(_uint iIndex, _uint iStage)
{
	if (iIndex >= m_iNumTextures)
		return E_FAIL;

	return m_pGraphic_Device->SetTexture(iStage, m_Textures[iIndex]);
}

CTexture * CTexture::Create(LPDIRECT3DDEVICE9 pGraphic_Device, TYPE eTextureType, const _wstring & strTextureFilePath, _uint iNumTextures)
{
	CTexture*		pInstance = new CTexture(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(eTextureType, strTextureFilePath, iNumTextures)))
	{
		MSG_BOX(TEXT("Failed to Created :  vCTexture"));
		Safe_Release(pInstance);
	}

 	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CTexture::Clone(void* pArg)
{
	CTexture*		pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pTexture : m_Textures)
		Safe_Release(pTexture);

	m_Textures.clear();
}
