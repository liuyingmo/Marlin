 #pragma once

 #ifdef EXTRN
	#undef EXTRN
#endif

#ifdef here_to_exec
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
#include <SoftwareSerial.h>

EXTRN  const uint32_t MaskBits[32]
#if INIT_END
= {
	0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
	(0x1UL << 8), (0x1UL << 9), (0x1UL << 10), (0x1UL << 11),
	(0x1UL << 12), (0x1UL << 13), (0x1UL << 14), (0x1UL << 15),
	(0x1UL << 16), (0x1UL << 17), (0x1UL << 18), (0x1UL << 19),
	(0x1UL << 20), (0x1UL << 21), (0x1UL << 22), (0x1UL << 23),
	(0x1UL << 24), (0x1UL << 25), (0x1UL << 26), (0x1UL << 27),
	(0x1UL << 28), (0x1UL << 29), (0x1UL << 30), (0x1UL << 31),
}
#endif
;
