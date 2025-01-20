
#include "Boss_DeathCat.h"

#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Collider_Cube.h"

#include "State_DeathCatIdle.h"
#include "State_DeathCatChainAttack.h"
#include "State_DeathCatFlyBallAttack.h"
#include "State_DeathCatDisappear.h"
#include "State_DeathCatHurt.h"

#include "Player.h"
#include "Effect.h"
#include "Player_Bullet.h"

CBoss_DeathCat::CBoss_DeathCat(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CBoss_DeathCat::CBoss_DeathCat(const CBoss_DeathCat& Prototype)
	: CMonster{ Prototype }
{
}


HRESULT CBoss_DeathCat::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_DeathCat::Initialize(void* pArg)
{
	m_eMonsterId = DEATHCAT;

	MONSTER_INFO* MonsterInfo = static_cast<MONSTER_INFO*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	if (FAILED(Ready_Fsm()))
		return E_FAIL;

	_float3 vPosition = _float3{ 15.f,1.f,21.f };
	_float3 vScale = _float3{ 14.f,14.f,1.f };

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);

	m_tMonsterInfo.iMaxHp = 100;
	m_tMonsterInfo.iHp = 100;
	m_tMonsterInfo.iAttackPower = 3;

	return S_OK;
}

void CBoss_DeathCat::Priority_Update(_float fTimeDelta)
{
}

void CBoss_DeathCat::Update(_float fTimeDelta)
{
	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

	if (m_isHit)
	{
		m_fHitEffectTime += fTimeDelta;
	}

	if ((!m_isPhase2 && m_tMonsterInfo.iHp <= m_tMonsterInfo.iMaxHp * 0.5f))
	{
		m_isPhase2 = true;
		m_pFsmCom->Change_State(HURT);
	}

	if (m_tMonsterInfo.iHp <= 0 && !m_isZeroHp)
	{
		m_isZeroHp = true;
		m_pFsmCom->Change_State(HURT);
	}

	m_pFsmCom->Update(fTimeDelta);
}

void CBoss_DeathCat::Late_Update(_float fTimeDelta)
{
	m_pAnimatorCom->Update(fTimeDelta);

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBoss_DeathCat::Render()
{
	if (m_isActive)
	{
		m_pVIBufferCom->Filp_Texture(false);

		if (FAILED(m_pTransformCom->Bind_Transform()))
			return E_FAIL;

		m_pAnimatorCom->Render(NULL);

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
		m_pShaderCom->Set_Texture("g_Texture", m_pAnimatorCom->Get_CurrentTexture());

		if (m_isHit)
		{
			if (m_fHitEffectTime <= 0.1f)
			{
				m_pShaderCom->Begin_Shader(1);
			}
			else
			{
				m_isHit = false;
				m_fHitEffectTime = 0.f;
				m_pShaderCom->Begin_Shader(0);
			}
		}
		else
		{
			m_pShaderCom->Begin_Shader(0);
		}

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		m_pShaderCom->End_Shader();

		//디버그 모드 추가하기
		m_pColliderCom->Render_Collider();
	}

	return S_OK;
}

_uint CBoss_DeathCat::Get_CurrentState()
{
	return m_pFsmCom->Get_CurrentState(); 
}

void CBoss_DeathCat::On_ColliderActive()
{
	m_pColliderCom->Set_IsActive(true);
}

void CBoss_DeathCat::Off_ColliderActive()
{
	m_pColliderCom->Set_IsActive(false);
}

void CBoss_DeathCat::OnCollisionEnter(CGameObject* pOther)
{
	if (m_tMonsterInfo.iHp > 0)
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
		{
			if (static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK1
				|| static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK2
				|| static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK3)
			{
				m_pGameInstance->Play_Sound(TEXT("DeathCat_GetHit.wav"), SOUND_DEATHCAT, 0.6f);
				m_isHit = true;
				if (pOther->Get_isFilp())
				{
					if (Get_Position().x > pOther->Get_Position().x)
					{
						m_tMonsterInfo.iHp -= static_cast<CPlayer*>(pOther)->Get_Player_Info().iAttackPower;
						//터지는 이펙트
						CEffect::EFFECT_DESC Effect_Desc = {};
						Effect_Desc.vPos = Get_Position();
						Effect_Desc.vPos.y = Get_Position().y - 3.f;
						Effect_Desc.vScale = _float3(10.0f, 10.0f, 0.1f);
						Effect_Desc.fEffectDura = 0.4f;
						Effect_Desc.strTextureTag = TEXT("Hit_Blood");
						Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
						Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
						m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
					}
				}

				else if (!pOther->Get_isFilp())
				{
					if (Get_Position().x < pOther->Get_Position().x)
					{
						m_tMonsterInfo.iHp -= static_cast<CPlayer*>(pOther)->Get_Player_Info().iAttackPower;						//터지는 이펙트
						CEffect::EFFECT_DESC Effect_Desc = {};
						Effect_Desc.vPos = Get_Position();
						Effect_Desc.vPos.y = Get_Position().y - 4.f;
						Effect_Desc.vScale = _float3(8.0f, 8.0f, 0.1f);
						Effect_Desc.fEffectDura = 0.4f;
						Effect_Desc.strTextureTag = TEXT("Hit_Blood");
						Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
						Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
						m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
					}
				}
			}
		}

		if (pOther->Get_LayerTag() == TEXT("Layer_Player_Bullet"))
		{
			m_isHit = true;
			m_pGameInstance->Play_Sound(TEXT("DeathCat_GetHit.wav"), SOUND_DEATHCAT, 0.6f);
			m_tMonsterInfo.iHp-=static_cast<CPlayer_Bullet*>(pOther)->Get_Player_Bullet_Info().fAtackPower;
		}
	}
}

