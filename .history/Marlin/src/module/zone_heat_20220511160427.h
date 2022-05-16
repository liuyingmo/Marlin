 #pragma once

 #ifdef EXTRN
	#undef EXTRN
#endif

#ifdef to_exec_file
	#define	EXTRN
	#define	INIT_END	1
#else
	#define	EXTRN extern
	#define	INIT_END	0
#endif
