long syscall(long number, long arg1, long arg2, long arg3) {
  long res;
  asm volatile("syscall"
               : "=a"(res)    // Output: rax will contain the return value
               : "a"(number), // Input: syscall number in rax
                 "D"(arg1),   // First argument in rdi
                 "S"(arg2),   // Second argument in rsi
                 "d"(arg3)    // Third argument in rdx
               : "rcx", "r11", "memory" // Clobbered registers
  );
  return res;
}
