#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Rect;
class CFsm;
class CRigidBody;
END

BEGIN(Client)

class CMonster abstract : public CLandObject
{
public:
	typedef struct {
		_float iHp = {};
		_float iMaxHp = {};

		_float iAttackPower = {};
		_float3 vInitPosition = { 0.f,0.f,0.f };
		LANDOBJECT_DESC tLandObjectDesc = {};

	}MONSTER_INFO;

protected:
	CMonster(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;	


public:
	const MONSTER_INFO& Get_MonInfo() { return m_tMonsterInfo; }
	const MONSTER_ID Get_MonsterId() { return m_eMonsterId; }


	static _uint	Get_CurLevel_CreateMon_Count(LEVELID eCurLevel) { return m_iCreateCount[eCurLevel]; }
	static _uint	Get_CurLevel_DeadMon_Count(LEVELID eCurLevel) { return m_iDeadCount[eCurLevel]; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	void Start_KnockBack() { m_isKnockBack = true; }

public:
	void Set_InitState(_uint eState, void* pArg = nullptr);
	void Change_State(_uint eState, void* pArg = nullptr);
	void Change_Animation(const _wstring &strAnimationTag, _uint iStartFrame = 0);
	bool IsEnd_CurrentAnimation();
	_uint Get_CurrentFrame();

	void Calculate_Flip();
	_float3 Calculate_Dir_To_Player();
	_float Follow_Player(_float fTimeDelta, _float fSpeedPerSec = 0.f);

	virtual void Change_State_Hurt() = 0;

protected:
	CAnimator* m_pAnimatorCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CFsm* m_pFsmCom = { nullptr };
	CRigidBody* m_pRigidBodyCom = { nullptr };


	MONSTER_INFO		m_tMonsterInfo = {};
	_float3				m_vInitPosition = {};
	_float3				m_vKnockBackDir = { 0.f,0.f,0.f };


	_bool				m_bAttackOnce = true;
	_bool				m_bTransform = true;
	_float				m_fAttackTimer = 0.f;

	_bool				m_isKnockBack = { false };

	MONSTER_ID			m_eMonsterId = { MONSTER_END };


	static	_uint		m_iDeadCount[LEVELID::LEVEL_END];
	static  _uint		m_iCreateCount[LEVELID::LEVEL_END];
	static  _uint       m_iPreLevel;


protected:
	virtual HRESULT Ready_Components() = 0;
	virtual HRESULT Ready_Animation() = 0;
	virtual HRESULT SetUp_RenderState() = 0;
	virtual HRESULT Reset_RenderState() = 0;

	void KncokBack(_float fTimeDelta, _float3 vDir);
	void Create_Bone(_uint iBoneType, _float3 vFlyDir);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END