#ifndef __SEQUETASK_H__
#define __SEQUETASK_H__

#ifdef EXTRN
	#undef EXTRN
#endif

#ifdef HERE_EXEC
	#define	EXTRN
	#define	INIT_END	1
#else
	#define	EXTRN extern
	#define	INIT_END	0
#endif

#include "../inc/MarlinConfig.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "Arduino.h"

#endif// __SEQUETASK_H__
