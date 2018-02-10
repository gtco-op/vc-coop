#include "main.h"
#include <d3d9.h>

CCore *core = nullptr;

class VCCoop 
{
public:
	VCCoop() 
	{
		core = new CCore();
	}
	~VCCoop() 
	{	
		delete core;
	}
} vcCoop;