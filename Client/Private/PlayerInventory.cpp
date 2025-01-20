#include "../Public/PlayerInventory.h"
#include "GameInstance.h"

#include "Level_Logo.h"
#include "Player.h"

#include "ItemIconUI.h"


#include "Layer.h"
CPlayerInventoryUI::CPlayerInventoryUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CPlayerInventoryUI::CPlayerInventoryUI(const CPlayerInventoryUI& Prototype)
	: CUI{ Prototype }
{


}


HRESULT CPlayerInventoryUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerInventoryUI::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pOwner = static_cast<CPlayer*>(pArg);
	Safe_AddRef(m_pOwner);



	_float4x4 ViewMat = {};
	_float4x4 ProjectionMat = {};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjectionMat);


	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = g_iWinSizeX /2.f ;
	m_fSizeY = g_iWinSizeY;


	m_fTargetX = g_iWinSizeX / 4.f;
	m_fX = m_fTargetX - (m_fSizeX);
	m_fY = g_iWinSizeY / 2.f;


	



	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);



	CItemIconUI::ITEMICON_DESC Desc = {};
	Desc.eType = CItemIconUI::ITEMICON_TYPE::COIN;
	Desc.vOffset = { -100.f,-100.f };
	//if(FAILED(CreateMyChildUI(&Desc,)))
	//	return E_FAIL;
	Desc.eType = CItemIconUI::ITEMICON_TYPE::ACORN;
	Desc.vOffset = { -200.f, 70.f };
	if(FAILED(CreateMyChildUI(&Desc,L"Prototype_GameObject_InventoryItemUI")))
		return E_FAIL;


	Desc.eType = CItemIconUI::ITEMICON_TYPE::BERRIES;
	Desc.vOffset = { -10.f, 70.f };
	if (FAILED(CreateMyChildUI(&Desc, L"Prototype_GameObject_InventoryItemUI")))
		return E_FAIL;

	

	Desc.eType = CItemIconUI::ITEMICON_TYPE::COIN;
	Desc.vOffset = { 190.f, 70.f };
	if (FAILED(CreateMyChildUI(&Desc, L"Prototype_GameObject_InventoryItemUI")))
		return E_FAIL;


	Desc.eType = CItemIconUI::ITEMICON_TYPE::GOLDREFINED;
	Desc.vOffset = { -200.f, 70.f };
	if (FAILED(CreateMyChildUI(&Desc, L"Prototype_GameObject_InventoryItemUI")))
		return E_FAIL;


	Desc.eType = CItemIconUI::ITEMICON_TYPE::GRASS;
	Desc.vOffset = { -10.f, 70.f };
	if (FAILED(CreateMyChildUI(&Desc, L"Prototype_GameObject_InventoryItemUI")))
		return E_FAIL;



	Desc.eType = CItemIconUI::ITEMICON_TYPE::WOOD;
	Desc.vOffset = { 190.f, 70.f };
	if (FAILED(CreateMyChildUI(&Desc, L"Prototype_GameObject_InventoryItemUI")))
		return E_FAIL;



	Desc.eType = CItemIconUI::ITEMICON_TYPE::LUMBER;
	Desc.vOffset = { -200.f, 70.f };
	if (FAILED(CreateMyChildUI(&Desc, L"Prototype_GameObject_InventoryItemUI")))
		return E_FAIL;


	Desc.eType = CItemIconUI::ITEMICON_TYPE::STONE;
	Desc.vOffset = { -10.f, 70.f };
	if (FAILED(CreateMyChildUI(&Desc, L"Prototype_GameObject_InventoryItemUI")))
		return E_FAIL;


	if (FAILED(Set_ItemText()))
		return E_FAIL;



	m_bActivate = false;  // 얘는 키 눌러야 켜져

	
	return S_OK;
}

