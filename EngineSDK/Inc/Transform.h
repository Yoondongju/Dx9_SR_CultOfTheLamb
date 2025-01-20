#pragma once

#include "Component.h"

/* 객체의 상태(월드스페이스 상의)를 가지고 있는다. */
/* 상태 : 크기, 회전, 위치 -> WorldMatrix */
/* WorldMatrix를 통해서 월드 공간안에서의 다양한 기능을 구현한다. */
/* WorldMatrix -> 로컬스페이스에 존재하는 정점의 위치를 월드 스페이스 변환하낟. */
/* -> 월드행렬의 각 행(Right, Up, Look, Position)이 월드스페이스에서의 객체정보를 의미한다. */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct
	{
		_float		fSpeedPerSec{};
		_float		fRotationPerSec{};
	}TRANSFORM_DESC;

private:
	CTransform(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTransform(const CTransform& Prototype);
	virtual ~CTransform() = default;


public:
	const _float4x4& Get_CameraProjMatrix() { return m_Camera_ProjMatrix; }
	void Set_CameraProjMatrix(const _float4x4& CamProjMat) { m_Camera_ProjMatrix = CamProjMat; }


public:
	_float2 Get_ViewportPos();

	_float3 Get_State(STATE eState) {
		return *(_float3*)&m_WorldMatrix.m[eState][0];
	}

	_float3 Get_Scaled() {
		return _float3(D3DXVec3Length(&Get_State(STATE_RIGHT)),
			D3DXVec3Length(&Get_State(STATE_UP)),
			D3DXVec3Length(&Get_State(STATE_LOOK)));
	}

	_float4x4	Get_WorldMatrix() const {
		return m_WorldMatrix;
	}

	_float4x4 Get_WorldMatrix_Inverse() {
		_float4x4	WorldMatrixInverse;
		return *D3DXMatrixInverse(&WorldMatrixInverse, nullptr, &m_WorldMatrix);
	}

	void Set_State(STATE eState, const _float3& vState) {
		memcpy(&m_WorldMatrix.m[eState][0], &vState, sizeof(_float3));
	}

	_float Get_SpeedPerSec() { return m_fSpeedPerSec; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_Transform();

public:
	void Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);
	void Scaling(_float3 vScale);




	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);


	void Go_Straight_BuildingObj();
	void Go_Backward_BuildingObj();
	void Go_Right_BuildingObj();
	void Go_Left_BuildingObj();



	void Turn(const _float3& vAxis, _float fTimeDelta);
	void ZTurn(const _float3& vAxis, _float m_fRotationAngle);
	void Rotation(const _float3& vAxis, _float fRadian);
	void LookAt(const _float3& vAt);

	void Change_SpeedPerSec(_float& fSpeedPerSec);
	void Change_RotationPerSec(_float& fRotationPerSec);
	void Move_Dir(_float fTimeDelta, _float3 vDir, _float fSpeedPerSec = 0.f);
	void Move_DirY(_float fTimeDelta, _float3 vDir, _float fSpeedPerSec = 0.f);
	void Move_Dgree(_float fTimeDelta, _float fDgree, _float fSpeedPerSec = 0.f);
	

	

private:
	_float4x4					m_WorldMatrix = {};

	static _float4x4			m_Camera_ProjMatrix;

	_float						m_fSpeedPerSec = {};
	_float						m_fRotationPerSec = {};

public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END