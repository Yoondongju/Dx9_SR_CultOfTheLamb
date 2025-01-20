#include "..\Public\Camera.h"
#include "Player.h"
#include "FadeInOut.h"

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CCamera::CCamera(const CCamera& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vEye);
	m_pTransformCom->LookAt(pDesc->vAt);
	m_fDistance = pDesc->vEye;

	m_fFovy = pDesc->fFovy;
	m_fTargetFovy = pDesc->fFovy;;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;

	CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_FadeInOut"));
	if (pGameObj != nullptr)
	{
		CFadeInOut* pChild = dynamic_cast<CFadeInOut*>(pGameObj->Clone(pArg));
		if (pChild != nullptr)
			m_pFadeInOut = pChild;
	}

	GetCursorPos(&m_ptOldMousePos);


	return S_OK;
}

HRESULT CCamera::InitCamera_Desc(CAMERA_DESC* pDesc)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vEye);
	m_pTransformCom->LookAt(pDesc->vAt);
	m_pTransformCom->Change_SpeedPerSec(pDesc->fSpeedPerSec);
	m_fDistance = pDesc->vEye;
	m_fOriginSpeed = pDesc->fSpeedPerSec;
	m_fFovy = pDesc->fFovy;
	m_fTargetFovy = pDesc->fFovy;;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;

	return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
	/* 기본적인 제어*/
	/*if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD)
		m_pTransformCom->Go_Straight(fTimeDelta);
	if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD)
		m_pTransformCom->Go_Backward(fTimeDelta);*/

	//if (m_pGameInstance->Get_KeyState(KEY::Q) == KEY_STATE::TAP)
	//	Start_FadeIn();
	//if (m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP)
	//	Start_FadeOut();


	if (m_LookAtPlayer && m_bisLookingPlayer == false)
		Follow_Player(fTimeDelta);
	else if(m_LookAtPlayer && m_bisLookingPlayer == true)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) + m_fDistance);
	else
		Active_MoveToTarget(fTimeDelta);

	//Zoom In, Zoom Out 코드
	if (m_bZoomIn && m_fFovy > m_fTargetFovy)
	{
		m_fFovy -= 0.05f;
		if (m_fFovy < 0)
			m_fFovy = 0.01f;
	}
	else
	m_bZoomIn = false;
	
	if (m_bZoomOut && m_fFovy < m_fTargetFovy)
		m_fFovy += 0.05f;
	else
	m_bZoomOut = false;
	
	if (m_bShake)
		Active_Shaking();

	POINT ptMouse{};
	GetCursorPos(&ptMouse);



/* 장치에 뷰랑 투영행렬을 한번 셋팅 해주면 다시 갱신해 주기 전까지 모든 객체들을 렌더링시에 셋팅해준 행렬을 곱하면서 그리게 된다. */
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_pTransformCom->Get_WorldMatrix_Inverse());
	
	_float4x4 viewMat;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &viewMat);
	
	
	_float4x4		ProjMatrix{};
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, D3DXMatrixPerspectiveFovLH(&ProjMatrix, m_fFovy, (_float)g_iWinSizeX / g_iWinSizeY, m_fNear, m_fFar));	
	m_pTransformCom->Set_CameraProjMatrix(ProjMatrix);
	

	m_ptOldMousePos = ptMouse;

	if(m_pFadeInOut != nullptr)
		m_pFadeInOut->Priority_Update(fTimeDelta);
}

void CCamera::Update(_float fTimeDelta)
{
	if (m_pFadeInOut != nullptr)
		m_pFadeInOut->Update(fTimeDelta);

}

void CCamera::Late_Update(_float fTimeDelta)
{
	if (m_pFadeInOut != nullptr)
		m_pFadeInOut->Late_Update(fTimeDelta);
}

HRESULT CCamera::Render()
{
	if (m_pFadeInOut != nullptr)
		m_pFadeInOut->Render();

	return S_OK;
}

