mov $42, r1
mov r2, r3
mov r1, &3038
mov &3038, r1
mov $3ACD, &1DEA
mov &r1, r2
mov $42, &r1, r2

add $42, r1
add r1, r2
sub $42, r1
sub r1, r2
inc r1
dec r2
mul $42, r1
mul r1, r2

lsf r1, $01
lsf r1, r2
rsf r1, $01
rsf r1, r2
and r1, $42
and r1, r2
or r1, $42
or r1, r2
xor r1, r2
not r3

jne $55A4, &1DEA
jne r2, &1DEA
jeq $55A4, &1DEA
jeq r2, &1DEA
jlt $55A4, &1DEA
jlt r2, &1DEA
jgt $55A4, &1DEA
jgt r2, &1DEA
jle $55A4, &1DEA
jle r2, &1DEA
jge $55A4, &1DEA
jge r2, &1DEA

psh $4200
psh sp
pop r6
cal $1DEA
cal acc
ret 

hlt
