# Write the assembly code for the main of the compare program

#
# Usage: ./compare-main <arg1> <arg2>
#


# Make `main` accessible outside of this module
.global main

.text

# int main(int argc, char argv[][])
main:
  # Function prologue
  enter $0, $0

  # checking to see how many variables where given to the function
  cmp $3, %rdi
  jne arg_num_issue
  
  # Variable mappings:
  # arg1 -> %r13
  # arg2 -> %r14
  movq 8(%rsi), %r13  # arg1 = argv[1]
  movq 16(%rsi), %r14 # arg2 = argv[2]

  # Converting the 1st operand to long int
  mov %r13, %rdi
  mov $0, %al
  call atol
  mov %rax, %r13
  mov $0, %rax

  # Converting the 2nd operand to long int
  mov %r14, %rdi
  mov $0, %al
  call atol
  mov %rax, %r14

  mov %r13, %rdi
  mov %r14, %rsi
  mov $0, %al
  call compare
  mov %rax, %r14

  cmp $0, %r14
  je result_equal

  cmp $-1, %r14
  je result_less

  cmp $1, %r14
  je result_greater
 
  # Function epilogue
  mov $fund_error, %rdi
  mov $0, %al
  call printf
  leave
  ret

result_equal:
  movq $equal, %rdi
  jmp end

result_less:
  movq $less, %rdi
  jmp end

result_greater:
  movq $greater, %rdi
  jmp end

arg_num_issue:
  movq $error, %rdi
  mov $0, %al
  call printf
  #mov eax, 1
  leave
  ret

end:
  mov $0, %al
  call printf
  leave
  ret

.data

equal:
  .asciz "equal\n"

less:
  .asciz "less\n"

greater:
  .asciz "greater\n"

error:
  .asciz "Two arguments required\n"

fund_error:
   .asciz "Arguments not sorted correctly\n"
