#include "../Public/ItemIcon.h"
#include "GameInstance.h"

#include "Player.h"

#include "ItemGetUI.h"
#include "DungeonClearUI.h"


CItemIcon::CItemIcon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CItemIcon::CItemIcon(const CItemIcon& Prototype)
	: CUI{ Prototype }
{
}


HRESULT CItemIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItemIcon::Initialize(void* pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);


	CItemGetUI::ITEM_GETUI_DESC* pDesc = reinterpret_cast<CItemGetUI::ITEM_GETUI_DESC*>(pArg);

	m_vOffset.x = pDesc->eUIDesc.m_vOffset.x;
	m_vOffset.y = pDesc->eUIDesc.m_vOffset.y;

	m_fSizeX = pDesc->eUIDesc.m_vSize.x;
	m_fSizeY = pDesc->eUIDesc.m_vSize.y;


	m_iBindNum = pDesc->eItemId;		// 코인 , 하트 , 돌 , 나무


	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	return S_OK;
}

void CItemIcon::Priority_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}
 
void CItemIcon::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;


	m_fX = m_pParentUI->Get_fX() + m_vOffset.x;
	m_fY = m_pParentUI->Get_fY() + m_vOffset.y;





	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));



	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}
}

void CItemIcon::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CItemIcon::Render()
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




	if (nullptr != dynamic_cast<CDungeonClearUI*>(m_pParentUI))
	{
		RECT rt = {
			(LONG)(m_fX + 25),
			(LONG)(m_fY  -10),
			(LONG)(m_fX + 400),
			(LONG)(m_fY + 300)
		};

		_uint* pItemCount = CItem::Get_CurLevel_CreateItem_Count((LEVELID)m_pGameInstance->Get_LevelIndex());

		m_pGameInstance->RenderText(L"획득한 양: " + to_wstring(pItemCount[m_iBindNum]), &rt, D3DCOLOR_ARGB(_uint(255), 255, 255, 0), FONT_TYPE::MEDIUM);
	}
	






	for (auto& pChildUI : m_childUI_List)
		pChildUI->Render();

	return S_OK;
}

HRESULT CItemIcon::Ready_Components()
{
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ItemIcon"),
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

HRESULT CItemIcon::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CItemIcon::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);



	return S_OK;
}

void CItemIcon::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CItemIcon"));
}

CItemIcon* CItemIcon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CItemIcon* pInstance = new CItemIcon(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CItemIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CItemIcon::Clone(void* pArg)
{
	CItemIcon* pInstance = new CItemIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CItemIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemIcon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
