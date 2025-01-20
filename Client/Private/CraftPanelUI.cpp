#include "../Public/CraftPanelUI.h"
#include "GameInstance.h"

#include "Level_Logo.h"
#include "Player.h"

#include "CraftIconUI.h"
#include "Layer.h"

CCraftPaenlUI::CCraftPaenlUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CCraftPaenlUI::CCraftPaenlUI(const CCraftPaenlUI& Prototype)
	: CUI{ Prototype }
{


}


HRESULT CCraftPaenlUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCraftPaenlUI::Initialize(void* pArg)
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

	m_fSizeX = g_iWinSizeX / 2.f;
	m_fSizeY = g_iWinSizeY;


	m_fTargetX = g_iWinSizeX / 4.f;
	m_fX = m_fTargetX - (m_fSizeX);
	m_fY = g_iWinSizeY / 2.f;






	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

				
	

	CCraftIconUI::CRAFTICON_DESC Desc = {};
	Desc.eType = CCraftIconUI::CRAFTICON_TYPE::SHRINE;
	Desc.vInitPos = { 800.f, 360.f };
	
	
	
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer_UI(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_CraftIconUI"), &Desc, (void**)&m_pCraftIconUI)))
		MSG_BOX(L"Can't Create Prototype_GameObject_CraftIconUI");

	Safe_AddRef(m_pCraftIconUI);


	m_bActivate = false;  // 얘는 키 눌러야 켜져


	return S_OK;
}

void CCraftPaenlUI::Priority_Update(_float fTimeDelta)
{ 
	if (m_pOwner->IsCanBuilding() && m_pGameInstance->Get_KeyState(KEY::P) == KEY_STATE::TAP)	// 조건 주고
	{
		if (false == m_bActivate)
		{
			m_bActivate = true;
			m_pOwner->Set_Player_State(CPlayer::PLAYERSTATE::BUILD);
			m_pGameInstance->Play_Sound(TEXT("UI_Open_Menu.wav"), SOUND_UI, g_fEffectVolume);

			list<CGameObject*>& GameObjects = m_pGameInstance->Find_Layer(LEVEL_STATIC, L"Layer_MainPlayerUI")->Get_ObjectList();

			if (GameObjects.size() > 0)
			{
				for (auto& pGameObj : GameObjects)
					dynamic_cast<CUI*>(pGameObj)->SetActivate(false);
			}

			CGameObject* pGameObj = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_GuideUI"), 0);

			if (nullptr != pGameObj && !pGameObj->Get_Dead())
			{
				if (FAILED(m_pGameInstance->Event_DestoryObject(pGameObj)))
					MSG_BOX(L"Can't Delete GuideUI");
			}

		}
		else
		{
			m_bActivate = false;
			m_iPlayCount = 0;
			m_pOwner->Set_Player_State(CPlayer::PLAYERSTATE::IDLE);
			m_pGameInstance->Play_Sound(TEXT("UI_Confirm_Selection.wav"), SOUND_UI, g_fEffectVolume);

			list<CGameObject*>& GameObjects = m_pGameInstance->Find_Layer(LEVEL_STATIC, L"Layer_MainPlayerUI")->Get_ObjectList();
			for (auto& pGameObj : GameObjects)
				dynamic_cast<CUI*>(pGameObj)->SetActivate(true);


			CGameObject* pGameObj = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_GuideUI"), 0);
			if (nullptr != pGameObj && !pGameObj->Get_Dead())
			{
				if (FAILED(m_pGameInstance->Event_DestoryObject(pGameObj)))
					MSG_BOX(L"Can't Delete GuideUI");
			}

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

void CCraftPaenlUI::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	if (m_bMouseOn)
		m_iMouseOnCount++;
	if (m_iMouseOnCount == 1)
		m_pGameInstance->Play_Sound(TEXT("UI_Change_Selection.wav"), SOUND_UI, g_fEffectVolume);
	if (m_bMouseOn == false)
		m_iMouseOnCount = 0;

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


	MouseOnBindBox();

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

}

void CCraftPaenlUI::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CCraftPaenlUI::Render()
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

	m_fX;  // 310
	m_fY;  // 430

	RECT rt = {
		(LONG)(m_fX - 75),
		(LONG)(m_fY - 280),
		(LONG)(m_fX - 18),
		(LONG)(m_fY - 180)
	};
	m_pGameInstance->RenderText(L"건설", &rt, D3DCOLOR_XRGB(255, 0, 0), FONT_TYPE::MEDIUM);








	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Render();
	}

	return S_OK;
}

