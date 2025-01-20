
#include "Bat.h"
#include "MonsterBone.h"
#include "Layer.h"
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

CBat::CBat(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CBat::CBat(const CBat& Prototype)
	: CMonster{ Prototype }
{
}


HRESULT CBat::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBat::Initialize(void* pArg)
{
	m_eMonsterId = BAT;

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

	m_fFindDistance = 7.f;
	m_fAttackDsitance = 3.5f;

	m_eCurrentState = FLY;

	m_pRigidBodyCom->SetOwner(this);
	m_pRigidBodyCom->Set_IsGravity(false);
	m_pRigidBodyCom->Set_Mass(2.f);
	m_pRigidBodyCom->Set_Friction(2.f);

	m_tMonsterInfo.iMaxHp = 7.f;
	m_tMonsterInfo.iHp = m_tMonsterInfo.iMaxHp;

	return S_OK;
}

void CBat::Priority_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

}

void CBat::Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	Calculate_Flip();

	//보스 맵
	if (m_iLevelIndex == 5 && m_iStageIndex == 3)
	{
		if (nullptr != m_pGameInstance->Find_Layer(LEVEL_DUNGEON1, TEXT("Layer_Boss")))
		{
			if (dynamic_cast<CMonster*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON1, TEXT("Layer_Boss"), 0))->Get_MonInfo().iHp <= 0)
			{
				m_tMonsterInfo.iHp = 0;
				m_eCurrentState = HURT;
			}
		}
	}

	switch (m_eCurrentState)
	{
	case FLY:
		State_Fly(fTimeDelta);
		break;

	case ATTACK:
		State_Attack(fTimeDelta);
		break;

	case 
		HURT:
		State_HURT(fTimeDelta);
		break;

	default:
		m_eCurrentState = FLY;
		break;
	}

	KncokBack(fTimeDelta, m_vKnockBackDir);
}

void CBat::Late_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	m_pAnimatorCom->Update(fTimeDelta);

	m_pRigidBodyCom->Update(fTimeDelta);

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBat::Render()
{
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

void CBat::Change_State_Hurt()
{
	Start_KnockBack();
	m_ePrevState = m_eCurrentState;
	m_eCurrentState = HURT;
}

void CBat::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
	{
		if (m_eCurrentState == ATTACK)
		{

		}

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
					m_tMonsterInfo.iHp -= static_cast<CPlayer*>(pOther)->Get_Player_Info().iAttackPower;
					CEffect::EFFECT_DESC Effect_Desc = {};
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

			Start_KnockBack();
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
			Effect_Desc.vScale = _float3(1.5f, 1.5f, 0.1f);
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
			Effect_Desc.vScale = _float3(1.5f, 1.5f, 0.1f);
			Effect_Desc.fEffectDura = 0.2f;
			Effect_Desc.strTextureTag = TEXT("HitEffect_Circle");
			Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
			Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

			m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

			Change_State_Hurt();
		}
	}
}




void CBat::OnCollisionStay(CGameObject* pOther)
{
}


void CBat::OnCollisionExit(CGameObject* pOther)
{
}

void CBat::State_Fly(_float fTimeDelta)
{
	_float fDistance = D3DXVec3Length(&(m_pGameInstance->Find_Player()->Get_Position() - Get_Position()));
	m_pGameInstance->Play_SoundRepeat(TEXT("Bat_Fly.wav"), SOUND_MONSTER, g_fEffectVolume);

	if (m_fFindDistance >= fDistance)
	{
		m_isFindTarget = true;
	}

	if (m_isFindTarget)
	{
		Follow_Player(fTimeDelta);
	}

	if (m_fAttackDsitance >= fDistance)
	{
		m_ePrevState = FLY;
		m_eCurrentState = ATTACK;
	}
}

