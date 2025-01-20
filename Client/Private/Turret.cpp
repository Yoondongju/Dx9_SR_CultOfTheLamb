
#include "Turret.h"
#include "MonsterBone.h"
#include "Player.h"

#include "FlyBallAttack.h"

#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Collider_Cube.h"

#include "Effect.h"

#include "Player_Bullet.h"

CTurret::CTurret(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CTurret::CTurret(const CTurret& Prototype)
	: CMonster{ Prototype }
{
}


HRESULT CTurret::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTurret::Initialize(void* pArg)
{
	m_eMonsterId = TURRET;

	MONSTER_INFO* MonsterInfo = static_cast<MONSTER_INFO*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	m_vInitPosition = MonsterInfo->vInitPosition;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitPosition);

	_float3 vScale = _float3{ 3.f,3.f,1.f };
	m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);

	m_fFindDistance = 10.f;

	m_eCurrentState = IDLE;

	m_tMonsterInfo.iMaxHp = 13.f;
	m_tMonsterInfo.iHp = m_tMonsterInfo.iMaxHp;

	return S_OK;
}

void CTurret::Priority_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

}

void CTurret::Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	switch (m_eCurrentState)
	{
	case IDLE:
		State_Idle(fTimeDelta);
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
}

void CTurret::Late_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	m_pAnimatorCom->Update(fTimeDelta);

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTurret::Render()
{
	m_pVIBufferCom->Filp_Texture(false);

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

void CTurret::Change_State_Hurt()
{
	m_ePrevState = m_eCurrentState;
	m_eCurrentState = HURT;
}

void CTurret::OnCollisionEnter(CGameObject* pOther)
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
					Effect_Desc.vPos.x = Get_Position().x - 1.5f;
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
				
					m_tMonsterInfo.iHp -= static_cast<CPlayer*>(pOther)->Get_Player_Info().iAttackPower;					CEffect::EFFECT_DESC Effect_Desc = {};
					Effect_Desc.vPos = Get_Position();
					Effect_Desc.vPos.x = Get_Position().x + 1.5f;
					Effect_Desc.vScale = _float3(10.0f, 10.0f, 0.1f);
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

void CTurret::OnCollisionStay(CGameObject* pOther)
{
}

void CTurret::OnCollisionExit(CGameObject* pOther)
{
}

void CTurret::State_Idle(_float fTimeDelta)
{
	_float fDistance = D3DXVec3Length(&(m_pGameInstance->Find_Player()->Get_Position() - Get_Position()));

	if (m_fFindDistance >= fDistance)
	{
		m_isFindTarget = true;
	}

	if (m_isFindTarget)
	{
		m_ePrevState = IDLE;
		m_eCurrentState = ATTACK;
	}
}

void CTurret::State_Attack(_float fTimeDelta)
{
	m_fAttackDelayTime += fTimeDelta;

	if (m_fAttackDelayTime >= 3.f)
	{
		if (m_iAttackCount < 3)
		{
			if (!m_isAttacking)
			{
				m_isAttacking = true;
				m_pAnimatorCom->Change_Animation(TEXT("Attack"));
			}

			m_fAttackDelayTime = 2.8f;
			++m_iAttackCount;
			Create_Bullet();
		}
	}

	if (m_iAttackCount >= 3 && m_pAnimatorCom->IsEnd_CurrentAnimation())
	{
		m_pAnimatorCom->Change_Animation(TEXT("Idle"));

		m_isAttacking = false;
		m_fAttackDelayTime = 0.f;
		m_iAttackCount = 0;

		m_ePrevState = ATTACK;
		m_eCurrentState = IDLE;
	}
}

void CTurret::State_Hurt(_float fTimeDelta)
{
	if (m_tMonsterInfo.iHp <= 0)
	{
		m_pGameInstance->Play_Sound(TEXT("Turret_Death.wav"), SOUND_MONSTER, g_fEffectVolume);
		__super::Create_Bone(CMonsterBone::BONE_TYPE::STRANGE, m_vKnockBackDir);
		m_pGameInstance->Event_DestoryObject(this);
	}
	else
	{
		m_pGameInstance->Play_Sound(TEXT("Turret_GetHit.wav"), SOUND_MONSTER, g_fEffectVolume);
		m_ePrevState = HURT;
		m_eCurrentState = IDLE;
	}
}

void CTurret::Create_Bullet()
{
	CFlyBallAttack::FLYBALL_DESC tFlyBallDesc = {};

	tFlyBallDesc.vPosition = Get_Position();
	tFlyBallDesc.vDir = m_pGameInstance->Find_Player()->Get_Position() - Get_Position();
	tFlyBallDesc.fDelayTime = 0.f;
	tFlyBallDesc.eType = CFlyBallAttack::STRAGIHT_DIR;
	tFlyBallDesc.iLevelIndex = m_iLevelIndex;
	tFlyBallDesc.iStageIndex = m_iStageIndex;

	m_pGameInstance->Event_CreateObject(m_iLevelIndex, TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &tFlyBallDesc);
}

HRESULT CTurret::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 1.5f;
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

	return S_OK;
}

HRESULT CTurret::Ready_Animation()
{
	CTexture* pTextureCom;

	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_TurretIdle"),
		TEXT("Com_Texture_TurretIdle"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Idle"), pTextureCom, 0, 23, 0.03f);

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_TurretShoot"),
		TEXT("Com_Texture_TurretShoot"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Attack"), pTextureCom, 0, 35, 0.03f, false);

	if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_TurretAnticipation"),
		TEXT("Com_Texture_TurretAnticipation"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Hurt"), pTextureCom, 0, 28, 0.01f, false);

	m_pAnimatorCom->Change_Animation(TEXT("Idle"), 0);

	return S_OK;
}

HRESULT CTurret::SetUp_RenderState()
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

HRESULT CTurret::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CTurret* CTurret::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTurret* pInstance = new CTurret(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CTurret::Clone(void* pArg)
{
	CTurret* pInstance = new CTurret(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTurret::Free()
{
	__super::Free();

	//일단 임시로 이렇게 해둠 아이디어 있으면 반영좀
	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);

	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pVIBufferCom);
}
