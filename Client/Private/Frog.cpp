#include "Frog.h"
#include "FrogSummon.h"
#include "MonsterBone.h"

#include "Player.h"

#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Collider_Cube.h"

#include "State_DeathCatIdle.h"
#include "State_DeathCatChainAttack.h"
#include "State_DeathCatFlyBallAttack.h"

#include "Effect.h"

#include "Player_Bullet.h"

CFrog::CFrog(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CFrog::CFrog(const CFrog& Prototype)
	: CMonster{ Prototype }
{
}


HRESULT CFrog::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFrog::Initialize(void* pArg)
{
	m_eMonsterId = FROG;

	MONSTER_INFO* MonsterInfo = static_cast<MONSTER_INFO*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	m_vInitPosition = MonsterInfo->vInitPosition;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitPosition);

	_float3 vScale = _float3{ 5.f,5.f,1.f };
	m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);

	m_fFindDistance = 10.f;
	m_fAttackDsitance = 4.5f;
	
	m_tMonsterInfo.iMaxHp = 5.f;
	m_tMonsterInfo.iHp = m_tMonsterInfo.iMaxHp;

	m_eCurrentState = IDLE;

	m_vSummonPosition = new _float3;

	m_pRigidBodyCom->SetOwner(this);
	m_pRigidBodyCom->Set_IsGravity(false);
	m_pRigidBodyCom->Set_Mass(2.f);
	m_pRigidBodyCom->Set_Friction(2.f);

	return S_OK;
}

void CFrog::Priority_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	//if(m_eCurrentState != JUMP)
		__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

}

void CFrog::Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	switch (m_eCurrentState)
	{
	case IDLE:
		State_Idle(fTimeDelta);
		break;

	case JUMP:
		State_Jump(fTimeDelta);
		break;

	case ATTACK:
		State_Attack(fTimeDelta);
		break;

	case HURT:
		State_Hurt(fTimeDelta);
		break;

	default:
		m_eCurrentState = IDLE;
		break;
	}

	KncokBack(fTimeDelta, m_vKnockBackDir);
}

void CFrog::Late_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	m_pAnimatorCom->Update(fTimeDelta);

	m_pRigidBodyCom->Update(fTimeDelta);

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	Calculate_Flip();
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CFrog::Render()
{
	m_pVIBufferCom->Filp_Texture(m_isFilp);

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	m_pAnimatorCom->Render(NULL);

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	//디버그 모드 추가하기
	//m_pColliderCom->Render_Collider();

	return S_OK;
}

void CFrog::Change_State_Hurt()
{
	Start_KnockBack();
	m_ePrevState = m_eCurrentState;
	m_eCurrentState = HURT;
}

void CFrog::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
	{
		if (static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK1
			|| static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK2
			|| static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK3)
		{

			if (pOther->Get_isFilp())
			{
				if (Get_Position().x > pOther->Get_Position().x)
				{
					m_tMonsterInfo.iHp -= static_cast<CPlayer*>(pOther)->Get_Player_Info().iAttackPower;				
					CEffect::EFFECT_DESC Effect_Desc = {};
					Effect_Desc.vPos = Get_Position();
					Effect_Desc.vPos.x = Get_Position().x;
					Effect_Desc.vScale = _float3(10.0f, 10.0f, 0.1f);
					Effect_Desc.fEffectDura = 0.4f;
					Effect_Desc.strTextureTag = TEXT("Hit_Blood");
					Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
					Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
					m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

					Change_State_Hurt();
				}
			}

			else if (!pOther->Get_isFilp())
			{
				if (Get_Position().x < pOther->Get_Position().x)
				{
					m_tMonsterInfo.iHp -= static_cast<CPlayer*>(pOther)->Get_Player_Info().iAttackPower;					
					CEffect::EFFECT_DESC Effect_Desc = {};
					Effect_Desc.vPos = Get_Position();
					Effect_Desc.vPos.x = Get_Position().x;
					Effect_Desc.vScale = _float3(10.0f, 10.0f, 0.1f);
					Effect_Desc.fEffectDura = 0.4f;
					Effect_Desc.strTextureTag = TEXT("Hit_Blood");
					Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
					Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
					Change_State_Hurt();
					m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

					Change_State_Hurt();
				}
			}
		}
	}
	if (pOther->Get_LayerTag() == TEXT("Layer_Player_Bullet"))
	{
		m_tMonsterInfo.iHp -= static_cast<CPlayer_Bullet*>(pOther)->Get_Player_Bullet_Info().fAtackPower;

		if (Get_Position().x < pOther->Get_Position().x)
		{
			CEffect::EFFECT_DESC Effect_Desc = {};
			Effect_Desc.vPos = Get_Position();
			Effect_Desc.vPos.x = Get_Position().x+0.5f;
			Effect_Desc.vScale = _float3(2.0f, 2.0f, 0.1f);
			Effect_Desc.fEffectDura = 0.2f;
			Effect_Desc.strTextureTag = TEXT("HitEffect_Circle");
			Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
			Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

			m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

			Change_State_Hurt();
		}

		else if (Get_Position().x > pOther->Get_Position().x)
		{
			CEffect::EFFECT_DESC Effect_Desc = {};
			Effect_Desc.vPos = Get_Position();
			Effect_Desc.vPos.x = Get_Position().x - 0.5f;
			Effect_Desc.vScale = _float3(2.0f, 2.0f, 0.1f);
			Effect_Desc.fEffectDura = 0.2f;
			Effect_Desc.strTextureTag = TEXT("HitEffect_Circle");
			Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
			Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

			m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

			Change_State_Hurt();
		}
	}
}

