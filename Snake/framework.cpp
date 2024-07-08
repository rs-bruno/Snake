#include "framework.h"

REFERENCE_TIME operator-(FILETIME ft1, FILETIME ft2)
{
	ULARGE_INTEGER op1, op2;
	op1.LowPart = ft1.dwLowDateTime;
	op1.HighPart = ft1.dwHighDateTime;
	op2.LowPart = ft2.dwLowDateTime;
	op2.HighPart = ft2.dwHighDateTime;
	return op1.QuadPart - op2.QuadPart;
}
FILETIME operator+(FILETIME ft, REFERENCE_TIME rt)
{
	ULARGE_INTEGER op;
	op.LowPart = ft.dwLowDateTime;
	op.HighPart = ft.dwHighDateTime;
	op.QuadPart += rt;
	ft.dwLowDateTime = op.LowPart;
	ft.dwHighDateTime = op.HighPart;
	return ft;
}