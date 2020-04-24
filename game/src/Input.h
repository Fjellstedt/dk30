/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#ifndef _INPUT_H_
#define _INPUT_H_

namespace Cryptic
{
	class Input
	{
	public:
		Input();
		~Input();

		enum class Frame
		{
			Current,
			Previous,
			_FrameSize,
		};

		enum class Axis
		{
			Mouse,
			_AxisSize,
		};

		struct ButtonState
		{
			B32 isDown = false;
		};

		struct AxisState
		{
			F32 x;
			F32 y;
			F32 z;
		};

		static constexpr int BUTTONSIZE = 256;
		struct InputState
		{
			ButtonState buttons[(U32)(Frame::_FrameSize)][BUTTONSIZE];
			AxisState axes[(U32)(Frame::_FrameSize)][(U32)(Axis::_AxisSize)];
		};

		ButtonState GetButtonState(U32 btn, Frame frame = Frame::Current);

		B32 GetButtonPressed(U32 btn);

		B32 GetButtonReleased(U32 btn);

		B32 GetButtonHeld(U32 btn);

		AxisState GetAxisState(Axis axis, Frame frame = Frame::Current);

		InputState *state;
	};
}
#endif //!_INPUT_H_
