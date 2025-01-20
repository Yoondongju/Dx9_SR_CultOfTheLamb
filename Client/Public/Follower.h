#pragma once
#include "Client_Defines.h"
#include "LandObject.h"
#include "Follower_Command_UI.h"

BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Rect;
class CFsm;
class CTexture;
END


BEGIN(Client)



class CFollower :
    public CLandObject
{
public:
	enum FOLLOWER_TYPE {BEE, DEER, RABBIT, ELIGOS, TYPE_END};
	enum FOLLOWER_STATE {IDLE, WALK, AXE, MINING, PRAY, WORSHIP, INDO, STATE_END};
	typedef struct
	{
		FOLLOWER_TYPE   e_type;
		_int			iHunger;
		//이름
		_float			fX;
		_float			fZ;
		LANDOBJECT_DESC tLandObjectDesc;
	}FOLLOWER_INFO;

private:
	CFollower(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFollower(const CFollower& Prototype);
	virtual ~CFollower() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_InitState(FOLLOWER_STATE eState, void* pArg = nullptr);
	void Change_State(FOLLOWER_STATE eState, void* pArg = nullptr);
	void Change_Animation(const _wstring& strAnimationTag, _uint iStartFrame = 0);
	bool IsEnd_CurrentAnimation();
	_uint Get_CurrentFrame();
 	CFsm* Get_Fsm() {return m_pFsmCom;}

	void Set_IsAxe(_bool _b) { m_bIsAxe = _b; }
	void Set_IsMining(_bool _b) { m_bIsMining = _b; }
	void Set_IsPray(_bool _b) { m_bIsPray = _b; }

	_bool Get_IsAxe() { return m_bIsAxe; }
	_bool Get_IsMining() { return m_bIsMining; }
	_bool Get_IsPray() { return m_bIsPray; }
	_bool	Get_Collision_Interact_Object() { return m_bCollision_Interact_Object; }

	class CInteractObject* Get_Interactive_Object() { return m_pInteract_Object; }
	void Set_Interactive_Object(CInteractObject* _object) { m_pInteract_Object = _object; }
	void	Get_Collision_Interact_Object(_bool _b) { m_bCollision_Interact_Object = _b; }
	void	Reset_Command()
	{
		m_bIsAxe = false;
		m_bIsMining = false;
		m_bIsPray = false;
	}
	_float Get_Distance_with_Player() { return m_fDistance_with_Player; }
	void Set_isIndo(_bool _b) { m_bisIndo = _b; }
	_bool Get_isIndo() { return m_bisIndo; }

private:
	HRESULT Ready_Components();
	HRESULT Ready_Animation();
	HRESULT Ready_Fsm();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
private:
	HRESULT Ready_Animation_Bee(CTexture* pTextureCom);
	HRESULT Ready_Animation_Deer(CTexture* pTextureCom);
	HRESULT Ready_Animation_Rabbit(CTexture* pTextureCom);
	HRESULT Ready_Animation_Eligos(CTexture* pTextureCom);


private:
	virtual void OnCollisionEnter(CGameObject* pOther);
	virtual void OnCollisionStay(CGameObject* pOther);
	virtual void OnCollisionExit(CGameObject* pOther);
	void    CulculateIsCanWorship( );
private:
	FOLLOWER_INFO m_tInfo;

	CAnimator* m_pAnimatorCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CFsm* m_pFsmCom = { nullptr };
	CFollower_Command_UI* m_pCommandUI = { nullptr };
	class CInteractObject* m_pInteract_Object = { nullptr };	//상호작용할 돌 or 나무

	_float	m_fDistance_with_Player = { 0.f };
	_bool    m_bCanWorship = { false };
	_bool	m_bIsAxe = { false };
	_bool	m_bIsMining = { false };
	_bool	m_bIsPray= { false };
	_bool	m_bCollision_Interact_Object = { false };
	_bool	m_bisIndo = { false };
public:
	static CFollower* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END