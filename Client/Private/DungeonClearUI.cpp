#include"../Public/DungeonClearUI.h"
#include "GameInstance.h"


#include "ItemGetUI.h"
#include "OpenIcon.h"

#include "Animator.h"
#include <Camera.h>

#include "Monster.h"
#include <sstream>
#include <iomanip>

#include "MonologuePanelUI.h"

_bool CDungeonClearUI::m_bExitActive;
_bool CDungeonClearUI::m_bExitClick;


CDungeonClearUI::CDungeonClearUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CDungeonClearUI::CDungeonClearUI(const CDungeonClearUI& Prototype)
	: CUI{ Prototype }
{


}


HRESULT CDungeonClearUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDungeonClearUI::Initialize(void* pArg)
{

	_float4x4 ViewMat = {};
	_float4x4 ProjectionMat = {};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjectionMat);


	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	
	DUNGEON_CLEARUI_DESC* pDesc = reinterpret_cast<DUNGEON_CLEARUI_DESC*>(pArg);

	m_fX = pDesc->tDesc.m_vPos.x;
	m_fY= pDesc->tDesc.m_vPos.y;

	m_fSizeX = pDesc->tDesc.m_vSize.x;
	m_fSizeY = pDesc->tDesc.m_vSize.y;

	m_vOriginSize = { m_fSizeX , m_fSizeY };


	m_vOffest = pDesc->tDesc.m_vOffset;


	m_eMyType = pDesc->eType;

	m_strDungeonName = pDesc->strDungeonName;
	m_strAdditionalName = pDesc->strAdditionalName;
	
	
	m_fProceedingTime = pDesc->fProceedingTime;


	if (FAILED(Ready_Components()))
		return E_FAIL;






	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));


	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);



	switch (m_eMyType)
	{
	case Client::CDungeonClearUI::PANEL:
	{
		if (FAILED(Ready_MySetting(pArg)))		// 내 아기들 추가
			return E_FAIL;
		m_fAlpha = 1.f;
	
	}
		break;
	case Client::CDungeonClearUI::TITLE:
	{
		m_strDungeonName = pDesc->strDungeonName;
		m_strAdditionalName = pDesc->strAdditionalName;

		m_fAlpha = 0.f;
	}		
		break;
	case Client::CDungeonClearUI::EXIT:
		m_fAlpha = 0.f;
		break;
	case Client::CDungeonClearUI::ICON:
		break;
	case Client::CDungeonClearUI::ICON1:
		break;
	case Client::CDungeonClearUI::ICON2:
		break;
	case Client::CDungeonClearUI::ICON3:
		break;
	case Client::CDungeonClearUI::CLOCK:
	{
		m_fAlpha = 0.f;
		m_fProceedingTime = pDesc->fProceedingTime;
	}		
		break;
	case Client::CDungeonClearUI::SKULL:
		m_fAlpha = 0.f;
		break;
	case Client::CDungeonClearUI::DUNGEON_CLEARUI_TYPE_END:
		break;
	default:
		break;
	}

	m_bActivate = pDesc->tDesc.bActivate;  




	m_bExitActive = false;
	m_bExitClick = false;

	return S_OK;
}

void CDungeonClearUI::Priority_Update(_float fTimeDelta)
{	
	if (!m_bActivate)
		return;

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}

