#include "..\Public\BackGround.h"

#include "GameInstance.h"
#include "..\Public\Effect.h"

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBlendObject{ pGraphic_Device }
{
}

CEffect::CEffect(const CEffect & Prototype)
	: CBlendObject{ Prototype }
{
}


HRESULT CEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	EFFECT_DESC* Effect_Desc = static_cast<EFFECT_DESC*>(pArg);
	m_Effect_Desc.vPos = Effect_Desc->vPos;
	m_Effect_Desc.vScale = Effect_Desc->vScale;
	m_Effect_Desc.vDir = Effect_Desc->vDir;
	m_Effect_Desc.vSpeed = Effect_Desc->vSpeed;
	m_Effect_Desc.bIsFilp = Effect_Desc->bIsFilp;
	m_Effect_Desc.fDelayTimer = Effect_Desc->fDelayTimer;
	m_Effect_Desc.fJumpPower = Effect_Desc->fJumpPower;
	m_Effect_Desc.fEffectDura = Effect_Desc->fEffectDura;
	m_Effect_Desc.strTextureTag = Effect_Desc->strTextureTag;
	m_Effect_Desc.iLevelIndex = Effect_Desc->iLevelIndex;
	m_Effect_Desc.iStageIndex = Effect_Desc->iStageIndex;

	m_bEffectEnd = false;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_Effect_Desc.vPos);
	m_pTransformCom->Scaling(m_Effect_Desc.vScale);
	m_pAnimCom->Change_Animation(m_Effect_Desc.strTextureTag);

	if (m_pAnimCom->Get_AnimationTag() == TEXT("Target"))
	{
		_float fRadian = D3DXToRadian(90);
		m_pTransformCom->Rotation(_float3(1.f,0.f,0.f), fRadian);
	}

	if (m_pAnimCom->Get_AnimationTag() == TEXT("Aiming_Recticule"))
	{
		_float fRadian = D3DXToRadian(90);
		m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), fRadian);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, m_Effect_Desc.vDir);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, _float3(0.f, -1.f, 0.f));
		_float3	vUp = {};
		D3DXVec3Cross(&vUp, &m_pTransformCom->Get_State(CTransform::STATE_LOOK), &m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	}

	return S_OK;
}

void CEffect::Priority_Update(_float fTimeDelta)
{
	if (m_bEffectEnd)
	{
		m_pGameInstance->Event_DestoryObject(this);
	}
}

void CEffect::Update(_float fTimeDelta)
{
	m_fEffectTimer += fTimeDelta;
	m_fDelayTimer += fTimeDelta;

	if (m_pAnimCom->Get_AnimationTag() == TEXT("Aiming_Recticule"))
	{
		m_Effect_Desc.vScale.x += fTimeDelta * 10.f;

		m_pTransformCom->Scaling(m_Effect_Desc.vScale);

		if (m_pGameInstance->Get_KeyState(KEY::RBUTTON) == KEY_STATE::AWAY)
			m_bEffectEnd = true;
	}

	if (m_pAnimCom->Get_AnimationTag() == TEXT("Heart_Effect"))
	{
		m_Effect_Desc.fJumpPower -= fTimeDelta * 5.f;

		m_Effect_Desc.vPos.x += m_Effect_Desc.vDir.x * fTimeDelta* m_Effect_Desc.vSpeed.x;
		m_Effect_Desc.vPos.y += m_Effect_Desc.fJumpPower * fTimeDelta;
		m_Effect_Desc.vPos.z += m_Effect_Desc.vDir.z * fTimeDelta* m_Effect_Desc.vSpeed.z;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_Effect_Desc.vPos);


		if (m_Effect_Desc.vPos.y < 3.f)
		{
			m_pGameInstance->Event_DestoryObject(this);
		}

	}

	if(m_pAnimCom->Get_AnimationTag() != TEXT("Aiming_Recticule"))
	{
		if (m_fEffectTimer >= m_Effect_Desc.fEffectDura)
		{
			m_bEffectEnd = true;
			m_fEffectTimer = 0.f;
		}
	}
}

