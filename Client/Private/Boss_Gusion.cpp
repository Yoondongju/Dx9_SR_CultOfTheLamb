#include "Boss_Gusion.h"
#include "GameInstance.h"

#include "Player.h"
#include "Frog.h"

#include "State_GusionIdle.h"
#include "State_GusionRoar.h"
#include "State_GusionBurp.h"
#include "State_GusionLE.h"
#include "State_GusionJump.h"
#include "State_GusionDie.h"

#include "Effect.h"

#include "Player_Bullet.h"

CBoss_Gusion::CBoss_Gusion(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CMonster(pGraphic_Device)
{
}

CBoss_Gusion::CBoss_Gusion(const CBoss_Gusion& Prototype)
    :CMonster(Prototype)
{
}

HRESULT CBoss_Gusion::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBoss_Gusion::Initialize(void* pArg)
{
	m_eMonsterId = GUSION;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	if (FAILED(Ready_Fsm()))
		return E_FAIL;


	MONSTER_INFO* Monster_Info = static_cast<MONSTER_INFO*>(pArg);
	m_tMonsterInfo.iHp = Monster_Info->iHp;
	m_tMonsterInfo.iMaxHp = m_tMonsterInfo.iHp;
	m_tMonsterInfo.iAttackPower = Monster_Info->iAttackPower;

	_float3 vScale = _float3{ 5.f,5.f,0.1f };
	m_vInitPosition = _float3{ 12.f,2.5f,10.f };
	m_fAttackTimer = 0.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitPosition);
	m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);

	return S_OK;
}

void CBoss_Gusion::Priority_Update(_float fTimeDelta)
{
	if (m_tMonsterInfo.iHp <= 0.f)
			m_tMonsterInfo.iHp = 0.f;

	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);
}

void CBoss_Gusion::Update(_float fTimeDelta)
{
	if (m_isHit)
	{
		m_fHitEffectTime += fTimeDelta;
	}

	Calculate_Flip();
	m_pFsmCom->Update(fTimeDelta);
}

