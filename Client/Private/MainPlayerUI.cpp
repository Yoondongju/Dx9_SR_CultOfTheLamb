#include "../Public/MainPlayerUI.h"
#include "GameInstance.h"

#include "Player.h"

#include "PlayerInventory.h"
#include "CraftPanelUI.h"

CMainPlayerUI::CMainPlayerUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CMainPlayerUI::CMainPlayerUI(const CMainPlayerUI& Prototype)
	: CUI{ Prototype }
{


}


HRESULT CMainPlayerUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMainPlayerUI::Initialize(void* pArg)
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

	m_fTargetY = m_fY += 100.f;








	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);






	m_bActivate = true;  // 얘는 키 눌러야 켜져


	return S_OK;
}

void CMainPlayerUI::Priority_Update(_float fTimeDelta)
{
	_bool bFlagInven = dynamic_cast<CPlayerInventoryUI*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_InventoryUI"), 0))->GetActivate();
	_bool bFlagCraft = dynamic_cast<CCraftPaenlUI*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_CraftUI"), 0))->GetActivate();

	if (LEVELID::LEVEL_GAMEPLAY != m_pGameInstance->Get_LevelIndex())
		m_bActivate = false;
	else if (LEVELID::LEVEL_GAMEPLAY == m_pGameInstance->Get_LevelIndex() &&
		false == bFlagInven &&
		false == bFlagCraft)
		m_bActivate = true;


	


	if (false == m_bActivate)
	{
		m_fY = 0.f;
		m_fVelocityY = 100.f;
		m_fTime = 0.f;
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

void CMainPlayerUI::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_fTime += fTimeDelta;

	if (!m_bBouncing)
	{
		if (m_fY < m_fTargetY)
		{
			m_fY += m_fVelocityY * m_fTime;
		}
		else if (abs(m_fY - m_fTargetY) < EPSILON || m_fY > m_fTargetY)
		{
			m_fY = m_fTargetY;
			m_bBouncing = true;
		}
	}
	else
	{
		m_fVelocityY -= 9.8f * m_fTime;
		m_fY += m_fVelocityY * fTimeDelta;

		if (m_fY <= m_fTargetY) {
			m_fY = m_fTargetY;
			m_fVelocityY = -m_fVelocityY * 0.7f; // 반발 계수 적용

			// 속도가 매우 작아지면 튀기는 효과 중지
			if (fabs(m_fVelocityY) < EPSILON) {
				m_fVelocityY = 0.0f;
				m_bBouncing = false;
			}
		}
	}



	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));



	


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

void CMainPlayerUI::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CMainPlayerUI::Render()
{
	if (!m_bActivate || m_pGameInstance->Get_LevelIndex() != LEVEL_GAMEPLAY)
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


	_uint iFollowerCount = static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_FollowerCount();

	RECT rt = {
		(LONG)(m_fX + 25),
		(LONG)(m_fY + 10),
		(LONG)(m_fX + 150),
		(LONG)(m_fY + 40)
	};
	m_pGameInstance->RenderText(L"세뇌한 신도: "+ to_wstring(iFollowerCount) + L"명", &rt, D3DCOLOR_XRGB(255, 255, 0), FONT_TYPE::SMALL);
	rt.top += 15;
	m_pGameInstance->RenderText(L"수용가능 신도: 4명", &rt, D3DCOLOR_XRGB(255, 255, 255), FONT_TYPE::SMALL);


	
	


	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Render();
	}

	return S_OK;
}

HRESULT CMainPlayerUI::CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum)
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

HRESULT CMainPlayerUI::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerState"),
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

HRESULT CMainPlayerUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CMainPlayerUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}



void CMainPlayerUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CMainPlayerUI"));
}

CMainPlayerUI* CMainPlayerUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMainPlayerUI* pInstance = new CMainPlayerUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainPlayerUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMainPlayerUI::Clone(void* pArg)
{
	CMainPlayerUI* pInstance = new CMainPlayerUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CMainPlayerUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainPlayerUI::Free()
{
	__super::Free();

	for (auto& pChildUI : m_childUI_List)
		Safe_Release(pChildUI);
	m_childUI_List.clear();


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
