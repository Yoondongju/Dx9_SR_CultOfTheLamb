#include "..\Public\LogoButtonUI.h"
#include "GameInstance.h"

#include "Camera.h"
#include "MonologueUI.h"

#include "Level_Logo.h"
#include "Level_Loading.h"

CLogoButtonUI::CLogoButtonUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CLogoButtonUI::CLogoButtonUI(const CLogoButtonUI& Prototype)
	: CUI{ Prototype }
{
}


HRESULT CLogoButtonUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLogoButtonUI::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = 150;
	m_fSizeY = 50;


	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_fX = CLevel_Logo::m_rtStartBindBox.left + (CLevel_Logo::m_rtStartBindBox.right - CLevel_Logo::m_rtStartBindBox.left) * 0.5f;
	m_fY = CLevel_Logo::m_rtStartBindBox.top + (CLevel_Logo::m_rtStartBindBox.bottom - CLevel_Logo::m_rtStartBindBox.top) * 0.5f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));


	// 직교 행렬
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	m_fAlpha = 0.f;

	return S_OK;
}

void CLogoButtonUI::Priority_Update(_float fTimeDelta)
{
}

void CLogoButtonUI::Update(_float fTimeDelta)
{
	CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera());

	if (nullptr != pCamera)
	{
		if (pCamera->Get_IsShowCrew())
		{
			if (m_fAlpha > 0.f)
				m_fAlpha -= 2 * fTimeDelta;
			else
				m_fAlpha = 0.f;
		}
		else
		{
			if (m_fAlpha < 1.f)
				m_fAlpha += 2.f / 60.f;
			else
				m_fAlpha = 1.f;
		}
	}



	if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
		return;

	if (m_iMouseOnTime == 1)
	{
		m_pGameInstance->StopSound(SOUND_UI);
		m_pGameInstance->Play_Sound(TEXT("UI_Change_Selection.wav"), SOUND_UI, g_fEffectVolume);
	}

	m_eCurType = MouseOnCheck_LogoBindBox();
}

void CLogoButtonUI::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CLogoButtonUI::Render()
{
	/*m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);*/

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


	return S_OK;
}

HRESULT CLogoButtonUI::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Button"),
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

HRESULT CLogoButtonUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);


	//D3DCOLOR color = D3DCOLOR_ARGB(m_iAlpha, 255, 255, 255);
	//m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, color);
	//
	//
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);	// 이게 꼭있어야 알파블랜딩댐
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);	// 이게 꼭있어야 알파블랜딩댐
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);	// 이게 꼭있어야 알파블랜딩댐
	//																				// 이게 꼭있어야 알파블랜딩댐
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// 이게 꼭있어야 알파블랜딩댐
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);	// 이게 꼭있어야 알파블랜딩댐
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// 이게 꼭있어야 알파블랜딩댐

	return S_OK;
}

HRESULT CLogoButtonUI::Reset_RenderState()
{
	
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

void CLogoButtonUI::MouseLBtnDown()
{
	switch (m_eCurType)
	{
	case Client::CLogoButtonUI::START:
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, LEVEL_DUNGEON0))))
			MSG_BOX(TEXT("게임 레벨로 넘어가지 못했습니다."));
		break;
	case Client::CLogoButtonUI::CREW:
	{
		CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera());
		if (nullptr != pCamera)
			pCamera->Set_IsShowCrew(true);
	}
		break;
	case Client::CLogoButtonUI::EXIT:
		PostQuitMessage(0);
		break;
	case Client::CLogoButtonUI::BTN_END:
		break;
	default:
		break;
	}
}


CLogoButtonUI::LOGO_BUTTONTYPE CLogoButtonUI::MouseOnCheck_LogoBindBox()
{
	POINT vMousePos = m_pGameInstance->Get_MosePos();

	if (vMousePos.x > CLevel_Logo::m_rtStartBindBox.left
		&& vMousePos.x < CLevel_Logo::m_rtStartBindBox.right
		&& vMousePos.y > CLevel_Logo::m_rtStartBindBox.top
		&& vMousePos.y < CLevel_Logo::m_rtStartBindBox.bottom)
	{
		m_fX = CLevel_Logo::m_rtStartBindBox.left + (CLevel_Logo::m_rtStartBindBox.right - CLevel_Logo::m_rtStartBindBox.left) * 0.5f;
		m_fY = CLevel_Logo::m_rtStartBindBox.top + (CLevel_Logo::m_rtStartBindBox.bottom - CLevel_Logo::m_rtStartBindBox.top) * 0.5f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
			-m_fY + g_iWinSizeY * 0.5f, 0.f));

		m_bMouseOn = true;
		m_iMouseOnTime++;
		return LOGO_BUTTONTYPE::START;
	}
	else if (vMousePos.x > CLevel_Logo::m_rtNameBindBox.left
		&& vMousePos.x < CLevel_Logo::m_rtNameBindBox.right
		&& vMousePos.y > CLevel_Logo::m_rtNameBindBox.top
		&& vMousePos.y < CLevel_Logo::m_rtNameBindBox.bottom)
	{
		m_fX = CLevel_Logo::m_rtNameBindBox.left + (CLevel_Logo::m_rtNameBindBox.right - CLevel_Logo::m_rtNameBindBox.left) * 0.5f;
		m_fY = CLevel_Logo::m_rtNameBindBox.top + (CLevel_Logo::m_rtNameBindBox.bottom - CLevel_Logo::m_rtNameBindBox.top) * 0.5f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
			-m_fY + g_iWinSizeY * 0.5f, 0.f));

		m_bMouseOn = true;
		m_iMouseOnTime++;
		return LOGO_BUTTONTYPE::CREW;
	}

	else if (vMousePos.x > CLevel_Logo::m_rtExitBindBox.left
		&& vMousePos.x < CLevel_Logo::m_rtExitBindBox.right
		&& vMousePos.y > CLevel_Logo::m_rtExitBindBox.top
		&& vMousePos.y < CLevel_Logo::m_rtExitBindBox.bottom)
	{
		m_fX = CLevel_Logo::m_rtExitBindBox.left + (CLevel_Logo::m_rtExitBindBox.right - CLevel_Logo::m_rtExitBindBox.left) * 0.5f;
		m_fY = CLevel_Logo::m_rtExitBindBox.top + (CLevel_Logo::m_rtExitBindBox.bottom - CLevel_Logo::m_rtExitBindBox.top) * 0.5f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
			-m_fY + g_iWinSizeY * 0.5f, 0.f));

		m_bMouseOn = true;
		m_iMouseOnTime++;
		return LOGO_BUTTONTYPE::EXIT;
	}
	else
	{
		m_bMouseOn = false;
		m_iMouseOnTime = 0;
	}

	return LOGO_BUTTONTYPE::BTN_END;
}




CLogoButtonUI* CLogoButtonUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLogoButtonUI* pInstance = new CLogoButtonUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoButtonUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CLogoButtonUI::Clone(void* pArg)
{
	CLogoButtonUI* pInstance = new CLogoButtonUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoButtonUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoButtonUI::Free()
{
	__super::Free();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