void CFrog::OnCollisionStay(CGameObject* pOther)
{
}

void CFrog::OnCollisionExit(CGameObject* pOther)
{
}

void CFrog::State_Idle(_float fTimeDelta)
{
	_float fDistance = D3DXVec3Length(&(m_pGameInstance->Find_Player()->Get_Position() - Get_Position()));
	m_fIdleWaitTime += fTimeDelta;

	Calculate_Flip();

	if (m_fFindDistance >= fDistance && !m_isFindTarget)
	{
		m_isFindTarget = true;
	}

	if (m_isFindTarget)
	{
		if (m_fIdleWaitTime >= 1.5f)
		{
			if (m_fAttackDsitance >= fDistance)
			{
				if (m_pGameInstance->Find_Player()->Get_Position().z > Get_Position().z)
				{
					m_pAnimatorCom->Change_Animation(TEXT("Back_Attack"));
				}
				else
				{
					m_pAnimatorCom->Change_Animation(TEXT("Front_Attack"));
				}

				m_vJumpDir = Calculate_Dir_To_Player();
				m_ePrevState = IDLE;
				m_eCurrentState = ATTACK;
			}
			else
			{
				if (m_pGameInstance->Find_Player()->Get_Position().z > Get_Position().z)
				{
					m_pAnimatorCom->Change_Animation(TEXT("Back_Jump"));
				}
				else
				{
					m_pAnimatorCom->Change_Animation(TEXT("Front_Jump"));
				}

				m_vJumpDir = Calculate_Dir_To_Player();
				m_ePrevState = IDLE;
				m_eCurrentState = JUMP;
			}
		}
	}
}

void CFrog::State_Jump(_float fTimeDelta)
{
	_float fDistance = D3DXVec3Length(&(m_pGameInstance->Find_Player()->Get_Position() - Get_Position()));

	if (2 < m_pAnimatorCom->Get_CurrentFrame() && m_pAnimatorCom->Get_CurrentFrame() < 15)
	{
		m_pGameInstance->Play_SoundRepeat(TEXT("Frog_Jump.wav"), SOUND_MONSTER, g_fEffectVolume);
		m_pTransformCom->Move_Dir(fTimeDelta, Calculate_Dir_To_Player());
	}

	if (m_pAnimatorCom->IsEnd_CurrentAnimation())
	{
		m_fIdleWaitTime = 1.2f;

		m_ePrevState = JUMP;
		m_eCurrentState = IDLE;
	}

}

void CFrog::State_Attack(_float fTimeDelta)
{
	// 공격 상태일 때 콜라이더 크기를 조정해야할까 새로 붙여줄까

	Calculate_Flip();

	if (20 < m_pAnimatorCom->Get_CurrentFrame() && m_pAnimatorCom->Get_CurrentFrame() < 23)
	{
		if (!m_isSummon)
		{
			m_isSummon = true;
			m_vSummonPosition->x = Get_Position().x;
			m_vSummonPosition->y = Get_Position().y;
			m_vSummonPosition->z = Get_Position().z;
			m_pGameInstance->Play_Sound(TEXT("Frog_Attack.wav"), SOUND_MONSTER, g_fEffectVolume);
			Create_Summon();
		}
	}

	if (m_pAnimatorCom->IsEnd_CurrentAnimation())
	{
		if (m_pGameInstance->Find_Player()->Get_Position().z > Get_Position().z)
		{
			m_pAnimatorCom->Change_Animation(TEXT("Back_Idle"));
		}
		else
		{
			m_pAnimatorCom->Change_Animation(TEXT("Front_Idle"));
		}

		m_isSummon = false;
		m_fIdleWaitTime = 0.f;

		m_ePrevState = ATTACK;
		m_eCurrentState = IDLE;
	}



}

