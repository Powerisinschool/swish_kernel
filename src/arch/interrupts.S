.intel_syntax noprefix

// Macro for interrupts that DO NOT push an error code
.macro ISR_NOERR vector
.global isr\vector
isr\vector:
    push 0          // Push a dummy error code to keep the stack uniform
    push \vector    // Push the interrupt number
    jmp isr_common
.endm

// Macro for CPU exceptions that DO push an error code
.macro ISR_ERR vector
.global isr\vector
isr\vector:
    // The CPU already pushed the error code, so we just push the number
    push \vector
    jmp isr_common
.endm

// CPU Exceptions (0 to 31)
// Exceptions 8, 10, 11, 12, 13, 14, 17, 21, 29, and 30 automatically push error codes.
ISR_NOERR 0
ISR_NOERR 1
// ... (You would typically define all 0-31 here for a full OS)

// Hardware IRQs (We remapped the PIC to start at 32)
ISR_NOERR 32 // IRQ 0: System Timer
ISR_NOERR 33 // IRQ 1: Keyboard
// ... (You can define up to 255)

// The Common Stub: All macros jump here
.extern interrupt_handler
isr_common:
    // 1. Save all general-purpose registers
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    // 2. Pass the stack pointer to C++
    // According to the System V ABI, the first argument to a C function goes in 'rdi'
    mov rdi, rsp

    // 3. Call our C++ handler
    call interrupt_handler

    // 4. Restore all general-purpose registers (in reverse order)
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax

    // 5. Clean up the interrupt number and error code we pushed earlier
    // 2 uint64_t values = 16 bytes. We just move the stack pointer past them.
    add rsp, 16

    // 6. Return from the interrupt
    iretq