void CDungeonClearUI::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	if (m_fMouseOnTime == 1)
		m_pGameInstance->Play_Sound(TEXT("UI_Change_Selection.wav"), SOUND_UI, g_fEffectVolume);

	if(m_fMouseClickTime == 1)
		m_pGameInstance->Play_Sound(TEXT("UI_Confirm_Selection.wav"), SOUND_UI, g_fEffectVolume);

	CMonologuePanelUI* pMonoPanelUI = dynamic_cast<CMonologuePanelUI*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_MonologuePanelUI"), 0));
	pMonoPanelUI->SetActivate(true);
	pMonoPanelUI->Set_PanelType(CMonologuePanelUI::PANEL_TYPE::DUNGEON_CLEAR);


	if (false == m_bExitActive)
	{
		if (m_fTime >= 1.f)		// 활성화되고 1초가 지났다면
		{
			if (m_fIconDelay > 0.4f)
			{
				bool bAllActivate = true;

				for (auto& iter = m_childUI_List.begin(); iter != m_childUI_List.end(); ++iter)
				{
					COpenIcon* pIcon = dynamic_cast<COpenIcon*>(*iter);
					if (nullptr == pIcon)
						continue;

					if (false == pIcon->GetActivate())
					{
						pIcon->SetActivate(true);
						m_fIconDelay = 0.f;

						bAllActivate = false;	// 하나라도 덜 켜진게 있니
						break;
					}
				}

				if (bAllActivate)
				{
					for (auto& pUI : m_childUI_List)
					{
						CDungeonClearUI* pChildUI = dynamic_cast<CDungeonClearUI*>(pUI);

						if (nullptr != pChildUI && pChildUI->m_eMyType == DUNGEON_CLEARUI_TYPE::EXIT)
						{
							pChildUI->SetActivate(true);
							m_bExitActive = true;
							m_fTime = 0.f;

							break;
						}
					}
				}
			}
			m_fIconDelay += fTimeDelta;
		}
		m_fTime += fTimeDelta;
	}
	else
	{
		if (m_eMyType == DUNGEON_CLEARUI_TYPE::TITLE ||
			m_eMyType == DUNGEON_CLEARUI_TYPE::EXIT || 
			m_eMyType == DUNGEON_CLEARUI_TYPE::CLOCK || 
			m_eMyType == DUNGEON_CLEARUI_TYPE::SKULL)
		{
			if (m_fAlpha < 1.f)
				m_fAlpha += 0.01f;
			else
				m_fAlpha = 1.f;


			if(m_eMyType == DUNGEON_CLEARUI_TYPE::EXIT)
				MouseOnCheck();

			if (m_bMouseOn)
				m_fMouseOnTime++;
			else
				m_fMouseOnTime = 0;
		}
		
	}

	if (true == m_bExitClick)
	{
		if (m_eMyType == DUNGEON_CLEARUI_TYPE::PANEL)
		{
			m_fMouseClickTime++;

			if (m_fTime > 2.f)
			{
				m_fY += 250 * fTimeDelta;

				if ((m_fY - (m_fSizeY / 2.f)) > g_iWinSizeY)
				{
					CGameObject* pObj = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_MonologuePanelUI"), 0);

					m_pGameInstance->Event_DestoryObject(this);				
					dynamic_cast<CUI*>(pObj)->SetActivate(false);
				}
			}

			m_fTime += fTimeDelta;
		}	
	}
	
	

	// 모든 아이콘 다 뜨면 Exit 아이콘 떠야해 그리고 얘는마우스 On 체크해서 사이즈변경 가야해  /  클릭이벤트도 잇어야하고

	
	if (nullptr != m_pParentUI)
	{
		m_fX = m_pParentUI->Get_fX() + m_vOffest.x;
		m_fY = m_pParentUI->Get_fY() + m_vOffest.y;
	}


	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));



	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}

}

void CDungeonClearUI::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CDungeonClearUI::Render()
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
	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(0));
	m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float));

	m_pShaderCom->Begin_Shader(1);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pShaderCom->End_Shader();





	


	RenderText(m_eMyType);





	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Render();
	}

	return S_OK;
}

HRESULT CDungeonClearUI::CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum)
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

void CDungeonClearUI::MouseClick()
{
	// Exit만 이벤트 들어옴  MouseOnCheck를 Exit에서만 호출하고잇음 

	for (auto& pUI : m_pParentUI->GetChildUI())
	{
		COpenIcon* pOpenIcon = dynamic_cast<COpenIcon*>(pUI);
		if (nullptr == pOpenIcon)
			continue;


		CAnimator* pAnimator = pOpenIcon->Get_Animator();
		DUNGEON_CLEARUI_TYPE eType = (DUNGEON_CLEARUI_TYPE)pOpenIcon->Get_IconType();	

		_wstring strAniName = L"";
		switch (eType)
		{
		case Client::CDungeonClearUI::ICON:
			strAniName = TEXT("Open0");
			break;
		case Client::CDungeonClearUI::ICON1:
			strAniName = TEXT("Open1");
			break;
		case Client::CDungeonClearUI::ICON2:
			break;
		case Client::CDungeonClearUI::ICON3:
			strAniName = TEXT("Open3");
			break;
		default:
			break;

		}

		if (nullptr != pAnimator)
		{
			pAnimator->Change_Animation(strAniName, 0);
		}
	}

	static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(1.f, 0.1f, 1);
	m_bExitClick = true;
}


