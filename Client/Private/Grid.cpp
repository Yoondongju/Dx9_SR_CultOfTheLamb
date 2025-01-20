#include "Grid.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Texture.h"

#include "BuildingObject.h"

#include "Player.h"

CGrid::CGrid(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CBlendObject{ pGraphic_Device }
{
}

CGrid::CGrid(const CGrid& Prototype)
    :CBlendObject{ Prototype }
{
}

HRESULT CGrid::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGrid::Initialize(void* pArg)
{
	GRID_DESC *pGridDesc = static_cast<GRID_DESC*>(pArg);
	

	m_tMyDesc.pOwner = pGridDesc->pOwner;
	m_tMyDesc.iBindNum = pGridDesc->iBindNum;
	m_tMyDesc.vPos = pGridDesc->vPos;
	m_tMyDesc.vScale = pGridDesc->vScale;

	if (nullptr == m_tMyDesc.pOwner)
	{
		MSG_BOX(TEXT("Can't Grid InitClone"));
		return E_FAIL;
	}
	else
		


	if (FAILED(Ready_Components()))
		return E_FAIL;

	

	m_pTransformCom->Scaling(m_tMyDesc.vScale.x, m_tMyDesc.vScale.y, m_tMyDesc.vScale.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tMyDesc.vPos);
	
	m_vInitOffset = m_tMyDesc.vPos - m_tMyDesc.pOwner->Get_Position();

	

    return S_OK;
}

void CGrid::Priority_Update(_float fTimeDelta)
{
	if (((CPlayer*)m_pGameInstance->Find_Player())->Get_Player_State() == CPlayer::PLAYERSTATE::BUILDING)
		m_bActivate = true;
	else
		m_bActivate = false;
}

void CGrid::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	if (true == m_tMyDesc.pOwner->Get_Dead())
		m_pGameInstance->Event_DestoryObject(this);
	
	if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
		return;

	CBuildingObject* pBuildingObj = dynamic_cast<CBuildingObject*>(m_tMyDesc.pOwner);

	if (nullptr != pBuildingObj)
	{		
		_float3 vBuildingObjectPos = m_tMyDesc.pOwner->Get_Position();

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3{ vBuildingObjectPos.x + m_vInitOffset.x ,0.62f, vBuildingObjectPos.z + m_vInitOffset.z });

		if (pBuildingObj->IsLand())
			m_tMyDesc.iBindNum = 2;
	}
	
}

void CGrid::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CGrid::Render()
{
	if (!m_bActivate)
		return S_OK;

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(m_tMyDesc.iBindNum)))			// << ¿©±â¼­ 
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;


	SetUp_RenderState();
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	Reset_RenderState();

	return S_OK;
}

HRESULT CGrid::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Placement"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rhombus"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;



	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 20.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	
	return S_OK;
}

HRESULT CGrid::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 127);

	return S_OK;
}

HRESULT CGrid::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CGrid* CGrid::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGrid* pInstance = new CGrid(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGrid"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGrid::Clone(void* pArg)
{
	CGrid* pInstance = new CGrid(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CGrid"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGrid::Free()
{
	__super::Free();


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
