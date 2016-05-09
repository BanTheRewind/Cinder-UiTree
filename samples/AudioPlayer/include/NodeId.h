#pragma once

#include <stdint.h>

enum : uint64_t
{
	NodeId_Root = 0, 
		NodeId_Waveform, 
		NodeId_Scrub, 
			NodeId_ScrubRect, 
			NodeId_ScrubImage, 
		NodeId_Rewind,
			NodeId_RewindRect, 
			NodeId_RewindImage, 
		NodeId_Play,
			NodeId_PlayRect, 
			NodeId_PlayImage, 
			NodeId_PauseImage, 
		NodeId_Forward,
			NodeId_ForwardRect,
			NodeId_ForwardImage
} typedef NodeId;
 