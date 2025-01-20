
#include "FlyBallAttack.h"
#include "Boss_DeathCat.h"

#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Collider_Cube.h"

#include "Terrain.h"

CFlyBallAttack::CFlyBallAttack(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBlendObject{ pGraphic_Device }
{
}

CFlyBallAttack::CFlyBallAttack(const CFlyBallAttack& Prototype)
	:CBlendObject{ Prototype }
{
}

HRESULT CFlyBallAttack::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFlyBallAttack::Initialize(void* pArg)
{
	FLYBALL_DESC* pFlyDesc = static_cast<FLYBALL_DESC*>(pArg);

	m_tFlyBallDesc.vPosition = pFlyDesc->vPosition;
	m_tFlyBallDesc.vCenterPosition = pFlyDesc->vCenterPosition;
	m_tFlyBallDesc.vDir = pFlyDesc->vDir;
	m_tFlyBallDesc.fDelayTime = pFlyDesc->fDelayTime;
	m_tFlyBallDesc.fDgree = pFlyDesc->fDgree;
	m_tFlyBallDesc.eType = pFlyDesc->eType;

	m_iLevelIndex = pFlyDesc->iLevelIndex;
	m_iStageIndex = pFlyDesc->iStageIndex;

	m_fRadius = D3DXVec3Length(&(m_tFlyBallDesc.vPosition - m_tFlyBallDesc.vCenterPosition));

	if (STRAGIHT_DGREE == m_tFlyBallDesc.eType || STRAGIHT_DIR == m_tFlyBallDesc.eType)
		m_fDestoryTime = 3.f;
	else
		m_fDestoryTime = 8.f;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_tFlyBallDesc.vPosition.y = 1.5f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tFlyBallDesc.vPosition);

	if (SHADOW == m_tFlyBallDesc.eType)
	{
		_float3 vScale = _float3(2.f, 2.f, 1.f);
		m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);
	}
	else
	{
		_float3 vScale = _float3(1.5f, 1.5f, 1.f);
		m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);
	}

	_float3 vAxis = { m_tFlyBallDesc.vPosition.x, 0.f,0.f };
	m_pTransformCom->Rotation(vAxis, D3DXToRadian(90.f));

	D3DXVec3Normalize(&m_tFlyBallDesc.vDir, &m_tFlyBallDesc.vDir);

	m_fAccTime = 0.f;

	//m_pGameInstance->Play_Sound(TEXT("FlyBall_Shoot.wav"), SOUND_MONSTERATTACK, g_fEffectVolume);

	return S_OK;
}

void CFlyBallAttack::Priority_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

		if (!m_IsMove && m_tFlyBallDesc.fDelayTime < m_fAccTime && m_tFlyBallDesc.eType != SHADOW)
			m_IsMove = true;
}

void CFlyBallAttack::Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	m_fAccTime += fTimeDelta;
	m_fShadowCreateTime += fTimeDelta;

	if (m_IsMove)
	{
		if (STRAGIHT_DGREE == m_tFlyBallDesc.eType)
		{
			m_pTransformCom->Move_Dgree(fTimeDelta, m_tFlyBallDesc.fDgree);
		}
		else if (STRAGIHT_DIR == m_tFlyBallDesc.eType)
		{
			m_pTransformCom->Move_Dir(fTimeDelta, m_tFlyBallDesc.vDir);
		}
		else if (CIRCLE == m_tFlyBallDesc.eType)
		{
			_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_tFlyBallDesc.fDgree += 1.f;

			vPosition.x = m_tFlyBallDesc.vCenterPosition.x + cosf(D3DXToRadian(m_tFlyBallDesc.fDgree)) * m_fRadius;
			vPosition.z = m_tFlyBallDesc.vCenterPosition.z + sinf(D3DXToRadian(m_tFlyBallDesc.fDgree)) * m_fRadius;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

			
		}
		else if (REFLECT == m_tFlyBallDesc.eType)
		{
			Calculate_Reflect();
			m_pTransformCom->Move_Dir(fTimeDelta, m_tFlyBallDesc.vDir);
		}

		if (m_fShadowCreateTime > 0.02f)
		{
			CFlyBallAttack::FLYBALL_DESC tFlyBallDesc = {};
			tFlyBallDesc.vPosition = Get_Position();
			tFlyBallDesc.vPosition.y = -0.01f;
			tFlyBallDesc.eType = SHADOW;
			tFlyBallDesc.iLevelIndex = m_iLevelIndex;
			tFlyBallDesc.iStageIndex = m_iStageIndex;

			m_fShadowCreateTime = 0.f;
			m_pGameInstance->Event_CreateObject(m_iLevelIndex, TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &tFlyBallDesc);
		}
	}

}