void CCamera::Zoom_In(_float fZoom)
{
	m_bZoomIn = true;
	if(dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player()) != nullptr)
	{
		dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Player_State(CPlayer::CAMERA_ON);
		dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_Animation(TEXT("IDLE"));
	}
	m_fTargetFovy = m_fFovy / fZoom;
}

void CCamera::Zoom_Out(_float fZoom)
{
	m_bZoomOut = true;
	if (m_fFovy * fZoom <= D3DXToRadian(180.f))
		m_fTargetFovy = m_fFovy * fZoom;
	if (dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player()) != nullptr)
	{
		if(dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_Player_State() != CPlayer::GET_SWORD)
			dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::IDLE);
	}
}

void CCamera::Target_to_Player()
{
	m_LookAtPlayer = true;
	m_bisLookingPlayer = false;
}

void CCamera::Target_to(CGameObject* pGameObject, _float3 _offset, _float _fSpeed)
{
	m_LookAtPlayer = false;
	m_pTargetTransform = dynamic_cast<CTransform*>(pGameObject->Find_Component(TEXT("Com_Transform")));
	m_fTargetOffset = _offset;
	m_fChangeSpeed = _fSpeed;
}

void CCamera::Set_Shaking(_float fAngle, _float fSpeed, _int fShakeCount)
{
	if(m_bShake == false)
	{
		m_bShake = true;
		m_fMaxRotateAngle = D3DXToRadian(fAngle);
		m_fMinRotateAngle = D3DXToRadian(fAngle * -1.f);
		m_iMaxShakeCount = fShakeCount;
		m_fShakeSpeed = fSpeed;
		m_iCurShakeCount = 0;
		m_iShakeLevel = 0;
		m_fCurRotateAngle = 0.f;
	}
}

void CCamera::Start_FadeIn(_float fFadeTime)
{
	if (m_pFadeInOut != nullptr)
		m_pFadeInOut->Start_FadeIn(fFadeTime);
}

void CCamera::Start_FadeOut(_float fFadeTime)
{
	if (m_pFadeInOut != nullptr)
		m_pFadeInOut->Start_FadeOut(fFadeTime);
}

void CCamera::Start_Blink()
{
	if (m_pFadeInOut != nullptr)
		m_pFadeInOut->Start_Blink();
}

void CCamera::Set_Position_to_Player()
{

	if (m_pPlayerTransform == nullptr)
	{
		CPlayer* m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player());
		if (m_pPlayer)
		{
			m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")));
		}
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) + m_fDistance);
	m_bisLookingPlayer = true;
}

_bool CCamera::Get_IsFadeIn()
{
	if(m_pFadeInOut)
		return m_pFadeInOut->Get_IsFadeIn();
	return false;
}

_bool CCamera::Get_IsFadeOut()
{
	if(m_pFadeInOut)
		return m_pFadeInOut->Get_IsFadeIn();
	return false;
}

_float CCamera::Get_FadeAlpha()
{
	if (m_pFadeInOut != nullptr)
		return m_pFadeInOut->Get_FadeAlpha();

	return 0.f;
}

void CCamera::Active_Shaking()
{
	_float fShakePower = m_fMaxRotateAngle / m_fShakeSpeed;
	if (m_iCurShakeCount < m_iMaxShakeCount)
	{
		if (m_iShakeLevel == 0)
		{
			m_fCurRotateAngle += fShakePower;
			m_pTransformCom->ZTurn(_float3(0.f, 0.f, 1.f), fShakePower);
			if (m_fCurRotateAngle > m_fMaxRotateAngle)
			{
				++m_iShakeLevel;
			}
		}
		else if (m_iShakeLevel == 1)
		{
			m_fCurRotateAngle -= fShakePower;
			m_pTransformCom->ZTurn(_float3(0.f, 0.f, 1.f), -fShakePower);
			if (m_fCurRotateAngle < m_fMinRotateAngle)
			{
				++m_iShakeLevel;
			}
		}
		else if (m_iShakeLevel == 2)
		{
			m_fCurRotateAngle += fShakePower;
			m_pTransformCom->ZTurn(_float3(0.f, 0.f, 1.f), fShakePower);
		
			if (m_fCurRotateAngle >= 0)
			{
				m_iShakeLevel = 0;
				++m_iCurShakeCount;
				m_fCurRotateAngle = 0;
				m_pTransformCom->ZTurn(_float3(0.f, 0.f, 1.f), m_fCurRotateAngle);
			}
		}
	}
	else
		m_bShake = false;
}

