#include "..\Public\BuildingPanelUI.h"
#include "GameInstance.h"

#include "Level_Logo.h"
#include "BuildingObject.h"

#include "Camera.h"

#include "Building_IconUI.h"
#include "Layer.h"

#include "Player.h"

CBuildingPanelUI::CBuildingPanelUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CBuildingPanelUI::CBuildingPanelUI(const CBuildingPanelUI& Prototype)
	: CUI{ Prototype }
{
}


HRESULT CBuildingPanelUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBuildingPanelUI::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pOwner = static_cast<CBuildingObject*>(pArg);
	Safe_AddRef(m_pOwner);



	m_fX = g_iWinSizeX / 2.f;
	m_fY = g_iWinSizeY / 2.f - 200.f;
	

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = 500;
	m_fSizeY = 80;

	
	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);






	CBuilding_IconUI::ICON_DESC tIcon_Desc = {};

	CBuildingObject::BUILDING_TYPE eOwnerType = (CBuildingObject::BUILDING_TYPE)m_pOwner->Get_BindTextNum();

	switch (eOwnerType)
	{
	case Client::CBuildingObject::SHRINE:
	{
		tIcon_Desc.eType = CBuilding_IconUI::ICON_TYPE::COIN;
		tIcon_Desc.vOffset.x = -180.f;
		tIcon_Desc.vOffset.y = 0.f;
		if (FAILED(CreateMyChildUI(&tIcon_Desc)))
			return E_FAIL;

		tIcon_Desc.eType = CBuilding_IconUI::ICON_TYPE::LUMBER;
		tIcon_Desc.vOffset.x = 45.f;
		tIcon_Desc.vOffset.y = 0.f;
		if (FAILED(CreateMyChildUI(&tIcon_Desc)))
			return E_FAIL;
	}
		break;
	case Client::CBuildingObject::SANCTUARY:
	{
		tIcon_Desc.eType = CBuilding_IconUI::ICON_TYPE::COIN;
		tIcon_Desc.vOffset.x = -180.f;
		tIcon_Desc.vOffset.y = 0.f;
		if (FAILED(CreateMyChildUI(&tIcon_Desc)))
			return E_FAIL;

		tIcon_Desc.eType = CBuilding_IconUI::ICON_TYPE::STONE;
		tIcon_Desc.vOffset.x = 45.f;
		tIcon_Desc.vOffset.y = 0.f;
		if (FAILED(CreateMyChildUI(&tIcon_Desc)))
			return E_FAIL;
	}
		break;
	case Client::CBuildingObject::UPGRADE_SHRINE:
	{
		tIcon_Desc.eType = CBuilding_IconUI::ICON_TYPE::STONE;
		tIcon_Desc.vOffset.x = -180.f;
		tIcon_Desc.vOffset.y = 0.f;
		if (FAILED(CreateMyChildUI(&tIcon_Desc)))
			return E_FAIL;

		tIcon_Desc.eType = CBuilding_IconUI::ICON_TYPE::LUMBER;
		tIcon_Desc.vOffset.x = 45.f;
		tIcon_Desc.vOffset.y = 0.f;
		if (FAILED(CreateMyChildUI(&tIcon_Desc)))
			return E_FAIL;
	}
		break;
	default:
		break;
	}

	
	CUI::UI_DESC Desc = {};

	Desc.m_vPos = { g_iWinSizeX / 2.f , g_iWinSizeY - 70.f };
	Desc.m_vSize = { 200.f,40.f };
	Desc.m_iBindNum = 0;
	Desc.eUItype = UI_TYPE::GUIDE_BUILDING;

	if (FAILED(m_pGameInstance->Event_CreateObject(LEVEL_STATIC, TEXT("Layer_GuideUI"), TEXT("Prototype_GameObject_GuideUI"), sizeof(CUI::UI_DESC), &Desc)))
		MSG_BOX(L"Can't Create GuideUI");


	return S_OK;
}

void CBuildingPanelUI::Priority_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}

void CBuildingPanelUI::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	if (m_pOwner->IsLand())
	{
		m_bActivate = false;

		list<CGameObject*>& GameObjects = m_pGameInstance->Find_Layer(LEVEL_STATIC, L"Layer_MainPlayerUI")->Get_ObjectList();
		for (auto& pGameObj : GameObjects)
			dynamic_cast<CUI*>(pGameObj)->SetActivate(true);
	}		
	else
		m_bActivate = true;



	CMonologuePanelUI* pMonoPanelUI = dynamic_cast<CMonologuePanelUI*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_MonologuePanelUI"), 0));
	pMonoPanelUI->SetActivate(true);
	pMonoPanelUI->Set_PanelType(CMonologuePanelUI::PANEL_TYPE::BUILDING);




	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));


	

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}

	MouseOnCheck();
}

