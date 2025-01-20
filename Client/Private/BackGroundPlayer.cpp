#include "..\Public\BackGroundPlayer.h"

#include "GameInstance.h"

#include "Camera.h"

CBackGroundPlayer::CBackGroundPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CBackGroundPlayer::CBackGroundPlayer(const CBackGroundPlayer& Prototype)
	: CGameObject{ Prototype }
{
}


HRESULT CBackGroundPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackGroundPlayer::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION,)

	return S_OK;
}

void CBackGroundPlayer::Priority_Update(_float fTimeDelta)
{
}

void CBackGroundPlayer::Update(_float fTimeDelta)
{
	if (m_bPlaySound)
	{
		m_bPlaySound = false;
		m_pGameInstance->Play_Sound(TEXT("Intro Zoom.wav"), SOUND_EFFECT, g_fEffectVolume);
	}
	m_fZoomInTime += fTimeDelta;
}

void CBackGroundPlayer::Late_Update(_float fTimeDelta)
{
	m_pAnimatorCom->Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CBackGroundPlayer::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;


	CCamera* pCamera = static_cast<CCamera*>(m_pGameInstance->Find_Camera());
	CTransform* pCameraTransform = static_cast<CTransform*>(pCamera->Find_Component(COMPONENT_TYPE::TRANSFORM));


	if (m_fZoomInTime <= 0.5f)
		pCameraTransform->Go_Straight(m_fZoomInTime * 0.06f);


	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &pCameraTransform->Get_WorldMatrix_Inverse());
	_float4x4		ProjMatrix{};
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, D3DXMatrixPerspectiveFovLH(&ProjMatrix, pCamera->Get_Fovy(), (_float)g_iWinSizeX / g_iWinSizeY, pCamera->Get_Near(), pCamera->Get_Far()));

	m_pAnimatorCom->Render(NULL);


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;


	SetUp_RenderState();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Reset_RenderState();

	return S_OK;
}

HRESULT CBackGroundPlayer::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_MainMenu"),
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

HRESULT CBackGroundPlayer::Ready_Animation()
{
	CTexture* pTextureCom = nullptr;

	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(Client::LEVEL_LOGO, TEXT("Prototype_Component_Texture_MainMenu"),
		TEXT("Com_Texture_MainMenu"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;

	m_pAnimatorCom->Add_Animation(TEXT("MainMenu"), pTextureCom, 0, 100, 0.03f);
	m_pAnimatorCom->Change_Animation(TEXT("MainMenu"), 0);

	return S_OK;
}

HRESULT CBackGroundPlayer::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);



	D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255);
	m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, color);


	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);	// 이게 꼭있어야 알파블랜딩댐
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);	// 이게 꼭있어야 알파블랜딩댐
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);	// 이게 꼭있어야 알파블랜딩댐
																					// 이게 꼭있어야 알파블랜딩댐
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// 이게 꼭있어야 알파블랜딩댐
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);	// 이게 꼭있어야 알파블랜딩댐
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// 이게 꼭있어야 알파블랜딩댐
	return S_OK;
}

HRESULT CBackGroundPlayer::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}



CBackGroundPlayer* CBackGroundPlayer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBackGroundPlayer* pInstance = new CBackGroundPlayer(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBackGroundPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CBackGroundPlayer::Clone(void* pArg)
{
	CBackGroundPlayer* pInstance = new CBackGroundPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBackGroundPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGroundPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pAnimatorCom);

}