void CPlayerInventoryUI::Priority_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyState(KEY::I) == KEY_STATE::TAP)
	{
		if (false == m_bActivate)
		{
			m_bActivate = true;
			m_pGameInstance->Play_Sound(TEXT("UI_Open_Menu.wav"), SOUND_UI, g_fEffectVolume);
			list<CGameObject*>& GameObjects = m_pGameInstance->Find_Layer(LEVEL_STATIC, L"Layer_MainPlayerUI")->Get_ObjectList();
			for (auto& pGameObj : GameObjects)
				dynamic_cast<CUI*>(pGameObj)->SetActivate(false);

		}
			
		else
		{
			m_bActivate = false;
			m_iPlayCount = 0;
			m_pGameInstance->Play_Sound(TEXT("UI_Confirm_Selection.wav"), SOUND_UI, g_fEffectVolume);
			list<CGameObject*>& GameObjects = m_pGameInstance->Find_Layer(LEVEL_STATIC, L"Layer_MainPlayerUI")->Get_ObjectList();
			for (auto& pGameObj : GameObjects)
				dynamic_cast<CUI*>(pGameObj)->SetActivate(true);

		}
			

		m_fVelocityX = 150.f;
		m_fTime = 0.f;
		m_fX = m_fTargetX - (m_fSizeX);
		m_bBouncing = false;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
			-m_fY + g_iWinSizeY * 0.5f, 0.f));

	}

	if (!m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}

void CPlayerInventoryUI::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	
	CMonologuePanelUI* pMonoPanelUI = dynamic_cast<CMonologuePanelUI*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_MonologuePanelUI"), 0));
	pMonoPanelUI->SetActivate(true);
	pMonoPanelUI->Set_PanelType(CMonologuePanelUI::PANEL_TYPE::INVEN_AND_BUILD);


	m_fTime += fTimeDelta;

	if (!m_bBouncing)
	{
		if (m_fX < m_fTargetX)
		{
			m_fX += m_fVelocityX * m_fTime;
		}
		else if (abs(m_fX - m_fTargetX) < EPSILON || m_fX > m_fTargetX)
		{
			m_fX = m_fTargetX;
			m_bBouncing = true;
		}		
	}
	else
	{
		m_fVelocityX -= 9.8f * m_fTime;
		m_fX += m_fVelocityX * fTimeDelta;

		if (m_fX <= m_fTargetX) {
			m_fX = m_fTargetX;
			m_fVelocityX = -m_fVelocityX * 0.7f; // 반발 계수 적용

			if (m_iPlayCount < 4)
			{
				m_pGameInstance->Play_Sound(TEXT("UI_Change_Selection.wav"), SOUND_UI2, g_fEffectVolume);
				m_iPlayCount++;
			}
			// 속도가 매우 작아지면 튀기는 효과 중지
			if (fabs(m_fVelocityX) < EPSILON) {
				m_fVelocityX = 0.0f;
				m_bBouncing = false;
			}
		
		}

	}
	


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));



	if (m_pGameInstance->Get_KeyState(KEY::KEY_TAP) == KEY_STATE::TAP)
	{
			m_pGameInstance->Play_Sound(TEXT("UI_PageFlip.wav"), SOUND_UI, g_fEffectVolume);
		if (++m_iBindNum >= 3)
			m_iBindNum = 0;
	}



	_float4x4 ViewMat = {};
	_float4x4 ProjectionMat = {};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjectionMat);



	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));


	if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
		return;

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}

	MouseOnCheck();
}

