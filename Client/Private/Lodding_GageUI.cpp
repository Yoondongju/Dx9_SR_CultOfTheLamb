#include "..\Public\Lodding_GageUI.h"
#include "GameInstance.h"

#include "Level_Loading.h"



_float CLodding_GageUI::m_fLoadingTime = 0.f;

CLodding_GageUI::CLodding_GageUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CLodding_GageUI::CLodding_GageUI(const CLodding_GageUI& Prototype)
	: CUI{ Prototype }
{
}


HRESULT CLodding_GageUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLodding_GageUI::Initialize(void* pArg)
{
	m_fLoadingTime = 0.1f;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = 830;
	m_fSizeY = 30;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY * 0.8f;

	
	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - (g_iWinSizeX * 0.5f) - (m_fSizeX * 0.5f) ,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
	
	return S_OK;
}

void CLodding_GageUI::Priority_Update(_float fTimeDelta)
{
}

void CLodding_GageUI::Update(_float fTimeDelta)
{
	if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
		return;

	MouseOnCheck();

}

void CLodding_GageUI::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CLodding_GageUI::Render()
{
	/*m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);*/

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;



	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);


	

	

	_float fRatioSize = m_fSizeX * m_fLoadingTime;

	if (fRatioSize >= m_fSizeX)
		fRatioSize = m_fSizeX;
	m_pTransformCom->Scaling(fRatioSize, m_fSizeY, 1.f);


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - (g_iWinSizeX * 0.5f) - (m_fSizeX * 0.5f) + fRatioSize * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	if (FAILED(m_pTextureCom->Bind_Texture(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT CLodding_GageUI::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lodding_Gage"),
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

void CLodding_GageUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CLodding_GageUI"));
}

CLodding_GageUI* CLodding_GageUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLodding_GageUI* pInstance = new CLodding_GageUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLodding_GageUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CLodding_GageUI::Clone(void* pArg)
{
	CLodding_GageUI* pInstance = new CLodding_GageUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLodding_GageUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLodding_GageUI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
