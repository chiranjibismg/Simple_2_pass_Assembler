 ;program to find output of first n natural number             
loop:   ldc num     ; A= addr of Num 
        ldnl 0      ; load num into A ,in this case A=10 
        ldc sum     ; A= addr of sum , B= num 
        ldnl 0      ; load sum into A 
        add         ; A = B + A
        ldc sum     ; load sum
        stnl 0      ; Store new value of sum at data segment 
        ldc num     ; load num
        ldnl 0      ; load num into A again
        adc -1      ; num =num -1 
        brz done    ; if A == 0, stop sum calculation
        ldc num     ; load num 
        stnl 0      ; otherwise update num value
        br loop     ; repeat calculation
done:   
       
        ldl 0       
        HALT      

num: data -10    ; n =10  in this case    
sum: data 0         

; output = 55 (sum of first 10 numbers)
