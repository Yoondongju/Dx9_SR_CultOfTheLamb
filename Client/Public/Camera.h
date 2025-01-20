#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "GameInstance.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CCamera final : public CGameObject
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		_float3			vEye;
		_float3			vAt;
		_float			fFovy;
		_float			fNear;
		_float			fFar;
	}CAMERA_DESC;


private:
	CCamera(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;


public:
	const _float&		Get_Fovy() { return m_fFovy; }			
	const _float&		Get_Near() { return m_fNear; }
	const _float&		Get_Far() { return m_fFar;}

	_bool Get_IsEnding() { return m_isEnding; }
	void Set_IsEnding(_bool isEnding) { m_isEnding = isEnding; }

	_bool Get_IsShowCrew() { return m_isShowCrew; }
	void Set_IsShowCrew(_bool isCrew) { m_isShowCrew = isCrew; }

	void				Set_NULL_PlayerTransform() { m_pPlayerTransform = nullptr; }  // 이거 06-14금 검사받고 지우삼 이렇게 처리하면안댐 Building_Object


	bool		IsFinishZoomIn()
	{
		if (fabs(m_fFovy - m_fTargetFovy) < 2.f)			// << 얼마나 줌인했는지에 따라 달라져요
			return true;
		else
			return false;
	}


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT			InitCamera_Desc(CAMERA_DESC* pDesc);		// 레벨변화시 카메라 세팅 변경하고싶을대


	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void	Zoom_In(_float fZoom);					//얼만큼 확대할지(배율)
	void	Zoom_Out(_float fZoom);					//얼만큼 축소할지(배율)
	void	Target_to_Player();						//플레이어 따라다님
	void	Target_to(CGameObject* pGameObject, _float3 _offset = {0.f,0.f,0.f}, _float _fSpeed = {30.f});	//카메라가 중심으로 할 오브젝트
	void	Set_Shaking(_float fAngle, _float fSpeed,_int fShakeCount);	//흔들 각도, 흔드는 속도 (클수록 느림), 몇번 흔들지
	void	Start_FadeIn(_float fFadeTime = 3.f);												//Fade in (어두워짐)
	void	Start_FadeOut(_float fFadeTime = 3.f);											//Fade Out(밝아짐)
	void	Start_Blink();
	void    Set_Position_to_Player();

	_bool Get_IsFadeIn();
	_bool Get_IsFadeOut();
	_float  Get_FadeAlpha();											//FadeIn UI의 알파값을 가져오는 함수

private:
	CTransform*				m_pPlayerTransform = { nullptr };
	CTransform*				m_pTargetTransform = { nullptr };
	class CFadeInOut*		m_pFadeInOut = nullptr;

	_bool					m_bZoomIn = { false };
	_bool					m_bZoomOut = { false };
	_bool					m_LookAtPlayer = { true };
	_bool					m_bShake = { false };
	//_bool					m_bTargetChange = { false };
	_bool					m_bisLookingPlayer = { false }; //현재 카메라가 플레이어 위치에 있는지 확인 용
	_bool					m_bMoveStop = { false };

	_bool					m_isEnding = { false };
	_bool					m_isShowCrew = { false };


	_float					m_fTargetFovy = {};
	_float					m_fFovy = {};
	_float					m_fNear = {};
	_float					m_fFar = {};
	_float					m_fOriginSpeed = {};

	_float					m_fMaxRotateAngle = {};
	_float					m_fMinRotateAngle = {};
	_float					m_fCurRotateAngle = {};
	_float					m_fShakeSpeed = {};		//클수록 느려짐
	_float					m_fChangeSpeed = {};	
	_int					m_iMaxShakeCount = {};
	_int					m_iCurShakeCount = {};
	_int					m_iShakeLevel = {};

	_float3					m_fDistance = {};
	_float3					m_fTargetOffset = {};
	//_float3					vDirtoObject = {};

	POINT					m_ptOldMousePos = {};

private:
	HRESULT Ready_Components(void* pArg);
	void	Active_Shaking();
	void	Active_MoveToTarget(_float _fTimeDelta);
	void	Follow_Player(_float _fTimeDelta);

	


public:
	static CCamera* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END