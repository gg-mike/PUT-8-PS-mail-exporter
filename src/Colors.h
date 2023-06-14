#ifndef __COLORS_H__
#define __COLORS_H__

#define _RST  "\x1B[0m"
#define _RED  "\x1B[31m"
#define _GRN  "\x1B[32m"
#define _YEL  "\x1B[33m"
#define _BLU  "\x1B[34m"
#define _MAG  "\x1B[35m"
#define _CYN  "\x1B[36m"
#define _WHT  "\x1B[37m"

#define RED(x) _RED x _RST
#define GRN(x) _GRN x _RST
#define YEL(x) _YEL x _RST
#define BLU(x) _BLU x _RST
#define MAG(x) _MAG x _RST
#define CYN(x) _CYN x _RST
#define WHT(x) _WHT x _RST

#define BOLD(x) "\x1B[1m" x _RST
#define UNDL(x) "\x1B[4m" x _RST

#define ERROR(x) BOLD(RED(x))

#endif /*__COLORS_H__*/
