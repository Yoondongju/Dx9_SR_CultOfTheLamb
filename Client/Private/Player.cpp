#include "..\Public\Player.h"

#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Collider_Rect.h"

#include "Boss_DeathCat.h"
#include "Boss_Eligor.h"
#include "Boss_Gusion.h"
#include "Boss_Hekat.h"

#include "Camera.h"

#include "State_PlayerIdle.h"
#include "State_PlayerAttack1.h"
#include "State_PlayerAttack2.h"
#include "State_PlayerAttack3.h"
#include "State_PlayerRun.h"
#include "State_PlayerRoll.h"
#include "State_PlayerResurrect.h"
#include "State_PlayerDie.h"
#include "State_PlayerSpell.h"
#include "State_PlayerKnockBack.h"
#include "State_PlayerWarp.h"
#include "State_PlayerAxe.h"
#include "State_PlayerPickAxe.h"
#include "State_PlayerHammer.h"
#include "State_PlayerSermon.h"
#include "State_PlayerGF.h"
#include "State_PlayerGS.h"
#include "State_PlayerCard.h"
#include "State_Player_Tarot.h"
#include "State_PlayerWI.h"
#include "State_PlayerWO.h"

#include "InteractObject.h"
#include "PlayerInventory.h"
#include "SpellGauge.h"
#include "Road.h"
#include "Effect.h"

#include "MonologuePanelUI.h"

#include "Layer.h"
#include "Quest.h"

const _float CPlayer::fRollCoolTime = 1.f;

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject{ pGraphic_Device }
{
}

CPlayer::CPlayer(const CPlayer & Prototype)
	: CLandObject{ Prototype }
	, m_tPlayerInfo{Prototype.m_tPlayerInfo}
{

}

void CPlayer::Apply_Tarot(CTarotCard::CARDTYPE _eType)
{
	switch (_eType)
	{
	case Client::CTarotCard::ATTACKPOWERUP:
		m_tPlayerInfo.iAttackPower = 4.5f;
		break;
	case Client::CTarotCard::BONUSHEART:
	{
		m_tPlayerInfo.iHp += 2;
		m_bGreenHeart = true;
	}
		break;
	case Client::CTarotCard::DOUBLEBULLET:
		m_bDoubleBullet = true;
		break;
	case Client::CTarotCard::HEAL:
		m_tPlayerInfo.iHp = m_tPlayerInfo.iMaxHp;
		break;
	case Client::CTarotCard::SPEEDUP:
		m_tPlayerInfo.fSpeed += 6.f;
		break;
	}
	m_bPickCard = true;
}


