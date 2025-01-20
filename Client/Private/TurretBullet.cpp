
#include "TurretBullet.h"

#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Collider_Cube.h"

CTurretBullet::CTurretBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject{ pGraphic_Device }
{
}

CTurretBullet::CTurretBullet(const CTurretBullet& Prototype)
	:CGameObject{ Prototype }
{
}

HRESULT CTurretBullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTurretBullet::Initialize(void* pArg)
{
	TURRETBullet_DESC* pBulletDesc = static_cast<TURRETBullet_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tTurretBulletDesc.vPosition);

	m_tTurretBulletDesc.vPosition = pBulletDesc->vPosition;

	_float3 vScale = _float3(1.f, 1.f, 0.1f);
	m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);

	return S_OK;
}

void CTurretBullet::Priority_Update(_float fTimeDelta)
{
}

void CTurretBullet::Update(_float fTimeDelta)
{
	m_pTransformCom->Move_Dir(fTimeDelta, m_tTurretBulletDesc.vDir);
}

void CTurretBullet::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTurretBullet::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	//디버그 모드 추가하기
	m_pColliderCom->Render_Collider();

	return S_OK;
}

HRESULT CTurretBullet::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(Client::LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FireFly"),
		TEXT("Com_Texture_FireFly"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
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

	/* For.Com_Collider_Cube */
	CCollider_Cube::COLLIDER_CUBE_INFO ColliderInfo;

	ZeroMemory(&ColliderInfo, sizeof(CCollider_Cube::COLLIDER_CUBE_INFO));
	ColliderInfo.fSizeX = 0.15f;
	ColliderInfo.fSizeY = 0.2f;
	ColliderInfo.fSizeZ = 0.2f;
	ColliderInfo.fOffSetX = 0.f;
	ColliderInfo.fOffSetY = 0.f;
	ColliderInfo.fOffSetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderInfo)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTurretBullet::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

	return S_OK;
}

HRESULT CTurretBullet::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CTurretBullet* CTurretBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTurretBullet* pInstance = new CTurretBullet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFlyBallAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CTurretBullet::Clone(void* pArg)
{
	CTurretBullet* pInstance = new CTurretBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CFlyBallAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTurretBullet::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
