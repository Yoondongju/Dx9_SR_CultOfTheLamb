#include "Collider_Manager.h"

#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"

CCollider_Manager::CCollider_Manager()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
}

HRESULT CCollider_Manager::Initialize()
{
    return S_OK;
}

void CCollider_Manager::Collision_Cube(_wstring strLayerTag1, _wstring strLayerTag2, _uint iLevelIndex1, _uint iLevelIndex2)
{
	list<CGameObject*> GameObject1 = m_pGameInstance->Find_Layer(iLevelIndex1, strLayerTag1)->Get_ObjectList();
 	list<CGameObject*> GameObject2 = m_pGameInstance->Find_Layer(iLevelIndex2, strLayerTag2)->Get_ObjectList();

	//gameobject1이 비교하는 주체
	for (auto& pGameObject1 : GameObject1)
	{
		for (auto& pGameObject2 : GameObject2)
		{
			CCollider_Cube* pCollider1 = dynamic_cast<CCollider_Cube*>(pGameObject1->Find_Component(COLLIDER_CUBE));
			CCollider_Cube* pCollider2 = dynamic_cast<CCollider_Cube*>(pGameObject2->Find_Component(COLLIDER_CUBE));

			// 콜라이더가 없으면 넘겨
			if (nullptr == pCollider1 || nullptr == pCollider2)
				continue;

			// 현재 스테이지 없으면 넘겨
			if (pGameObject1->Get_StageIndex() != m_pGameInstance->Get_StageIndex()
				|| pGameObject2->Get_StageIndex() != m_pGameInstance->Get_StageIndex())
				continue;

			//만약 충돌체크가 꺼져있으면
			if (!pCollider1->Get_IsActive() || !pCollider2->Get_IsActive())
			{
				//충돌 체크가 꺼져있는데 목록에 들어있네?
				if (pCollider1->GetCollisionSet()->find(pGameObject2) != pCollider1->GetCollisionSet()->end())
				{
					pGameObject1->OnCollisionExit(pGameObject2);
					pGameObject2->OnCollisionExit(pGameObject1);
					pCollider1->GetCollisionSet()->erase(pGameObject2);
					pCollider2->GetCollisionSet()->erase(pGameObject1);
				}
				
				continue;
			}

			//충돌했다

			if (pCollider1->IsCollision(pCollider2) || pCollider2->IsCollision(pCollider1))
			{
				//충돌했는데 충돌 목록에 없다
				//막 충돌한상태다
				if (pCollider1->GetCollisionSet()->find(pGameObject2) == pCollider1->GetCollisionSet()->end())
				{
					pGameObject1->OnCollisionEnter(pGameObject2);
					pGameObject2->OnCollisionEnter(pGameObject1);
					pCollider1->GetCollisionSet()->insert(pGameObject2);
					pCollider2->GetCollisionSet()->insert(pGameObject1);
				}
				//충돌했는데 충돌 목록에 있다
				//충돌중이다
				else if (pCollider1->GetCollisionSet()->find(pGameObject2) != pCollider1->GetCollisionSet()->end())
				{
					pGameObject1->OnCollisionStay(pGameObject2);
					pGameObject2->OnCollisionStay(pGameObject1);
				}
			}
			//충돌을 안 했어
			else 
			{
				//근데 목록에 있네
				if (pCollider1->GetCollisionSet()->find(pGameObject2) != pCollider1->GetCollisionSet()->end())
				{
					pGameObject1->OnCollisionExit(pGameObject2);
					pGameObject2->OnCollisionExit(pGameObject1);
					pCollider1->GetCollisionSet()->erase(pGameObject2);
					pCollider2->GetCollisionSet()->erase(pGameObject1);
				}
			}
		}
	}

}

CCollider_Manager* CCollider_Manager::Create()
{
	CCollider_Manager* pInstance = new CCollider_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CCollider_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Manager::Free()
{
	__super::Free();
}
