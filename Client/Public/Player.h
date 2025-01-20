#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "InteractObject.h"
#include "TarotCard.h"

#include "Item.h"
#include "MonologuePanelUI.h"


BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
class CFsm;
END

BEGIN(Client)

class CPlayer final : public CLandObject
{
public:
	static const _float fRollCoolTime;
	enum PLAYERSTATE
	{
		IDLE,
		ATTACK1,
		ATTACK2,
		ATTACK3,
		RUN,
		ROLL,
		SPELL,
		KNOCKBACK,
		WARP_IN,
		WARP_OUT,
		AXE,
		PICKAXE,
		HAMMER,
		SERMON,
		GET_FOLLOWER,
		CARD,
		GET_SWORD,
		RESURRECT,
		DIE,
		CHANGE_LEVEL,

		BUILD,
		BUILDING,  

		CAMERA_ON,
		CAMERA_OFF,

		TAROT,

		STATE_END

	};

public:
	typedef struct {
		_float iHp;
		_float iMaxHp;
		_float iAttackPower;
		_float fSpeed;
	}PLAYER_INFO;

private:
	CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	PLAYER_INFO Get_Player_Info() 
	{
		return m_tPlayerInfo;
	}
	void		Set_Player_Info()
	{
		m_tPlayerInfo.iHp = m_tPlayerInfo.iMaxHp;
	}
  

	void Set_Player_State(PLAYERSTATE _eState) { m_eCurState = _eState; }
	void Set_Player_PreState(PLAYERSTATE _eState) { m_ePreState = _eState; }
	void Apply_Tarot(CTarotCard::CARDTYPE _eType);
	void Set_Player_Attack(_bool bAttack)
	{
		m_bAttack = bAttack;
	}
	class CTransform* Get_Player_Trans()
	{
		return m_pTransformCom;
	}
	class CCollider_Cube* Get_Player_Coll()
	{
		return m_pColliderCom;
	}
	class CAnimator* Get_Player_Anim()
	{
		return m_pAnimatorCom;
	}
	LPDIRECT3DDEVICE9 Get_Device()
	{
		return m_pGraphic_Device;
	}
	PLAYERSTATE Get_Player_State() { return m_eCurState; }
	PLAYERSTATE Get_Player_PreState() { return m_ePreState; }
	_bool IsHit() { return m_bPlayerHit; }
	_bool Get_CanMouseClick() { return m_bCanMouseClick; }
	_bool Get_Player_Attack()
	{
		return m_bAttack;
	}
	_bool Get_Monster_Attack()
	{
		return m_bMonsterAttack;
	}
	
	void  Set_InteractEnd(_bool bInteractEnd)
	{
		m_bInteractEnd = bInteractEnd;
	}
	void  Set_CanMouseClick(_bool _b) {  m_bCanMouseClick = _b; }
	void  Set_Monster_Attack(_bool bMonsterAttack)
	{
		m_bMonsterAttack = bMonsterAttack;
	}
	void  Set_Player_Re_Hit(_bool bCanHit) { m_bCanHit = bCanHit; }

	CInteractObject* Get_InteractObject() { return m_pCurrentInteractObj; }
	void Set_InteractObject(CInteractObject* pInteractObj) { m_pCurrentInteractObj = pInteractObj; }
	_float Get_Roll_Timer()
	{
		return m_fRollTimer;
	}
	void Set_Roll_Timer()
	{
		m_fRollTimer = 0.f;
	}


	void Add_ItemCount(CItem::ITEM_ID _eItemType, _int _iItemCount)
	{
		if (_eItemType >= CItem::ITEM_ID::ITEM_END)
			MSG_BOX(TEXT("Can't Add ItemCount"));

		m_iMyItemCount[_eItemType] += _iItemCount;
	}

	void Discount_ItemCount(CItem::ITEM_ID _eItemType, _int _iItemCount)
	{
		if (_eItemType >= CItem::ITEM_ID::ITEM_END)
			MSG_BOX(TEXT("Can't Add ItemCount"));

		m_iMyItemCount[_eItemType] -= _iItemCount;
	}

	const _int& Get_ItemCount(CItem::ITEM_ID _eItemType)
	{
		return m_iMyItemCount[_eItemType];
	}

	const _float&	Get_SermonGauge() { return m_fSermonGauge; }


	void Add_FollowerCount() { ++m_iFollowerCount; }
	_int Get_FollowerCount() {return  m_iFollowerCount; }


public:
	bool	IsCanBuilding() { return m_bCanBuilding; }
	void    Set_CanBuilding(bool _b) { m_bCanBuilding = _b; }
	bool	Get_PickCard() const { return m_bPickCard; }
	void	Set_Kill_Eligos(_bool _b) { m_bKill_Eligos = _b; }
	_bool   Get_Kill_Eligos() { return m_bKill_Eligos; }
	void	Set_LevelChange(_bool bChange)
	{
		m_bLevelChange = bChange;
	}
	_bool	Get_DoubleBullet()
	{
		return m_bDoubleBullet;
	}
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_InitState(PLAYERSTATE eState, void* pArg = nullptr);
	void Change_State(PLAYERSTATE eState, void* pArg = nullptr);
	void Change_Animation(const _wstring& strAnimationTag, _uint iStartFrame = 0);
	_uint Get_CurrentFrame();
	bool IsEnd_CurrentAnimation();

private:
	CAnimator*		m_pAnimatorCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CFsm*			m_pFsmCom = { nullptr };

	PLAYER_INFO		m_tPlayerInfo = {};
	PLAYERSTATE		m_eCurState = { STATE_END };
	PLAYERSTATE		m_ePreState = { STATE_END };

	_bool			m_bPlayerHit = { false };
	_bool			m_bCanMouseClick = { false };
	_bool			m_bAttack = { false };
	_bool			m_bInteractEnd = { false };
	_bool			m_bMonsterAttack = { false };
	_bool			m_bKnockBackTrue = { false };

	_bool			m_bPickCard = false;
	_bool			m_bPickItem = true;

	bool			m_bCanBuilding = false;
	_bool			m_bGreenHeart = false;

	CInteractObject* m_pCurrentInteractObj = { nullptr };

	class CSpellGauge*	m_pMySpellUI = { nullptr };

	_float			m_fCanHitTimer = 0.f;
	_bool			m_bCanHit = true;

	_uint			m_iRoadDir = {};

	_bool			m_bKill_Eligos = { false };

	_int			m_iMyItemCount[CItem::ITEM_ID::ITEM_END] = {0,0,0,0};
	_int			m_iFollowerCount = { 0 };

	_bool			m_bDoubleBullet = false;
	_bool			m_bLevelChange = false;
	_float			m_fRollTimer = 0.f;


	_float			m_fSermonGauge = { 0.f };		// MAX == 0.6     


private:
	HRESULT Ready_Components();
	HRESULT Ready_Animation();
	HRESULT Ready_Fsm();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();


private:
	HRESULT Ready_PlayerUI();

	HRESULT CreateMyInventory();
	HRESULT CreateMyCraft();
	HRESULT CreateMainPlayerUI();


	void	Activate_MonoPanel(CMonologuePanelUI::PANEL_TYPE _eType);




private:
	virtual void OnCollisionEnter(CGameObject* pOther);
	virtual void OnCollisionStay(CGameObject* pOther);
	virtual void OnCollisionExit(CGameObject* pOther);

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END