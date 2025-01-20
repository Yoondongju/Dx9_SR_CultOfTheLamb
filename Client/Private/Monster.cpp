
#include "Monster.h"
#include "Player.h"
#include "MonsterBone.h"

#include "GameInstance.h"

#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Collider_Cube.h"
#include "Animator.h"
#include "Fsm.h"
#include "RigidBody.h"

#include "MonsterHpUI.h"

_uint CMonster::m_iPreLevel = {};

_uint CMonster::m_iCreateCount[LEVELID::LEVEL_END] = { 0 };
_uint CMonster::m_iDeadCount[LEVELID::LEVEL_END] = { 0 };

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CLandObject{ pGraphic_Device }
{
	
}

CMonster::CMonster(const CMonster& Prototype)
	:CLandObject{ Prototype }
{
	
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{		
		MONSTER_INFO* MonsterInfo = reinterpret_cast<MONSTER_INFO*>(pArg);

		__super::Initialize(&MonsterInfo->tLandObjectDesc);

		m_tMonsterInfo.iHp = MonsterInfo->iHp;
		m_tMonsterInfo.iMaxHp = m_tMonsterInfo.iHp;

		m_tMonsterInfo.iAttackPower = MonsterInfo->iAttackPower;

		if (nullptr != MonsterInfo->vInitPosition)
			m_vInitPosition = MonsterInfo->vInitPosition;
		

		CUI::UI_DESC Desc = { };
		Desc.m_vPos = { 0.f,0.f };
		Desc.m_vSize = { 4.f,0.5f };
		Desc.m_vOffset = { 0.f,2.f };			//  << y offset 임시 조정
		Desc.m_iBindNum = 0;
		Desc.eUItype = UI_TYPE::UI_BAR;
		Desc.pReferenceOwner = this;

		if (FAILED(m_pGameInstance->Event_CreateObject(MonsterInfo->tLandObjectDesc.iLevelIndex, TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonsterHpUI"),sizeof(CUI::UI_DESC), &Desc)))
			MSG_BOX(L"Can't Create Prototype_GameObject_MonsterHpUI");


		if (m_iPreLevel != MonsterInfo->tLandObjectDesc.iLevelIndex)
		{
			m_iCreateCount[m_iPreLevel] = 0;
			m_iDeadCount[m_iPreLevel] = 0;
		}
	
		m_iCreateCount[MonsterInfo->tLandObjectDesc.iLevelIndex]++;

		m_iPreLevel = MonsterInfo->tLandObjectDesc.iLevelIndex;
	}



	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
}

void CMonster::Update(_float fTimeDelta)
{
}

void CMonster::Late_Update(_float fTimeDelta)
{
}

HRESULT CMonster::Render()
{
	return S_OK;
}

void CMonster::Set_InitState(_uint eState, void* pArg)
{
	m_pFsmCom->Set_State(eState, pArg);
}

void CMonster::Change_State(_uint eState, void* pArg)
{
	m_pFsmCom->Change_State(eState, pArg);
}

void CMonster::Change_Animation(const _wstring& strAnimationTag, _uint iStartFrame)
{
	m_pAnimatorCom->Change_Animation(strAnimationTag, iStartFrame);
}

bool CMonster::IsEnd_CurrentAnimation()
{
	return m_pAnimatorCom->IsEnd_CurrentAnimation();
}

_uint CMonster::Get_CurrentFrame()
{
	return m_pAnimatorCom->Get_CurrentFrame(); 
}

void CMonster::Calculate_Flip()
{
	if (m_pGameInstance->Find_Player()->Get_Position().x > m_pTransformCom->Get_State(CTransform::STATE_POSITION).x)
		m_isFilp = true;
	else
		m_isFilp = false;
}

_float3 CMonster::Calculate_Dir_To_Player()
{
	_float3 vChaseDir = m_pGameInstance->Find_Player()->Get_Position() - Get_Position();
	//vChaseDir.y = Get_Position();
	D3DXVec3Normalize(&vChaseDir, &vChaseDir);

	return vChaseDir;
}

_float CMonster::Follow_Player(_float fTimeDelta, _float fSpeedPerSec)
{
	_float3 vDir = Calculate_Dir_To_Player();

	m_pTransformCom->Move_Dir(fTimeDelta, vDir, fSpeedPerSec);

	return D3DXVec3Length(&(m_pGameInstance->Find_Player()->Get_Position() - Get_Position()));
}

void CMonster::KncokBack(_float fTimeDelta, _float3 vDir)
{
	if (m_isKnockBack)
	{
		if (nullptr != m_pRigidBodyCom)
		{
			m_pRigidBodyCom->Add_Force_Direction(_float3(vDir.x, 1.f, vDir.z), _float3(25.f, 0.f, 25.f), CRigidBody::FORCE); // Shoot!
		}

		m_isKnockBack = false;
	}
}

void CMonster::Create_Bone(_uint iBoneType, _float3 vFlyDir)
{
	CLandObject::LANDOBJECT_DESC LandDesc = {};

	LandDesc.pTerrainTransform = m_pTerrainTransform;
	LandDesc.pTerrainVIBuffer = m_pTerrainVIBuffer;
	LandDesc.iLevelIndex = m_iLevelIndex;
	LandDesc.iStageIndex = m_iStageIndex;

	CMonsterBone::BONE_DESC BoneDesc = {};
	BoneDesc.eBoneType = CMonsterBone::BONE_TYPE(iBoneType);
	BoneDesc.vFlyDir = vFlyDir;
	BoneDesc.vInitPos = Get_Position();
	BoneDesc.tLandDesc = LandDesc;

	m_pGameInstance->Event_CreateObject(m_iLevelIndex, TEXT("Layer_MonsterBone"), TEXT("Prototype_GameObject_MonsterBone"), sizeof(CMonsterBone::BONE_DESC), &BoneDesc);
}

void CMonster::Free()
{
	__super::Free();


	switch (m_pGameInstance->Get_LevelIndex())
	{
	case LEVELID::LEVEL_DUNGEON0:

		break;
	case LEVELID::LEVEL_DUNGEON1:

		break;
	case LEVELID::LEVEL_DUNGEON2:

		break;
	case LEVELID::LEVEL_DUNGEON3:

		break;
	default:
		break;
	}
	
		
	
	
	++m_iDeadCount[m_pGameInstance->Get_LevelIndex()];



	//일단 임시로 이렇게 해둠 아이디어 있으면 반영좀
	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);

	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pVIBufferCom);
}

