          .entry STRADD
          .entry MAIN
          .extern REVERSE
          .extern PRTSTR
          .extern COUNT
STRADD:   .data   0
STR:      .string "abcdef"
LASTCHAR: .data   0
LEN:      .data   0
k:        .data   0


MAIN:	lea/0,1   STR{*LEN}, STRADD
		
jsr/0,0 COUNT
		
jsr/0,0 PRTSTR
		
mov/1/1/0,0 STRADD{5},LASTCHAR{r3}
		
mov/1/1/1,0 STR{7}, r7
		
add/0,0 COUNT{*k},r3
		
dec/1/1/1,0 LASTCHAR{*k}
		
inc/1,1 k
		
jsr/0,0 REVERSE
		
jsr/0,0 PRTSTR
		
stop/0,0
