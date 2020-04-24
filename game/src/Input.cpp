/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#include "g_pch.h"
#include "Input.h"
namespace Cryptic
{
	Input::Input()
	{
	}

	Input::~Input()
	{
	}

	Input::ButtonState Input::GetButtonState(U32 btn, Frame frame)
	{
		Input::ButtonState result = state->buttons[(U32)(frame)][(U32)(btn)];
		return result;
	}

	B32 Input::GetButtonPressed(U32 btn)
	{
		bool result = (state->buttons[(U32)(Frame::Current)][btn].isDown &&
					   !state->buttons[(U32)(Frame::Previous)][btn].isDown);
		return result;
	}

	B32 Input::GetButtonReleased(U32 btn)
	{
		bool result = (!state->buttons[(U32)(Frame::Current)][btn].isDown &&
					   state->buttons[(U32)(Frame::Previous)][btn].isDown);
		return result;
	}

	B32 Input::GetButtonHeld(U32 btn)
	{
		bool result = (state->buttons[(U32)(Frame::Current)][btn].isDown &&
					   state->buttons[(U32)(Frame::Previous)][btn].isDown);
		return result;
	}

	Input::AxisState Input::GetAxisState(Axis axis, Frame frame)
	{
		AxisState result = state->axes[(U32)(frame)][(U32)(axis)];
		return result;
	}
}