void CBuildingPanelUI::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CBuildingPanelUI::Render()
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


	RECT rt = {
		(LONG)(m_fX - (m_fSizeX / 2.f) + 100),
		(LONG)(m_fY - 20) ,
		(LONG)(m_fX + (m_fSizeX / 2.f) + 30),
		(LONG)(m_fY + (m_fSizeY /2.f))
	};



	CBuildingObject::BUILDING_TYPE iOwnerType = (CBuildingObject::BUILDING_TYPE)m_pOwner->Get_BindTextNum();

	_uint iNeedItem0_Count = {};
	_uint iNeedItem1_Count = {};

	_uint iHaveItem0_Count = {};
	_uint iHaveItem1_Count = {};

	switch (iOwnerType)
	{
	case Client::CBuildingObject::SHRINE:			
	{
		iNeedItem0_Count = 3;							// 코인 
		iNeedItem1_Count = 3;							// 낡은목재
	
		iHaveItem0_Count = static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_ItemCount(CItem::ITEM_ID::COIN);
		iHaveItem1_Count = static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_ItemCount(CItem::ITEM_ID::WOOD);
	}
		break;
	case Client::CBuildingObject::SANCTUARY:
	{
		iNeedItem0_Count = 7;							// 코인
		iNeedItem1_Count = 10;						    // 돌조각

		iHaveItem0_Count = static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_ItemCount(CItem::ITEM_ID::COIN);
		iHaveItem1_Count = static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_ItemCount(CItem::ITEM_ID::ROCK);
	}
		break;
	case Client::CBuildingObject::UPGRADE_SHRINE:
	{		
		iNeedItem0_Count = 6;							// 돌조각
		iNeedItem1_Count = 15;							// 낡은목재

		iHaveItem0_Count = static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_ItemCount(CItem::ITEM_ID::ROCK);
		iHaveItem1_Count = static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_ItemCount(CItem::ITEM_ID::WOOD);
	}
		break;
	default:
		break;
	}

	


	if (iNeedItem0_Count <= iHaveItem0_Count)
	{
		m_pGameInstance->RenderText(L"필요갯수: " + to_wstring(iNeedItem0_Count) + L"개", &rt, D3DCOLOR_XRGB(255, 255, 0), FONT_TYPE::SMALL);
	}
	else
		m_pGameInstance->RenderText(L"필요갯수: " + to_wstring(iNeedItem0_Count) + L"개", &rt, D3DCOLOR_XRGB(255, 0, 0), FONT_TYPE::SMALL);

	rt.top += 20;
	m_pGameInstance->RenderText(L"보유갯수: " + to_wstring(iHaveItem0_Count) + L"개", &rt, D3DCOLOR_XRGB(255, 255, 0), FONT_TYPE::MEDIUM);



	rt.left += 225;
	rt.top -= 20;
	


	if (iNeedItem1_Count <= iHaveItem1_Count)
	{
		m_pGameInstance->RenderText(L"필요갯수: " + to_wstring(iNeedItem1_Count) + L"개", &rt, D3DCOLOR_XRGB(255, 255, 0), FONT_TYPE::SMALL);
	}
	else
		m_pGameInstance->RenderText(L"필요갯수: " + to_wstring(iNeedItem1_Count) + L"개", &rt, D3DCOLOR_XRGB(255, 0, 0), FONT_TYPE::SMALL);
	
	rt.top += 20;
	m_pGameInstance->RenderText(L"보유갯수: " + to_wstring(iHaveItem1_Count) + L"개", &rt, D3DCOLOR_XRGB(255, 255, 0), FONT_TYPE::MEDIUM);

	
	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Render();
	}

	return S_OK;
}

HRESULT CBuildingPanelUI::CreateMyChildUI(void *pArg)
{
	CGameObject* pPrototype = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_BuildingIconUI"));
	if (nullptr != pPrototype)	// 존재한다면
	{
		CUI* pBuildingIcon = dynamic_cast<CUI*>(pPrototype->Clone(pArg)); // 프로토타입의 태그명을 보고 객체의 실 타입을 유추할수있을것 같다.

		m_childUI_List.push_back(pBuildingIcon);
		pBuildingIcon->SetParentUI(this);
	}

	return S_OK;
}

HRESULT CBuildingPanelUI::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BuildingPanelUI"),
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

HRESULT CBuildingPanelUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CBuildingPanelUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

void CBuildingPanelUI::MouseLBtnDown()
{
	


}

CBuildingPanelUI* CBuildingPanelUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBuildingPanelUI* pInstance = new CBuildingPanelUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBuildingPanelUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CBuildingPanelUI::Clone(void* pArg)
{
	CBuildingPanelUI* pInstance = new CBuildingPanelUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBuildingPanelUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBuildingPanelUI::Free()
{
	__super::Free();

	if (m_childUI_List.size() > 0)
	{
		for (auto& pChildUI : m_childUI_List)
			Safe_Release(pChildUI);
		m_childUI_List.clear();
	}


	Safe_Release(m_pOwner);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
