#include "Human.h"
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

CHuman::CHuman(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CHuman::CHuman(const CHuman& Prototype)
	: CMonster{ Prototype }
{
}


HRESULT CHuman::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHuman::Initialize(void* pArg)
{
	m_eMonsterId = HUMAN;

	MONSTER_INFO* MonsterInfo = static_cast<MONSTER_INFO*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	m_vInitPosition = MonsterInfo->vInitPosition;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitPosition);

	_float3 vScale = _float3{ 4.f,4.f,1.f };
	m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);

	m_fFindDistance = 10.f;
	m_fAttackDsitance = 1.5f;

	m_tMonsterInfo.iMaxHp = 9.f;
	m_tMonsterInfo.iHp = m_tMonsterInfo.iMaxHp;

	m_eCurrentState = IDLE;

	m_pRigidBodyCom->SetOwner(this);
	m_pRigidBodyCom->Set_IsGravity(false);
	m_pRigidBodyCom->Set_Mass(2.f);
	m_pRigidBodyCom->Set_Friction(2.f);

	return S_OK;
}

void CHuman::Priority_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

}

void CHuman::Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	if (m_isDead)
		m_fDeadDelayTime += fTimeDelta;

	switch (m_eCurrentState)
	{
	case IDLE:
		State_Idle(fTimeDelta);
		break;

	case RUN:
		State_Run(fTimeDelta);
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

void CHuman::Late_Update(_float fTimeDelta)
{
	m_pVIBufferCom->Filp_Texture(m_isFilp);

	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	m_pAnimatorCom->Update(fTimeDelta);

	m_pRigidBodyCom->Update(fTimeDelta);

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	Calculate_Flip();
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CHuman::Render()
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
	m_pColliderCom->Render_Collider();

	return S_OK;
}

void CHuman::Change_State_Hurt()
{
	Start_KnockBack();
	m_ePrevState = m_eCurrentState;
	m_eCurrentState = HURT;
}

void CHuman::OnCollisionEnter(CGameObject* pOther)
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
					m_tMonsterInfo.iHp -= static_cast<CPlayer*>(pOther)->Get_Player_Info().iAttackPower;					CEffect::EFFECT_DESC Effect_Desc = {};
					Effect_Desc.vPos = Get_Position();
					Effect_Desc.vPos.x = Get_Position().x - 1.5f;
					Effect_Desc.vScale = _float3(6.0f, 6.0f, 0.1f);
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
					m_tMonsterInfo.iHp -= static_cast<CPlayer*>(pOther)->Get_Player_Info().iAttackPower;					CEffect::EFFECT_DESC Effect_Desc = {};
					Effect_Desc.vPos = Get_Position();
					Effect_Desc.vPos.x = Get_Position().x + 1.5f;
					Effect_Desc.vScale = _float3(6.0f, 6.0f, 0.1f);
					Effect_Desc.fEffectDura = 0.4f;
					Effect_Desc.strTextureTag = TEXT("Hit_Blood");
					Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
					Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
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
			Effect_Desc.vPos.x = Get_Position().x + 1.f;
			Effect_Desc.vScale = _float3(2.5f, 2.5f, 0.1f);
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
			Effect_Desc.vPos.x = Get_Position().x - 1.f;
			Effect_Desc.vScale = _float3(2.5f, 2.5f, 0.1f);
			Effect_Desc.fEffectDura = 0.2f;
			Effect_Desc.strTextureTag = TEXT("HitEffect_Circle");
			Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
			Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

			m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

			Change_State_Hurt();
		}
	}
}

void CHuman::OnCollisionStay(CGameObject* pOther)
{
}

void CHuman::OnCollisionExit(CGameObject* pOther)
{
}

void CHuman::State_Idle(_float fTimeDelta)
{
	_float fDistance = D3DXVec3Length(&(m_pGameInstance->Find_Player()->Get_Position() - Get_Position()));

	if (m_fFindDistance >= fDistance && !m_isFindTarget)
	{
		m_isFindTarget = true;
		m_pAnimatorCom->Change_Animation(TEXT("Notice"));

		if (m_pAnimatorCom->IsEnd_CurrentAnimation())
		{
			m_ePrevState = IDLE;
			m_eCurrentState = RUN;
		}
	}

	if (m_isFindTarget)
	{
		m_ePrevState = IDLE;
		m_eCurrentState = RUN;
	}
}

