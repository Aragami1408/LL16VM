psh $3333
psh $2222
psh $1111

mov $1234, r1
mov $5678, r4

psh $0000
cal [!my_subroutine]
psh $4444

my_subroutine:
  psh $0102
  psh $0304
  psh $0506

  mov $0708, r1
  mov $090A, r8
  ret

