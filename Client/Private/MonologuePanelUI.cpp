#include "../Public/MonologuePanelUI.h"
#include "GameInstance.h"

CMonologuePanelUI::CMonologuePanelUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CMonologuePanelUI::CMonologuePanelUI(const CMonologuePanelUI& Prototype)
	: CUI{ Prototype }
{


}


HRESULT CMonologuePanelUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonologuePanelUI::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;





	_float4x4 ViewMat = {};
	_float4x4 ProjectionMat = {};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjectionMat);


	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	m_fX = g_iWinSizeX / 2.f;
	m_fY = g_iWinSizeY / 2.f;



	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.1f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);


	m_fAlpha = 1.f;


	m_bActivate = false;

	return S_OK;
}

void CMonologuePanelUI::Priority_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;



	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}

void CMonologuePanelUI::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;


	
	if (m_bInCrease == false)		// 감소해	
	{
		m_fAlpha -= 0.02f;	

		if (m_fAlpha <= 0.5f)
			m_bInCrease = true;
		
	}
	else                            // 증가해
	{
		m_fAlpha += 0.02f;

		if (m_fAlpha >= 1.f)
			m_bInCrease = false;
		
	}


	switch (m_eMyType)
	{
	case Client::CMonologuePanelUI::NOBLOOD:
		m_vColor = { 1.f,0.f,0.f };
		break;
	case Client::CMonologuePanelUI::DEAD:
		m_vColor = { 0.1f,0.1f,0.1f };
		break;
	case Client::CMonologuePanelUI::INVEN_AND_BUILD:
		m_vColor = { 0.2f,0.2f,0.2f };
		m_fAlpha = 0.7f;
		break;
	case Client::CMonologuePanelUI::BUILDING:
		m_vColor = { 0.f,0.f,1.f };
		m_fAlpha = 0.4f;
		break;
	case Client::CMonologuePanelUI::DUNGEON_CLEAR:
		m_vColor = { 1.f,0.1f,0.1f };
		break;
	case Client::CMonologuePanelUI::PANEL_END:
		break;
	default:
		break;
	}
	
	if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
		return;


	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}

}

void CMonologuePanelUI::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CMonologuePanelUI::Render()
{
	if (!m_bActivate)
		return S_OK;



	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	_float4x4 ViewMat = {};
	D3DXMatrixIdentity(&ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);


	
	if (m_eMyType == PANEL_TYPE::PANEL_END)
		return S_OK;


	if (FAILED(m_pTextureCom[m_eMyType]->Bind_Texture(0)))
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
	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom[m_eMyType]->Get_Texture(0));


	m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float));

	m_pShaderCom->Set_RawValue("g_fColor", &m_vColor, sizeof(_float3));


	m_pShaderCom->Begin_Shader(4);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pShaderCom->End_Shader();



	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Render();
	}

	return S_OK;
}


HRESULT CMonologuePanelUI::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monologue_Panel_NoBlood"),
		TEXT("Com_Texture_NOBLOOD"), reinterpret_cast<CComponent**>(&m_pTextureCom[PANEL_TYPE::NOBLOOD]), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monologue_Panel_Dead"),
		TEXT("Com_Texture_DEAD"), reinterpret_cast<CComponent**>(&m_pTextureCom[PANEL_TYPE::DEAD]), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monologue_Panel_DungeonClear"),
		TEXT("Com_Texture_DUNGEON_CLEAR"), reinterpret_cast<CComponent**>(&m_pTextureCom[PANEL_TYPE::DUNGEON_CLEAR]), nullptr)))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monologue_Panel_Inven_And_Build"),
		TEXT("Com_Texture_INVEN_AND_BUILD"), reinterpret_cast<CComponent**>(&m_pTextureCom[PANEL_TYPE::INVEN_AND_BUILD]), nullptr)))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monologue_Panel_Building"),
		TEXT("Com_Texture_BUILDING"), reinterpret_cast<CComponent**>(&m_pTextureCom[PANEL_TYPE::BUILDING]), nullptr)))
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

HRESULT CMonologuePanelUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CMonologuePanelUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}



void CMonologuePanelUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CMonologuePanelUI"));
}

CMonologuePanelUI* CMonologuePanelUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMonologuePanelUI* pInstance = new CMonologuePanelUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonologuePanelUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMonologuePanelUI::Clone(void* pArg)
{
	CMonologuePanelUI* pInstance = new CMonologuePanelUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CMonologuePanelUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonologuePanelUI::Free()
{
	__super::Free();

	for (auto& pChildUI : m_childUI_List)
		Safe_Release(pChildUI);
	m_childUI_List.clear();


	Safe_Release(m_pShaderCom);

	for (_uint i = 0; i < PANEL_TYPE::PANEL_END; i++)
	{
		Safe_Release(m_pTextureCom[i]);
	}
	

	Safe_Release(m_pVIBufferCom);
}