void CBoss_Gusion::Late_Update(_float fTimeDelta)
{
	m_pAnimatorCom->Update(fTimeDelta);
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBoss_Gusion::Render()
{
	m_pVIBufferCom->Filp_Texture(m_isFilp);

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

	m_pColliderCom->Render_Collider();

    return S_OK;
}

void CBoss_Gusion::OnCollisionEnter(CGameObject* pOther)
{
	if (m_eCurState != ROAR)
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
		{
			if (static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK1
				|| static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK2
				|| static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK3)
			{
				m_isHit = true;
				if (pOther->Get_isFilp())
				{
					if (Get_Position().x > pOther->Get_Position().x)
					{
						m_pGameInstance->Play_Sound(TEXT("Gusion_Hit3.wav"), SOUND_GUSION, g_fEffectVolume);
						m_tMonsterInfo.iHp -= static_cast<CPlayer*>(pOther)->Get_Player_Info().iAttackPower;
						CEffect::EFFECT_DESC Effect_Desc = {};
						Effect_Desc.vPos = Get_Position();
						Effect_Desc.vPos.x = Get_Position().x - 1.5f;
						Effect_Desc.vScale = _float3(20.0f, 20.0f, 0.1f);
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
						m_pGameInstance->Play_Sound(TEXT("Gusion_Hit3.wav"), SOUND_GUSION, g_fEffectVolume);
						m_tMonsterInfo.iHp -= static_cast<CPlayer*>(pOther)->Get_Player_Info().iAttackPower;
						CEffect::EFFECT_DESC Effect_Desc = {};
						Effect_Desc.vPos = Get_Position();
						Effect_Desc.vPos.x = Get_Position().x + 1.5f;
						Effect_Desc.vScale = _float3(20.0f, 20.0f, 0.1f);
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
			m_tMonsterInfo.iHp -= static_cast<CPlayer_Bullet*>(pOther)->Get_Player_Bullet_Info().fAtackPower;
			m_pGameInstance->Play_Sound(TEXT("Gusion_Hit3.wav"), SOUND_GUSION, g_fEffectVolume);

			if (Get_Position().x < pOther->Get_Position().x)
			{
				CEffect::EFFECT_DESC Effect_Desc = {};
				Effect_Desc.vPos = Get_Position();
				Effect_Desc.vPos.x = Get_Position().x + 1.f;
				Effect_Desc.vScale = _float3(4.0f, 4.0f, 0.1f);
				Effect_Desc.fEffectDura = 0.2f;
				Effect_Desc.strTextureTag = TEXT("HitEffect_Circle");
				Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
				Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

				m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
			}

			else if (Get_Position().x > pOther->Get_Position().x)
			{
				CEffect::EFFECT_DESC Effect_Desc = {};
				Effect_Desc.vPos = Get_Position();
				Effect_Desc.vPos.x = Get_Position().x - 1.f;
				Effect_Desc.vScale = _float3(4.0f, 4.0f, 0.1f);
				Effect_Desc.fEffectDura = 0.2f;
				Effect_Desc.strTextureTag = TEXT("HitEffect_Circle");
				Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
				Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

				m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
			}
		}

	}
}

void CBoss_Gusion::OnCollisionStay(CGameObject* pOther)
{
}

void CBoss_Gusion::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CBoss_Gusion::Ready_Components()
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
	ColliderInfo.fSizeX = 0.3f;
	ColliderInfo.fSizeY = 0.3f;
	ColliderInfo.fSizeZ = 5.f;
	ColliderInfo.fOffSetX = 0.f;
	ColliderInfo.fOffSetY = 0.f;
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

HRESULT CBoss_Gusion::Ready_Animation()
{
	CTexture* pTextureCom;

	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	// IDLE
	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON0, TEXT("Prototype_Component_Texture_Boss_Gusion_Idle"),
		TEXT("Com_Texture_Boss_Gusion_Idle"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("IDLE"), pTextureCom, 0, 34, 0.03f,true);

	// Roar
	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON0, TEXT("Prototype_Component_Texture_Boss_Gusion_Roar"),
		TEXT("Com_Texture_Boss_Gusion_Roar"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Roar"), pTextureCom, 0, 112, 0.03f, false);

	// Burp
	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON0, TEXT("Prototype_Component_Texture_Boss_Gusion_Burp"),
		TEXT("Com_Texture_Boss_Gusion_Burp"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Burp"), pTextureCom, 0, 55, 0.03f, false);

	// Lay_Egg
	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON0, TEXT("Prototype_Component_Texture_Boss_Gusion_Lay_Egg"),
		TEXT("Com_Texture_Boss_Gusion_Lay_Egg"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Lay_Egg"), pTextureCom, 0, 43, 0.03f, false);

	// Jump_Start
	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON0, TEXT("Prototype_Component_Texture_Boss_Gusion_Jump_Start"),
		TEXT("Com_Texture_Boss_Gusion_Jump_Start"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Jump_Start"), pTextureCom, 0, 34, 0.03f, false);

	// Jump_End
	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON0, TEXT("Prototype_Component_Texture_Boss_Gusion_Jump_End"),
		TEXT("Com_Texture_Boss_Gusion_Jump_End"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Jump_End"), pTextureCom, 0, 37, 0.03f, false);

	// Die
	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON0, TEXT("Prototype_Component_Texture_Boss_Gusion_Die"),
		TEXT("Com_Texture_Boss_Gusion_Die"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Die"), pTextureCom, 0, 43, 0.03f, false);

	return S_OK;
}

HRESULT CBoss_Gusion::Ready_Fsm()
{	
	/* For.Com_Fsm */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_GusionIdle::Create(m_pFsmCom,IDLE));
	m_pFsmCom->Add_State(CState_GusionRoar::Create(m_pFsmCom,ROAR));
	m_pFsmCom->Add_State(CState_GusionBurp::Create(m_pFsmCom, BURP));
	m_pFsmCom->Add_State(CState_GusionLE::Create(m_pFsmCom, LAY_EGG));
	m_pFsmCom->Add_State(CState_GusionJump::Create(m_pFsmCom, JUMP));
	m_pFsmCom->Add_State(CState_GusionDie::Create(m_pFsmCom,DIE));
	Set_InitState(ROAR);

	return S_OK;
}

HRESULT CBoss_Gusion::SetUp_RenderState()
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

HRESULT CBoss_Gusion::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CBoss_Gusion* CBoss_Gusion::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBoss_Gusion* pInstance = new CBoss_Gusion(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_Gusion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Gusion::Clone(void* pArg)
{
	CBoss_Gusion* pInstance = new CBoss_Gusion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBoss_Gusion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Gusion::Free()
{
	__super::Free();

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFsmCom);
	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pVIBufferCom);
}
