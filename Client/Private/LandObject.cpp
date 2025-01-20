
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
	/* ������ Ÿ���ϴ� ��ü�� ���� ��ġ�� ���´�. */
	_float3		vWorldPos = pTransform->Get_State(CTransform::STATE_POSITION);

	/* ���� ������ �� ��, ���ý����̽����� ���� �䷮�̱�Ť̤��� ��ü�� ��ġ�� ���������� ���÷� ��ȯ��Ű��. */
	/* ��ü�� ������ġ * �������� �׸��� ��ü�� ���� ����� */
	_float4x4		WorldMatrixInverse = m_pTerrainTransform->Get_WorldMatrix_Inverse();
	_float4x4		WorldMatrix =		 m_pTerrainTransform->Get_WorldMatrix();

	_float3		vLocalPos{};
	D3DXVec3TransformCoord(&vLocalPos, &vWorldPos, &WorldMatrixInverse);

	_float		fHeight = m_pTerrainVIBuffer->Compute_Height(vLocalPos);

	vLocalPos.y = fHeight + (fOffsetY * pTransform->Get_Scaled().y);

	/* ������ ���� �����̽� �� ���ŵ� ������ġ�� ���Ѱ��̱⶧���� */
	/* �̸� �ٽ� ����� ������(���������� �����͸� -> ���� )���ؼ��� ��������� ���ؾ��Ѵ�. */
	D3DXVec3TransformCoord(&vWorldPos, &vLocalPos, &WorldMatrix);

	pTransform->Set_State(CTransform::STATE_POSITION, vWorldPos);

	return S_OK;
}

_float CLandObject::Get_TerrainHeight(CTransform* pTransform, _float fOffsetY)
{
	/* ������ Ÿ���ϴ� ��ü�� ���� ��ġ�� ���´�. */
	_float3		vWorldPos = pTransform->Get_State(CTransform::STATE_POSITION);

	/* ���� ������ �� ��, ���ý����̽����� ���� �䷮�̱�Ť̤��� ��ü�� ��ġ�� ���������� ���÷� ��ȯ��Ű��. */
	/* ��ü�� ������ġ * �������� �׸��� ��ü�� ���� ����� */
	_float4x4		WorldMatrixInverse = m_pTerrainTransform->Get_WorldMatrix_Inverse();
	_float4x4		WorldMatrix = m_pTerrainTransform->Get_WorldMatrix();

	_float3		vLocalPos{};
	D3DXVec3TransformCoord(&vLocalPos, &vWorldPos, &WorldMatrixInverse);

	_float		fHeight = m_pTerrainVIBuffer->Compute_Height(vLocalPos);

	vLocalPos.y = fHeight + (fOffsetY * pTransform->Get_Scaled().y);

	/* ������ ���� �����̽� �� ���ŵ� ������ġ�� ���Ѱ��̱⶧���� */
	/* �̸� �ٽ� ����� ������(���������� �����͸� -> ���� )���ؼ��� ��������� ���ؾ��Ѵ�. */
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