void CBat::State_Attack(_float fTimeDelta)
{
	m_fAttackStartTime += fTimeDelta;
	if (!m_isAttackEnd)
	{
		if (m_fAttackStartTime >= 1.0f)
		{
			if (!m_isAttackCharging)
			{
				m_isAttackCharging = true;
				m_fAttackDir = Calculate_Dir_To_Player();
				m_isFilp = false;
				m_pGameInstance->Play_SoundRepeat(TEXT("Bat_Charge.wav"), SOUND_MONSTER, g_fEffectVolume);
				m_pAnimatorCom->Change_Animation(TEXT("AttackCharge"));
			}
			else if (m_isAttackCharging && !m_isAttacking)
			{
				m_pTransformCom->Move_Dir(fTimeDelta, -m_fAttackDir, m_pTransformCom->Get_SpeedPerSec() - 4.f);

				if (m_pAnimatorCom->IsEnd_CurrentAnimation())
				{
					m_fAttackDir = Calculate_Dir_To_Player();
					m_isAttacking = true;
					m_pGameInstance->Play_SoundRepeat(TEXT("Bat_Attack.wav"), SOUND_MONSTER, g_fEffectVolume);
					m_pAnimatorCom->Change_Animation(TEXT("Attack"));
				}
			}

			if (m_isAttacking)
			{
				m_pTransformCom->Move_Dir(fTimeDelta, m_fAttackDir, m_pTransformCom->Get_SpeedPerSec() + 8.f);

				if (m_pAnimatorCom->IsEnd_CurrentAnimation() || D3DXVec3Length(&(m_pGameInstance->Find_Player()->Get_Position() - Get_Position())) <= 0.2f)
				{
					m_fAttackStartTime = 0.f;
					m_isAttackCharging = false;
					m_isAttacking = false;
					m_isAttackEnd = true;

					m_pAnimatorCom->Change_Animation(TEXT("Fly"));
				}
			}
		}
	}
	if (m_isAttackEnd)
	{
		if (m_fAttackStartTime > 2.f)
		{
			m_fAttackStartTime = 0.f;
			m_isAttackEnd = false;

			m_ePrevState = ATTACK;
			m_eCurrentState = FLY;
		}
	}

}

void CBat::State_HURT(_float fTimeDelta)
{


	if (m_tMonsterInfo.iHp <= 0)
	{
		m_pGameInstance->Play_SoundRepeat(TEXT("Bat_Dead.wav"), SOUND_MONSTER, g_fEffectVolume);
		__super::Create_Bone(CMonsterBone::BONE_TYPE::ANIMAL, m_vKnockBackDir);
		m_pGameInstance->Event_DestoryObject(this);
	}
	else
	{
		m_pGameInstance->Play_SoundRepeat(TEXT("Bat_Hit.wav"), SOUND_MONSTER, g_fEffectVolume);
		m_vKnockBackDir = Get_Position() - m_pGameInstance->Find_Player()->Get_Position();
		m_pAnimatorCom->Set_CurrentFrame(18);
	}

	m_ePrevState = m_eCurrentState;
	m_eCurrentState = FLY;
}

HRESULT CBat::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 5.f;
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_Collider_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBat::Ready_Animation()
{
	CTexture* pTextureCom;

	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_BatFly"),
		TEXT("Com_Texture_BatFly"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Fly"), pTextureCom, 0, 23, 0.03f);

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_BatAttackCharge"),
		TEXT("Com_Texture_BatAttackCharge"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("AttackCharge"), pTextureCom, 0, 39, 0.0f, false);

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_BatAttack"),
		TEXT("Com_Texture_BatAttack"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Attack"), pTextureCom, 0, 22, 0.01f, false);

	m_pAnimatorCom->Change_Animation(TEXT("Fly"), 0);

	return S_OK;
}

HRESULT CBat::SetUp_RenderState()
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

HRESULT CBat::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CBat* CBat::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBat* pInstance = new CBat(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CBat::Clone(void* pArg)
{
	CBat* pInstance = new CBat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBat::Free()
{
	__super::Free();

	//일단 임시로 이렇게 해둠 아이디어 있으면 반영좀
	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);

	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pVIBufferCom);
}
