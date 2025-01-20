#include "Boss_Aim.h"

#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Collider_Cube.h"

#include "State_DeathCatIdle.h"					// 참고용으로 냅두고 다지울거당
#include "State_DeathCatChainAttack.h"			// 참고용으로 냅두고 다지울거당
#include "State_DeathCatFlyBallAttack.h"		// 참고용으로 냅두고 다지울거당
#include "State_DeathCatDisappear.h"			// 참고용으로 냅두고 다지울거당


#include "State_Aim_Idle.h"
#include "State_Aim_IdleToAttack.h"
#include "State_Aim_Walk.h"
#include <State_Aim_Attack1.h>
#include <State_Aim_Attack2.h>
#include <State_Aim_Attack3.h>
#include <Player.h>
#include <Effect.h>


#include "Player_Bullet.h"
#include <State_Aim_Die.h>

CBoss_Aim::CBoss_Aim(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CBoss_Aim::CBoss_Aim(const CBoss_Aim& Prototype)
	: CMonster{ Prototype }
{
}


HRESULT CBoss_Aim::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Aim::Initialize(void* pArg)
{
	m_eMonsterId = AYM;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	if (FAILED(Ready_Fsm()))
		return E_FAIL;

	_float3 vPosition = _float3{ 15.f,1.f,15.f };
	_float3 vScale = _float3{ 10.f, 10.f,0.1f };

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);


	m_vRecognition_Range_ToAttack = { vScale.x/2.f, 0.1f, vScale.z/2.f };

	return S_OK;
}

void CBoss_Aim::Priority_Update(_float fTimeDelta)
{
}

void CBoss_Aim::Update(_float fTimeDelta)
{
	if (m_isHit)
	{
		m_fHitEffectTime += fTimeDelta;
	}

	if (m_tMonsterInfo.iHp <= 0.f && CBoss_Aim::AIM_STATE::DIE != m_pFsmCom->Get_CurrentState())
	{	
		Change_State(CBoss_Aim::AIM_STATE::DIE);
	}
		

	__super::SetUp_OnTerrain(m_pTransformCom, 0.35f);

	m_pFsmCom->Update(fTimeDelta);
}

void CBoss_Aim::Late_Update(_float fTimeDelta)
{
	m_pAnimatorCom->Update(fTimeDelta);

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBoss_Aim::Render()
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

	//디버그 모드 추가하기
	m_pColliderCom->Render_Collider();

	return S_OK;
}

_float CBoss_Aim::Get_FootPosY()
{
	return _float();
}

void CBoss_Aim::OnCollisionEnter(CGameObject* pOther)
{
	if (CBoss_Aim::AIM_STATE::DIE == m_pFsmCom->Get_CurrentState())
		return;

	if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
	{
		if (static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK1
			|| static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK2
			|| static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK3)
		{
			m_isHit = true;

			m_pGameInstance->Play_Sound(TEXT("DeathCat_GetHit.wav"), SOUND_DEATHCAT, 0.6f);

			if (pOther->Get_isFilp())
			{
				if (Get_Position().x > pOther->Get_Position().x)
				{
					m_tMonsterInfo.iHp -= 10.f;
					CEffect::EFFECT_DESC Effect_Desc = {};
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
					m_tMonsterInfo.iHp -= 10.f;
					CEffect::EFFECT_DESC Effect_Desc = {};
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
		m_isHit = true;
		m_tMonsterInfo.iHp -= static_cast<CPlayer_Bullet*>(pOther)->Get_Player_Bullet_Info().fAtackPower;
	}
}

void CBoss_Aim::OnCollisionStay(CGameObject* pOther)
{
}

void CBoss_Aim::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CBoss_Aim::Ready_Components()
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
	ColliderInfo.fSizeZ = 2.f;
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

HRESULT CBoss_Aim::Ready_Animation()
{
	CTexture* pTextureCom = nullptr;

	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;


	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Aim_Idle"),
		TEXT("Com_Texture_Boss_Aim_Idle"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Idle"), pTextureCom, 0, 28, 0.04f);


	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Aim_IdleToAttack"),
		TEXT("Com_Texture_Boss_Aim_IdleToAttack"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("IdleToAttack"), pTextureCom, 0, 57, 0.04f, false);



	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Aim_Walk"),
		TEXT("Com_Texture_Boss_Aim_Walk"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Walk"), pTextureCom, 0, 34, 0.07f, false);



	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Aim_Attack1"),
		TEXT("Com_Texture_Boss_Aim_Attack1"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Attack1"), pTextureCom, 0, 23, 0.05f, false);


	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Aim_Attack2"),
		TEXT("Com_Texture_Boss_Aim_Attack2"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Attack2"), pTextureCom, 0, 70, 0.05f, false);


	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Aim_Attack3"),
		TEXT("Com_Texture_Boss_Aim_Attack3"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Attack3"), pTextureCom, 0, 44, 0.05f, false);



	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Aim_Die"),
		TEXT("Com_Texture_Boss_Aim_Die"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Die"), pTextureCom, 0, 53, 0.05f, false);




	
	m_pAnimatorCom->Change_Animation(TEXT("Idle"), 0);

	return S_OK;
}

HRESULT CBoss_Aim::Ready_Fsm()
{
	/* For.Com_Fsm */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;


	m_pFsmCom->Add_State(CState_Aim_Idle::Create(m_pFsmCom, AIM_STATE::IDLE));
	m_pFsmCom->Add_State(CState_Aim_IdleToAttack::Create(m_pFsmCom, AIM_STATE::IDLE_TO_ATTACK));
	m_pFsmCom->Add_State(CState_Aim_Walk::Create(m_pFsmCom, AIM_STATE::WALK));
	m_pFsmCom->Add_State(CState_Aim_Attack1::Create(m_pFsmCom, AIM_STATE::ATTACK1));
	m_pFsmCom->Add_State(CState_Aim_Attack2::Create(m_pFsmCom, AIM_STATE::ATTACK2));
	m_pFsmCom->Add_State(CState_Aim_Attack3::Create(m_pFsmCom, AIM_STATE::ATTACK3));
	m_pFsmCom->Add_State(CState_Aim_Die::Create(m_pFsmCom, AIM_STATE::DIE));


	Set_InitState(IDLE);

	return S_OK;
}

HRESULT CBoss_Aim::SetUp_RenderState()
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

HRESULT CBoss_Aim::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CBoss_Aim* CBoss_Aim::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBoss_Aim* pInstance = new CBoss_Aim(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_Aim"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CBoss_Aim::Clone(void* pArg)
{
	CBoss_Aim* pInstance = new CBoss_Aim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_Aim"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Aim::Free()
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
