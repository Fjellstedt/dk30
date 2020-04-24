/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#include "cu_pch.h"
#include "CUTimer.h"

CUTimer::CUTimer():
	deltaTime(0), totalTime(0), frameCount(0), scale(1), playState(EPlayState::Paused)
{

}

CUTimer::~CUTimer()
{
}

void CUTimer::Start()
{
	playState = EPlayState::Started;
}

void CUTimer::Stop()
{
	playState = EPlayState::Paused;
	Reset();
}

void CUTimer::Reset()
{
	totalTime = 0;
	frameCount = 0;
}

void CUTimer::Pause()
{
	playState = EPlayState::Paused;
}

void CUTimer::Tick(F32 dt)
{
	if (playState == EPlayState::Started)
	{
		playState = EPlayState::Ticking;
	}
	else if (playState == EPlayState::Ticking)
	{
		totalTime += dt;
		++frameCount;
	}
}
