
#include "Boss_DeathCat2.h"
#include "Boss_Eye.h"

#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Collider_Cube.h"

#include "State_DeathCatIdle.h"
#include "State_DeathCatChainAttack.h"
#include "State_DeathCatFlyBallAttack.h"
#include "State_DeathCatDisappear.h"

CBoss_DeathCat2::CBoss_DeathCat2(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CBoss_DeathCat2::CBoss_DeathCat2(const CBoss_DeathCat2 & Prototype)
	: CMonster{ Prototype }
{
}


HRESULT CBoss_DeathCat2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_DeathCat2::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	if (FAILED(Ready_Fsm()))
		return E_FAIL;

	_float3 vPosition = _float3{ 20.f,1.f,20.f };
	_float3 vScale = _float3{ 12.f,12.f,0.1f };

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);

	//CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Boss_Eye"));
	//if (pGameObj != nullptr)
	//{
	//	for (_uint i = 0; i < 3; ++i)
	//	{
	//		m_pEyes[i] = dynamic_cast<CBoss_Eye*>(pGameObj->Clone(pArg));
	//	}
	//}

	return S_OK;
}

void CBoss_DeathCat2::Priority_Update(_float fTimeDelta)
{
}

void CBoss_DeathCat2::Update(_float fTimeDelta)
{
	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

	m_pFsmCom->Update(fTimeDelta);
}

void CBoss_DeathCat2::Late_Update(_float fTimeDelta)
{
	m_pAnimatorCom->Update(fTimeDelta);

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBoss_DeathCat2::Render()
{	
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;
	
	m_pAnimatorCom->Render(NULL);

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;		

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	//디버그 모드 추가하기
	m_pColliderCom->Render_Collider();

	return S_OK;
}

_float CBoss_DeathCat2::Get_FootPosY()
{
	return _float();
}

_uint CBoss_DeathCat2::Get_CurrentState()
{
	return m_pFsmCom->Get_CurrentState();
}

void CBoss_DeathCat2::OnCollisionEnter(CGameObject* pOther)
{
	//if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
	//	m_pGameInstance->Event_DestoryObject(this);
}

void CBoss_DeathCat2::OnCollisionStay(CGameObject* pOther)
{
}

void CBoss_DeathCat2::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CBoss_DeathCat2::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider_Cube */
	CCollider_Cube::COLLIDER_CUBE_INFO ColliderInfo;

	ZeroMemory(&ColliderInfo, sizeof(CCollider_Cube::COLLIDER_CUBE_INFO));
	ColliderInfo.fSizeX = 0.15f;
	ColliderInfo.fSizeY = 0.2f;
	ColliderInfo.fSizeZ = 2.f;
	ColliderInfo.fOffSetX = 0.f;
	ColliderInfo.fOffSetY = 0.f;
	ColliderInfo.fOffSetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderInfo)))
		return E_FAIL;





	return S_OK;
}

HRESULT CBoss_DeathCat2::Ready_Animation()
{
	CTexture* pTextureCom;

	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(Client::LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossCat_Phase1_Idle"),
		TEXT("Com_Texture_Boss_Idle"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("IDLE"), pTextureCom, 0, 34, 0.04f);

	if (FAILED(__super::Add_Component(Client::LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossCat_Phase1_Attack"),
		TEXT("Com_Texture_Boss_Attack"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Attack"), pTextureCom, 0, 33, 0.04f, false);

	if (FAILED(__super::Add_Component(Client::LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossCat_Phase1_Disappear"),
		TEXT("Com_Texture_Boss_Disappear"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Disappear"), pTextureCom, 0, 19, 0.05f, false);

	if (FAILED(__super::Add_Component(Client::LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossCat_Phase1_Appear"),
		TEXT("Com_Texture_Boss_Appear"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Appear"), pTextureCom, 0, 20, 0.05f, false);

	m_pAnimatorCom->Change_Animation(TEXT("IDLE"), 0);

	return S_OK;
}

HRESULT CBoss_DeathCat2::Ready_Fsm()
{	
	/* For.Com_Fsm */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;
	
	m_pFsmCom->Add_State(CState_DeatCatIdle::Create(m_pFsmCom, IDLE));

	Set_InitState(IDLE);

	return S_OK;
}

HRESULT CBoss_DeathCat2::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 127);

	return S_OK;
}

HRESULT CBoss_DeathCat2::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CBoss_DeathCat2 * CBoss_DeathCat2::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBoss_DeathCat2*		pInstance = new CBoss_DeathCat2(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_DeathCat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CBoss_DeathCat2::Clone(void* pArg)
{
	CBoss_DeathCat2*		pInstance = new CBoss_DeathCat2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_DeathCat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_DeathCat2::Free()
{
	__super::Free();

	//일단 임시로 이렇게 해둠 아이디어 있으면 반영좀
	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);





	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pVIBufferCom);
}