void CDungeonClearUI::MouseOn()
{
	m_fSizeX = m_vOriginSize.x * 1.3f;
	m_fSizeY = m_vOriginSize.y * 1.3f;
}


void CDungeonClearUI::MouseOut()
{
	m_fSizeX = m_vOriginSize.x;
	m_fSizeY = m_vOriginSize.y;
}


void CDungeonClearUI::RenderText(DUNGEON_CLEARUI_TYPE _eType)
{
	switch (_eType)
	{
	case Client::CDungeonClearUI::TITLE:
	{
		RECT rt = {
			(LONG)(m_fX - 120),
			(LONG)(m_fY - 25),
			(LONG)(m_fX + 400),
			(LONG)(m_fY + 300)
		};

		//m_strDungeonName 로바꾸셈
		m_pGameInstance->RenderText(m_strDungeonName, &rt, D3DCOLOR_ARGB(_uint(255 * m_fAlpha), 255, 255, 255), FONT_TYPE::VERY_BIG);

		rt.left += 50;
		rt.top += 60;

		//m_strAdditionalName으로 바꾸셈
		m_pGameInstance->RenderText(m_strAdditionalName, &rt, D3DCOLOR_ARGB(_uint(255 * m_fAlpha), 128, 128, 128), FONT_TYPE::MEDIUM);
	}
		break;
	case Client::CDungeonClearUI::CLOCK:
	{
		RECT rt = {
			(LONG)(m_fX + 25),
			(LONG)(m_fY - 5),
			(LONG)(m_fX + 400),
			(LONG)(m_fY + 300)
		};


		std::wstringstream wss;
		_float fResult = std::floor(m_fProceedingTime);		//  소수점 자르고

		_uint  iMin = (_uint)(fResult / 60.f);
		_float fSecond = (_float)((_uint)fResult % 60);

		wss << std::fixed << std::setprecision(1) << fSecond;


		m_pGameInstance->RenderText(L"클리어 시간: " + to_wstring(iMin) + L"분" + wss.str() + L"초", &rt, D3DCOLOR_ARGB(_uint(255 * m_fAlpha), 255, 255, 255), FONT_TYPE::MEDIUM);
	}
		break;
	case Client::CDungeonClearUI::SKULL:
	{
		RECT rt = {
			(LONG)(m_fX + 20),
			(LONG)(m_fY - 10),
			(LONG)(m_fX + 400),
			(LONG)(m_fY + 300)
		};

		_uint iCreateMonCount = CMonster::Get_CurLevel_CreateMon_Count((LEVELID)m_pGameInstance->Get_LevelIndex());
		_uint iDeadMonCount = CMonster::Get_CurLevel_DeadMon_Count((LEVELID)m_pGameInstance->Get_LevelIndex());

		//m_pGameInstance->RenderText(L"스폰된 몬스터: " + to_wstring(iCreateMonCount) , &rt, D3DCOLOR_ARGB(_uint(255 * m_fAlpha), 255, 255, 255), FONT_TYPE::MEDIUM);

		//rt.top += 25;
		m_pGameInstance->RenderText(L"처치한 몬스터: " + to_wstring(iDeadMonCount), &rt, D3DCOLOR_ARGB(_uint(255 * m_fAlpha), 255, 0, 0), FONT_TYPE::MEDIUM);
	}
		break;
	default:
		return;
		break;
	}

}

