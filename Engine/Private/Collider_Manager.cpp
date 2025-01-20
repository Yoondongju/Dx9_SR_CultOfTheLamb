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

	//gameobject1�� ���ϴ� ��ü
	for (auto& pGameObject1 : GameObject1)
	{
		for (auto& pGameObject2 : GameObject2)
		{
			CCollider_Cube* pCollider1 = dynamic_cast<CCollider_Cube*>(pGameObject1->Find_Component(COLLIDER_CUBE));
			CCollider_Cube* pCollider2 = dynamic_cast<CCollider_Cube*>(pGameObject2->Find_Component(COLLIDER_CUBE));

			// �ݶ��̴��� ������ �Ѱ�
			if (nullptr == pCollider1 || nullptr == pCollider2)
				continue;

			// ���� �������� ������ �Ѱ�
			if (pGameObject1->Get_StageIndex() != m_pGameInstance->Get_StageIndex()
				|| pGameObject2->Get_StageIndex() != m_pGameInstance->Get_StageIndex())
				continue;

			//���� �浹üũ�� ����������
			if (!pCollider1->Get_IsActive() || !pCollider2->Get_IsActive())
			{
				//�浹 üũ�� �����ִµ� ��Ͽ� ����ֳ�?
				if (pCollider1->GetCollisionSet()->find(pGameObject2) != pCollider1->GetCollisionSet()->end())
				{
					pGameObject1->OnCollisionExit(pGameObject2);
					pGameObject2->OnCollisionExit(pGameObject1);
					pCollider1->GetCollisionSet()->erase(pGameObject2);
					pCollider2->GetCollisionSet()->erase(pGameObject1);
				}
				
				continue;
			}

			//�浹�ߴ�

			if (pCollider1->IsCollision(pCollider2) || pCollider2->IsCollision(pCollider1))
			{
				//�浹�ߴµ� �浹 ��Ͽ� ����
				//�� �浹�ѻ��´�
				if (pCollider1->GetCollisionSet()->find(pGameObject2) == pCollider1->GetCollisionSet()->end())
				{
					pGameObject1->OnCollisionEnter(pGameObject2);
					pGameObject2->OnCollisionEnter(pGameObject1);
					pCollider1->GetCollisionSet()->insert(pGameObject2);
					pCollider2->GetCollisionSet()->insert(pGameObject1);
				}
				//�浹�ߴµ� �浹 ��Ͽ� �ִ�
				//�浹���̴�
				else if (pCollider1->GetCollisionSet()->find(pGameObject2) != pCollider1->GetCollisionSet()->end())
				{
					pGameObject1->OnCollisionStay(pGameObject2);
					pGameObject2->OnCollisionStay(pGameObject1);
				}
			}
			//�浹�� �� �߾�
			else 
			{
				//�ٵ� ��Ͽ� �ֳ�
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
