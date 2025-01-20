
#include "ChainAttack.h"

#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Collider_Cube.h"

#include "CChainGround.h"

CChainAttack::CChainAttack(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject{ pGraphic_Device }
{
}

CChainAttack::CChainAttack(const CChainAttack& Prototype)
	:CGameObject{ Prototype }
{
}

HRESULT CChainAttack::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChainAttack::Initialize(void* pArg)
{
	m_iLevelIndex = m_pGameInstance->Get_LevelIndex();
	m_iStageIndex = m_pGameInstance->Get_StageIndex();

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	_float3* vPosition = static_cast<_float3*>(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *vPosition);

	_float3 vScale = _float3(2.2f, 5.f, 1.f);
	m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);

	m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_ChainGround"),
		sizeof(_float3), &Get_Position());

	return S_OK;
}

void CChainAttack::Priority_Update(_float fTimeDelta)
{
}

void CChainAttack::Update(_float fTimeDelta)
{
	m_fCreateDelayTime += fTimeDelta;
}

void CChainAttack::Late_Update(_float fTimeDelta)
{
	if (m_fCreateDelayTime > 0.6f)
	{
		
		m_pColliderCom->Set_IsActive(true);
		m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

		m_pAnimatorCom->Update(fTimeDelta);

		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	}
	else
	{
		m_pColliderCom->Set_IsActive(false);
	}

	if (m_pAnimatorCom->Get_CurrentFrame() >= 3)
	{
		if (!m_isChainSound)
		{
			m_isChainSound = true;
			m_pGameInstance->Play_Sound(TEXT("Chain_Appear.wav"), SOUND_MONSTERATTACK, g_fEffectVolume);
		}
	}

	if (m_pAnimatorCom->Get_CurrentFrame() >= 31 && !m_isDead)
	{
		m_pGameInstance->Event_DestoryObject(this);
	}
}

HRESULT CChainAttack::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	m_pAnimatorCom->Render(NULL);

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	_float4x4		ViewMatrix{};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

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

HRESULT CChainAttack::Ready_Components()
{	
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

	/*Prototype_Component_Texture_ChainAttack_Ground*/

	return S_OK;
}

HRESULT CChainAttack::Ready_Animation()
{
	CTexture* pTextureCom;

	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_pGameInstance->Get_LevelIndex(), TEXT("Prototype_Component_Texture_ChainAttack"),
		TEXT("Com_Texture_ChainAttack"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;

	m_pAnimatorCom->Add_Animation(TEXT("ChainAttack"), pTextureCom, 0, 33, 0.05f, false);

	m_pAnimatorCom->Change_Animation(TEXT("ChainAttack"), 33);
	return S_OK;
}

HRESULT CChainAttack::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 127);

	return S_OK;
}

HRESULT CChainAttack::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CChainAttack* CChainAttack::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CChainAttack* pInstance = new CChainAttack(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CChainAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CChainAttack::Clone(void* pArg)
{
	CChainAttack* pInstance = new CChainAttack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CChainAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChainAttack::Free()
{
	__super::Free();
	
	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pVIBufferCom);
}