void CCamera::Active_MoveToTarget(_float _fTimeDelta)
{
	_float3 vDirtoObject = (m_pTargetTransform->Get_State(CTransform::STATE_POSITION) + m_fDistance + m_fTargetOffset)- m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	

	if (D3DXVec3Length(&vDirtoObject) < 0.5f)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTargetTransform->Get_State(CTransform::STATE_POSITION) + m_fDistance + m_fTargetOffset);
		m_pTransformCom->Change_SpeedPerSec(m_fOriginSpeed);
	}
	else 
	{
		m_pTransformCom->Change_SpeedPerSec(m_fChangeSpeed);
		D3DXVec3Normalize(&vDirtoObject, &(vDirtoObject));
		m_pTransformCom->Move_DirY(_fTimeDelta, vDirtoObject);
	}
}

void CCamera::Follow_Player(_float _fTimeDelta)
{
	m_LookAtPlayer = true;

	if (m_pPlayerTransform == nullptr)
	{
		CPlayer* m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player());
		if (m_pPlayer)
		{
			m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")));
		}
	}
	else
	{
		if (m_pTargetTransform != m_pPlayerTransform)
		{
			m_pTargetTransform = m_pPlayerTransform;
			m_bisLookingPlayer = false;
		}

		m_fTargetOffset = _float3(0.f, 0.f, 0.f);

		_float3 vDirtoObject = m_pTargetTransform->Get_State(CTransform::STATE_POSITION) + m_fDistance - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    	if (D3DXVec3Length(&vDirtoObject) < 0.3f)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTargetTransform->Get_State(CTransform::STATE_POSITION) + m_fDistance);
			m_pTransformCom->Change_SpeedPerSec(m_fOriginSpeed);
			m_bisLookingPlayer = true;
		}
		else 
		{
			m_pTransformCom->Change_SpeedPerSec(m_fOriginSpeed);
			D3DXVec3Normalize(&vDirtoObject, &(vDirtoObject));
			m_pTransformCom->Move_DirY(_fTimeDelta, vDirtoObject);
		}
		/*
		else if (D3DXVec3Length(&vDirtoObject) < 3.0f)
		{
			_float fSpeed = 5.f;
			m_pTransformCom->Change_SpeedPerSec(fSpeed);
			D3DXVec3Normalize(&vDirtoObject, &(vDirtoObject));
			m_pTransformCom->Move_DirY(_fTimeDelta, vDirtoObject);
		}
		else if (D3DXVec3Length(&vDirtoObject) < 5.0f)
		{
			_float fSpeed = 10.f;
			m_pTransformCom->Change_SpeedPerSec(fSpeed);
			D3DXVec3Normalize(&vDirtoObject, &(vDirtoObject));
			m_pTransformCom->Move_DirY(_fTimeDelta, vDirtoObject);
		}
		else if (D3DXVec3Length(&vDirtoObject) )
		{
			_float fSpeed = 30.f;
			m_pTransformCom->Change_SpeedPerSec(fSpeed);
			D3DXVec3Normalize(&vDirtoObject, &(vDirtoObject));
			m_pTransformCom->Move_DirY(_fTimeDelta, vDirtoObject);
		}*/

	}
}

HRESULT CCamera::Ready_Components(void* pArg)
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), pArg)))
		return E_FAIL;

	return S_OK;
}

CCamera* CCamera::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera* pInstance = new CCamera(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CCamera::Clone(void* pArg)
{
	CCamera* pInstance = new CCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pFadeInOut);
}
