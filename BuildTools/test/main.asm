global main

extern put_char

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

BRK		