void CBoss_DeathCat::OnCollisionStay(CGameObject* pOther)
{
}

void CBoss_DeathCat::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CBoss_DeathCat::Ready_Components()
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
	ColliderInfo.fSizeZ = 0.5f;
	ColliderInfo.fOffSetX = 0.f;
	ColliderInfo.fOffSetY = -0.5f;
	ColliderInfo.fOffSetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderInfo)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Boss"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_DeathCat::Ready_Animation()
{
	CTexture* pTextureCom;

	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_BossCat_Phase1_Idle"),
		TEXT("Com_Texture_Boss_Idle"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("IDLE"), pTextureCom, 0, 34, 0.04f);

	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_BossCat_Phase1_Attack"),
		TEXT("Com_Texture_Boss_Attack"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Attack"), pTextureCom, 0, 33, 0.04f, false);

	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_BossCat_Phase1_Disappear"),
		TEXT("Com_Texture_Boss_Disappear"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Disappear"), pTextureCom, 0, 19, 0.05f, false);

	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_BossCat_Phase1_Appear"),
		TEXT("Com_Texture_Boss_Appear"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Appear"), pTextureCom, 0, 20, 0.05f, false);

	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_BossCat_Phase1_HurtStart"),
		TEXT("Com_Texture_Boss_HurtStart"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("HurtStart"), pTextureCom, 0, 37, 0.05f, false);

	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_BossCat_Phase1_HurtLoop"),
		TEXT("Com_Texture_Boss_HurtLoop"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("HurtLoop"), pTextureCom, 0, 27, 0.05f);

	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_BossCat_Phase1_HurtEnd"),
		TEXT("Com_Texture_Boss_HurtEnd"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("HurtEnd"), pTextureCom, 0, 23, 0.05f, false);

	m_pAnimatorCom->Change_Animation(TEXT("IDLE"), 0);

	return S_OK;
}

HRESULT CBoss_DeathCat::Ready_Fsm()
{	
	/* For.Com_Fsm */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;
	
	m_pFsmCom->Add_State(CState_DeatCatIdle::Create(m_pFsmCom, IDLE));
	m_pFsmCom->Add_State(CState_DeathCatChainAttack::Create(m_pFsmCom, CHAINATTACK));
	m_pFsmCom->Add_State(CState_DeathCatFlyBallAttack::Create(m_pFsmCom, FLYBALLATTACK));
	m_pFsmCom->Add_State(CState_DeathCatDisappear::Create(m_pFsmCom, DISAPPEAR));
	m_pFsmCom->Add_State(CState_DeathCatHurt::Create(m_pFsmCom, HURT));

	Set_InitState(IDLE);

	return S_OK;
}

HRESULT CBoss_DeathCat::SetUp_RenderState()
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

HRESULT CBoss_DeathCat::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CBoss_DeathCat * CBoss_DeathCat::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBoss_DeathCat*		pInstance = new CBoss_DeathCat(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_DeathCat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CBoss_DeathCat::Clone(void* pArg)
{
	CBoss_DeathCat*		pInstance = new CBoss_DeathCat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_DeathCat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_DeathCat::Free()
{
	__super::Free();

	//일단 임시로 이렇게 해둠 아이디어 있으면 반영좀
	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pVIBufferCom);
}
