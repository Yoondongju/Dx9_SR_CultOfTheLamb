#include "../Public/Building_IconUI.h"
#include "GameInstance.h"

#include "Player.h"

CBuilding_IconUI::CBuilding_IconUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CBuilding_IconUI::CBuilding_IconUI(const CBuilding_IconUI& Prototype)
	: CUI{ Prototype }
{
}


HRESULT CBuilding_IconUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBuilding_IconUI::Initialize(void* pArg)
{
	ICON_DESC* pIConDesc = reinterpret_cast<ICON_DESC*>(pArg);

	if (nullptr == pIConDesc)
		return E_FAIL;
	else
	{
		m_tMyDesc.eType = pIConDesc->eType;
		m_tMyDesc.vOffset = pIConDesc->vOffset;
	}
		


	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = 50;
	m_fSizeY = 50;


	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	return S_OK;
}

void CBuilding_IconUI::Priority_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}

void CBuilding_IconUI::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;



	m_fX = m_pParentUI->Get_fX() + m_tMyDesc.vOffset.x;
	m_fY = m_pParentUI->Get_fY() + m_tMyDesc.vOffset.y;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));



	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}
}

void CBuilding_IconUI::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CBuilding_IconUI::Render()
{
	if (!m_bActivate)
		return S_OK;

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


	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Render();

	return S_OK;
}

HRESULT CBuilding_IconUI::Ready_Components()
{
	/* For.Com_Texture */
	
	_wstring strMyPrototypeName = {};
	switch (m_tMyDesc.eType)
	{
	case Client::CBuilding_IconUI::COIN:
		strMyPrototypeName = L"Prototype_Component_Texture_UI_Building_ItemIcon_Coin";
		break;
	case Client::CBuilding_IconUI::LUMBER:
		strMyPrototypeName = L"Prototype_Component_Texture_UI_Building_ItemIcon_Lumber";
		break;
	case Client::CBuilding_IconUI::STONE:
		strMyPrototypeName = L"Prototype_Component_Texture_UI_Building_ItemIcon_Stone";
		break;
	case Client::CBuilding_IconUI::ICON_TYPEEND:
		break;
	default:
		break;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, strMyPrototypeName,
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

HRESULT CBuilding_IconUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CBuilding_IconUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);



	return S_OK;
}

void CBuilding_IconUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CBuilding_IconUI"));
}

CBuilding_IconUI* CBuilding_IconUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBuilding_IconUI* pInstance = new CBuilding_IconUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBuilding_IconUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CBuilding_IconUI::Clone(void* pArg)
{
	CBuilding_IconUI* pInstance = new CBuilding_IconUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBuilding_IconUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBuilding_IconUI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
