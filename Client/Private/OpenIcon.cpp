#include"../Public/OpenIcon.h"
#include "GameInstance.h"

#include "DungeonClearUI.h"

COpenIcon::COpenIcon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

COpenIcon::COpenIcon(const COpenIcon& Prototype)
	: CUI{ Prototype }
{


}


HRESULT COpenIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT COpenIcon::Initialize(void* pArg)
{

	_float4x4 ViewMat = {};
	_float4x4 ProjectionMat = {};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjectionMat);


	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);


	CDungeonClearUI::DUNGEON_CLEARUI_DESC* pDesc = reinterpret_cast<CDungeonClearUI::DUNGEON_CLEARUI_DESC*>(pArg);
	
	m_fX = pDesc->tDesc.m_vPos.x;
	m_fY = pDesc->tDesc.m_vPos.y;

	m_fSizeX = pDesc->tDesc.m_vSize.x;
	m_fSizeY = pDesc->tDesc.m_vSize.y;

	m_vOffest = pDesc->tDesc.m_vOffset;

	m_iIconType = pDesc->eType;



	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;





	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);


	m_bActivate = false;
	
	return S_OK;
}

void COpenIcon::Priority_Update(_float fTimeDelta)
{


	if (!m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}

void COpenIcon::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;



	if (nullptr != m_pParentUI)
	{
		m_fX = m_pParentUI->Get_fX() + m_vOffest.x;
		m_fY = m_pParentUI->Get_fY() + m_vOffest.y;
	}




	m_pAnimatorCom->Update(fTimeDelta);





	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));






	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}

	
}

void COpenIcon::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT COpenIcon::Render()
{
	if (!m_bActivate)
		return S_OK;

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	_float4x4 ViewMat = {};
	D3DXMatrixIdentity(&ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);


	m_pAnimatorCom->Render(NULL);


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;


	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;













	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Render();
	}

	return S_OK;
}

HRESULT COpenIcon::CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum)
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

HRESULT COpenIcon::Ready_Components()
{
	/* For.Com_Texture */

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

HRESULT COpenIcon::Ready_Animation()
{
	CTexture* pTextureCom = nullptr;

	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;




	// 기본 Icon
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_Icon"),
		TEXT("Com_Texture_Icon"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Icon0"), pTextureCom, 0, 28, 0.04f, true);


	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_Icon1"),
		TEXT("Com_Texture_Icon1"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Icon1"), pTextureCom, 0, 28, 0.04f, true);



	// Open Icon
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_OpenIcon"),
		TEXT("Com_Texture_OpenIcon"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Open0"), pTextureCom, 0, 67, 0.01f, false);


	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_OpenIcon1"),
		TEXT("Com_Texture_OpenIcon1"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Open1"), pTextureCom, 0, 67, 0.01f, false);



	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_OpenIcon2"),
		TEXT("Com_Texture_OpenIcon2"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Open2"), pTextureCom, 0, 84, 0.01f, false);


	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_OpenIcon3"),
		TEXT("Com_Texture_OpenIcon3"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Open3"), pTextureCom, 0, 84, 0.01f, false);




	if (false == m_bReadyOpen)
	{
		if(m_iIconType == CDungeonClearUI::DUNGEON_CLEARUI_TYPE::ICON ||
			m_iIconType == CDungeonClearUI::DUNGEON_CLEARUI_TYPE::ICON1)
			m_pAnimatorCom->Change_Animation(TEXT("Icon0"), 0);

		else if (m_iIconType == CDungeonClearUI::DUNGEON_CLEARUI_TYPE::ICON2 ||
			m_iIconType == CDungeonClearUI::DUNGEON_CLEARUI_TYPE::ICON3)
			m_pAnimatorCom->Change_Animation(TEXT("Icon1"), 0);		
	}

	

	return S_OK;
}

HRESULT COpenIcon::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT COpenIcon::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

HRESULT COpenIcon::Ready_MySetting(void* pArg)
{

	return S_OK;
}



COpenIcon* COpenIcon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	COpenIcon* pInstance = new COpenIcon(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : COpenIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* COpenIcon::Clone(void* pArg)
{
	COpenIcon* pInstance = new COpenIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : COpenIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COpenIcon::Free()
{
	__super::Free();

	for (auto& pChildUI : m_childUI_List)
		Safe_Release(pChildUI);
	m_childUI_List.clear();


	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
