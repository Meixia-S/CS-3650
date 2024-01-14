#
# Usage: ./calculator <op> <arg1> <arg2>
#
        
# Make `main` accessible outside of this module
.global main

.text

# int main(int argc, char argv[][])
main:
  # Function prologue
  enter $0, $0

  # Variable mappings:
  # op -> %r12
  # arg1 -> %r13
  # arg2 -> %r14
  movq 8(%rsi), %r12  # op = argv[1]
  movq 16(%rsi), %r13 # arg1 = argv[2]
  movq 24(%rsi), %r14 # arg2 = argv[3]

  # Converting the 1st operand to long int
  mov %r13, %rdi
  call atol
  mov %rax, %r13
  mov $0, %rax

  # Converting the 2nd operand to long int
  mov %r14, %rdi
  call atol
  mov %rax, %r14

  # Copying the first char of op into an 8-bit register
  # i.e., op_char = op[0] - something like mov 0(%r12), bl
  mov 0(%r12), %bl 

  cmp $'+, %bl
  je addition

  cmp $'-, %bl
  je subtraction

  cmp $'*, %bl
  je multiplication

  cmp $'/, %bl
  je division

  mov $error, %rdi
  mov $0, %al
  call printf
 
  # Function epilogue
  leave
  ret

addition:
  add %r13, %r14
  mov %r14, %rsi
  jmp end

subtraction:
  sub %r14, %r13
  mov %r13, %rsi
  jmp end

multiplication:
  imul %r14, %r13
  mov %r13, %rsi
  jmp end

division:
  cmp $0, %r14
  je divzero

  mov %r13, %rax
  cqto
  idiv %r14
  mov %rax, %rsi
  jmp end

divzero:
  mov $divid_by_zero, %rdi
  mov $0, %al
  call printf
  mov $0, %rax
  leave
  ret
  
end:
  movq $format, %rdi
  mov $0, %al
  call printf
  mov $0, %rax
  leave
  ret
  
.data

format:
  .asciz "%ld\n"
  
error:
  .asciz "Unknown operation\n"

divid_by_zero:
  .asciz "Cannot divid by zero\n"
