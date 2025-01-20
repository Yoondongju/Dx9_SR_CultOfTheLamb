#include "Boss_Hekat.h"
#include "GameInstance.h"

#include "Player.h"

#include "State_HekatDie.h"
#include "State_HekatIdle.h"
#include "State_HekatJump.h"
#include "State_HekatLE.h"
#include "State_HekatRoar.h"
#include "State_HekatTB.h"
#include "State_HekatTF.h"
#include "State_HekatTH.h"

#include "Effect.h"

#include "Player_Bullet.h"

CBoss_Hekat::CBoss_Hekat(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CMonster(pGraphic_Device)
{
}

CBoss_Hekat::CBoss_Hekat(const CBoss_Hekat& Prototype)
	:CMonster(Prototype)
{
}

HRESULT CBoss_Hekat::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Hekat::Initialize(void* pArg)
{
	m_eMonsterId = HEKAT;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	if (FAILED(Ready_Fsm()))
		return E_FAIL;

	_float3 vScale = _float3{ 5.f,5.f,0.1f };
	m_vInitPosition = _float3(11.5f, 2.5f, 8.5f);
	m_fAttackTimer = 0.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitPosition);
	m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);

	return S_OK;
}

void CBoss_Hekat::Priority_Update(_float fTimeDelta)
{
	if (m_tMonsterInfo.iHp <= 0.f)
		m_tMonsterInfo.iHp = 0.f;

	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);
}

void CBoss_Hekat::Update(_float fTimeDelta)
{
	if (m_isHit)
	{
		m_fHitEffectTime += fTimeDelta;
	}

	Calculate_Flip();
	m_pFsmCom->Update(fTimeDelta);
}

void CBoss_Hekat::Late_Update(_float fTimeDelta)
{
	m_pAnimatorCom->Update(fTimeDelta);
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBoss_Hekat::Render()
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

void CBoss_Hekat::OnCollisionEnter(CGameObject* pOther)
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
						m_pGameInstance->Play_Sound(TEXT("Enemy_Hit1.wav"), SOUND_HEKAT, g_fEffectVolume);
						m_tMonsterInfo.iHp -= static_cast<CPlayer*>(pOther)->Get_Player_Info().iAttackPower;						CEffect::EFFECT_DESC Effect_Desc = {};
						Effect_Desc.vPos = Get_Position();
						Effect_Desc.vPos.x = Get_Position().x - 1.5f;
						Effect_Desc.vScale = _float3(6.0f, 6.0f, 0.1f);
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
						m_pGameInstance->Play_Sound(TEXT("Enemy_Hit1.wav"), SOUND_HEKAT, g_fEffectVolume);
						m_tMonsterInfo.iHp -= static_cast<CPlayer*>(pOther)->Get_Player_Info().iAttackPower;						CEffect::EFFECT_DESC Effect_Desc = {};
						Effect_Desc.vPos = Get_Position();
						Effect_Desc.vPos.x = Get_Position().x + 1.5f;
						Effect_Desc.vScale = _float3(6.0f, 6.0f, 0.1f);
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
			m_pGameInstance->Play_Sound(TEXT("Enemy_Hit1.wav"), SOUND_HEKAT, g_fEffectVolume);
			m_tMonsterInfo.iHp -= static_cast<CPlayer_Bullet*>(pOther)->Get_Player_Bullet_Info().fAtackPower;
			m_isHit = true;
			if (Get_Position().x < pOther->Get_Position().x)
			{
				CEffect::EFFECT_DESC Effect_Desc = {};
				Effect_Desc.vPos = Get_Position();
				Effect_Desc.vPos.x = Get_Position().x + 1.f;
				Effect_Desc.vScale = _float3(2.0f, 2.0f, 0.1f);
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
				Effect_Desc.vScale = _float3(2.0f, 2.0f, 0.1f);
				Effect_Desc.fEffectDura = 0.2f;
				Effect_Desc.strTextureTag = TEXT("HitEffect_Circle");
				Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
				Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

				m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
			}
		}
	
}

void CBoss_Hekat::OnCollisionStay(CGameObject* pOther)
{
}

