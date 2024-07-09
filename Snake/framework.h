// header.h: archivo de inclusión para archivos de inclusión estándar del sistema,
// o archivos de inclusión específicos de un proyecto
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Excluir material rara vez utilizado de encabezados de Windows

// Archivos de encabezado de Windows
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <strmif.h>
#include <winnt.h>

// Archivos de encabezado en tiempo de ejecución de C
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>

// C++ standard library
#include <list>
#include <map>
#include <random>

#define MS_TO_100NS(ms) ((REFERENCE_TIME)(ms) * 10000)

REFERENCE_TIME operator-(FILETIME ft1, FILETIME ft2);
FILETIME operator+(FILETIME ft, REFERENCE_TIME rt);