#include "..\Public\Lodding_GagePanelUI.h"
#include "GameInstance.h"

CLodding_GagePanelUI::CLodding_GagePanelUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CLodding_GagePanelUI::CLodding_GagePanelUI(const CLodding_GagePanelUI& Prototype)
	: CUI{ Prototype }
{
}


HRESULT CLodding_GagePanelUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLodding_GagePanelUI::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = 830;
	m_fSizeY = 30;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY * 0.8f;



	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	return S_OK;
}

void CLodding_GagePanelUI::Priority_Update(_float fTimeDelta)
{
}

void CLodding_GagePanelUI::Update(_float fTimeDelta)
{
	if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
		return;

	MouseOnCheck();

}

void CLodding_GagePanelUI::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CLodding_GagePanelUI::Render()
{
	/*m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);*/

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;



	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);



	if (FAILED(m_pTextureCom->Bind_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT CLodding_GagePanelUI::Ready_Components()
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

void CLodding_GagePanelUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CLodding_GagePanelUI"));
}

CLodding_GagePanelUI* CLodding_GagePanelUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLodding_GagePanelUI* pInstance = new CLodding_GagePanelUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLodding_GagePanelUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CLodding_GagePanelUI::Clone(void* pArg)
{
	CLodding_GagePanelUI* pInstance = new CLodding_GagePanelUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLodding_GagePanelUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLodding_GagePanelUI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
