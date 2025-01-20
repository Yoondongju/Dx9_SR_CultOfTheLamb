#pragma once

namespace Engine
{
	enum KEY
	{
		LEFT,
		RIGHT,
		UP,
		DOWN,
		Q,
		W,
		E,
		R,
		T,
		Y,
		U,
		I,
		O,
		P,


		A,
		S,
		D,
		F,
		G,
		Z,
		X,
		C,
		V,
		B,

		ALT,
		CTRL,
		LSHIFT,
		SPACE,
		ENTER,
		ESC,

		LBUTTON,
		RBUTTON,
		KEY_TAP,
		ONE,

		LAST,
	};

	enum KEY_STATE
	{
		NONE, // �������ʰ� �������� �ȴ���
		TAP,  // �� ��������
		HOLD, // ������ �ִ�
		AWAY, // �� �� ����

	};

	enum UI_TYPE
	{
		UI_NONE,
		UI_ICON,

		UI_BAR,
		UI_BAR_PREVIEW,
		UI_ELEMENT,

		GUIDE_ROCK_AND_TREE,
		GUIDE_BUILD,
		GUIDE_BUILDING,
		BUTTON,

		NOGOLD,
		UI_END
	};

	enum EVENT_TYPE
	{
		CREATE_OBJECT,
		DELETE_OBJECT,
		SCENE_CHANGE,

		ENVET_END
	};

	enum GROUP_TYPE
	{
		DEFAULT,
		PLAYER,
		MONSTER,
		PROJ_PLAYER,
		PROJ_MONSTER,

		END = 32
	};

	enum COMPONENT_TYPE
	{
		ANIMATOR,
		COLLIDER_CUBE,
		COLLIDER_RECT,
		FSM,
		TEXTURE,
		TRANSFORM,
		VIBUFFER_CUBE,
		VIBUFFER_RECT,
		VIBUFFER_TERRAIN,
		SHADER,
		RIGIDBODY,
		COMPONENT_END
	};


	enum FONT_TYPE 
	{
		SMALL,
		MEDIUM,
		BIG,
		VERY_BIG,
		ULTRA_BIG,
		FONT_END
	};

}