void CHuman::State_Run(_float fTimeDelta)
{
	_float fDistance = D3DXVec3Length(&(m_pGameInstance->Find_Player()->Get_Position() - Get_Position()));

	if (m_fAttackDsitance >= fDistance)
	{
		m_pAnimatorCom->Change_Animation(TEXT("Idle"));

		m_ePrevState = RUN;
		m_eCurrentState = ATTACK;
	}
	else
	{
		if (m_pGameInstance->Find_Player()->Get_Position().z > Get_Position().z)
		{
			if (m_pAnimatorCom->Get_AnimationTag() != TEXT("RunUp"))
				m_pAnimatorCom->Change_Animation(TEXT("RunUp"));
		}
		else
		{
			if (m_pAnimatorCom->Get_AnimationTag() != TEXT("Run"))
				m_pAnimatorCom->Change_Animation(TEXT("Run"));
		}

		Follow_Player(fTimeDelta, m_pTransformCom->Get_SpeedPerSec());
	}
}

void CHuman::State_Attack(_float fTimeDelta)
{
	m_fAttackTime += fTimeDelta;

	if (!m_isAttackEnd)
	{
		if (!m_isAttacking && m_fAttackTime >= 0.7f)
		{
			m_isAttacking = true;
			m_fAttackDir = Calculate_Dir_To_Player();
			m_pGameInstance->Play_SoundRepeat(TEXT("Humman_Attack.wav"), SOUND_MONSTER, g_fEffectVolume);
			m_pAnimatorCom->Change_Animation(TEXT("Attack"));
		}

		if (m_isAttacking && m_pAnimatorCom->IsEnd_CurrentAnimation())
		{
			m_fAttackTime = 0.f;
			m_isAttackEnd = true;

			m_pAnimatorCom->Change_Animation(TEXT("Idle"));
		}
		else if (m_isAttacking && 33 <= m_pAnimatorCom->Get_CurrentFrame() && m_pAnimatorCom->Get_CurrentFrame() <= 37)
		{
			m_pTransformCom->Move_Dir(fTimeDelta, m_fAttackDir, m_pTransformCom->Get_SpeedPerSec() + 10.f);
		}
	}
	else if (m_isAttackEnd)
	{
		if (m_fAttackTime >= 0.5f)
		{
			m_isAttacking = false;
			m_isAttackEnd = false;
			m_fAttackTime = 0.f;

			m_ePrevState = ATTACK;
			m_eCurrentState = IDLE;
		}
	}

}

void CHuman::State_Hurt(_float fTimeDelta)
{
	if (m_tMonsterInfo.iHp <= 0)
	{
		m_pGameInstance->Play_SoundRepeat(TEXT("Humman_Die.wav"), SOUND_MONSTER, g_fEffectVolume);
		__super::Create_Bone(CMonsterBone::BONE_TYPE::HUMAN, m_vKnockBackDir);
		m_pGameInstance->Event_DestoryObject(this);
	}
	else
	{
		if (!m_isHit)
		{
			m_pAnimatorCom->Change_Animation(TEXT("Hurt"));

			m_vKnockBackDir = Get_Position() - m_pGameInstance->Find_Player()->Get_Position();

			m_pGameInstance->Play_SoundRepeat(TEXT("Humman_GetHit.wav"), SOUND_MONSTER, g_fEffectVolume);
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

HRESULT CHuman::Ready_Components()
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
	ColliderInfo.fSizeZ = 0.15f;
	ColliderInfo.fOffSetX = 0.f;
	ColliderInfo.fOffSetY = 0.f;
	ColliderInfo.fOffSetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderInfo)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_Collider_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHuman::Ready_Animation()
{
	CTexture* pTextureCom;

	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_HumanIdle"),
		TEXT("Com_Texture_HumanIdle"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Idle"), pTextureCom, 0, 34, 0.03f);

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_HumanRun"),
		TEXT("Com_Texture_HumanRun"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Run"), pTextureCom, 0, 19, 0.03f);

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_HumanRunUp"),
		TEXT("Com_Texture_HumanRunUp"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("RunUp"), pTextureCom, 0, 23, 0.03f);

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_HumanNotice"),
		TEXT("Com_Texture_HumanNotice"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Notice"), pTextureCom, 0, 33, 0.02f, false);

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_HumanAttack"),
		TEXT("Com_Texture_HumanAttack"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Attack"), pTextureCom, 0, 66, 0.01f, false);

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_HumanHurt"),
		TEXT("Com_Texture_HumanHurt"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Hurt"), pTextureCom, 0, 29, 0.02f, false);

	m_pAnimatorCom->Change_Animation(TEXT("Idle"), 0);

	return S_OK;
}

HRESULT CHuman::SetUp_RenderState()
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

HRESULT CHuman::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CHuman* CHuman::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHuman* pInstance = new CHuman(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CHuman"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CHuman::Clone(void* pArg)
{
	CHuman* pInstance = new CHuman(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CHuman"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHuman::Free()
{
	__super::Free();

	//일단 임시로 이렇게 해둠 아이디어 있으면 반영좀
	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);

	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pVIBufferCom);
}
