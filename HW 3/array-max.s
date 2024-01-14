# Write the assembly code for the array_max function

#
# Usage: ./array_max <arg1> <arg2>
#
       
.global array_max

.text

array_max:
  mov %rdi, %r12
  mov %rsi, %r13
  mov $0, %r14  #max value
  mov $0, %r15

loop:
  cmp %r12, %r15
  jge end

  mov (%r13, %r15, 8), %rdx

  cmp %rdx, %r14
  jle less_or_equal
  inc %r15
  jmp loop

less_or_equal:
  mov %rdx, %r14
  inc %r15
  jmp loop

end:
  mov %r14, %rax
  ret
  
.data
