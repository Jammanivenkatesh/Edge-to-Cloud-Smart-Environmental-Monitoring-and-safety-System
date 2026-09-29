/* defines.h */

#ifndef _DEFINES_H_
#define _DEFINES_H_

#define SETBIT(WORD,BP) (WORD=WORD|(1<<BP))
#define CLRBIT(WORD,BP) (WORD=(WORD&~(1<<BP)))          // FIXED: Added missing closing )
#define	STATUSBIT(WORD,BP) ((WORD>>BP)&1)
#define WRITEBIT(WORD,BP,BIT) (WORD=(WORD&~(1<<BP))|(BIT<<BP)) // FIXED: Added missing closing )
#define	WRITENIBBLE(WORD,SBP,NIBBLE) (WORD=(WORD&~(0XF<<SBP))|(NIBBLE<<SBP))
#define	WRITEBYTE(WORD,SBP,BYTE) (WORD=(WORD&~(0XFF<<SBP))|(BYTE<<SBP))
#define	READNIBBLE(WORD,BP) ((WORD>>BP)&0XF)
#define	READBYTE(WORD,BP) ((WORD>>BP)&0XFF)
#define	SSETBIT(WORD,BP)	(WORD=1<<BP)
#define	SCLRBIT(WORD,BP)	(WORD=1<<BP)
#define SCPLBIT(WORD,BP) (WORD^=(1<<BP))

#endif