HRESULT CDungeonClearUI::Ready_Components()
{
	/* For.Com_Texture */
	
	_wstring strName = L"";
	switch (m_eMyType)
	{
	case Client::CDungeonClearUI::PANEL:
		strName = L"Prototype_Component_Texture_UI_DungeonClearUI_Panel";
		break;
	case Client::CDungeonClearUI::TITLE:
		strName = L"Prototype_Component_Texture_UI_DungeonClearUI_Title";
		break;
		
	case Client::CDungeonClearUI::EXIT:
		strName = L"Prototype_Component_Texture_UI_DungeonClearUI_Exit";
		break;

	case Client::CDungeonClearUI::CLOCK:
		strName = L"Prototype_Component_Texture_UI_DungeonClearUI_Clock";
		break;

	case Client::CDungeonClearUI::SKULL:
		strName = L"Prototype_Component_Texture_UI_DungeonClearUI_Skull";
		break;

	case Client::CDungeonClearUI::DUNGEON_CLEARUI_TYPE_END:
		break;
	default:
		break;
	}


	if (FAILED(__super::Add_Component(LEVEL_STATIC, strName,
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


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDungeonClearUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}


HRESULT CDungeonClearUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}


HRESULT CDungeonClearUI::Ready_MySetting(void* pArg)
{
	// TITLE
	DUNGEON_CLEARUI_DESC Desc = {};
	Desc.tDesc.m_vPos = { 0.f,0.f };
	Desc.tDesc.m_vOffset = { 0.f, -250.f };
	Desc.tDesc.m_vSize = { 800.f, 100.f };
	Desc.tDesc.bActivate = true;

	DUNGEON_CLEARUI_DESC* pDesc = reinterpret_cast<DUNGEON_CLEARUI_DESC*>(pArg);
	
	Desc.strDungeonName = pDesc->strDungeonName;
	Desc.strAdditionalName = pDesc->strAdditionalName;

	Desc.eType = CDungeonClearUI::DUNGEON_CLEARUI_TYPE::TITLE;
	if (FAILED(CreateMyChildUI(&Desc, TEXT("Prototype_GameObject_DungeonClearUI"))))
		return E_FAIL;



	// Exit
	Desc.tDesc.m_vPos = { 0.f,0.f };
	Desc.tDesc.m_vOffset = { 350.f, 180.f };
	Desc.tDesc.m_vSize = { 50.f, 40.f };
	Desc.tDesc.bActivate = false;
	
	Desc.eType = CDungeonClearUI::DUNGEON_CLEARUI_TYPE::EXIT;
	if (FAILED(CreateMyChildUI(&Desc, TEXT("Prototype_GameObject_DungeonClearUI"))))
		return E_FAIL;



	// Clock
	Desc.tDesc.m_vPos = { 0.f,0.f };
	Desc.tDesc.m_vOffset = { -370.f, -100.f };
	Desc.tDesc.m_vSize = { 40.f, 40.f };
	Desc.tDesc.bActivate = true;
	Desc.fProceedingTime = pDesc->fProceedingTime;
	Desc.eType = CDungeonClearUI::DUNGEON_CLEARUI_TYPE::CLOCK;
	if (FAILED(CreateMyChildUI(&Desc, TEXT("Prototype_GameObject_DungeonClearUI"))))
		return E_FAIL;

	// Skull
	Desc.tDesc.m_vPos = { 0.f,0.f };
	Desc.tDesc.m_vOffset = { -370.f, 50.f };
	Desc.tDesc.m_vSize = { 40.f, 40.f };
	Desc.tDesc.bActivate = true;
	Desc.eType = CDungeonClearUI::DUNGEON_CLEARUI_TYPE::SKULL;
	if (FAILED(CreateMyChildUI(&Desc, TEXT("Prototype_GameObject_DungeonClearUI"))))
		return E_FAIL;






	// Item Icon
	CItemGetUI::ITEM_GETUI_DESC ItemIconDesc = {};
	ItemIconDesc.eUIDesc.m_vPos = { 0.f,0.f };
	ItemIconDesc.eUIDesc.m_vOffset = { -90.f, -120.f };
	ItemIconDesc.eUIDesc.m_vSize = { 50.f, 50.f };
	ItemIconDesc.eItemId = CItem::ITEM_ID::COIN;				// 던전에서 뭘 먹었는지 체크해야해  일단 임시용
	if (FAILED(CreateMyChildUI(&ItemIconDesc, TEXT("Prototype_GameObject_ItemIconUI"))))
		return E_FAIL;


	ItemIconDesc.eUIDesc.m_vPos = { 0.f,0.f };
	ItemIconDesc.eUIDesc.m_vOffset = { -90.f, -70.f };
	ItemIconDesc.eUIDesc.m_vSize = { 50.f, 50.f };
	ItemIconDesc.eItemId = CItem::ITEM_ID::HEART;				// 던전에서 뭘 먹었는지 체크해야해  일단 임시용
	if (FAILED(CreateMyChildUI(&ItemIconDesc, TEXT("Prototype_GameObject_ItemIconUI"))))
		return E_FAIL;


	ItemIconDesc.eUIDesc.m_vPos = { 0.f,0.f };
	ItemIconDesc.eUIDesc.m_vOffset = { -90.f, -10.f };
	ItemIconDesc.eUIDesc.m_vSize = { 50.f, 50.f };
	ItemIconDesc.eItemId = CItem::ITEM_ID::ROCK;				// 던전에서 뭘 먹었는지 체크해야해  일단 임시용
	if (FAILED(CreateMyChildUI(&ItemIconDesc, TEXT("Prototype_GameObject_ItemIconUI"))))
		return E_FAIL;


	ItemIconDesc.eUIDesc.m_vPos = { 0.f,0.f };
	ItemIconDesc.eUIDesc.m_vOffset = { -90.f, 40.f };
	ItemIconDesc.eUIDesc.m_vSize = { 50.f, 50.f };
	ItemIconDesc.eItemId = CItem::ITEM_ID::WOOD;				// 던전에서 뭘 먹었는지 체크해야해  일단 임시용
	if (FAILED(CreateMyChildUI(&ItemIconDesc, TEXT("Prototype_GameObject_ItemIconUI"))))
		return E_FAIL;




	

	// Icon Animation
	Desc.tDesc.m_vPos = { 0.f,0.f };
	Desc.tDesc.m_vOffset = { -100.f, 70.f };
	Desc.tDesc.m_vSize = { 200.f, 200.f };
	Desc.tDesc.bActivate = false;
	Desc.eType = CDungeonClearUI::DUNGEON_CLEARUI_TYPE::ICON;
	if (FAILED(CreateMyChildUI(&Desc, TEXT("Prototype_GameObject_OepnIconUI"))))
		return E_FAIL;
	

	Desc.tDesc.m_vPos = { 0.f,0.f };
	Desc.tDesc.m_vOffset = { -25.f, 70.f };
	Desc.tDesc.m_vSize = { 200.f, 200.f };
	Desc.tDesc.bActivate = false;
	Desc.eType = CDungeonClearUI::DUNGEON_CLEARUI_TYPE::ICON1;
	if (FAILED(CreateMyChildUI(&Desc, TEXT("Prototype_GameObject_OepnIconUI"))))
		return E_FAIL;

	
	Desc.tDesc.m_vPos = { 0.f,0.f };
	Desc.tDesc.m_vOffset = { 50.f, 70.f };
	Desc.tDesc.m_vSize = { 200.f, 200.f };
	Desc.tDesc.bActivate = false;
	Desc.eType = CDungeonClearUI::DUNGEON_CLEARUI_TYPE::ICON1;
	if (FAILED(CreateMyChildUI(&Desc, TEXT("Prototype_GameObject_OepnIconUI"))))
		return E_FAIL;


	Desc.tDesc.m_vPos = { 0.f,0.f };
	Desc.tDesc.m_vOffset = { 125.f, 70.f };
	Desc.tDesc.m_vSize = { 200.f, 200.f };
	Desc.tDesc.bActivate = false;
	Desc.eType = CDungeonClearUI::DUNGEON_CLEARUI_TYPE::ICON3;
	if (FAILED(CreateMyChildUI(&Desc, TEXT("Prototype_GameObject_OepnIconUI"))))
		return E_FAIL;


	

	return S_OK;
}



CDungeonClearUI* CDungeonClearUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDungeonClearUI* pInstance = new CDungeonClearUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDungeonClearUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CDungeonClearUI::Clone(void* pArg)
{
	CDungeonClearUI* pInstance = new CDungeonClearUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CDungeonClearUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDungeonClearUI::Free()
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