void CEffect::Late_Update(_float fTimeDelta)
{
	m_pAnimCom->Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CEffect::Render()
{
	m_pVIBufferCom->Filp_Texture(m_Effect_Desc.bIsFilp);

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	m_pAnimCom->Render(NULL);

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect::Ready_Components()
{
	CTexture* pTexture = { nullptr };

	// Anim Component
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimCom), nullptr)))
		return E_FAIL;

	// Boss_Dead
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Dead_Effect"),
		TEXT("Com_Texture_Boss_Dead_Effect"),
		reinterpret_cast<CComponent**>(&pTexture))))
		return E_FAIL;

	m_pAnimCom->Add_Animation(TEXT("Boss_Dead"), pTexture, 0, 19, 0.03f, false);

	// Pick_Up_Item
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pick_Up_Item_Effect"),
		TEXT("Com_Texture_Pick_Up_Item_Effect"),
		reinterpret_cast<CComponent**>(&pTexture))))
		return E_FAIL;

	m_pAnimCom->Add_Animation(TEXT("Pick_Up_Item"), pTexture, 0, 5, 0.03f, false);

	// Target
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Target_Effect"),
		TEXT("Com_Texture_Target_Effect"),
		reinterpret_cast<CComponent**>(&pTexture))))
		return E_FAIL;

	m_pAnimCom->Add_Animation(TEXT("Target"), pTexture, 0, 1, 0.03f, true);

	// WalkDust
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WalkDust_Effect"),
		TEXT("Com_Texture_WalkDust_Effect"),
		reinterpret_cast<CComponent**>(&pTexture))))
		return E_FAIL;

	m_pAnimCom->Add_Animation(TEXT("WalkDust"), pTexture, 0, 9, 0.03f, true);

	// Hit_Effect_Blood
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_Blood_Effect"),
		TEXT("Com_Texture_Hit_Blood_Effect"),
		reinterpret_cast<CComponent**>(&pTexture))))
		return E_FAIL;

	m_pAnimCom->Add_Animation(TEXT("Hit_Blood"), pTexture, 0, 12, 0.03f, false);

	// Hit_Effect_Star
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_Star_Effect"),
		TEXT("Com_Texture_Hit_Star_Effect"),
		reinterpret_cast<CComponent**>(&pTexture))))
		return E_FAIL;

	m_pAnimCom->Add_Animation(TEXT("Hit_Star"), pTexture, 0, 14, 0.03f, false);

	// Enemy_Suicide
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Enemy_Suicide"),
		TEXT("Com_Texture_Enemy_Suicide_Effect"),
		reinterpret_cast<CComponent**>(&pTexture))))
		return E_FAIL;

	m_pAnimCom->Add_Animation(TEXT("Enemy_Sucide"), pTexture, 0, 97, 0.03f, false);

	// Heart_Effect
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Heart_Effect"),
		TEXT("Com_Texture_Heart_Effect"),
		reinterpret_cast<CComponent**>(&pTexture))))
		return E_FAIL;

	m_pAnimCom->Add_Animation(TEXT("Heart_Effect"), pTexture, 6, 76, 0.03f, true);

	// HitEffect_Rise
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HitEffect_Rise"),
		TEXT("Com_Texture_HitEffect_Rise"),
		reinterpret_cast<CComponent**>(&pTexture))))
		return E_FAIL;

	m_pAnimCom->Add_Animation(TEXT("HitEffect_Rise"), pTexture, 0, 19, 0.03f, false);

	// StrengthBuff
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StrengthBuff_Effect"),
		TEXT("Com_Texture_StrengthBuff_Effect"),
		reinterpret_cast<CComponent**>(&pTexture))))
		return E_FAIL;

	m_pAnimCom->Add_Animation(TEXT("StrengthBuff"), pTexture, 0, 61, 0.03f, false);

	// Aiming_Recticule
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Aiming_Recticule_Effect"),
		TEXT("Com_Texture_Aiming_Recticule_Effect"),
		reinterpret_cast<CComponent**>(&pTexture))))
		return E_FAIL;

	m_pAnimCom->Add_Animation(TEXT("Aiming_Recticule"), pTexture, 0, 1, 0.03f, true);

	// HitEffect_Circle
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HitEffect_Circle"),
		TEXT("Com_Texture_Aiming_HitEffect_Circle"),
		reinterpret_cast<CComponent**>(&pTexture))))
		return E_FAIL;

	m_pAnimCom->Add_Animation(TEXT("HitEffect_Circle"), pTexture, 0, 9, 0.03f, true);

	// After_Harvest
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_After_Harvest_Effect"),
		TEXT("Com_Texture_After_Harvest_Effect"),
		reinterpret_cast<CComponent**>(&pTexture))))
		return E_FAIL;

	m_pAnimCom->Add_Animation(TEXT("After_Harvest"), pTexture, 0, 30, 0.03f, true);


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

	return S_OK;
}

HRESULT CEffect::SetUp_RenderState()
{
	_float3		vSourColor, vDestColor;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 127);

	return S_OK;
}

HRESULT CEffect::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CEffect * CEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect*		pInstance = new CEffect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CEffect::Clone(void* pArg)
{
	CEffect*		pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pAnimCom);
}
