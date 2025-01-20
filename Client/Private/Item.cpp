
#include "Item.h"
#include "Player.h"

#include "GameInstance.h"

#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Collider_Cube.h"
#include "Animator.h"
#include "Fsm.h"

#include "ItemGetUI.h"

#include "Layer.h"


_uint	CItem::m_iCreateCount[LEVELID::LEVEL_END][ITEM_ID::ITEM_END]{0};
_uint   CItem::m_iPreLevel = {};


CItem::CItem(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CLandObject{ pGraphic_Device }
{
	srand(unsigned int(time(NULL)));
}

CItem::CItem(const CItem& Prototype)
	:CLandObject{ Prototype }
{
}

HRESULT CItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		ITEM_DESC* ItemDesc = static_cast<ITEM_DESC*>(pArg);

		m_vInitPos = ItemDesc->vInitPos;
		m_eItemId = ItemDesc->eItemId;

		if (FAILED(__super::Initialize(&ItemDesc->tLandDesc)))
			return E_FAIL;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Texture()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitPos);
	m_pTransformCom->Scaling(_float3(1.f, 1.f, 1.f));

	m_pRigidBodyCom->SetOwner(this);
	m_pRigidBodyCom->Set_IsGravity(true);
	m_pRigidBodyCom->Set_GravityAccel(2.f);
	m_pRigidBodyCom->Set_Mass(2.f);
	m_pRigidBodyCom->Set_Friction(1.f);

	m_fFlyDgree = _float(rand() % 180) + 180.f;

	m_isFly = true;
	m_pRigidBodyCom->Add_Force_Direction(_float3(0.f, 1.f, 0.f), _float3(0.f, 120.f, 0.f), CRigidBody::FORCE); // Shoot!




	_uint iCurLevel = m_pGameInstance->Get_LevelIndex();

	if (m_iPreLevel != iCurLevel)
	{
		for (_uint iItemType = 0; iItemType < ITEM_ID::ITEM_END; iItemType++)
		{
			m_iCreateCount[m_iPreLevel][iItemType] = 0;
		}
	}

	m_iCreateCount[iCurLevel][m_eItemId]++;

	m_iPreLevel = iCurLevel;


	return S_OK;
}

void CItem::Priority_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

}

void CItem::Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;


	_float3 vDir_To_Player = m_pGameInstance->Find_Player()->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float vDistance_To_Player = D3DXVec3Length(&vDir_To_Player);

	D3DXVec3Normalize(&vDir_To_Player, &vDir_To_Player);

	if (Get_Position().y - 0.6f <= __super::Get_TerrainHeight(m_pTransformCom))
	{

		m_pRigidBodyCom->Set_IsGravity(false);
		m_isFly = false;

		__super::SetUp_OnTerrain(m_pTransformCom, 0.9f);
	}
	else if(m_isFly)
	{
		_float3 vScale = m_pTransformCom->Get_Scaled();
		vScale.x += fTimeDelta;
		vScale.y += fTimeDelta;

		if (vScale.x > 1.4f)
		{
			vScale.x = 1.4f;
			vScale.y = 1.4f;
		}

		m_pTransformCom->Move_Dgree(fTimeDelta, m_fFlyDgree, 6.f);
		m_pTransformCom->Scaling(vScale);
	}
	if (vDistance_To_Player < 8.f && !m_isFly)
	{
		m_pTransformCom->Move_Dir(fTimeDelta, vDir_To_Player);
	}

}

void CItem::Late_Update(_float fTimeDelta)
{
 	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	m_pRigidBodyCom->Update(fTimeDelta);

 	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CItem::Render()
{
	m_pVIBufferCom->Filp_Texture(false);

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	//디버그 모드 추가하기
	m_pColliderCom->Render_Collider();

	return S_OK;
}

void CItem::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
	{
		m_pGameInstance->Play_Sound(TEXT("Get_Item.wav"), SOUND_ITEM, g_fEffectVolume);

		_int iRandNum = 2;
		dynamic_cast<CPlayer*>(pOther)->Add_ItemCount(m_eItemId, iRandNum);

		CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_ItemGetUI"));
		_uint iUI_Count = { 0 };

		if (pLayer)
			iUI_Count = (_uint)pLayer->Get_ObjectList().size();

		CItemGetUI::ITEM_GETUI_DESC Desc = {};

		Desc.eUIDesc.m_vPos = { -100.f , 190.f + (50 * iUI_Count) };
		Desc.eUIDesc.m_vSize = { 200.f,40.f };
		Desc.eUIDesc.m_iBindNum = 0;
		Desc.eUIDesc.eUItype = UI_TYPE::UI_NONE;
		Desc.eItemId = m_eItemId;
		Desc.iAddedItem_Count = iRandNum;

		Desc.iLevelIndex = m_iLevelIndex;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_ItemGetUI"), TEXT("Prototype_GameObject_ItemGetUI"), &Desc)))
			MSG_BOX(L"Can't Create ItemGetUI");

		m_pGameInstance->Event_DestoryObject(this);
	}
}

void CItem::OnCollisionStay(CGameObject* pOther)
{

}

void CItem::OnCollisionExit(CGameObject* pOther)
{
}

_float3 CItem::Calculate_Dir_To_Player()
{
	_float3 vChaseDir = m_pGameInstance->Find_Player()->Get_Position() - Get_Position();
	//vChaseDir.y = Get_Position();
	D3DXVec3Normalize(&vChaseDir, &vChaseDir);

	return vChaseDir;
}

void CItem::Move_To_Player(_float _fTimeDelta)
{
	_float3 vDir_Player = Calculate_Dir_To_Player();

	m_pTransformCom->Move_Dir(_fTimeDelta, vDir_Player);
}

HRESULT CItem::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 25.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider_Cube */
	CCollider_Cube::COLLIDER_CUBE_INFO ColliderInfo;

	ZeroMemory(&ColliderInfo, sizeof(CCollider_Cube::COLLIDER_CUBE_INFO));
	ColliderInfo.fSizeX = 0.6f;
	ColliderInfo.fSizeY = 0.6f;
	ColliderInfo.fSizeZ = 0.6f;
	ColliderInfo.fOffSetX = 0.f;
	ColliderInfo.fOffSetY = 0.f;
	ColliderInfo.fOffSetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderInfo)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_Collider_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem::Ready_Texture()
{
	switch (m_eItemId)
	{
	case Client::CItem::COIN:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Coin"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
			return E_FAIL;
		break;

	case Client::CItem::HEART:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Heart"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
			return E_FAIL;
		break;

	case Client::CItem::ROCK:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Rock"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
			return E_FAIL;
		break;

	case Client::CItem::WOOD:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Wood"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
			return E_FAIL;
		break;
	}

	return S_OK;
}

HRESULT CItem::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

	return S_OK;
}

HRESULT CItem::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CItem* CItem::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CItem* pInstance = new CItem(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CItem::Clone(void* pArg)
{
	CItem* pInstance = new CItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem::Free()
{
	__super::Free();

	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}

