#include "..\Public\Title.h"
#include "GameInstance.h"

#include "Camera.h"

#include "Level_Logo.h"

CTitle::CTitle(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CTitle::CTitle(const CTitle& Prototype)
	: CUI{ Prototype }
{
}


HRESULT CTitle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTitle::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = 270;
	m_fSizeY = 150;

	m_fX = g_iWinSizeX >> 3;
	m_fY = g_iWinSizeY >> 2;


	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);


	m_fAlpha = 0.f;

	return S_OK;
}

void CTitle::Priority_Update(_float fTimeDelta)
{
}

void CTitle::Update(_float fTimeDelta)
{
	CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera());

	if (nullptr != pCamera)
	{
		if (pCamera->Get_IsShowCrew())
		{
			if (m_fAlpha > 0.f)
				m_fAlpha -= 2 * fTimeDelta;
			else
				m_fAlpha = 0.f;
		}
		else
		{
			if (m_fAlpha < 1.f)
				m_fAlpha += 2.f / 60.f;
			else
				m_fAlpha = 1.f;
		}
	}



	if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
		return;

	if (LEVELID::LEVEL_LOGO == m_pGameInstance->Get_LevelIndex())
	{
		m_iAlpha = static_cast<CLevel_Logo*>(m_pGameInstance->Get_CurLevel())->Get_LogoAlpha();
	}

	MouseOnCheck();

}

void CTitle::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CTitle::Render()
{
	/*m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);*/

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	_float4x4 ViewMat = {};
	D3DXMatrixIdentity(&ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;



	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;

	WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4));

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(0));
	m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float));

	m_pShaderCom->Begin_Shader(1);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pShaderCom->End_Shader();
	

	return S_OK;
}

HRESULT CTitle::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Title"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTitle::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);


	D3DCOLOR color = D3DCOLOR_ARGB(m_iAlpha, 255, 255, 255);
	m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, color);
	
	
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);	// 이게 꼭있어야 알파블랜딩댐
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);	// 이게 꼭있어야 알파블랜딩댐
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);	// 이게 꼭있어야 알파블랜딩댐
																					// 이게 꼭있어야 알파블랜딩댐
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// 이게 꼭있어야 알파블랜딩댐
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);	// 이게 꼭있어야 알파블랜딩댐
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// 이게 꼭있어야 알파블랜딩댐

	return S_OK;
}

HRESULT CTitle::Reset_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, 255);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

void CTitle::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CTitle"));
}

CTitle* CTitle::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTitle* pInstance = new CTitle(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTitle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CTitle::Clone(void* pArg)
{
	CTitle* pInstance = new CTitle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTitle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTitle::Free()
{
	__super::Free();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
