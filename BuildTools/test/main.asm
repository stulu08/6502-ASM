global main

extern put_char

:___c_str_hello_world_
.db #72
.db #69
.db #76
.db #76
.db #79
.db #32
.db #87
.db #79
.db #82
.db #76
.db #68
.db #33
.db #10
.db #00


:main
LDA #72						; 'H'
JSR put_char					

LDA #69						; 'E'
JSR put_char					

LDA #76						; 'L'
JSR put_char					
JSR put_char					

LDA #79						; 'O'		
JSR put_char

LDA #32						; ' '
JSR put_char				

LDA #87						; 'W'
JSR put_char		

LDA #79						; 'O'		
JSR put_char

LDA #82						; 'R'		
JSR put_char

LDA #76						; 'L'
JSR put_char

LDA #68						; 'D'
JSR put_char

LDA #33						; '!'		
JSR put_char

LDA #10						; '\n'		
JSR put_char	

LDA #10


LDA #72
PHA
LDA #0
PLA

BRK		