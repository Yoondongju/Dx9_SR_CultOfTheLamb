#include "..\Public\BackGround_Crown.h"

#include "GameInstance.h"

CBackGround_Crown::CBackGround_Crown(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CBackGround_Crown::CBackGround_Crown(const CBackGround_Crown& Prototype)
	: CUI{ Prototype }
{
}


HRESULT CBackGround_Crown::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackGround_Crown::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;


	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);


	m_fSizeX = 300;
	m_fSizeY = 300;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;


	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 1.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	return S_OK;
}

void CBackGround_Crown::Priority_Update(_float fTimeDelta)
{
}

void CBackGround_Crown::Update(_float fTimeDelta)
{
	m_fZoomInTime += fTimeDelta;

	if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CBackGround_Crown::Late_Update(_float fTimeDelta)
{
	m_pAnimatorCom->Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CBackGround_Crown::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;


	
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);


	m_pAnimatorCom->Render(NULL);


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;


	SetUp_RenderState();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Reset_RenderState();

	return S_OK;
}

HRESULT CBackGround_Crown::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lodding_Crown"),
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

	return S_OK;
}

HRESULT CBackGround_Crown::Ready_Animation()
{
	CTexture* pTextureCom = nullptr;

	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lodding_Crown"),
		TEXT("Com_Texture_Lodding_Crown"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;

	m_pAnimatorCom->Add_Animation(TEXT("LoddingCrown"), pTextureCom, 0, 200, 0.01f);
	m_pAnimatorCom->Change_Animation(TEXT("LoddingCrown"), 0);

	return S_OK;
}

HRESULT CBackGround_Crown::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//
	///* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	//
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 127);

	return S_OK;
}

HRESULT CBackGround_Crown::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}



CBackGround_Crown* CBackGround_Crown::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBackGround_Crown* pInstance = new CBackGround_Crown(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBackGround_Crown"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CBackGround_Crown::Clone(void* pArg)
{
	CBackGround_Crown* pInstance = new CBackGround_Crown(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBackGround_Crown"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround_Crown::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pAnimatorCom);

}
