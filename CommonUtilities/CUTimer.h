/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#ifndef _CU_TIMER_H_
#define _CU_TIMER_H_

class CUTimer
{
public:
	CUTimer();
	~CUTimer();

	void Start();
	void Stop();
	void Reset();
	void Pause();
	void Tick(F32 dt);

	F64 totalTime;
	F64 deltaTime;
	I64 frameCount;
	U32 scale;
private:

	enum class EPlayState
	{
		Started,
		Ticking,
		Paused,
		_Size,
	};

	EPlayState playState;
};

#endif //!_TIMER_H_