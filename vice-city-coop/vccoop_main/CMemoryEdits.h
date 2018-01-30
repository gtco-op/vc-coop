#pragma once
#include "includes.h"

using namespace Hook;

class CMemoryEdits
{
public:
	void Patch();

	static void Hook_CRunningScript__Process();

	CLogger* l;

	CMemoryEdits(CLogger* log) { l = log; };
	~CMemoryEdits() {};
};

