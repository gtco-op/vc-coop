#ifndef _C_INPUTMANAGER_H_DEFINED
#define _C_INPUTMANAGER_H_DEFINED

#include "includes.h"
#include "CLogger.h"
#include "CGUI.h"

class CInputManager
{
public:
	CInputManager();
	~CInputManager();

	CGUI* g;
	CLogger *l;
};

#endif