void CPlayerInventoryUI::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CPlayerInventoryUI::Render()
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
		(LONG)(m_fX - 75),
		(LONG)(m_fY - 280),
		(LONG)(m_fX - 18),
		(LONG)(m_fY - 180)
	};
	m_pGameInstance->RenderText(L"가방", &rt, D3DCOLOR_XRGB(255, 0, 0), FONT_TYPE::MEDIUM);
	






	_uint iFristSapce_ItemCount  = { };
	_uint iThirdSapce_ItemCount0 = { };
	_uint iThirdSapce_ItemCount1 = { };
	
	switch (m_iBindNum)
	{
	case 0:
	{
		iFristSapce_ItemCount = m_pOwner->Get_ItemCount(CItem::ITEM_ID::COIN);

		rt = { (LONG)(m_fX - 253) ,(LONG)(m_fY + 100) ,(LONG)(m_fX - 73),(LONG)(m_fY + 200) };
		m_pGameInstance->RenderText(m_strItemText[m_iBindNum][0] + L"\n   7개", &rt, D3DCOLOR_XRGB(255, 255, 0), FONT_TYPE::MEDIUM);

		rt = { (LONG)(m_fX - 53),(LONG)(m_fY + 100),(LONG)(m_fX + 127),(LONG)(m_fY + 200) };
		m_pGameInstance->RenderText(m_strItemText[m_iBindNum][1] + L"\n   0개", &rt, D3DCOLOR_XRGB(255, 255, 0), FONT_TYPE::MEDIUM);

		rt = { (LONG)(m_fX + 147),(LONG)(m_fY + 100),(LONG)(m_fX + 327),(LONG)(m_fY + 200) };
		m_pGameInstance->RenderText(m_strItemText[m_iBindNum][2] + L"\n   " + to_wstring(iFristSapce_ItemCount) + L"개", &rt, D3DCOLOR_XRGB(255, 255, 0), FONT_TYPE::MEDIUM);
	}
		break;

	case 1:
	{
		rt = { (LONG)(m_fX - 253) ,(LONG)(m_fY + 100) ,(LONG)(m_fX - 73),(LONG)(m_fY + 200) };
		m_pGameInstance->RenderText(m_strItemText[m_iBindNum][0] + L"\n   4개", &rt, D3DCOLOR_XRGB(255, 255, 0), FONT_TYPE::MEDIUM);

		rt = { (LONG)(m_fX - 53),(LONG)(m_fY + 100),(LONG)(m_fX + 127),(LONG)(m_fY + 200) };
		m_pGameInstance->RenderText(m_strItemText[m_iBindNum][1] + L"\n   0개", &rt, D3DCOLOR_XRGB(255, 255, 0), FONT_TYPE::MEDIUM);

		rt = { (LONG)(m_fX + 147),(LONG)(m_fY + 100),(LONG)(m_fX + 327),(LONG)(m_fY + 200) };
		m_pGameInstance->RenderText(m_strItemText[m_iBindNum][2] + L"\n   0개", &rt, D3DCOLOR_XRGB(255, 255, 0), FONT_TYPE::MEDIUM);
	}
		break;

	case 2:
	{
		iThirdSapce_ItemCount0 = m_pOwner->Get_ItemCount(CItem::ITEM_ID::WOOD);
		iThirdSapce_ItemCount1 = m_pOwner->Get_ItemCount(CItem::ITEM_ID::ROCK);

		rt = { (LONG)(m_fX - 253) ,(LONG)(m_fY + 100) ,(LONG)(m_fX - 73),(LONG)(m_fY + 200) };
		m_pGameInstance->RenderText(m_strItemText[m_iBindNum][0] + L"\n   " + to_wstring(iThirdSapce_ItemCount0) + L"개", &rt, D3DCOLOR_XRGB(255, 255, 0), FONT_TYPE::MEDIUM);

		rt = { (LONG)(m_fX - 53),(LONG)(m_fY + 100),(LONG)(m_fX + 127),(LONG)(m_fY + 200) };
		m_pGameInstance->RenderText(m_strItemText[m_iBindNum][1] + L"\n   " + to_wstring(iThirdSapce_ItemCount1) + L"개", &rt, D3DCOLOR_XRGB(255, 255, 0), FONT_TYPE::MEDIUM);
	}
		break;
	default:
		break;
	}

	
	




	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Render();
	}

	return S_OK;
}

HRESULT CPlayerInventoryUI::CreateMyChildUI(void* pArg, _wstring strPrototypeTag , _uint iPosNum)
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

HRESULT CPlayerInventoryUI::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Inventory_Panel"),
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

HRESULT CPlayerInventoryUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CPlayerInventoryUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

HRESULT CPlayerInventoryUI::Set_ItemText()
{
	m_strItemText[0][0] = L"작은 도토리";
	m_strItemText[0][1] = L"붉은 체리";
	m_strItemText[0][2] = L"별 코인";
	

	m_strItemText[1][0] = L"희귀한 금";
	m_strItemText[1][1] = L"초록 잡초";
	m_strItemText[1][2] = L"단단한 목재";


	m_strItemText[2][0] = L"낡은 목재";
	m_strItemText[2][1] = L"돌 조각";
	m_strItemText[2][2] = L"";


	return S_OK;
}

void CPlayerInventoryUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CPlayerInventoryUI"));
}

CPlayerInventoryUI* CPlayerInventoryUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayerInventoryUI* pInstance = new CPlayerInventoryUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayerInventoryUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CPlayerInventoryUI::Clone(void* pArg)
{
	CPlayerInventoryUI* pInstance = new CPlayerInventoryUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayerInventoryUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerInventoryUI::Free()
{
	__super::Free();

	for (auto& pChildUI : m_childUI_List)
		Safe_Release(pChildUI);
	m_childUI_List.clear();


	Safe_Release(m_pOwner);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
