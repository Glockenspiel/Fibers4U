#ifndef PLAYER_H
#define PLAYER_H

namespace fbr
{
	// This class is exported from the MathFuncsDll.dll
	class Player
	{
	public:
		__declspec(dllexport) Player();
		__declspec(dllexport) void printNum(int a);
	};
} 

#endif