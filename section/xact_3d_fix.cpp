#include "global.h"

void __cdecl fix__XACTApplyFailed(const char *fmt, const char *reason)
{
	static bool once = true;
	if (once)
	{
		WarningF(fmt, reason);
		once = false;
	}
}