start: 
  mov $0A, &0050
loop:
  mov &0050, acc
  dec acc
  mov acc, &0050
  inc r2
  inc r2
  inc r2
  jne $00, &[!loop]
end:
  hlt
