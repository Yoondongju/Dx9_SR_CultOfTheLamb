#include "MonsterBone.h"

#include "GameInstance.h"
#include "Grid.h"

#include "Item.h"

#include "Player.h"
#include "Bone_Particle.h"

CMonsterBone::CMonsterBone(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CLandObject{ pGraphic_Device }
{
}

CMonsterBone::CMonsterBone(const CMonsterBone& Prototype)
	:CLandObject{ Prototype }
{

}

HRESULT CMonsterBone::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CMonsterBone::Initialize(void* pArg)
{
	BONE_DESC* pBoneDesc = static_cast<BONE_DESC*>(pArg);

	if (FAILED(__super::Initialize(&pBoneDesc->tLandDesc)))
		return E_FAIL;

	m_eBoneType = pBoneDesc->eBoneType;
	m_vFlyDir = pBoneDesc->vFlyDir;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pBoneDesc->vInitPos);

	_float3 vScale = { 1.5f,1.5f,1.5f };
	m_pTransformCom->Scaling(vScale);

	if (FAILED(Ready_Collider()))			// << 콜라이더 뒤로빼야함 이유: 본인 스케일과 위치가 정해져야함
		return E_FAIL;

	m_pRigidBodyCom->SetOwner(this);
	m_pRigidBodyCom->Set_IsGravity(true);
	m_pRigidBodyCom->Set_GravityAccel(2.f);
	m_pRigidBodyCom->Set_Mass(2.f);
	m_pRigidBodyCom->Set_Friction(2.f);

	m_pRigidBodyCom->Add_Force_Direction(_float3(m_vFlyDir.x, 1.f, m_vFlyDir.z), _float3(70.f, 0.f, 70.f), CRigidBody::FORCE); // Shoot!

	m_bIsAbleDead = false;

	return S_OK;
}

void CMonsterBone::Priority_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;
}

void CMonsterBone::Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	m_fAliveTimer += fTimeDelta;

	if (m_fAliveTimer > 0.3f)
	{
		m_bIsAbleDead = true;
	}

	m_pColliderCom->Set_IsActive(true);
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	/*m_fFlyTime += fTimeDelta;

	if (m_fFlyTime < 0.1f)
	{
		m_pTransformCom->Move_Dir(fTimeDelta, m_vFlyDir, 50.f);
	}*/

	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);
}

void CMonsterBone::Late_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pRigidBodyCom->Update(fTimeDelta);

	if (m_pGameInstance->Is_In_Sight(Get_Position(), m_fRadius))
	{
		if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
			return;

		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	}
}

HRESULT CMonsterBone::Render()
{
	m_pVIBufferCom->Filp_Texture(false);

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_Texture()))			// << 여기서 
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
	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_eBoneType));

	m_pShaderCom->Begin_Shader(1);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pShaderCom->End_Shader();

	m_pColliderCom->Render_Collider();

	return S_OK;
}

void CMonsterBone::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Player")
		&& m_bIsAbleDead )
	{
		if (static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK1
			|| static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK2
			|| static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK3)
		{

			m_pGameInstance->Play_Sound(TEXT("Bone_Break.wav"), SOUND_MONSTER, g_fEffectVolume);
			m_pGameInstance->Event_DestoryObject(this);

			CBone_Particle::BONE_PARTICLE_INFO Bone_Particle_Info = {};
			Bone_Particle_Info.iLevelIndex = m_pGameInstance->Get_LevelIndex();
			Bone_Particle_Info.iStageIndex = m_pGameInstance->Get_StageIndex();
			Bone_Particle_Info.vPos = Get_Position();
			Bone_Particle_Info.fSpeed = 30.f;
			Bone_Particle_Info.strTextureTag = TEXT("Prototype_Component_Texture_Bone_Particle_1");
			Bone_Particle_Info.iParticleNum = 1;
			
			Bone_Particle_Info.vDir= _float3((rand() % 5 + 1) * 0.1f, 0.f, (rand() % 5 + 1) * 0.1f);
			m_pGameInstance->Add_CloneObject_ToLayer(Bone_Particle_Info.iLevelIndex, TEXT("Layer_Bone_Particle"), TEXT("Prototype_GameObject_Bone_Particle"), &Bone_Particle_Info);

			Bone_Particle_Info.vDir = _float3((rand() % 5 + 1) * 0.1f, 0.f, -(rand() % 5 + 1) * 0.1f);
			m_pGameInstance->Add_CloneObject_ToLayer(Bone_Particle_Info.iLevelIndex, TEXT("Layer_Bone_Particle"), TEXT("Prototype_GameObject_Bone_Particle"), &Bone_Particle_Info);

			Bone_Particle_Info.strTextureTag = TEXT("Prototype_Component_Texture_Bone_Particle_2");
			Bone_Particle_Info.vDir = _float3(-(rand() % 5 + 1) * 0.1f, 0.f, (rand() % 5 + 1) * 0.1f);
			m_pGameInstance->Add_CloneObject_ToLayer(Bone_Particle_Info.iLevelIndex, TEXT("Layer_Bone_Particle"), TEXT("Prototype_GameObject_Bone_Particle"), &Bone_Particle_Info);

			Bone_Particle_Info.vDir = _float3(-(rand() % 5 + 1) * 0.1f, 0.f, -(rand() % 5 + 1) * 0.1f);
			m_pGameInstance->Add_CloneObject_ToLayer(Bone_Particle_Info.iLevelIndex, TEXT("Layer_Bone_Particle"), TEXT("Prototype_GameObject_Bone_Particle"), &Bone_Particle_Info);
		}
	}
}

void CMonsterBone::OnCollisionStay(CGameObject* pOther)
{
}

void CMonsterBone::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CMonsterBone::Ready_Components()
{
	/* For.Com_Texture */

	// 내가 어떤 텍스쳐를 써야해 ?

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_MonsterBone"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 20.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_Collider_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterBone::Ready_Collider()
{
	/* For.Com_Collider_Cube */
	CCollider_Cube::COLLIDER_CUBE_INFO ColliderInfo;

	_float3 vMyScale = m_pTransformCom->Get_Scaled();

	ZeroMemory(&ColliderInfo, sizeof(CCollider_Cube::COLLIDER_CUBE_INFO));
	ColliderInfo.fSizeX = vMyScale.x * 0.3f;
	ColliderInfo.fSizeY = vMyScale.y * 0.3f;
	ColliderInfo.fSizeZ = vMyScale.z * 0.3f;
	ColliderInfo.fOffSetX = 0.f;
	ColliderInfo.fOffSetY = 0.f;
	ColliderInfo.fOffSetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderInfo)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Blend"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterBone::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CMonsterBone::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CMonsterBone* CMonsterBone::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMonsterBone* pInstance = new CMonsterBone(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CInteractObject"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterBone::Clone(void* pArg)
{
	CMonsterBone* pInstance = new CMonsterBone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CInteractObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterBone::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
