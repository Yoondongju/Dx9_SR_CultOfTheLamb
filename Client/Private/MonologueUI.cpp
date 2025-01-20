#include "../Public/MonologueUI.h"
#include "GameInstance.h"

CMonologueUI::CMonologueUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CMonologueUI::CMonologueUI(const CMonologueUI& Prototype)
	: CUI{ Prototype }
{


}


HRESULT CMonologueUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonologueUI::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	MONOLOGUE_DESC* pDesc = static_cast<MONOLOGUE_DESC*>(pArg);


	m_strText = pDesc->strText;
	m_rtText = pDesc->rt;

	_float4x4 ViewMat = {};
	_float4x4 ProjectionMat = {};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjectionMat);


	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = 700;
	m_fSizeY = 500;

	m_fX = g_iWinSizeX / 2.f;
	m_fY = g_iWinSizeY / 2.f;



	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);


	m_fAlpha = 1.f;

	return S_OK;
}

void CMonologueUI::Priority_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}

void CMonologueUI::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	if (m_fAlpha > 0.f)
		m_fAlpha -= 0.007f;
	if (m_fAlpha <= 0.f)
	{
		m_fAlpha = 0.f;
		m_bActivate = false;
		m_pGameInstance->Event_DestoryObject(this);
	}
		

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}

}

void CMonologueUI::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CMonologueUI::Render()
{
	if (!m_bActivate)
		return S_OK;


	_float4x4 ViewMat = {};
	D3DXMatrixIdentity(&ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);


	
	m_pGameInstance->RenderText(m_strText, &m_rtText, D3DCOLOR_ARGB( _uint(255*m_fAlpha), 255, 0, 0), FONT_TYPE::VERY_BIG);



	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Render();
	}

	return S_OK;
}


HRESULT CMonologueUI::Ready_Components()
{

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

HRESULT CMonologueUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CMonologueUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}



void CMonologueUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CMonologueUI"));
}

CMonologueUI* CMonologueUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMonologueUI* pInstance = new CMonologueUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonologueUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMonologueUI::Clone(void* pArg)
{
	CMonologueUI* pInstance = new CMonologueUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CMonologueUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonologueUI::Free()
{
	__super::Free();

	for (auto& pChildUI : m_childUI_List)
		Safe_Release(pChildUI);
	m_childUI_List.clear();


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
