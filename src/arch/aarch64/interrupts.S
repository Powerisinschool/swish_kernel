.global vector_table
.align 11

.macro SAVE_CONTEXT
    // Allocate 272 bytes on the stack (enough for 34 registers)
    sub sp, sp, #272

    // Store all general-purpose registers in pairs
    stp x0, x1, [sp, #0]
    stp x2, x3, [sp, #16]
    stp x4, x5, [sp, #32]
    stp x6, x7, [sp, #48]
    stp x8, x9, [sp, #64]
    stp x10, x11, [sp, #80]
    stp x12, x13, [sp, #96]
    stp x14, x15, [sp, #112]
    stp x16, x17, [sp, #128]
    stp x18, x19, [sp, #144]
    stp x20, x21, [sp, #160]
    stp x22, x23, [sp, #176]
    stp x24, x25, [sp, #192]
    stp x26, x27, [sp, #208]
    stp x28, x29, [sp, #224]

    // We cannot push system registers directly to the stack.
    // We must read them into temporary general-purpose registers first.
    mrs x21, elr_el1
    mrs x22, spsr_el1

    // Store x30 (The Link Register) and ELR_EL1 together
    stp x30, x21, [sp, #240]

    // Store SPSR_EL1. We use xzr (the Zero Register) as a dummy
    // to fill the remaining 8 bytes and keep our 16-byte alignment.
    stp x22, xzr, [sp, #256]
.endm

.macro RESTORE_CONTEXT
    // Pull the system registers off the stack first
    ldp x22, xzr, [sp, #256]
    ldp x30, x21, [sp, #240]

    // Write them back into the CPU hardware state
    msr spsr_el1, x22
    msr elr_el1, x21

    // Restore all general-purpose registers
    ldp x28, x29, [sp, #224]
    ldp x26, x27, [sp, #208]
    ldp x24, x25, [sp, #192]
    ldp x22, x23, [sp, #176]
    ldp x20, x21, [sp, #160]
    ldp x18, x19, [sp, #144]
    ldp x16, x17, [sp, #128]
    ldp x14, x15, [sp, #112]
    ldp x12, x13, [sp, #96]
    ldp x10, x11, [sp, #80]
    ldp x8, x9, [sp, #64]
    ldp x6, x7, [sp, #48]
    ldp x4, x5, [sp, #32]
    ldp x2, x3, [sp, #16]
    ldp x0, x1, [sp, #0]

    // Free the stack space by moving the pointer back up
    add sp, sp, #272
.endm

// This is the stub we map out in the vector table block
handle_irq_current_el_spx:
    // 1. Freeze the CPU state and save it to the stack
    SAVE_CONTEXT

    // 2. Put the stack pointer into x0.
    // This allows your C++ interrupt_handler to read the saved registers.
    mov x0, sp

    // 3. Branch and Link (bl) to your C++ function
    bl interrupt_handler

    // 4. The C++ function finished. Restore the CPU state.
    RESTORE_CONTEXT

    // 5. Exception Return (eret). This tells the CPU to resume normal execution.
    eret

vector_table:
    .align 7
    b handle_sync_current_el_sp0
    .align 7
    b handle_irq_current_el_sp0
    .align 7
    b handle_fiq_current_el_sp0
    .align 7
    b handle_serror_current_el_sp0

    .align 7
    b handle_sync_current_el_spx
    .align 7
    b handle_irq_current_el_spx
    .align 7
    b handle_fiq_current_el_spx
    .align 7
    b handle_serror_current_el_spx

    .align 7
    b handle_sync_lower_el_aarch64
    .align 7
    b handle_irq_lower_el_aarch64
    .align 7
    b handle_fiq_lower_el_aarch64
    .align 7
    b handle_serror_lower_el_aarch64

    .align 7
    b handle_sync_lower_el_aarch32
    .align 7
    b handle_irq_lower_el_aarch32
    .align 7
    b handle_fiq_lower_el_aarch32
    .align 7
    b handle_serror_lower_el_aarch32
