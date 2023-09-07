00000000  ;program to find output of first n natural number             
00000000 00001000 loop:   ldc num     ; A= addr of Num  
00000001 00000004         ldnl 0      ; load num into A ,in this case A=10  
00000002 00001100         ldc sum     ; A= addr of sum , B= num  
00000003 00000004         ldnl 0      ; load sum into A  
00000004 00000006         add         ; A = B + A 
00000005 00001100         ldc sum     ; load sum 
00000006 00000005         stnl 0      ; Store new value of sum at data segment  
00000007 00001000         ldc num     ; load num 
00000008 00000004         ldnl 0      ; load num into A again 
00000009 ffffff01         adc -1      ; num =num -1  
0000000a 0000030f         brz done    ; if A == 0, stop sum calculation 
0000000b 00001000         ldc num     ; load num  
0000000c 00000005         stnl 0      ; otherwise update num value 
0000000d fffff211         br loop     ; repeat calculation 
0000000e done:   
0000000e        
0000000e 00000002         ldl 0        
0000000f 00000012         HALT       
00000010 
00000010 fffffff6 num: data -10    ; n =10  in this case     
00000011 00000000 sum: data 0          
00000012 
00000012 ; output = 55 (sum of first 10 numbers)
