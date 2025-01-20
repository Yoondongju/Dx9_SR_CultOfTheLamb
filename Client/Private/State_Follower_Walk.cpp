#include "State_Follower_Walk.h"
#include "GameInstance.h"
#include "Follower.h"
#include "Layer.h"
#include "InteractObject.h"

#include "Terrain.h"

CState_Follower_Walk::CState_Follower_Walk(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_Follower_Walk::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;
	return S_OK;
}

HRESULT CState_Follower_Walk::Start_State()
{
	m_pFollower = dynamic_cast<CFollower*>(m_pFsm->Get_Owner());
	m_pFollower->Change_Animation(TEXT("WALK"));

	m_pInteract_Object_Layer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Object"));
	if (m_pInteract_Object_Layer == nullptr)
		return E_FAIL;

	return S_OK;
}

void CState_Follower_Walk::Update(_float fTimeDelta)
{
	if (m_pFollower->Get_IsAxe())
	{
		if (m_pFollower->Get_Interactive_Object() == nullptr || m_pFollower->Get_Interactive_Object()->Get_Interact_Type() != CInteractObject::TREE) //목표가 없거나 나무가 아닐떄 검색
			Find_Wood();
	}
	if (m_pFollower->Get_IsMining())
	{
		if (m_pFollower->Get_Interactive_Object() == nullptr || m_pFollower->Get_Interactive_Object()->Get_Interact_Type() != CInteractObject::ROCK)
			Find_Rock();
	}
	if (m_pFollower->Get_IsPray())
	{
		if (m_pFollower->Get_Interactive_Object() == nullptr || m_pFollower->Get_Interactive_Object()->Get_BindTextNum() != 0
			||m_pFollower->Get_Interactive_Object()->Get_Interact_Type() != CInteractObject::SHRINE)
			Find_SHRINE();
		
	}



	if (m_pFollower->Get_Interactive_Object() != nullptr && m_pFollower->Get_Collision_Interact_Object() == false)
		Move(fTimeDelta);

	if (m_pFollower->Get_Interactive_Object() != nullptr && m_pFollower->Get_Collision_Interact_Object() == true)
	{
		if (m_pFollower->Get_IsAxe())
			m_pFollower->Change_State(CFollower::AXE);

		if (m_pFollower->Get_IsMining())
			m_pFollower->Change_State(CFollower::MINING);

	}

	if (m_pFollower->Get_IsPray() == true)
	{
		CGameObject* pObject= dynamic_cast<CGameObject*>(m_pFollower->Get_Interactive_Object());
		if (pObject == nullptr)
			return;

		CTransform* pObjectTrans = dynamic_cast<CTransform*>(pObject->Find_Component(TEXT("Com_Transform")));
		_float3 vDirtoObject = pObjectTrans->Get_State(CTransform::STATE_POSITION) - m_pFollower->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		if(D3DXVec3Length(&vDirtoObject) < 5.f)
			m_pFollower->Change_State(CFollower::PRAY);

	}
}

void CState_Follower_Walk::End_State()
{
}

void CState_Follower_Walk::Find_Wood()
{
	m_pFollower->Get_Collision_Interact_Object(false);

	if (m_pInteract_Object_Layer == nullptr)
		return;

	list<class CGameObject*>& ObjectList = m_pInteract_Object_Layer->Get_ObjectList();

	if (ObjectList.size() > 0)
	{
		for (auto& iter = ObjectList.begin(); iter != ObjectList.end(); iter++)
		{
			CInteractObject* _object = static_cast<CInteractObject*>(*iter);
			if (_object == nullptr)
				return;

			CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), 0));

			// 지형 안에 없으면 넘겨라
			if (!pTerrain->Check_InTerrain(_object->Get_Position()) || _object->Get_LandType() != CLandObject::INTERACT)
				continue;

			if (_object->Get_Interact_Type() == CInteractObject::TREE)
			{
				m_pFollower->Set_Interactive_Object(_object);
				return;
			}
		}
	}

	//찾아보고 나무가 없으면 IDLE로
	if (m_pFollower->Get_Interactive_Object() == nullptr)
		m_pFollower->Change_State(CFollower::IDLE);
}

void CState_Follower_Walk::Find_Rock()
{
	m_pFollower->Get_Collision_Interact_Object(false);

	if (m_pInteract_Object_Layer == nullptr)
		return;

	list<CGameObject*> ObjectList = m_pInteract_Object_Layer->Get_ObjectList();

	if (ObjectList.size() > 0)
	{
		for (auto& iter = ObjectList.begin(); iter != ObjectList.end(); iter++)
		{
			CInteractObject* _object = static_cast<CInteractObject*>(*iter);
			if (_object == nullptr)
				return;

			CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Terrain"), 0));

			// 지형 안에 없으면 넘겨라
			if (!pTerrain->Check_InTerrain(_object->Get_Position()) || _object->Get_LandType() != CLandObject::INTERACT)
				continue;

			if (_object->Get_Interact_Type() == CInteractObject::ROCK)
			{
				m_pFollower->Set_Interactive_Object(_object);
				return;
			}
		}
	}

	if (m_pFollower->Get_Interactive_Object() == nullptr)
		m_pFollower->Change_State(CFollower::IDLE);
}

void CState_Follower_Walk::Find_SHRINE()
{
	m_pFollower->Get_Collision_Interact_Object(false);
	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_BuildingObject"));
	if (pLayer == nullptr)
	{
		m_pFollower->Change_State(CFollower::IDLE);
		m_pFollower->Set_Interactive_Object(nullptr);
		return;
	}

	list<class CGameObject*>& ObjectList = pLayer->Get_ObjectList();
	for (auto& iter = ObjectList.begin(); iter != ObjectList.end(); iter++)
	{
		CInteractObject* _object = static_cast<CInteractObject*>(*iter);
		if (_object == nullptr)
			return;

		if (_object->Get_BindTextNum() == 0 && _object->Get_Interact_Type() == CLandObject::INTERACTTYPE_END)
		{
			m_pFollower->Set_Interactive_Object(_object);
			return;
		}
		else
			m_pFollower->Change_State(CFollower::IDLE);
	}

	if (m_pFollower->Get_Interactive_Object() == nullptr)
		m_pFollower->Change_State(CFollower::IDLE);
}
 

void CState_Follower_Walk::Move(_float fTimeDelta)
{
	CTransform* pObjectTrans = dynamic_cast<CTransform*>(m_pFollower->Get_Interactive_Object()->Find_Component(TEXT("Com_Transform")));
	if (pObjectTrans == nullptr)
		return;

	_float3 vDirtoObject = pObjectTrans->Get_State(CTransform::STATE_POSITION) - m_pFollower->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	D3DXVec3Normalize(&vDirtoObject, &vDirtoObject);
	m_pFollower->Get_Transform()->Move_Dir(fTimeDelta, vDirtoObject);

	if (pObjectTrans->Get_State(CTransform::STATE_POSITION).x > m_pFollower->Get_Transform()->Get_State(CTransform::STATE_POSITION).x)
	{
		m_pFollower->Set_isFilp(true);
	}
	else
		m_pFollower->Set_isFilp(false);
}

CState_Follower_Walk* CState_Follower_Walk::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_Follower_Walk* pInstance = new CState_Follower_Walk(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Follower_Walk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Follower_Walk::Free()
{
	__super::Free();
}
