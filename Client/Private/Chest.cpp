
#include "Chest.h"

#include "Item.h"

#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"

CChest::CChest(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject{ pGraphic_Device }
{
}

CChest::CChest(const CChest& Prototype)
	: CLandObject{ Prototype }
{
}


HRESULT CChest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChest::Initialize(void* pArg)
{
	CHEST_DESC* ChestDesc = static_cast<CHEST_DESC*>(pArg);

	if (FAILED(__super::Initialize(&ChestDesc->tLandObjectDesc)))
		return E_FAIL;

	_float3 vInitPos = ChestDesc->vIinitPos;
	_float3 vScale = _float3{ 5.f,5.f,1.f };

	m_eChestId = ChestDesc->eChestId;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vInitPos);
	m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);

	m_eCurrentState = REVEAL;

	m_iCreateItemCount += rand() % 5;

	return S_OK;
}

void CChest::Priority_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

}

void CChest::Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	if (m_eCurrentState == REVEAL && m_pAnimatorCom->IsEnd_CurrentAnimation())
	{
		m_pGameInstance->Play_Sound(TEXT("Chest_Land_and_Open.wav"), SOUND_ITEM, g_fEffectVolume);
		m_eCurrentState = OPEN;
		m_pAnimatorCom->Change_Animation(TEXT("Open"), 0);
	}

	if (m_eCurrentState == OPEN)
	{
		if (m_pAnimatorCom->Get_CurrentFrame() >= 8)
		{
			CItem::ITEM_DESC ItemDesc = {};

			ItemDesc.tLandDesc.iLevelIndex = m_iLevelIndex;
			ItemDesc.tLandDesc.iStageIndex = m_iStageIndex;
			ItemDesc.tLandDesc.pTerrainVIBuffer = m_pTerrainVIBuffer;
			ItemDesc.tLandDesc.pTerrainTransform = m_pTerrainTransform;


			ItemDesc.vInitPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			ItemDesc.vInitPos.z += 1.f;

			ItemDesc.eItemId = (CItem::ITEM_ID)::CItem::COIN;
			m_pGameInstance->Event_CreateObject(m_iLevelIndex, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), sizeof(CItem::ITEM_DESC), &ItemDesc);

			ItemDesc.eItemId = (CItem::ITEM_ID)::CItem::COIN;
			m_pGameInstance->Event_CreateObject(m_iLevelIndex, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), sizeof(CItem::ITEM_DESC), &ItemDesc);

			ItemDesc.eItemId = (CItem::ITEM_ID)::CItem::WOOD;
			m_pGameInstance->Event_CreateObject(m_iLevelIndex, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), sizeof(CItem::ITEM_DESC), &ItemDesc);

			ItemDesc.eItemId = (CItem::ITEM_ID)::CItem::ROCK;
			m_pGameInstance->Event_CreateObject(m_iLevelIndex, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), sizeof(CItem::ITEM_DESC), &ItemDesc);


			m_eCurrentState = OPENED;
		}
	}

}

void CChest::Late_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	if(m_eCurrentState == REVEAL || m_eCurrentState == OPEN)
		m_pAnimatorCom->Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CChest::Render()
{
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

	return S_OK;
}

void CChest::OnCollisionEnter(CGameObject* pOther)
{

}

void CChest::OnCollisionStay(CGameObject* pOther)
{
}

void CChest::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CChest::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CChest::Ready_Animation()
{
	CTexture* pTextureCom;

	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	switch (m_eChestId)
	{
	case GOLD:	

		if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_GoldChest_Open"),
		TEXT("Com_Texture_BatFly"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
		m_pAnimatorCom->Add_Animation(TEXT("Open"), pTextureCom, 0, 34, 0.03f, false);

		if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_GoldChest_Reveal"),
			TEXT("Com_Texture_BatAttackCharge"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
			return E_FAIL;
		m_pAnimatorCom->Add_Animation(TEXT("Reveal"), pTextureCom, 0, 38, 0.03f, false);

		break;

	case WOODEN:

		if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_WoodChest_Open"),
			TEXT("Com_Texture_BatFly"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
			return E_FAIL;
		m_pAnimatorCom->Add_Animation(TEXT("Open"), pTextureCom, 0, 34, 0.03f, false);

		if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_WoodChest_Reveal"),
			TEXT("Com_Texture_BatAttackCharge"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
			return E_FAIL;
		m_pAnimatorCom->Add_Animation(TEXT("Reveal"), pTextureCom, 0, 38, 0.03f, false);

		break;
	}



	m_pAnimatorCom->Change_Animation(TEXT("Reveal"), 0);

	return S_OK;
}

HRESULT CChest::SetUp_RenderState()
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

HRESULT CChest::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CChest* CChest::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CChest* pInstance = new CChest(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CChest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CChest::Clone(void* pArg)
{
	CChest* pInstance = new CChest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CChest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChest::Free()
{
	__super::Free();

	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pVIBufferCom);
}
