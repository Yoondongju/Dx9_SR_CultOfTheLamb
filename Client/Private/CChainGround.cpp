
#include "CChainGround.h"

#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Collider_Cube.h"

CChainGround::CChainGround(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject{ pGraphic_Device }
{
}

CChainGround::CChainGround(const CChainGround& Prototype)
	:CGameObject{ Prototype }
{
}

HRESULT CChainGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChainGround::Initialize(void* pArg)
{
	m_iLevelIndex = m_pGameInstance->Get_LevelIndex();
	m_iStageIndex = m_pGameInstance->Get_StageIndex();

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_float3* vInitPos = static_cast<_float3*>(pArg);

	vInitPos->y = 0.5f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *vInitPos);

	m_vScale = _float3(1.f, 1.f, 1.f);
	m_pTransformCom->Scaling(m_vScale.x, m_vScale.y, m_vScale.z);

	_float3 vAxis = { vInitPos->x, 0.f,0.f };
	m_pTransformCom->Rotation(vAxis, D3DXToRadian(90.f));
	 
	m_fMaxSize = 3.5f;
	m_fMinSize = 2.5f;

	m_fDestoryTime = 0.f;

	m_pGameInstance->Play_Sound(TEXT("Chain_GroundAppear.wav"), SOUND_MONSTERATTACK, g_fEffectVolume);

	return S_OK;
}

void CChainGround::Priority_Update(_float fTimeDelta)
{
	
}

void CChainGround::Update(_float fTimeDelta)
{
	if (m_vScale.x < m_fMaxSize && !m_isMaxsize)
	{
		m_vScale.x += fTimeDelta * 20.f;
		m_vScale.y += fTimeDelta * 20.f;
		m_vScale.z += fTimeDelta * 20.f;
	}
	else if(m_vScale.x >= m_fMaxSize)
	{
		m_isMaxsize = true;
	}

	if (m_isMaxsize && !m_isMinSize)
	{
		if (m_vScale.x >= m_fMinSize )
		{
			m_vScale.x -= fTimeDelta * 15.f;
			m_vScale.y -= fTimeDelta * 15.f;
			m_vScale.z -= fTimeDelta * 15.f;
		}
		else
		{
			m_vScale.x = m_vScale.y = m_vScale.z = m_fMinSize;
			m_isMinSize = true;
		}
	}

	m_pTransformCom->Scaling(m_vScale);
}

void CChainGround::Late_Update(_float fTimeDelta)
{
	m_fDestoryTime += fTimeDelta;

	if (m_fDestoryTime > 1.6f)
		m_pGameInstance->Event_DestoryObject(this);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CChainGround::Render()
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

	return S_OK;
}

HRESULT CChainGround::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_ChainAttackGround"),
		TEXT("Com_Texture_ChainGround"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
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

HRESULT CChainGround::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);

	return S_OK;
}

HRESULT CChainGround::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CChainGround* CChainGround::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CChainGround* pInstance = new CChainGround(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CChainGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CChainGround::Clone(void* pArg)
{
	CChainGround* pInstance = new CChainGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CChainGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChainGround::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
