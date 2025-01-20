#include "../Public/GuideUI.h"
#include "GameInstance.h"


CGuideUI::CGuideUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CGuideUI::CGuideUI(const CGuideUI& Prototype)
	: CUI{ Prototype }
{


}


HRESULT CGuideUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGuideUI::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);



	_float4x4 ViewMat = {};
	_float4x4 ProjectionMat = {};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjectionMat);


	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = pDesc->m_vSize.x;
	m_fSizeY = pDesc->m_vSize.y;

	m_fX = pDesc->m_vPos.x;
	m_fY = pDesc->m_vPos.y;
	m_iBindNum = pDesc->m_iBindNum;









	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);






	CUI::UI_DESC Desc = {};
	m_eType = pDesc->eUItype;

	switch (m_eType)
	{
	case UI_TYPE::GUIDE_BUILD:
	{
		Desc.m_vOffset = { -40.f,0.f };
		Desc.m_vSize = { 30.f,30.f };
		Desc.m_iBindNum = 0;
	}
		break;
	case UI_TYPE::GUIDE_BUILDING:
	{
		Desc.m_vOffset = { -40.f,0.f };
		Desc.m_vSize = { 30.f,30.f };
		Desc.m_iBindNum = 1;
	}
		break;
	case UI_TYPE::GUIDE_ROCK_AND_TREE:
	{
		Desc.m_vOffset = { -40.f,0.f };
		Desc.m_vSize = { 30.f,30.f };
		Desc.m_iBindNum = 0;
	}
	break;


	default:
		break;
	}
		

	CGameObject* pPrototype = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_CraftKeyUI"));
	if (nullptr != pPrototype)	// 존재한다면
	{
		CUI* pBuildingIcon = dynamic_cast<CUI*>(pPrototype->Clone(&Desc)); // 프로토타입의 태그명을 보고 객체의 실 타입을 유추할수있을것 같다.

		m_childUI_List.push_back(pBuildingIcon);
		pBuildingIcon->SetParentUI(this);
	}

	return S_OK;
}


void CGuideUI::Priority_Update(_float fTimeDelta)
{

	if (!m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}

void CGuideUI::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;



	_float4x4 ViewMat = {};
	_float4x4 ProjectionMat = {};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjectionMat);



	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));


	

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}

	MouseOnCheck();
}

void CGuideUI::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CGuideUI::Render()
{
	if (!m_bActivate)
		return S_OK;

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	_float4x4 ViewMat = {};
	D3DXMatrixIdentity(&ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_Texture(m_iBindNum)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;


	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;





	RECT rt = {
		(LONG)(m_fX - 20),
		(LONG)(m_fY - 9),
		(LONG)(m_fX + 150),
		(LONG)(m_fY + 40)
	};

	_wstring strName = L"";
	switch (m_eType)
	{
	case UI_TYPE::GUIDE_BUILD:
	{
		strName = L"건축 보기";
	}
	break;

	case UI_TYPE::GUIDE_BUILDING:
	{
		strName = L"배치 하기";
	}
	break;

	case UI_TYPE::GUIDE_ROCK_AND_TREE:
	{
		strName = L"상호 작용";
	}
	break;
	default:
		break;
	}
	m_pGameInstance->RenderText(strName, &rt, D3DCOLOR_XRGB(255, 255, 255), FONT_TYPE::MEDIUM);




	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Render();
	}

	return S_OK;
}


HRESULT CGuideUI::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SpeechBubble"),
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

HRESULT CGuideUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CGuideUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}



void CGuideUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CGuideUI"));
}

CGuideUI* CGuideUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGuideUI* pInstance = new CGuideUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGuideUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CGuideUI::Clone(void* pArg)
{
	CGuideUI* pInstance = new CGuideUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CGuideUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGuideUI::Free()
{
	__super::Free();

	if (m_childUI_List.size() > 0)
	{
		for (auto& pChildUI : m_childUI_List)
			Safe_Release(pChildUI);
		m_childUI_List.clear();
	}


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