void CFrog::State_Hurt(_float fTimeDelta)
{
	if (m_tMonsterInfo.iHp <= 0)
	{
		m_pGameInstance->Play_Sound(TEXT("Frog_Death.wav"), SOUND_MONSTER, g_fEffectVolume);
		__super::Create_Bone(CMonsterBone::BONE_TYPE::ANIMAL, m_vKnockBackDir);
		m_pGameInstance->Event_DestoryObject(this);
	}
	else
	{
		if (!m_isHit)
		{

			if (m_pGameInstance->Find_Player()->Get_Position().z > Get_Position().z)
			{
				m_pAnimatorCom->Change_Animation(TEXT("Back_Hit"));
			}
			else
			{
				m_pAnimatorCom->Change_Animation(TEXT("Front_Hit"));
			}

			m_vKnockBackDir = Get_Position() - m_pGameInstance->Find_Player()->Get_Position();

			m_pGameInstance->Play_Sound(TEXT("Frog_GetHit.wav"), SOUND_MONSTER, g_fEffectVolume);
			m_isHit = true;
		}

		if (m_pAnimatorCom->IsEnd_CurrentAnimation())
		{
			m_ePrevState = m_eCurrentState;
			m_isHit = false;
			m_eCurrentState = IDLE;
		}
	}

}

HRESULT CFrog::Create_Summon()
{
	CLandObject::LANDOBJECT_DESC	LandDesc = {};

	LandDesc.pTerrainTransform = m_pTerrainTransform;
	LandDesc.pTerrainVIBuffer = m_pTerrainVIBuffer;
	LandDesc.iStageIndex = m_iStageIndex;
	LandDesc.iLevelIndex = m_iLevelIndex;

	CMonster::MONSTER_INFO MonsterInfo = {};
	MonsterInfo.iHp = 100;
	MonsterInfo.iAttackPower = 10;
	MonsterInfo.vInitPosition = *m_vSummonPosition;
	MonsterInfo.tLandObjectDesc = LandDesc;

	if (FAILED(m_pGameInstance->Event_CreateObject(m_iLevelIndex, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_FrogSummon"), sizeof(CMonster::MONSTER_INFO), &MonsterInfo)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFrog::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 2.5f;
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
	ColliderInfo.fOffSetY = -0.05f;
	ColliderInfo.fOffSetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderInfo)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_Collider_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFrog::Ready_Animation()
{
	CTexture* pTextureCom;

	//앞면
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_Frog_Front_Idle"),
		TEXT("Com_Texture_Frog_Front_Idle"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Front_Idle"), pTextureCom, 0, 45, 0.03f);

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_Frog_Front_Jump"),
		TEXT("Com_Texture_Frog_Front_Jump"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Front_Jump"), pTextureCom, 0, 33, 0.03f, false);

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_Frog_Front_Attack"),
		TEXT("Com_Texture_Frog_Front_Attack"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Front_Attack"), pTextureCom, 0, 41, 0.03f, false);

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_Frog_Front_Hit"),
		TEXT("Com_Texture_Frog_Front_Hit"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Front_Hit"), pTextureCom, 0, 11, 0.02f, false);

	// 뒷면
	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_Frog_Back_Idle"),
		TEXT("Com_Texture_Frog_Back_Idle"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Back_Idle"), pTextureCom, 0, 45, 0.03f);

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_Frog_Back_Jump"),
		TEXT("Com_Texture_Frog_Back_Jump"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Back_Jump"), pTextureCom, 0, 33, 0.03f, false);

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_Frog_Back_Attack"),
		TEXT("Com_Texture_Frog_Back_Attack"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Back_Attack"), pTextureCom, 0, 41, 0.03f, false);

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_Frog_Back_Hit"),
		TEXT("Com_Texture_Frog_Back_Hit"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Back_Hit"), pTextureCom, 0, 11, 0.02f, false);

	m_pAnimatorCom->Change_Animation(TEXT("Front_Idle"), 0);

	return S_OK;
}

HRESULT CFrog::SetUp_RenderState()
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

HRESULT CFrog::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CFrog* CFrog::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFrog* pInstance = new CFrog(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFrog"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CFrog::Clone(void* pArg)
{
	CFrog* pInstance = new CFrog(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CFrog"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrog::Free()
{
	__super::Free();

	//일단 임시로 이렇게 해둠 아이디어 있으면 반영좀
	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);

	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pVIBufferCom);

	Safe_Delete(m_vSummonPosition);
}
