#include "../Public/ItemIconUI.h"
#include "GameInstance.h"

#include "Player.h"
#include "PlayerInventory.h"

CItemIconUI::CItemIconUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CItemIconUI::CItemIconUI(const CItemIconUI& Prototype)
	: CUI{ Prototype }
{
}


HRESULT CItemIconUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItemIconUI::Initialize(void* pArg)
{
	ITEMICON_DESC* pDesc = reinterpret_cast<ITEMICON_DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;
	else
	{
		m_tMyDesc.eType = pDesc->eType;
		m_tMyDesc.vOffset = pDesc->vOffset;

		m_iBindNum = (_uint)m_tMyDesc.eType;			//  0  ~  7  이미지 내가 순서대로 이름 바꿔놨어
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

	m_bActivate;

 	return S_OK;
}

void CItemIconUI::Priority_Update(_float fTimeDelta)
{
	switch (dynamic_cast<CPlayerInventoryUI*>(m_pParentUI)->Get_BindNum())
	{
	case 0:
	{
		if (m_tMyDesc.eType == ITEMICON_TYPE::ACORN ||
			m_tMyDesc.eType == ITEMICON_TYPE::BERRIES ||
			m_tMyDesc.eType == ITEMICON_TYPE::COIN)
			m_bActivate = true;
		else
			m_bActivate = false;
	}
	break;

	case 1:
	{
		if (m_tMyDesc.eType == ITEMICON_TYPE::GOLDREFINED ||
			m_tMyDesc.eType == ITEMICON_TYPE::GRASS ||
			m_tMyDesc.eType == ITEMICON_TYPE::WOOD)
			m_bActivate = true;
		else
			m_bActivate = false;
	}
	break;

	case 2:
	{
		if (m_tMyDesc.eType == ITEMICON_TYPE::LUMBER ||
			m_tMyDesc.eType == ITEMICON_TYPE::STONE)
			m_bActivate = true;
		else
			m_bActivate = false;
	}
	break;
	default:
		break;
	}


	if (!m_bActivate)
		return;



	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}

void CItemIconUI::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;

	m_fX = m_pParentUI->Get_fX() + m_tMyDesc.vOffset.x;
	m_fY = m_pParentUI->Get_fY() + m_tMyDesc.vOffset.y;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	MouseOnCheck();

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}
}

void CItemIconUI::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CItemIconUI::Render()
{
	if (!m_bActivate)
		return S_OK;

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;



	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
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


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Render();

	return S_OK;
}

HRESULT CItemIconUI::Ready_Components()
{
	// 0 ~ 7
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Texture_UI_Inventory_ItemIcon",
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

HRESULT CItemIconUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CItemIconUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);



	return S_OK;
}

void CItemIconUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CItemIconUI"));
}

CItemIconUI* CItemIconUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CItemIconUI* pInstance = new CItemIconUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CItemIconUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CItemIconUI::Clone(void* pArg)
{
	CItemIconUI* pInstance = new CItemIconUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CItemIconUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemIconUI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
