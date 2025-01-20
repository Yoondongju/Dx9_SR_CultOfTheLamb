
#include "Boss_Eye.h"
#include "Player.h"
#include "Layer.h"

#include "Level_Dungeon03.h"
#include "Eye_Controller.h"

#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Collider_Cube.h"

#include "State_EyeIdle.h"
#include "State_EyeAttack.h"
#include "State_EyeDisappear.h"
#include "State_EyeDie.h"

#include "Effect.h"
#include "Item.h"

#include "Player_Bullet.h"

CBoss_Eye::CBoss_Eye(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CBoss_Eye::CBoss_Eye(const CBoss_Eye& Prototype)
	: CMonster{ Prototype }
{

}


HRESULT CBoss_Eye::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Eye::Initialize(void* pArg)
{
	m_eMonsterId = EYE;

	
	EYE_DESC* MonsterInfo = static_cast<EYE_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	if (FAILED(Ready_Fsm()))
		return E_FAIL;

	_float3 vScale = _float3{ 4.f,4.f,1.f };

	m_iLevelIndex = LEVEL_DUNGEON3;
	m_iStageIndex = 0;

	m_tMonsterInfo.iAttackPower = 3.f;
	m_tMonsterInfo.iMaxHp = 50.f;
	m_tMonsterInfo.iHp = 50.f;

	
	m_eMyType = MonsterInfo->eEyeType;

	m_vInitPosition = MonsterInfo->eMonInfo.vInitPosition;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitPosition);

	m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);

	return S_OK;
}

void CBoss_Eye::Priority_Update(_float fTimeDelta)
{
}

void CBoss_Eye::Update(_float fTimeDelta)
{
	__super::SetUp_OnTerrain(m_pTransformCom, 0.8f);

	if (m_isHit)
	{
		m_fHitEffectTime += fTimeDelta;
	}

	if (!dynamic_cast<CLevel_Dungeon03*>(m_pGameInstance->Get_CurLevel())->Get_EyeController()->Get_IsStart())
		return;

	if (m_tMonsterInfo.iHp <= 0 && !m_isEndDieAnimation)
	{
		m_pGameInstance->Play_Sound(TEXT("Eye_Die.wav"), SOUND_EYE, g_fEffectVolume);
		m_pFsmCom->Change_State(DIE);
		m_isEndDieAnimation = true;
	}

	m_pFsmCom->Update(fTimeDelta);

	
}

void CBoss_Eye::Late_Update(_float fTimeDelta)
{
	m_pAnimatorCom->Update(fTimeDelta);

	m_pColliderCom->Set_IsActive(!m_isHide);
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBoss_Eye::Render()
{
	if (!m_isHide)
	{
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
	}

	//디버그 모드 추가하기
	m_pColliderCom->Render_Collider();

	return S_OK;
}

_float CBoss_Eye::Get_FootPosY()
{
	return _float();
}

void CBoss_Eye::Move_Dir(_float fTimeDelta, _float3 vDir, _float fSpeedPerSec)
{
	m_pTransformCom->Move_Dir(fTimeDelta, _float3(vDir.x, vDir.y, vDir.z), fSpeedPerSec);
}

void CBoss_Eye::OnCollisionEnter(CGameObject* pOther)
{
	if (m_tMonsterInfo.iHp > 0)
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
		{
			if (static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK1
				|| static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK2
				|| static_cast<CPlayer*>(pOther)->Get_Player_State() == CPlayer::ATTACK3)
			{
				m_isHit = true;
				m_pGameInstance->Play_Sound(TEXT("DeathCat_GetHit.wav"), SOUND_EYE, 0.6f);
				if (pOther->Get_isFilp())
				{
					if (Get_Position().x > pOther->Get_Position().x)
					{
						m_tMonsterInfo.iHp -= static_cast<CPlayer*>(pOther)->Get_Player_Info().iAttackPower;						//터지는 이펙트
						CEffect::EFFECT_DESC Effect_Desc = {};
						Effect_Desc.vPos = Get_Position();
						Effect_Desc.vPos.y = Get_Position().y;
						Effect_Desc.vScale = _float3(8.0f, 8.0f, 0.1f);
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
						Effect_Desc.vPos.y = Get_Position().y ;
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
			m_tMonsterInfo.iHp -= static_cast<CPlayer_Bullet*>(pOther)->Get_Player_Bullet_Info().fAtackPower;
		}
	}
}

void CBoss_Eye::OnCollisionStay(CGameObject* pOther)
{
}

void CBoss_Eye::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CBoss_Eye::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 0.3f;
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

HRESULT CBoss_Eye::Ready_Animation()
{
	CTexture* pTextureCom;

	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Eye_Idle"),
		TEXT("Com_Texture_Boss_Eye_Idle"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("IDLE"), pTextureCom, 0, 45, 0.08f);

	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Eye_AttackBegin"),
		TEXT("Com_Texture_Boss_Eye_AttackBegin"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Attack_Begin"), pTextureCom, 0, 23, 0.05f, false);

	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Eye_AttackLoop"),
		TEXT("Com_Texture_Boss_Eye_AttackLoop"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Attack_Loop"), pTextureCom, 0, 23, 0.08f);

	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Eye_AttackEnd"),
		TEXT("Com_Texture_Boss_Eye_AttackEnd"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Attack_End"), pTextureCom, 0, 23, 0.05f, false);

	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Eye_Appear"),
		TEXT("Com_Texture_Boss_Eye_Appear"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Appear"), pTextureCom, 0, 25, 0.05f, false);

	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Eye_Disappear"),
		TEXT("Com_Texture_Boss_Eye_Disappear"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Disappear"), pTextureCom, 0, 21, 0.05f, false);

	if (FAILED(__super::Add_Component(Client::LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Eye_Die"),
		TEXT("Com_Texture_Boss_Eye_Die"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Die"), pTextureCom, 0, 16, 0.07f, false);

	m_pAnimatorCom->Change_Animation(TEXT("IDLE"), 0);

	return S_OK;
}

HRESULT CBoss_Eye::Ready_Fsm()
{
	/* For.Com_Fsm */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_EyeIdle::Create(m_pFsmCom, IDLE));
	m_pFsmCom->Add_State(CState_EyeAttack::Create(m_pFsmCom, ATTACK));
	m_pFsmCom->Add_State(CState_EyeDisappear::Create(m_pFsmCom, DISAPPEAR));
	m_pFsmCom->Add_State(CState_EyeDie::Create(m_pFsmCom, DIE));

	Set_InitState(IDLE);

	return S_OK;
}

HRESULT CBoss_Eye::SetUp_RenderState()
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

HRESULT CBoss_Eye::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CBoss_Eye* CBoss_Eye::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBoss_Eye* pInstance = new CBoss_Eye(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_DeathCat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CBoss_Eye::Clone(void* pArg)
{
	CBoss_Eye* pInstance = new CBoss_Eye(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_DeathCat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Eye::Free()
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