void CCraftPaenlUI::MouseOnBindBox()
{
	POINT vMousePos = m_pGameInstance->Get_MosePos();
	D3DXVECTOR3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	
	switch (m_iBindNum)
	{
	case 0:
	{
		m_pCraftIconUI->SetActivate(false);
		m_bMouseOn = false;
	}
		break;
	case 1:
	{
		if (vMousePos.x > m_rtBindBox0.left
			&& vMousePos.x < m_rtBindBox0.right
			&& vMousePos.y > m_rtBindBox0.top
			&& vMousePos.y < m_rtBindBox0.bottom)
		{
			m_pCraftIconUI->Set_CraftIcon_Type(CCraftIconUI::CRAFTICON_TYPE::SHRINE);

			m_pCraftIconUI->SetActivate(true);
			m_bMouseOn = true;
		}
		else if (vMousePos.x > m_rtBindBox1.left
			&& vMousePos.x < m_rtBindBox1.right
			&& vMousePos.y > m_rtBindBox1.top
			&& vMousePos.y < m_rtBindBox1.bottom)
		{
			m_pCraftIconUI->Set_CraftIcon_Type(CCraftIconUI::CRAFTICON_TYPE::TEMPLE);

			m_pCraftIconUI->SetActivate(true);
			m_bMouseOn = true;
		}
		else
		{
			m_pCraftIconUI->SetActivate(false);
			m_bMouseOn = false;
		}
	}
		break;

	case 2:
	{
		if (vMousePos.x > m_rtBindBox0.left
			&& vMousePos.x < m_rtBindBox0.right
			&& vMousePos.y > m_rtBindBox0.top
			&& vMousePos.y < m_rtBindBox0.bottom)
		{
			m_pCraftIconUI->Set_CraftIcon_Type(CCraftIconUI::CRAFTICON_TYPE::UPGRADE_SHRINE);

			m_pCraftIconUI->SetActivate(true);
			m_bMouseOn = true;
		}
		else
		{
			m_pCraftIconUI->SetActivate(false);
			m_bMouseOn = false;
		}
	}		
		break;

	default:
		break;
	}


}


void CCraftPaenlUI::MouseClick()
{
	CCraftIconUI::CRAFTICON_TYPE eType = m_pCraftIconUI->Get_CraftIcon_Type();	// 얘가 빌딩오브젝트 타입이야, 그리고 Player State - > Building Change 
		
	CLandObject::LANDOBJECT_DESC	Desc = {};

	Desc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
	Desc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
	
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player());



	Desc.vInitPos.x = 11.f;
	Desc.vInitPos.y = pPlayer->Get_Position().y;
	Desc.vInitPos.z = 33.f;
	Desc.iLevelIndex = LEVELID::LEVEL_GAMEPLAY;
	Desc.iStageIndex = 0;

	switch (eType)
	{
	case Client::CCraftIconUI::SHRINE:
	{
		Desc.iInitBindTexNum = CCraftIconUI::SHRINE;
		Desc.vInitScale = _float3(5.f, 8.f, 1.f);		
	}
		break;
	case Client::CCraftIconUI::TEMPLE:
	{
		Desc.iInitBindTexNum = CCraftIconUI::TEMPLE;
		Desc.vInitScale = _float3(5.f, 6.f, 1.f);
	}
		break;
	case Client::CCraftIconUI::UPGRADE_SHRINE:
	{
		Desc.iInitBindTexNum = CCraftIconUI::UPGRADE_SHRINE;
		Desc.vInitScale = _float3(5.f, 8.f, 1.f);
	}
		break;
	case Client::CCraftIconUI::CRAFT_TYPE_END:
		return;			// 안만들거야
		break;
	default:
		return;			// 안만들거야
		break;
	}

	if (FAILED(m_pGameInstance->Event_CreateObject(LEVEL_STATIC, TEXT("Layer_BuildingObject"), TEXT("Prototype_GameObject_BuildingObject"), sizeof(CLandObject::LANDOBJECT_DESC), &Desc)))
		MSG_BOX(L"BuildingObj Error");


	// 해줘야할이 너무많은데 이렇게 처리하는게 맞나
	pPlayer->Set_Player_State(CPlayer::PLAYERSTATE::BUILDING);

	pPlayer->Set_CanBuilding(false);
	m_bMouseOn = false;
	m_bActivate = false;
	m_pCraftIconUI->SetActivate(false);

}


HRESULT CCraftPaenlUI::CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum)
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

HRESULT CCraftPaenlUI::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Craft_Menu"),
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

HRESULT CCraftPaenlUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CCraftPaenlUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}



void CCraftPaenlUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CCraftPaenlUI"));
}

CCraftPaenlUI* CCraftPaenlUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCraftPaenlUI* pInstance = new CCraftPaenlUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCraftPaenlUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CCraftPaenlUI::Clone(void* pArg)
{
	CCraftPaenlUI* pInstance = new CCraftPaenlUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CCraftPaenlUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCraftPaenlUI::Free()
{
	__super::Free();

	for (auto& pChildUI : m_childUI_List)
		Safe_Release(pChildUI);
	m_childUI_List.clear();



	Safe_Release(m_pCraftIconUI);
	Safe_Release(m_pOwner);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