void CBoss_Hekat::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CBoss_Hekat::Ready_Components()
{
	// Collider Component
	CCollider_Cube::COLLIDER_CUBE_INFO ColliderInfo = {};
	ColliderInfo.fSizeX = 0.3f;
	ColliderInfo.fSizeY = 0.3f;
	ColliderInfo.fSizeZ = 5.f;
	ColliderInfo.fOffSetX = 0.f;
	ColliderInfo.fOffSetY = 0.f;
	ColliderInfo.fOffSetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderInfo)))
		return E_FAIL;

	// CVIBuffer_Rect Component
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	// CTransform Component
	CTransform::TRANSFORM_DESC Transform_Desc = {};
	Transform_Desc.fSpeedPerSec = 10.0f;
	Transform_Desc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &Transform_Desc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Boss"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Hekat::Ready_Animation()
{
	CTexture* pTextureCom;

	// Anim Component
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animation"), TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimatorCom))))
		return E_FAIL;

	// Transform_Human
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Transform_Human"), TEXT("Com_Texture_Boss_Hekat_Transform_Human"), reinterpret_cast<CComponent**>(&pTextureCom))))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Transform_Human"), pTextureCom, 0, 378, 0.03f, false);

	// Transform_Frog
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Transform_Frog"), TEXT("Com_Texture_Boss_Hekat_Transform_Frog"), reinterpret_cast<CComponent**>(&pTextureCom))))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Transform_Frog"), pTextureCom, 0, 167, 0.03f, false);

	// IDLE
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Idle"), TEXT("Com_Texture_Boss_Hekat_Idle"), reinterpret_cast<CComponent**>(&pTextureCom))))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("IDLE"), pTextureCom, 0, 75, 0.03f, true);

	// Roar
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Roar"), TEXT("Com_Texture_Boss_Hekat_Roar"), reinterpret_cast<CComponent**>(&pTextureCom))))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Roar"), pTextureCom, 0, 118, 0.03f, false);

	// Jump_Start
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Jump_Start"), TEXT("Com_Texture_Boss_Hekat_Jump_Start"), reinterpret_cast<CComponent**>(&pTextureCom))))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Jump_Start"), pTextureCom, 0, 34, 0.03f, false);

	// Jump
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Jump"), TEXT("Com_Texture_Boss_Hekat_Jump"), reinterpret_cast<CComponent**>(&pTextureCom))))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Jump"), pTextureCom, 0, 19, 0.03f, false);

	// Jump_End
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Jump_End"), TEXT("Com_Texture_Boss_Hekat_Jump_End"), reinterpret_cast<CComponent**>(&pTextureCom))))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Jump_End"), pTextureCom, 0, 26, 0.03f, false);

	// Lay_Egg
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Lay_Egg"), TEXT("Com_Texture_Boss_Hekat_Lay_Egg"), reinterpret_cast<CComponent**>(&pTextureCom))))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Lay_Egg"), pTextureCom, 0, 67, 0.03f, false);

	// Throw_Bomb
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Throw_Bomb"), TEXT("Com_Texture_Boss_Hekat_Throw_Bomb"), reinterpret_cast<CComponent**>(&pTextureCom))))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Throw_Bomb"), pTextureCom, 0, 46, 0.03f, false);

	// Die
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Die"), TEXT("Com_Texture_Boss_Hekat_Die"), reinterpret_cast<CComponent**>(&pTextureCom))))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Die"), pTextureCom, 0, 112, 0.03f, false);

	return S_OK;
}

HRESULT CBoss_Hekat::Ready_Fsm()
{
	// Fsm Component
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"), TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_HekatIdle::Create(m_pFsmCom, IDLE));
	m_pFsmCom->Add_State(CState_HekatJump::Create(m_pFsmCom, JUMP));
	m_pFsmCom->Add_State(CState_HekatLE::Create(m_pFsmCom, LAY_EGG));
	m_pFsmCom->Add_State(CState_HekatDie::Create(m_pFsmCom, DIE));
	m_pFsmCom->Add_State(CState_HekatRoar::Create(m_pFsmCom, ROAR));
	m_pFsmCom->Add_State(CState_HekatTB::Create(m_pFsmCom, THROW_BOMB));
	m_pFsmCom->Add_State(CState_HekatTF::Create(m_pFsmCom, TRANS_FROG));
	m_pFsmCom->Add_State(CState_HekatTH::Create(m_pFsmCom, TRANS_HUMAN));

	m_pFsmCom->Set_State(TRANS_HUMAN);

	return S_OK;
}

HRESULT CBoss_Hekat::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 127);

	return S_OK;
}

HRESULT CBoss_Hekat::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}


CBoss_Hekat* CBoss_Hekat::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBoss_Hekat* pInstance = new CBoss_Hekat(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_Hekat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Hekat::Clone(void* pArg)
{
	CBoss_Hekat* pInstance = new CBoss_Hekat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBoss_Hekat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Hekat::Free()
{
	__super::Free();
	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFsmCom);
	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pVIBufferCom);
}
