#include "../Public/CraftIconUI.h"
#include "GameInstance.h"

#include "Player.h"


CCraftIconUI::CCraftIconUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CCraftIconUI::CCraftIconUI(const CCraftIconUI& Prototype)
	: CUI{ Prototype }
{
}


HRESULT CCraftIconUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCraftIconUI::Initialize(void* pArg , void** pOut)
{
	CRAFTICON_DESC* pDesc = reinterpret_cast<CRAFTICON_DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;
	else
	{
		if (nullptr != pOut)
		{
			*pOut = this;
		}
			

		m_eMyType = pDesc->eType;

		switch (m_eMyType)
		{
		case Client::CCraftIconUI::SHRINE:
			m_iBindNum = 0;
			break;
		case Client::CCraftIconUI::TEMPLE:
			m_iBindNum = 1;
			break;
		case Client::CCraftIconUI::UPGRADE_SHRINE:
			m_iBindNum = 2;
			break;
		case Client::CCraftIconUI::CRAFT_TYPE_END:
			break;
		default:
			break;
		}
		
	}


	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = 400;
	m_fSizeY = 250;
	m_fX = pDesc->vInitPos.x;
	m_fY = pDesc->vInitPos.y;

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	m_bActivate = false;

	return S_OK;
}

void CCraftIconUI::Priority_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;



	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}

void CCraftIconUI::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;


	switch (m_eMyType)
	{
	case Client::CCraftIconUI::SHRINE:
		m_iBindNum = 0;
		break;
	case Client::CCraftIconUI::TEMPLE:
		m_iBindNum = 1;
		break;
	case Client::CCraftIconUI::UPGRADE_SHRINE:
		m_iBindNum = 2;
		break;
	case Client::CCraftIconUI::CRAFT_TYPE_END:
		break;
	default:
		break;
	}

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}
}

void CCraftIconUI::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CCraftIconUI::Render()
{
	if (!m_bActivate)
		return S_OK;


	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;


	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
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
		(LONG)(m_fX - 30),
		(LONG)(m_fY + 10),
		(LONG)(m_fX + 150) ,
		(LONG)(m_fY + 50),
	};

	_wstring strName = L"";
	_wstring strDetail = L"";
	switch (m_eMyType)
	{
	case Client::CCraftIconUI::SHRINE:
		strName = L"성지";
		strDetail = L"추종자들이 기도를 드리는곳이다.";
		break;
	case Client::CCraftIconUI::TEMPLE:
		strName = L"교회";
		strDetail = L"추종자들을 관리하는 곳이다.";
		break;
	case Client::CCraftIconUI::UPGRADE_SHRINE:
		strName = L"큰 성지";
		strDetail = L"더 많은 추종자들을 다룰수있다.";
		break;
	case Client::CCraftIconUI::CRAFT_TYPE_END:
		break;
	default:
		break;
	}
		
	m_pGameInstance->RenderText(strName, &rt, D3DCOLOR_XRGB(255, 255, 0), FONT_TYPE::BIG);

	rt.left -= 50;
	rt.top = rt.bottom + 15;
	rt.bottom = rt.top + 25;
	m_pGameInstance->RenderText(strDetail, &rt, D3DCOLOR_XRGB(255, 255, 255), FONT_TYPE::SMALL);


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Render();

	return S_OK;
}

HRESULT CCraftIconUI::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Texture_UI_Craft_Detail",
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

HRESULT CCraftIconUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CCraftIconUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);



	return S_OK;
}

void CCraftIconUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CCraftIconUI"));
}

CCraftIconUI* CCraftIconUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCraftIconUI* pInstance = new CCraftIconUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCraftIconUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CCraftIconUI::Clone(void* pArg)
{
	CCraftIconUI* pInstance = new CCraftIconUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CCraftIconUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCraftIconUI::Clone_UI(void* pArg, void** pOut)
{
	CCraftIconUI* pInstance = new CCraftIconUI(*this);

	if (FAILED(pInstance->Initialize(pArg, pOut)))
	{
		MSG_BOX(TEXT("Failed to Created : CCraftIconUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCraftIconUI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