void CFlyBallAttack::Late_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	if (m_tFlyBallDesc.eType != SHADOW)
	{
		m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

		if (!m_isDead)
		{
			if (STRAGIHT_DGREE == m_tFlyBallDesc.eType || STRAGIHT_DIR == m_tFlyBallDesc.eType)
			{
				if (m_fAccTime >= m_fDestoryTime + m_tFlyBallDesc.fDelayTime)
				{
					m_pGameInstance->Event_DestoryObject(this);
				}
			}
			else if (CIRCLE == m_tFlyBallDesc.eType)
			{
				if (m_fAccTime >= m_fDestoryTime + (m_tFlyBallDesc.fDelayTime * 2.f))
				{
					m_pGameInstance->Event_DestoryObject(this);
				}
			}
			else if (REFLECT == m_tFlyBallDesc.eType)
			{
				if (m_fAccTime >= 10.f)
				{
					m_pGameInstance->Event_DestoryObject(this);
				}
			}
		}

		if (m_pGameInstance->Is_In_Sight(Get_Position(), m_fRadius))
		{
			m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
		}
	}
	else
	{
		if (m_fAccTime >= 0.3f)
		{
			m_pGameInstance->Event_DestoryObject(this);
		}

		if (m_pGameInstance->Is_In_Sight(Get_Position(), m_fRadius))
		{
			Compute_ViewZ(TEXT("Com_Transform"));
			m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
		}
	}

	if (nullptr != m_pGameInstance->Find_Layer(m_iLevelIndex, TEXT("Layer_Boss")))
	{
		if (dynamic_cast<CBoss_DeathCat*>(m_pGameInstance->Find_Object(m_iLevelIndex, TEXT("Layer_Boss"), 0)))
		{
			if (CBoss_DeathCat::HURT == dynamic_cast<CBoss_DeathCat*>(m_pGameInstance->Find_Object(m_iLevelIndex, TEXT("Layer_Boss"), 0))->Get_CurrentState() 
				&& dynamic_cast<CBoss_DeathCat*>(m_pGameInstance->Find_Object(m_iLevelIndex, TEXT("Layer_Boss"), 0))->Get_IsActive())
			{
				m_pGameInstance->Event_DestoryObject(this);
			}
		}
	}
}

HRESULT CFlyBallAttack::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture()))
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

	if (m_tFlyBallDesc.eType != SHADOW)
		m_pShaderCom->Begin_Shader(0);
	else
		m_pShaderCom->Begin_Shader(1);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pShaderCom->End_Shader();

	//디버그 모드 추가하기

	m_pColliderCom->Render_Collider();
	return S_OK;
}

HRESULT CFlyBallAttack::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_FireFly"),
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
	ColliderInfo.fSizeZ = 0.5f;
	ColliderInfo.fOffSetX = 0.f;
	ColliderInfo.fOffSetY = 0.f;
	ColliderInfo.fOffSetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderInfo)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_FlyBall"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlyBallAttack::SetUp_RenderState()
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

HRESULT CFlyBallAttack::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CFlyBallAttack::Calculate_Reflect()
{
	_float3 vNormal = { 0.f,0.f,0.f };
	_float3 vReflect = { 0.f,0.f,0.f };

	CGameObject* pTerrain = m_pGameInstance->Find_Object(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_BackGround_Terrain"), m_pGameInstance->Get_StageIndex());

	m_fTerrainX = dynamic_cast<CTerrain*>(pTerrain)->Get_TerrainSizeX() - 1.f;
	m_fTerrainZ = dynamic_cast<CTerrain*>(pTerrain)->Get_TerrainSizeZ() - 1.f;

	if (Get_Position().x <= 1.f)
	{
		vNormal = _float3{ 1.f,0.f,0.f };
	}
	else if (Get_Position().x >= m_fTerrainX)
	{
		vNormal = _float3{ -1.f,0.f,0.f };
	}
	else if (Get_Position().z <= 1.f)
	{
		vNormal = _float3{ 0.f,0.f,1.f };
	}
	else if (Get_Position().z >= m_fTerrainZ)
	{
		vNormal = _float3{ 0.f,0.f,-1.f };
	}
	else
	{
		return;
	}

	//R = P + 2n(-p*2n) -> 반사벡터 공식
	vReflect = m_tFlyBallDesc.vDir + 2 * vNormal * D3DXVec3Dot(&(-m_tFlyBallDesc.vDir), &vNormal);

	m_tFlyBallDesc.vDir = vReflect;
}

CFlyBallAttack* CFlyBallAttack::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFlyBallAttack* pInstance = new CFlyBallAttack(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFlyBallAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CFlyBallAttack::Clone(void* pArg)
{
	CFlyBallAttack* pInstance = new CFlyBallAttack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CFlyBallAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlyBallAttack::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
