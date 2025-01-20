#include "../Public/InteractObject.h"

#include "GameInstance.h"
#include "Grid.h"

#include "Item.h"

CInteractObject::CInteractObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CLandObject { pGraphic_Device}
{
}

CInteractObject::CInteractObject(const CInteractObject& Prototype)
	:CLandObject {Prototype}
	, m_iBindTexNum{Prototype.m_iBindTexNum}
	, m_eMyType{Prototype.m_eMyType}
{

}

HRESULT CInteractObject::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CInteractObject::Initialize(void* pArg)
{
	m_eLandType = INTERACT;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	LANDOBJECT_DESC* pDesc = static_cast<LANDOBJECT_DESC*>(pArg);

	_float3 vSacling = pDesc->vInitScale;
	m_iBindTexNum = pDesc->iInitBindTexNum;
	m_eMyType = pDesc->eInteractType;
	m_iPreModelType = pDesc->iPreModelType;
	m_iPreAdornType = pDesc->iPreAdornType;
	m_iPreLandType = pDesc->iPreLandType;
	m_iLevelIndex = pDesc->iLevelIndex;
	m_iStageIndex = pDesc->iStageIndex;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(vSacling.x, vSacling.y, vSacling.z);

	m_vInitPos = pDesc->vInitPos;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vInitPos);
	m_isDead = false;

	if (FAILED(Ready_Collider()))			// << 콜라이더 뒤로빼야함 이유: 본인 스케일과 위치가 정해져야함
		return E_FAIL;

	return S_OK;
}

void CInteractObject::Priority_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;
}

void CInteractObject::Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	if (m_iPreLandType != 2)
		__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

	if (m_isShake)
	{
		Shake_InterectObject(fTimeDelta);
	}
}

void CInteractObject::Late_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	if (m_pGameInstance->Is_In_Sight(Get_Position(), m_fRadius))
	{
		if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
			return;

		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
	}
}

HRESULT CInteractObject::Render()
{
	m_pVIBufferCom->Filp_Texture(false);

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_Texture(m_iBindTexNum)))			// << 여기서 
		return E_FAIL;

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
	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_iBindTexNum));

	m_pShaderCom->Begin_Shader(0);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pShaderCom->End_Shader();

	m_pColliderCom->Render_Collider();

	return S_OK;
}

_bool CInteractObject::Crash_InterectObject()
{
	--m_iHp;

	if (m_iHp <= 0)
	{
		CItem::ITEM_DESC ItemDesc = {};

		ItemDesc.tLandDesc.iLevelIndex = m_iLevelIndex;
		ItemDesc.tLandDesc.iStageIndex = m_iStageIndex;
		ItemDesc.tLandDesc.pTerrainVIBuffer = m_pTerrainVIBuffer;
		ItemDesc.tLandDesc.pTerrainTransform = m_pTerrainTransform;


		for (_uint i = 0; i < 3; ++i)
		{
			ItemDesc.vInitPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			ItemDesc.vInitPos.z += 1.f;

			if (m_eMyType == INTERACT_TYPE::ROCK)
				ItemDesc.eItemId = CItem::ROCK;
			else if(m_eMyType == INTERACT_TYPE::TREE)
				ItemDesc.eItemId = CItem::WOOD;

			m_pGameInstance->Event_CreateObject(m_iLevelIndex, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), sizeof(CItem::ITEM_DESC), &ItemDesc);
		}

		m_pColliderCom->Set_IsActive(false);

		if (m_eMyType == INTERACT_TYPE::ROCK)
			m_pGameInstance->Play_Sound(TEXT("Rock_Dead.wav"), SOUND_ITEM, g_fEffectVolume);
		else if (m_eMyType == INTERACT_TYPE::TREE)
			m_pGameInstance->Play_Sound(TEXT("Tree_Dead.wav"), SOUND_ITEM, g_fEffectVolume);

		m_pGameInstance->Event_DestoryObject(this);

		return false;
	}

	return true;
}

HRESULT CInteractObject::Ready_Components()
{
	/* For.Com_Texture */

	// 내가 어떤 텍스쳐를 써야해 ?
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Village_Interact"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;



	wstring ModelPrototypeTag = L"";
	switch (m_iPreModelType)
	{
	case MODELRECT:
		ModelPrototypeTag = TEXT("Prototype_Component_VIBuffer_Rect");
		break;
	case GRIDRECT:
		ModelPrototypeTag = TEXT("Prototype_Component_VIBuffer_GridRect");
		break;
	default:
		break;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, ModelPrototypeTag,
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;



	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 20.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;



	
	return S_OK;
}

HRESULT CInteractObject::Ready_Collider()
{
	/* For.Com_Collider_Cube */
	CCollider_Cube::COLLIDER_CUBE_INFO ColliderInfo;

	_float3 vMyScale = m_pTransformCom->Get_Scaled();

	ZeroMemory(&ColliderInfo, sizeof(CCollider_Cube::COLLIDER_CUBE_INFO));
	ColliderInfo.fSizeX = vMyScale.x * 0.15f;
	ColliderInfo.fSizeY = vMyScale.y * 0.15f;
	ColliderInfo.fSizeZ = vMyScale.z * 0.3f;
	ColliderInfo.fOffSetX = 0.f;
	ColliderInfo.fOffSetY = 0.f;
	ColliderInfo.fOffSetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderInfo)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Blend"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInteractObject::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CInteractObject::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

void CInteractObject::Shake_InterectObject(_float fTimeDelta)
{
	if(m_eMyType ==ROCK)
		m_pGameInstance->Play_SoundRepeat(TEXT("Rock_Hit.wav"), SOUND_ITEM, g_fEffectVolume);
	else if (m_eMyType == TREE)
		m_pGameInstance->Play_SoundRepeat(TEXT("Tree_Hit.wav"), SOUND_ITEM, g_fEffectVolume);


	m_fShakePower -= fTimeDelta;

	_float3 vShakePos = m_vInitPos;
	vShakePos.x -= m_fShakePower;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vShakePos);

	if (m_fShakePower < 0.01f)
	{
		m_isShake = false;
		m_fShakePower = 0.1f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitPos);
	}

}

CInteractObject* CInteractObject::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CInteractObject* pInstance = new CInteractObject(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CInteractObject"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInteractObject::Clone(void* pArg)
{
	CInteractObject* pInstance = new CInteractObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CInteractObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInteractObject::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
