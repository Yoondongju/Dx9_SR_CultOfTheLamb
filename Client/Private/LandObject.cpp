
#include "..\Public\LandObject.h"

#include "GameInstance.h"
#include "Level.h"

CLandObject::CLandObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBlendObject{ pGraphic_Device }
{
}

CLandObject::CLandObject(const CLandObject & Prototype)
	: CBlendObject{ Prototype }
{
}

HRESULT CLandObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLandObject::Initialize(void * pArg)
{
	LANDOBJECT_DESC*		pDesc = static_cast<LANDOBJECT_DESC*>(pArg);

	m_pTerrainVIBuffer = pDesc->pTerrainVIBuffer;
	Safe_AddRef(m_pTerrainVIBuffer);

	m_pTerrainTransform = pDesc->pTerrainTransform;
	Safe_AddRef(m_pTerrainTransform);

	m_iLevelIndex = pDesc->iLevelIndex;
	m_iStageIndex = pDesc->iStageIndex;

	return S_OK;
}

void CLandObject::Priority_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;
}

void CLandObject::Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;
}

void CLandObject::Late_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;
}

HRESULT CLandObject::Render()
{
	return S_OK;
}

HRESULT CLandObject::SetUp_OnTerrain(CTransform * pTransform, _float fOffsetY)
{
	/* 지형을 타고하는 객체의 월드 위치를 얻어온다. */
	_float3		vWorldPos = pTransform->Get_State(CTransform::STATE_POSITION);

	/* 지형 정점과 비교 시, 로컬스페이스에서 비교할 요량이긷매ㅜㄴ에 객체의 위치를 지형버퍼의 로컬로 변환시키자. */
	/* 객체의 월드위치 * 지형모델을 그리는 객체의 월드 역행렬 */
	_float4x4		WorldMatrixInverse = m_pTerrainTransform->Get_WorldMatrix_Inverse();
	_float4x4		WorldMatrix =		 m_pTerrainTransform->Get_WorldMatrix();

	_float3		vLocalPos{};
	D3DXVec3TransformCoord(&vLocalPos, &vWorldPos, &WorldMatrixInverse);

	_float		fHeight = m_pTerrainVIBuffer->Compute_Height(vLocalPos);

	vLocalPos.y = fHeight + (fOffsetY * pTransform->Get_Scaled().y);

	/* 지형의 로컬 스페이스 상에 갱신된 로컬위치를 구한것이기때문에 */
	/* 이를 다시 월드로 보내기(지형로컬의 데이터를 -> 월드 )위해서는 월드행렬을 곱해야한다. */
	D3DXVec3TransformCoord(&vWorldPos, &vLocalPos, &WorldMatrix);

	pTransform->Set_State(CTransform::STATE_POSITION, vWorldPos);

	return S_OK;
}

_float CLandObject::Get_TerrainHeight(CTransform* pTransform, _float fOffsetY)
{
	/* 지형을 타고하는 객체의 월드 위치를 얻어온다. */
	_float3		vWorldPos = pTransform->Get_State(CTransform::STATE_POSITION);

	/* 지형 정점과 비교 시, 로컬스페이스에서 비교할 요량이긷매ㅜㄴ에 객체의 위치를 지형버퍼의 로컬로 변환시키자. */
	/* 객체의 월드위치 * 지형모델을 그리는 객체의 월드 역행렬 */
	_float4x4		WorldMatrixInverse = m_pTerrainTransform->Get_WorldMatrix_Inverse();
	_float4x4		WorldMatrix = m_pTerrainTransform->Get_WorldMatrix();

	_float3		vLocalPos{};
	D3DXVec3TransformCoord(&vLocalPos, &vWorldPos, &WorldMatrixInverse);

	_float		fHeight = m_pTerrainVIBuffer->Compute_Height(vLocalPos);

	vLocalPos.y = fHeight + (fOffsetY * pTransform->Get_Scaled().y);

	/* 지형의 로컬 스페이스 상에 갱신된 로컬위치를 구한것이기때문에 */
	/* 이를 다시 월드로 보내기(지형로컬의 데이터를 -> 월드 )위해서는 월드행렬을 곱해야한다. */
	D3DXVec3TransformCoord(&vWorldPos, &vLocalPos, &WorldMatrix);

	return vWorldPos.y;
}

void CLandObject::Chagne_Terrain(CVIBuffer_Terrain* pTerrainVIBuffer, CTransform* pTerrainTransform)
{
	Safe_Release(m_pTerrainVIBuffer);
	Safe_Release(m_pTerrainTransform);

	m_pTerrainVIBuffer = pTerrainVIBuffer;
	m_pTerrainTransform = pTerrainTransform;
}

void CLandObject::Free()
{
	__super::Free();

	Safe_Release(m_pTerrainVIBuffer);
	Safe_Release(m_pTerrainTransform);
}
