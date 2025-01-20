#include "../Public/SpellGauge.h"
#include "GameInstance.h"

#include "Player.h"

CSpellGauge::CSpellGauge(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CSpellGauge::CSpellGauge(const CSpellGauge& Prototype)
	: CUI{ Prototype }
{


}


HRESULT CSpellGauge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpellGauge::Initialize(void* pArg, void** pOut)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);


	if (nullptr != pOut)
	{
		*pOut = this;
	}


	_float4x4 ViewMat = {};
	_float4x4 ProjectionMat = {};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjectionMat);


	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fX = pDesc->m_vPos.x;
	m_fY = pDesc->m_vPos.y;


	m_fSizeX = pDesc->m_vSize.x;
	m_fSizeY = pDesc->m_vSize.y;

	m_iBindNum = pDesc->m_iBindNum;

	m_eUItype = pDesc->eUItype;




	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);





	

	if (UI_TYPE::UI_BAR == m_eUItype)
	{
		CUI::UI_DESC Desc = {};
		Desc.m_vPos = { m_fX, m_fY + (m_fSizeY / 2.f)};
		Desc.m_vSize = { 20.f,10.f };
		Desc.m_iBindNum = 1;
		Desc.eUItype = UI_TYPE::UI_ELEMENT;
		if (FAILED(CreateMyChildUI(&Desc, L"Prototype_GameObject_PlayerSpellUI")))
			return E_FAIL;
	}


	m_bActivate = false;  // 얘는 키 눌러야 켜져

	return S_OK;
}

void CSpellGauge::Priority_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}

void CSpellGauge::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	// 야들의 위치는 플레이어가 들고있어서 거쪽에서 해줄거야
	if (m_eUItype == UI_TYPE::UI_BAR)
	{
		

	}

	
	if (m_eUItype == UI_TYPE::UI_ELEMENT)
	{
		if (m_fSizeY >= m_pParentUI->Get_fSizeY())
			m_fSizeY = m_pParentUI->Get_fSizeY();
		else
			m_fSizeY += 1.f;

		m_fX = m_pParentUI->Get_fX();
		m_fY = m_pParentUI->Get_fY() + (m_pParentUI->Get_fSizeY() /2.f) - (m_fSizeY/2.f);
		
	}





	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));




	if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
		return;

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}


}

void CSpellGauge::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CSpellGauge::Render()
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




	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Render();
	}

	return S_OK;
}

HRESULT CSpellGauge::CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum)
{
	CGameObject* pPrototype = m_pGameInstance->Find_Prototype(strPrototypeTag);
	if (nullptr != pPrototype)	// 존재한다면
	{
		CUI* pChildUI = dynamic_cast<CUI*>(pPrototype->Clone(pArg)); // 프로토타입의 태그명을 보고 객체의 실 타입을 유추할수있을것 같다.

		m_childUI_List.push_back(pChildUI);
		pChildUI->SetParentUI(this);
		pChildUI->SetActivate(true);
	}

	return S_OK;
}


HRESULT CSpellGauge::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerSpell"),
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

HRESULT CSpellGauge::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CSpellGauge::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}



void CSpellGauge::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CSpellGauge"));
}

CSpellGauge* CSpellGauge::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSpellGauge* pInstance = new CSpellGauge(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpellGauge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CSpellGauge::Clone(void* pArg)
{
	CSpellGauge* pInstance = new CSpellGauge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CSpellGauge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpellGauge::Clone_UI(void* pArg, void** pOut)
{
	CSpellGauge* pInstance = new CSpellGauge(*this);

	if (FAILED(pInstance->Initialize(pArg, pOut)))
	{
		MSG_BOX(TEXT("Failed to Created : CSpellGauge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpellGauge::Free()
{
	__super::Free();

	for (auto& pChildUI : m_childUI_List)
		Safe_Release(pChildUI);
	m_childUI_List.clear();


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