HRESULT CPlayer::Initialize_Prototype()
{
	m_tPlayerInfo.iHp = 6.f;
	m_tPlayerInfo.iMaxHp = 6.f;
	m_tPlayerInfo.iAttackPower = 2.5;
	m_tPlayerInfo.fSpeed = 25.f;

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	if (FAILED(Ready_Fsm()))
		return E_FAIL;


	_float3 vScale = _float3{ 5.f,5.f,0.1f };
	m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);


	if (FAILED(Ready_PlayerUI()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	m_bPickItem = true;
}

void CPlayer::Update(_float fTimeDelta)
{

	_float3 Temp = Get_Position();

	m_fRollTimer += fTimeDelta;
	
	if (m_ePreState == CPlayer::KNOCKBACK
		||m_ePreState==CPlayer::RESURRECT)
	{
		m_bCanHit = false;

		m_fCanHitTimer += fTimeDelta;
		if (m_fCanHitTimer > 3.f)
		{
			m_bCanHit = true;
			m_ePreState = CPlayer::IDLE;
			m_fCanHitTimer = 0.f;
		}
	}

	if (m_bLevelChange || m_pGameInstance->Get_LevelIndex() <= LEVEL_GAMEPLAY)
	{
		m_tPlayerInfo.iHp = 6.f;					// 6으로 바꿔야함
		m_tPlayerInfo.iMaxHp = 6.f;					// 6으로 바꿔야함
		m_tPlayerInfo.iAttackPower = 2.5f;			// 2.5로 바꿔야함
		m_tPlayerInfo.fSpeed = 25.f;
		m_bDoubleBullet = false;
		m_bLevelChange = false;
		m_bGreenHeart = false;
		m_bPickCard = false;
	}

	if ((m_eCurState != BUILD
		&& m_eCurState != BUILDING
		&& m_eCurState != CHANGE_LEVEL
		&& m_eCurState != CAMERA_ON
		&&m_pGameInstance->Get_LevelIndex()!=LEVEL_LOADING)
		&& m_eCurState != WARP_IN
		&& m_eCurState != WARP_OUT)
	{
		__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

		if (m_tPlayerInfo.iHp <= 0.f)
		{
			m_tPlayerInfo.iHp = 0.f;
		}

		m_pFsmCom->Update(fTimeDelta);
				
		if (m_eCurState == CHANGE_LEVEL)
		{
			if (CRoad::RIGHT == m_iRoadDir)
				m_pTransformCom->Go_Right(fTimeDelta * 0.5f);

			if (CRoad::LEFT == m_iRoadDir)
				m_pTransformCom->Go_Left(fTimeDelta * 0.5f);

			if (CRoad::UP == m_iRoadDir)
				m_pTransformCom->Go_Straight(fTimeDelta * 0.5f);

			if (CRoad::DOWN == m_iRoadDir)
				m_pTransformCom->Go_Backward(fTimeDelta * 0.5f);
		}

		if (m_pGameInstance->Get_KeyState(KEY::RBUTTON) == KEY_STATE::HOLD)
		{
			if (m_fSermonGauge >= 0.6f)
			{
				_float2 vScreenPos = m_pTransformCom->Get_ViewportPos();

				CState_PlayerSpell* pSpellState = dynamic_cast<CState_PlayerSpell*>(m_pFsmCom->Get_CurRealState());
				if (nullptr != pSpellState)
				{

					m_pMySpellUI->Set_fX(vScreenPos.x + 40);
					m_pMySpellUI->Set_fY(vScreenPos.y + 10);

					m_pMySpellUI->SetActivate(true);
				}
			}
		}

		if (m_pGameInstance->Get_KeyState(KEY::RBUTTON) == KEY_STATE::AWAY)
		{
			if (m_pMySpellUI->GetActivate())
			{
				m_fSermonGauge = 0.f;
			}
			
			m_pMySpellUI->GetChildUI().back()->Set_fSizeY(0.f);
			m_pMySpellUI->SetActivate(false);
		}


		if (m_eCurState != ATTACK1
			&& m_eCurState != ATTACK2
			&& m_eCurState != ATTACK3)
		{
			CCollider_Cube::COLLIDER_CUBE_INFO ColliderInfo;

			ZeroMemory(&ColliderInfo, sizeof(CCollider_Cube::COLLIDER_CUBE_INFO));
			ColliderInfo.fSizeX = 0.17f;
			ColliderInfo.fSizeY = 0.2f;
			ColliderInfo.fSizeZ = 3.5f;
			ColliderInfo.fOffSetX = 0.f;
			ColliderInfo.fOffSetY = -0.25f;
			ColliderInfo.fOffSetZ = 0.f;

			m_pColliderCom->Set_Collider_Cube_Scale(ColliderInfo);
		}
	}


	if (m_tPlayerInfo.iHp > 0 && m_tPlayerInfo.iHp <= 2)
	{
		Activate_MonoPanel(CMonologuePanelUI::PANEL_TYPE::NOBLOOD);
	}
	else if (m_tPlayerInfo.iHp <= 0)
		Activate_MonoPanel(CMonologuePanelUI::PANEL_TYPE::DEAD);
	else
	{
		CMonologuePanelUI* pMonoPanelUI = dynamic_cast<CMonologuePanelUI*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_MonologuePanelUI"), 0));
		pMonoPanelUI->SetActivate(false);
	}


}

void CPlayer::Late_Update(_float fTimeDelta)
{
	
	m_pAnimatorCom->Update(fTimeDelta);
	

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPlayer::Render()
{	
	m_pVIBufferCom->Filp_Texture(m_isFilp);

	if(m_pGameInstance->Get_LevelIndex() != LEVELID::LEVEL_LOADING)
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
	}

	m_pColliderCom->Render_Collider();

	return S_OK;
}

void CPlayer::Set_InitState(PLAYERSTATE eState, void* pArg)
{
	m_pFsmCom->Set_State(eState, pArg);
}

void CPlayer::Change_State(PLAYERSTATE eState, void* pArg)
{
	m_pFsmCom->Change_State(eState, pArg);
}

void CPlayer::Change_Animation(const _wstring& strAnimationTag, _uint iStartFrame)
{
	m_pAnimatorCom->Change_Animation(strAnimationTag, iStartFrame);
}

_uint CPlayer::Get_CurrentFrame()
{
	return m_pAnimatorCom->Get_CurrentFrame();
}

bool CPlayer::IsEnd_CurrentAnimation()
{
	return m_pAnimatorCom->IsEnd_CurrentAnimation();
}

HRESULT CPlayer::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = m_tPlayerInfo.fSpeed;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider_Cube */
	CCollider_Cube::COLLIDER_CUBE_INFO ColliderInfo;

	ZeroMemory(&ColliderInfo, sizeof(CCollider_Cube::COLLIDER_CUBE_INFO));
	ColliderInfo.fSizeX = 0.12f;
	ColliderInfo.fSizeY = 0.15f;
	ColliderInfo.fSizeZ = 6.f;
	ColliderInfo.fOffSetX = 0.f;
	ColliderInfo.fOffSetY = -0.25f;
	ColliderInfo.fOffSetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderInfo)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_Animation()
{
	CTexture* pTextureCom;

	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;
	// IDLE
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Idle"),
		TEXT("Com_Texture_Player_Idle"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("IDLE"), pTextureCom, 0, 51, 0.08f);

	// IDLE_UP
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Idle_Up"),
		TEXT("Com_Texture_Player_Idle_Up"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("IDLE_UP"), pTextureCom, 0, 51, 0.08f);

	
#pragma region GET

	// Get_Sword
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Get_Sword"),
		TEXT("Com_Texture_Player_Get_Sword"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Get_Sword"), pTextureCom, 0, 42, 0.05f, false);

	// Get_Follower
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Get_Follower"),
		TEXT("Com_Texture_Player_Get_Follower"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Get_Follower"), pTextureCom, 0, 48, 0.03f, false);

#pragma endregion

#pragma region ATTACK
	// Attack_Combo1
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Attack_Sword_Combo1_red"),
		TEXT("Com_Texture_Attack_Sword_Combo1_red"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Attack1"), pTextureCom, 0, 15, 0.05f , false);

	// Attack_Combo2
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Attack_Sword_Combo2_red"),
		TEXT("Com_Texture_Attack_Sword_Combo2_red"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Attack2"), pTextureCom, 0, 15, 0.05f, false);

	// Attack_Combo3
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Attack_Sword_Combo3_red"),
		TEXT("Com_Texture_Attack_Sword_Combo3_red"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Attack3"), pTextureCom, 0, 17, 0.05f, false);
#pragma endregion

#pragma region RUN
	// Run_Diagonal_Down
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Diagonal_Down"),
		TEXT("Com_Texture_Player_Run_Diagonal_Down"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Run_Diagonal_Down"), pTextureCom, 0, 13, 0.05f, true);

	// Run_Diagonal_Up
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Diagonal_Up"),
		TEXT("Com_Texture_Player_Run_Diagonal_Up"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Run_Diagonal_Up"), pTextureCom, 0, 13, 0.05f, true);

	// Run_Down
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Down"),
		TEXT("Com_Texture_Player_Run_Down"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Run_Down"), pTextureCom, 0, 13, 0.05f, true);

	// Run_Up
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Up"),
		TEXT("Com_Texture_Player_Run_Up"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Run_Up"), pTextureCom, 0, 13, 0.05f, true);

	// Run_Horizontal
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Horizontal"),
		TEXT("Com_Texture_Player_Run_Horizontal"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Run_Horizontal"), pTextureCom, 0, 13, 0.05f, true);
#pragma endregion

#pragma region ROLL
	// Roll_Up
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Roll_Up"),
		TEXT("Com_Texture_Player_Roll_Up"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Roll_Up"), pTextureCom, 0, 11, 0.02f,false);

	// Roll_Down
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Roll_Down"),
		TEXT("Com_Texture_Player_Roll_Down"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Roll_Down"), pTextureCom, 0, 11, 0.02f,false);

	// Roll_Horizontal
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Roll_Horizontal"),
		TEXT("Com_Texture_Player_Roll_Horizontal"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Roll_Horizontal"), pTextureCom, 0, 13, 0.02f,false);
#pragma endregion

	// Die
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Die"),
		TEXT("Com_Texture_Player_Die"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Die"), pTextureCom, 0, 16, 0.03f, false);

	// Spell_Charge
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Spell_Charge"),
		TEXT("Com_Texture_Player_Spell_Charge"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Spell_Charge"), pTextureCom, 0, 9, 0.03f, true);

	// Spell_Shoot
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Spell_Shot"),
		TEXT("Com_Texture_Player_Spell_Shot"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Spell_Shot"), pTextureCom, 0, 20, 0.03f, false);

	// KnockBack
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_KnockBack"),
		TEXT("Com_Texture_Player_KnockBack"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("KnockBack"), pTextureCom, 0, 27, 0.03f, false);

	// WarpIn
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_WarpIn"),
		TEXT("Com_Texture_Player_WarpIn"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("WarpIn"), pTextureCom, 0, 49, 0.03f, false);

	// WarpOut
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_WarpOut"),
		TEXT("Com_Texture_Player_WarpOut"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("WarpOut"), pTextureCom, 0, 36, 0.03f, false);

	// Resurrect
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Resurrect"),
		TEXT("Com_Texture_Player_Resurrect"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Resurrect"), pTextureCom, 0, 51, 0.03f, false);

	// Axe
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Axe"),
		TEXT("Com_Texture_Player_Axe"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Axe"), pTextureCom, 0, 26, 0.03f, true);

	// PickAxe
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_PickAxe"),
		TEXT("Com_Texture_Player_PickAxe"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("PickAxe"), pTextureCom, 0, 31, 0.03f, true);

	// Hammer
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Hammer"),
		TEXT("Com_Texture_Player_Hammer"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Hammer"), pTextureCom, 0, 19, 0.03f, true);

	// Sermon_Start
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Sermon_Start"),
		TEXT("Com_Texture_Player_Sermon_Start"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Sermon_Start"), pTextureCom, 0, 25, 0.03f, false);

	// Sermon_Loop
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Sermon_Loop"),
		TEXT("Com_Texture_Player_Sermon_Loop"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Sermon_Loop"), pTextureCom, 0, 51, 0.03f, false);

	// Sermon_End
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Sermon_End"),
		TEXT("Com_Texture_Player_Sermon_End"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Sermon_End"), pTextureCom, 0, 23, 0.03f, false);

	// Card_Start
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Cards_Start"),
		TEXT("Com_Texture_Player_Cards_Start"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Card_Start"), pTextureCom, 0, 37, 0.03f, false);

	// Card_Loop
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Cards_Loop"),
		TEXT("Com_Texture_Player_Cards_Loop"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Card_Loop"), pTextureCom, 0, 34, 0.03f, true);

	// Card_End
	if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Cards_End"),
		TEXT("Com_Texture_Player_Cards_End"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
		return E_FAIL;
	m_pAnimatorCom->Add_Animation(TEXT("Card_End"), pTextureCom, 0, 48, 0.04f, false);


	return S_OK;
}

HRESULT CPlayer::Ready_Fsm()
{	
	/* For.Com_Fsm */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_PlayerIdle::Create(m_pFsmCom, IDLE));

	
#pragma region 
	m_pFsmCom->Add_State(CState_PlayerAttack1::Create(m_pFsmCom,ATTACK1));
	m_pFsmCom->Add_State(CState_PlayerAttack2::Create(m_pFsmCom, ATTACK2));
	m_pFsmCom->Add_State(CState_PlayerAttack3::Create(m_pFsmCom, ATTACK3));
	m_pFsmCom->Add_State(CState_PlayerRun::Create(m_pFsmCom,RUN));
	m_pFsmCom->Add_State(CState_PlayerRoll::Create(m_pFsmCom,ROLL)); 
	
	m_pFsmCom->Add_State(CState_PlayerSpell::Create(m_pFsmCom,SPELL));
	m_pFsmCom->Add_State(CState_PlayerKnockBack::Create(m_pFsmCom,KNOCKBACK));
	m_pFsmCom->Add_State(CState_PlayerWI::Create(m_pFsmCom,WARP_IN));
	m_pFsmCom->Add_State(CState_PlayerWO::Create(m_pFsmCom, WARP_OUT));
	m_pFsmCom->Add_State(CState_PlayerAxe::Create(m_pFsmCom,AXE));
	m_pFsmCom->Add_State(CState_PlayerPickAxe::Create(m_pFsmCom,PICKAXE));
	m_pFsmCom->Add_State(CState_PlayerHammer::Create(m_pFsmCom,HAMMER));
	m_pFsmCom->Add_State(CState_PlayerResurrect::Create(m_pFsmCom,RESURRECT));
	m_pFsmCom->Add_State(CState_PlayerDie::Create(m_pFsmCom,DIE));
	m_pFsmCom->Add_State(CState_PlayerSermon::Create(m_pFsmCom,SERMON));
	m_pFsmCom->Add_State(CState_PlayerGF::Create(m_pFsmCom,GET_FOLLOWER));
	m_pFsmCom->Add_State(CState_PlayerCard::Create(m_pFsmCom,CARD));
	m_pFsmCom->Add_State(CState_PlayerGS::Create(m_pFsmCom,GET_SWORD));
	m_pFsmCom->Add_State(CState_Player_Tarot::Create(m_pFsmCom,TAROT));
#pragma endregion


	return S_OK;
}

HRESULT CPlayer::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 127);

	return S_OK;
}

HRESULT CPlayer::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CPlayer::Ready_PlayerUI()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_MonologuePanelUI"), TEXT("Prototype_GameObject_MonologuePanelUI"))))
		MSG_BOX(L"MonologuePanel_UI Error");


	if (FAILED(CreateMainPlayerUI()))
		return E_FAIL;

	if (FAILED(CreateMyInventory()))
		return E_FAIL;

	if (FAILED(CreateMyCraft()))
		return E_FAIL;

	CUI::UI_DESC Desc = { };
	Desc.m_vPos = { 680 , 370 };
	Desc.m_vSize = { 20.f,100.f };
	Desc.m_iBindNum = 0;
	Desc.m_vOffset = { 20.f,0.f };
	Desc.eUItype = UI_TYPE::UI_BAR;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer_UI(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_PlayerSpellUI"), &Desc, (void**)&m_pMySpellUI)))
		MSG_BOX(L"Can't Create PlayerSpellUI");
	


	Safe_AddRef(m_pMySpellUI);

	return S_OK;
}

HRESULT CPlayer::CreateMyInventory()
{
	// Owner == Player
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_InventoryUI"), TEXT("Prototype_GameObject_InventoryUI"),this)))
		MSG_BOX(L"Can't Create PlayerUI");

	return S_OK;
}

HRESULT CPlayer::CreateMyCraft()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_CraftUI"), TEXT("Prototype_GameObject_CraftPanelUI"), this)))
		MSG_BOX(L"Can't Create CraftUI");

	return S_OK;
}

HRESULT CPlayer::CreateMainPlayerUI()
{
	CUI::UI_DESC Desc = {};

	Desc.m_vPos = { 50.f,0.f };
	Desc.m_vSize = { 40.f,40.f };
	Desc.m_iBindNum = 0;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_MainPlayerUI"), TEXT("Prototype_GameObject_PlayerMainUI"), &Desc)))
		MSG_BOX(L"Can't Create MainUI");

	Desc.m_vPos = { 190.f,0.f };
	Desc.m_vSize = { 40.f,40.f };
	Desc.m_iBindNum = 1;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_MainPlayerUI"), TEXT("Prototype_GameObject_PlayerMainUI"), &Desc)))
		MSG_BOX(L"Can't Create MainUI");

	return S_OK;
}



void CPlayer::Activate_MonoPanel(CMonologuePanelUI::PANEL_TYPE _eType)
{
	CMonologuePanelUI* pMonoPanelUI = dynamic_cast<CMonologuePanelUI*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_MonologuePanelUI"), 0));
	pMonoPanelUI->SetActivate(true);
	pMonoPanelUI->Set_PanelType(_eType);
}


void CPlayer::OnCollisionEnter(CGameObject* pOther)
{
	CInteractObject* pInteract = dynamic_cast<CInteractObject*>(pOther);
	CInteractObject::INTERACT_TYPE eInteractType = { INTERACTTYPE_END };
	if (nullptr != pInteract)
	{
		eInteractType = pInteract->Get_Interact_Type();
	}

	

	if (pOther->Get_LayerTag() == TEXT("Layer_Item"))
	{
		m_pGameInstance->Play_Sound(TEXT("Player_Action.wav"), SOUND_PLAYER, g_fEffectVolume);
		CEffect::EFFECT_DESC Effect_Desc = {};
		Effect_Desc.vPos = Get_Position();
		Effect_Desc.vPos.y = Get_Position().y - 1.f;
		Effect_Desc.vScale = _float3(1.5f, 1.5f, 0.1f);
		Effect_Desc.fEffectDura = 0.2f;
		Effect_Desc.strTextureTag = TEXT("Pick_Up_Item");
		Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
		Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
		m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
	}

	if (eInteractType == CInteractObject::BUILD)
	{
		CUI::UI_DESC Desc = {};

		Desc.m_vPos = { g_iWinSizeX / 2.f , g_iWinSizeY - 70.f };
		Desc.m_vSize = { 200.f,40.f };
		Desc.m_iBindNum = 0;
		Desc.eUItype = UI_TYPE::GUIDE_BUILD;

		if (FAILED(m_pGameInstance->Event_CreateObject(LEVEL_STATIC, TEXT("Layer_GuideUI"), TEXT("Prototype_GameObject_GuideUI"), sizeof(CUI::UI_DESC), &Desc)))
			MSG_BOX(L"Can't Create GuideUI");
	}

	if (eInteractType == CInteractObject::TREE ||
		eInteractType == CInteractObject::ROCK )
	{
		CUI::UI_DESC Desc = {};

		Desc.m_vPos = { g_iWinSizeX / 2.f , g_iWinSizeY - 70.f };
		Desc.m_vSize = { 200.f,40.f };
		Desc.m_iBindNum = 0;
		Desc.eUItype = UI_TYPE::GUIDE_ROCK_AND_TREE;

		if (FAILED(m_pGameInstance->Event_CreateObject(LEVEL_STATIC, TEXT("Layer_GuideUI"), TEXT("Prototype_GameObject_GuideUI"), sizeof(CUI::UI_DESC), &Desc)))
			MSG_BOX(L"Can't Create GuideUI");
	}


	if (pOther->Get_LayerTag() == TEXT("Layer_Terrain_Road"))
	{
		m_iRoadDir = dynamic_cast<CRoad*>(pOther)->Get_RoadDir();

		if (CRoad::RIGHT == m_iRoadDir)
			Change_Animation(TEXT("Run_Horizontal"));

		if (CRoad::LEFT == m_iRoadDir)
			Change_Animation(TEXT("Run_Horizontal"));

		if (CRoad::UP == m_iRoadDir)
			Change_Animation(TEXT("Run_Up"));

		if (CRoad::DOWN == m_iRoadDir)
			Change_Animation(TEXT("Run_Down"));

		m_pGameInstance->Start_StageChange(dynamic_cast<CRoad*>(pOther)->Get_ChangeStageIndex());

	}







	if ((pOther->Get_LayerTag() == TEXT("Layer_Boss")
		|| pOther->Get_LayerTag() == TEXT("Layer_Monster")
		|| pOther->Get_LayerTag() == TEXT("Layer_Attack")) && m_bCanHit)
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(pOther);
		if (pMonster != nullptr)
		{
			if (pMonster->Get_MonsterId() == ELIGOS)
			{
				if (static_cast<CBoss_Eligor*>(pOther)->Get_Eligor_State() == CBoss_Eligor::TRANS_F
					&& static_cast<CBoss_Eligor*>(pOther)->Get_Eligor_State() == CBoss_Eligor::UNCONVERTED)
					return;
				else
				{
					// Player Attacked By Anything
					if (m_fSermonGauge < 0.6f)
						m_fSermonGauge += 0.2f;
					else
						m_fSermonGauge = 0.6f;

					if (m_eCurState != ATTACK1
						&& m_eCurState != ATTACK2
						&& m_eCurState != ATTACK3
						&& m_eCurState != ROLL
						&& m_eCurState != DIE
						&& m_eCurState != RESURRECT)
					{

						m_pGameInstance->Play_Sound(TEXT("Player_Hit.wav"), SOUND_PLAYER, g_fEffectVolume);

						if (m_bGreenHeart)
						{
							m_tPlayerInfo.iHp -= 2.f;
							m_bGreenHeart = false;
						}

						else
						{
							m_tPlayerInfo.iHp -= 1;
						}
						Change_State(KNOCKBACK);
						if (m_isFilp)
						{
							CEffect::EFFECT_DESC Effect_Desc = {};
							Effect_Desc.vPos = Get_Position();
							Effect_Desc.vPos.x = Get_Position().x + 0.5f;
							Effect_Desc.vScale = _float3(1.5f, 1.5f, 0.1f);
							Effect_Desc.fEffectDura = 0.2f;
							Effect_Desc.strTextureTag = TEXT("Hit_Star");
							Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
							Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
							m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
						}
						else if (!m_isFilp)
						{
							CEffect::EFFECT_DESC Effect_Desc = {};
							Effect_Desc.vPos = Get_Position();
							Effect_Desc.vPos.x = Get_Position().x - 0.5f;
							Effect_Desc.vScale = _float3(1.5f, 1.5f, 0.1f);
							Effect_Desc.fEffectDura = 0.2f;
							Effect_Desc.strTextureTag = TEXT("Hit_Star");
							Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
							Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
							m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

						}
					}
					else if (m_eCurState == ATTACK1
						|| m_eCurState == ATTACK2
						|| m_eCurState == ATTACK3)
					{
						if (m_isFilp)
						{
							if (pOther->Get_Position().x < Get_Position().x)
							{

								m_pGameInstance->Play_Sound(TEXT("Player_Hit.wav"), SOUND_PLAYER, g_fEffectVolume);
								if (m_bGreenHeart)
								{
									m_tPlayerInfo.iHp -= 2.f;
									m_bGreenHeart = false;
								}

								else
								{
									m_tPlayerInfo.iHp -= 1;
								}
								Change_State(KNOCKBACK);

								CEffect::EFFECT_DESC Effect_Desc = {};
								Effect_Desc.vPos = Get_Position();
								Effect_Desc.vPos.x = Get_Position().x + 0.5f;
								Effect_Desc.vScale = _float3(1.5f, 1.5f, 0.1f);
								Effect_Desc.fEffectDura = 0.3f;
								Effect_Desc.strTextureTag = TEXT("Hit_Star");
								Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
								Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
								m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
							}
							else
							{
								dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(0.7f, 0.5f, 1);
								m_fRollTimer = 1.f;
							}
						}

						else if (!m_isFilp)
						{
							if (pOther->Get_Position().x > Get_Position().x)
							{

								m_pGameInstance->Play_Sound(TEXT("Player_Hit.wav"), SOUND_PLAYER, g_fEffectVolume);
								if (m_bGreenHeart)
								{
									m_tPlayerInfo.iHp -= 2.f;
									m_bGreenHeart = false;
								}

								else
								{
									m_tPlayerInfo.iHp -= 1;
								}
								Change_State(KNOCKBACK);
								CEffect::EFFECT_DESC Effect_Desc = {};
								Effect_Desc.vPos = Get_Position();
								Effect_Desc.vPos.x = Get_Position().x - 0.5f;
								Effect_Desc.vScale = _float3(1.5f, 1.5f, 0.1f);
								Effect_Desc.fEffectDura = 0.2f;
								Effect_Desc.strTextureTag = TEXT("Hit_Star");
								Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
								Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
								m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
							}
							else
							{
								dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(0.7f, 0.5f, 1);
								m_fRollTimer = 1.f;
							}

						}


					}
				}
			}
			else
			{
				// Player Attacked By Anything
				if (m_fSermonGauge < 0.6f)
					m_fSermonGauge += 0.2f;
				else
					m_fSermonGauge = 0.6f;

				if (m_eCurState != ATTACK1
					&& m_eCurState != ATTACK2
					&& m_eCurState != ATTACK3
					&& m_eCurState != ROLL
					&& m_eCurState != DIE
					&& m_eCurState != RESURRECT)
				{

					m_pGameInstance->Play_Sound(TEXT("Player_Hit.wav"), SOUND_PLAYER, g_fEffectVolume);

					if (m_bGreenHeart)
					{
						m_tPlayerInfo.iHp -= 2.f;
						m_bGreenHeart = false;
					}

					else
					{
						m_tPlayerInfo.iHp -= 1;
					}
					Change_State(KNOCKBACK);
					if (m_isFilp)
					{
						CEffect::EFFECT_DESC Effect_Desc = {};
						Effect_Desc.vPos = Get_Position();
						Effect_Desc.vPos.x = Get_Position().x + 0.5f;
						Effect_Desc.vScale = _float3(1.5f, 1.5f, 0.1f);
						Effect_Desc.fEffectDura = 0.2f;
						Effect_Desc.strTextureTag = TEXT("Hit_Star");
						Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
						Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
						m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
					}
					else if (!m_isFilp)
					{
						CEffect::EFFECT_DESC Effect_Desc = {};
						Effect_Desc.vPos = Get_Position();
						Effect_Desc.vPos.x = Get_Position().x - 0.5f;
						Effect_Desc.vScale = _float3(1.5f, 1.5f, 0.1f);
						Effect_Desc.fEffectDura = 0.2f;
						Effect_Desc.strTextureTag = TEXT("Hit_Star");
						Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
						Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
						m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

					}
				}
				else if (m_eCurState == ATTACK1
					|| m_eCurState == ATTACK2
					|| m_eCurState == ATTACK3)
				{
					if (m_isFilp)
					{
						if (pOther->Get_Position().x < Get_Position().x)
						{

							m_pGameInstance->Play_Sound(TEXT("Player_Hit.wav"), SOUND_PLAYER, g_fEffectVolume);
							if (m_bGreenHeart)
							{
								m_tPlayerInfo.iHp -= 2.f;
								m_bGreenHeart = false;
							}

							else
							{
								m_tPlayerInfo.iHp -= 1;
							}
							Change_State(KNOCKBACK);

							CEffect::EFFECT_DESC Effect_Desc = {};
							Effect_Desc.vPos = Get_Position();
							Effect_Desc.vPos.x = Get_Position().x + 0.5f;
							Effect_Desc.vScale = _float3(1.5f, 1.5f, 0.1f);
							Effect_Desc.fEffectDura = 0.3f;
							Effect_Desc.strTextureTag = TEXT("Hit_Star");
							Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
							Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
							m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
						}
						else
						{
							dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(0.7f, 0.5f, 1);
							m_fRollTimer = 1.f;
						}
					}

					else if (!m_isFilp)
					{
						if (pOther->Get_Position().x > Get_Position().x)
						{

							m_pGameInstance->Play_Sound(TEXT("Player_Hit.wav"), SOUND_PLAYER, g_fEffectVolume);
							if (m_bGreenHeart)
							{
								m_tPlayerInfo.iHp -= 2.f;
								m_bGreenHeart = false;
							}

							else
							{
								m_tPlayerInfo.iHp -= 1;
							}
							Change_State(KNOCKBACK);
							CEffect::EFFECT_DESC Effect_Desc = {};
							Effect_Desc.vPos = Get_Position();
							Effect_Desc.vPos.x = Get_Position().x - 0.5f;
							Effect_Desc.vScale = _float3(1.5f, 1.5f, 0.1f);
							Effect_Desc.fEffectDura = 0.2f;
							Effect_Desc.strTextureTag = TEXT("Hit_Star");
							Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
							Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
							m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
						}
						else
						{
							dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(0.7f, 0.5f, 1);
							m_fRollTimer = 1.f;
						}

					}


				}
			}
		}
		else
		{
			// Player Attacked By Anything
			if (m_fSermonGauge < 0.6f)
				m_fSermonGauge += 0.2f;
			else
				m_fSermonGauge = 0.6f;

			if (m_eCurState != ATTACK1
				&& m_eCurState != ATTACK2
				&& m_eCurState != ATTACK3
				&& m_eCurState != ROLL
				&& m_eCurState != DIE
				&& m_eCurState != RESURRECT)
			{

				m_pGameInstance->Play_Sound(TEXT("Player_Hit.wav"), SOUND_PLAYER, g_fEffectVolume);

				if (m_bGreenHeart)
				{
					m_tPlayerInfo.iHp -= 2.f;
					m_bGreenHeart = false;
				}

				else
				{
					m_tPlayerInfo.iHp -= 1;
				}
				Change_State(KNOCKBACK);
				if (m_isFilp)
				{
					CEffect::EFFECT_DESC Effect_Desc = {};
					Effect_Desc.vPos = Get_Position();
					Effect_Desc.vPos.x = Get_Position().x + 0.5f;
					Effect_Desc.vScale = _float3(1.5f, 1.5f, 0.1f);
					Effect_Desc.fEffectDura = 0.2f;
					Effect_Desc.strTextureTag = TEXT("Hit_Star");
					Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
					Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
					m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
				}
				else if (!m_isFilp)
				{
					CEffect::EFFECT_DESC Effect_Desc = {};
					Effect_Desc.vPos = Get_Position();
					Effect_Desc.vPos.x = Get_Position().x - 0.5f;
					Effect_Desc.vScale = _float3(1.5f, 1.5f, 0.1f);
					Effect_Desc.fEffectDura = 0.2f;
					Effect_Desc.strTextureTag = TEXT("Hit_Star");
					Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
					Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
					m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

				}
			}
			else if (m_eCurState == ATTACK1
				|| m_eCurState == ATTACK2
				|| m_eCurState == ATTACK3)
			{
				if (m_isFilp)
				{
					if (pOther->Get_Position().x < Get_Position().x)
					{

						m_pGameInstance->Play_Sound(TEXT("Player_Hit.wav"), SOUND_PLAYER, g_fEffectVolume);
						if (m_bGreenHeart)
						{
							m_tPlayerInfo.iHp -= 2.f;
							m_bGreenHeart = false;
						}

						else
						{
							m_tPlayerInfo.iHp -= 1;
						}
						Change_State(KNOCKBACK);

						CEffect::EFFECT_DESC Effect_Desc = {};
						Effect_Desc.vPos = Get_Position();
						Effect_Desc.vPos.x = Get_Position().x + 0.5f;
						Effect_Desc.vScale = _float3(1.5f, 1.5f, 0.1f);
						Effect_Desc.fEffectDura = 0.3f;
						Effect_Desc.strTextureTag = TEXT("Hit_Star");
						Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
						Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
						m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
					}
					else
					{
						dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(0.7f, 0.5f, 1);
						m_fRollTimer = 1.f;
					}
				}

				else if (!m_isFilp)
				{
					if (pOther->Get_Position().x > Get_Position().x)
					{

						m_pGameInstance->Play_Sound(TEXT("Player_Hit.wav"), SOUND_PLAYER, g_fEffectVolume);
						if (m_bGreenHeart)
						{
							m_tPlayerInfo.iHp -= 2.f;
							m_bGreenHeart = false;
						}

						else
						{
							m_tPlayerInfo.iHp -= 1;
						}
						Change_State(KNOCKBACK);
						CEffect::EFFECT_DESC Effect_Desc = {};
						Effect_Desc.vPos = Get_Position();
						Effect_Desc.vPos.x = Get_Position().x - 0.5f;
						Effect_Desc.vScale = _float3(1.5f, 1.5f, 0.1f);
						Effect_Desc.fEffectDura = 0.2f;
						Effect_Desc.strTextureTag = TEXT("Hit_Star");
						Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
						Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
						m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
					}
					else
					{
						dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(0.7f, 0.5f, 1);
						m_fRollTimer = 1.f;
					}

				}


			}
		}


	}
}

void CPlayer::OnCollisionStay(CGameObject* pOther)
{
	CInteractObject* pInteractObject = dynamic_cast<CInteractObject*>(pOther);

	if (nullptr != pInteractObject)
	{
		if (CInteractObject::INTERACT_TYPE::BUILD == pInteractObject->Get_Type())
		{
			m_bCanBuilding = true;
		}
		else
			m_bCanBuilding = false;
	}


	if (pOther->Get_LayerTag() == TEXT("Layer_BackGround_Object") && m_pGameInstance->Get_KeyState(KEY::P) == KEY_STATE::TAP)
	{
		if (static_cast<CInteractObject*>(pOther)->Get_Interact_Type() == CInteractObject::TREE)
		{
			m_pCurrentInteractObj = dynamic_cast<CInteractObject*>(pOther);
			Change_State(AXE);

			CQuest::Get_Instance()->Succeed_Quest(L"나무와 돌캐기");
		}
		else if (static_cast<CInteractObject*>(pOther)->Get_Interact_Type() == CInteractObject::ROCK)
		{
			m_pCurrentInteractObj = dynamic_cast<CInteractObject*>(pOther);
			Change_State(PICKAXE);

			CQuest::Get_Instance()->Succeed_Quest(L"나무와 돌캐기");
		}

	}

	if (m_bInteractEnd && !m_bCanBuilding && m_eCurState != BUILD && m_eCurState != BUILDING)
		m_pGameInstance->Event_DestoryObject(pOther);
}

void CPlayer::OnCollisionExit(CGameObject* pOther)
{
	CInteractObject* pInteract = dynamic_cast<CInteractObject*>(pOther);
	CInteractObject::INTERACT_TYPE eInteractType = { INTERACT_TYPE::INTERACTTYPE_END };
	if (nullptr != pInteract)
	{
		eInteractType = pInteract->Get_Interact_Type();
	}

	if (eInteractType == CInteractObject::BUILD ||
		eInteractType == CInteractObject::TREE ||
		eInteractType == CInteractObject::ROCK)
	{

		CGameObject* pGameObj = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_GuideUI"), 0);
		if (nullptr != pGameObj)
		{
			if (FAILED(m_pGameInstance->Event_DestoryObject(pGameObj)))
				MSG_BOX(L"Can't Delete GuideUI");
		}

	}
}

CPlayer * CPlayer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer*		pInstance = new CPlayer(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void* pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();


	Safe_Release(m_pMySpellUI);


	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);

	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pVIBufferCom);
}
