
#include "../Public/DecoObejct.h"

#include "GameInstance.h"
#include "Grid.h"

CDecoObject::CDecoObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CLandObject(pGraphic_Device)
{
}

CDecoObject::CDecoObject(const CDecoObject& Prototype)
	: CLandObject(Prototype)
	, m_iBindTexNum{ Prototype.m_iBindTexNum }
{
}

HRESULT CDecoObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecoObject::Initialize(void* pArg)
{
	m_eLandType = DECO;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	LANDOBJECT_DESC* pDesc = static_cast<LANDOBJECT_DESC*>(pArg);


	_float3 vSacling = pDesc->vInitScale;
	m_iBindTexNum = pDesc->iInitBindTexNum;

	m_iPreModelType = pDesc->iPreModelType;
	m_iPreAdornType = pDesc->iPreAdornType;
	m_iPreLandType = pDesc->iPreLandType;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(vSacling.x, vSacling.y, vSacling.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vInitPos);

	m_fRadius = m_pTransformCom->Get_Scaled().y * 0.5f;

	return S_OK;
}

void CDecoObject::Priority_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;
}

void CDecoObject::Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	if(m_iPreLandType != 2)
		__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

	if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
		return;

}

void CDecoObject::Late_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	if (m_pGameInstance->Is_In_Sight(Get_Position(), m_fRadius))
	{
		if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
			return;

		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
	}
}

HRESULT CDecoObject::Render()
{
	 m_pVIBufferCom->Filp_Texture(false);

 	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	
	if (FAILED(m_pTextureCom->Bind_Texture(m_iBindTexNum)))			// << ¿©±â¼­ 
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;


	SetUp_RenderState();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Reset_RenderState();

	return S_OK;
}

HRESULT CDecoObject::Ready_Components()
{
	/* For.Com_Texture */


	wstring AdornPrototypeTag = L"";
	switch (m_iPreAdornType)
	{
	case VILLAGE:
		AdornPrototypeTag = TEXT("Prototype_Component_Texture_Village_Deco");
		break;
	case STAGE1:
		AdornPrototypeTag = TEXT("Prototype_Component_Texture_Dungeon_Deco");
		break;
	case STAGE2:
		AdornPrototypeTag = TEXT("Prototype_Component_Texture_Dungeon2_Deco");
		break;
	case STAGE3:
		AdornPrototypeTag = TEXT("Prototype_Component_Texture_Dungeon_ETC_Deco");
		break;
	default:
		break;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, AdornPrototypeTag,
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

HRESULT CDecoObject::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CDecoObject::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CDecoObject* CDecoObject::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDecoObject* pInstance = new CDecoObject(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDecoObject"));

		(pInstance);
	}

	return pInstance;
}

CGameObject* CDecoObject::Clone(void* pArg)
{
	CDecoObject* pInstance = new CDecoObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecoObject::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
