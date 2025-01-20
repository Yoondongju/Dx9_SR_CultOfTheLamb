#include "../Public/ItemGetUI.h"
#include "GameInstance.h"

#include "Level_Logo.h"
#include "Player.h"

#include "ItemIcon.h"
#include "Item.h"

#include "Layer.h"



CItemGetUI::CItemGetUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CItemGetUI::CItemGetUI(const CItemGetUI& Prototype)
	: CUI{ Prototype }
{


}


HRESULT CItemGetUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItemGetUI::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;



	_float4x4 ViewMat = {};
	_float4x4 ProjectionMat = {};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjectionMat);


	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);


	ITEM_GETUI_DESC* pDesc = reinterpret_cast<ITEM_GETUI_DESC*>(pArg);

	m_fX = pDesc->eUIDesc.m_vPos.x;
	m_fY = pDesc->eUIDesc.m_vPos.y;

	m_fSizeX = pDesc->eUIDesc.m_vSize.x;
	m_fSizeY = pDesc->eUIDesc.m_vSize.y;

	m_iBindNum = pDesc->eUIDesc.m_iBindNum;			// 0  or  1 

	m_iLevelIndex = pDesc->iLevelIndex;
	m_iNeedItemCount = pDesc->iNeedItem_Count;
	m_iAddedItem_Count = pDesc->iAddedItem_Count;



	m_fTargetX = 100.f;



	



	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);



	ITEM_GETUI_DESC Desc = {};
	Desc.eUIDesc.m_vOffset = { -70.f,0.f };
	Desc.eUIDesc.m_vSize = { 30.f,30.f };
	Desc.eItemId = pDesc->eItemId;


	CGameObject* pPrototype = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_ItemIconUI"));
	if (nullptr != pPrototype)	// 존재한다면
	{
		CUI* pArrowtUI = dynamic_cast<CUI*>(pPrototype->Clone(&Desc)); // 프로토타입의 태그명을 보고 객체의 실 타입을 유추할수있을것 같다.
	
		m_childUI_List.push_back(pArrowtUI);
		pArrowtUI->SetParentUI(this);
	}




	return S_OK;
}

void CItemGetUI::Priority_Update(_float fTimeDelta)
{
	if (m_fTime > 1.5f)
	{
		m_fX -= 300.f * fTimeDelta;

		if (m_fAlpha > 0.f)
			m_fAlpha -= 0.8f / 60;
		else
		{
			m_fAlpha = 0.f;			
			m_pGameInstance->Event_DestoryObject(this);
		}
	
	}
	
	m_fTime += fTimeDelta;


	if (!m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}

void CItemGetUI::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;


	if (fabs(m_fTargetX - m_fX) < EPSILON || m_fX > m_fTargetX)
	{
		m_bInitSet = true;
		m_fX = m_fTargetX;
	}
	else if (!m_bInitSet)
	{
		m_fX += 300.f * fTimeDelta;
	}




	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));


	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}

}

void CItemGetUI::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CItemGetUI::Render()
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


	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;

	WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4));

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_iBindNum));
	m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float));

	m_pShaderCom->Begin_Shader(1);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pShaderCom->End_Shader();


	

	_wstring	strName = L"";
	CItem::ITEM_ID eType = (CItem::ITEM_ID)dynamic_cast<CItemIcon*>(m_childUI_List.back())->Get_BindNum();

	if (m_iBindNum == 0)
	{
		switch (eType)
		{
		case CItem::ITEM_ID::COIN:
			strName = L"코인 +" + to_wstring(m_iAddedItem_Count);
			break;

		case CItem::ITEM_ID::HEART:
			strName = L"생명력 하트 +" + to_wstring(m_iAddedItem_Count);
			break;

		case CItem::ITEM_ID::ROCK:
			strName = L"돌 +" + to_wstring(m_iAddedItem_Count);
			break;

		case CItem::ITEM_ID::WOOD:
			strName = L"나무 +" + to_wstring(m_iAddedItem_Count);
			break;

		default:
			break;
		}
	}
	else if (m_iBindNum == 1)
	{
		switch (eType)
		{
		case CItem::ITEM_ID::COIN:
			strName = L"코인 -" + to_wstring(m_iNeedItemCount);
			break;

		case CItem::ITEM_ID::HEART:
			strName = L"생명력 하트 -" + to_wstring(m_iNeedItemCount);
			break;

		case CItem::ITEM_ID::ROCK:
			strName = L"돌 -" + to_wstring(m_iNeedItemCount);
			break;

		case CItem::ITEM_ID::WOOD:
			strName = L"나무 -" + to_wstring(m_iNeedItemCount);
			break;

		default:
			break;
		}
	}
	


	RECT rt = {
		(LONG)(m_fX - 20),
		(LONG)(m_fY - 10),
		(LONG)(m_fX + 200),
		(LONG)(m_fY + 100)
	};
	m_pGameInstance->RenderText(strName, &rt, D3DCOLOR_ARGB(_uint(255 * m_fAlpha),255, 255, 255), FONT_TYPE::MEDIUM);




	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Render();
	}

	return S_OK;
}

HRESULT CItemGetUI::CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum)
{
	CGameObject* pPrototype = m_pGameInstance->Find_Prototype(strPrototypeTag);
	if (nullptr != pPrototype)	// 존재한다면
	{
		CUI* pChildUI = dynamic_cast<CUI*>(pPrototype->Clone(pArg)); // 프로토타입의 태그명을 보고 객체의 실 타입을 유추할수있을것 같다.

		m_childUI_List.push_back(pChildUI);
		pChildUI->SetParentUI(this);
	}

	return S_OK;
}

HRESULT CItemGetUI::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ItemGet"),
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


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemGetUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CItemGetUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}


void CItemGetUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CItemGetUI"));
}

CItemGetUI* CItemGetUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CItemGetUI* pInstance = new CItemGetUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CItemGetUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CItemGetUI::Clone(void* pArg)
{
	CItemGetUI* pInstance = new CItemGetUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CItemGetUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemGetUI::Free()
{
	__super::Free();

	if (m_childUI_List.size() > 0)
	{
		for (auto& pChildUI : m_childUI_List)
			Safe_Release(pChildUI);
		m_childUI_List.clear();
	}

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
