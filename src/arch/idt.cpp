#include <arch/idt.h>
#include <stdint.h>

extern "C" void isr0();
extern "C" void isr8();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr32();
extern "C" void isr33();

static idt_entry idt[256];
static idt_ptr idtr;

idt_flags::idt_flags(const uint8_t gate_type, const uint8_t dpl, const bool present) : gate_type(gate_type), storage(0), dpl(dpl), present(present) {}

idt_flags::idt_flags(const uint8_t val) {
    gate_type = val & 0x0F;             // Bottom 4 bits
    storage   = (val >> 4) & 0x01;      // 5th bit
    dpl       = (val >> 5) & 0x03;      // 6th and 7th bits
    present   = (val >> 7) & 0x01;      // 8th bit
}

idt_flags idt_flags::kernel_interrupt() {
    return idt_flags { (uint8_t) 0xE, (uint8_t) 0, true, };
}

idt_flags idt_flags::user_trap() {
    return idt_flags { (uint8_t) 0xF, (uint8_t) 3, true, };
}

idt_flags::operator uint8_t() const {
    const union {
        idt_flags flags;
        uint8_t raw;
    } converter = { *this };
    return converter.raw;
}

idt_flags &idt_flags::operator=(const uint8_t &value) {
    this->gate_type = value & 0xF;
    this->storage = 0;
    this->dpl = (value >> 5) & 0x3;
    this->present = value >> 7;
    return *this;
}

namespace IDT {
    void set_descriptor(const uint8_t vector, void *isr, const idt_flags flags) {
        const auto descriptor = reinterpret_cast<uint64_t>(isr);

        idt[vector].isr_low = descriptor & 0xFFFF; // 0b1111'1111'1111'1111;
        idt[vector].kernel_cs = 0x28; // limine
        idt[vector].ist = 0;
        idt[vector].reserved0 = 0;
        idt[vector].attributes = flags;
        idt[vector].isr_mid = (descriptor >> 16) & 0xFFFF; // 0b1111'1111'1111'1111;
        idt[vector].isr_high = (descriptor >> 32) & 0xFFFF'FFFF; // 0b1111'1111'1111'1111'1111'1111'1111'1111;
        idt[vector].reserved1 = 0;
    }

    void initialize() {
        // loop and set descriptors
        // for (uint8_t i = 0; i < 32; i++) { // TODO: remove loop and implement direct calls
        //     set_descriptor(i, reinterpret_cast<void*>(isr_base), idt_flags{0});
        // }
        set_descriptor(0, reinterpret_cast<void*>(isr0), idt_flags::kernel_interrupt());
        set_descriptor(8, reinterpret_cast<void*>(isr8), idt_flags::kernel_interrupt());
        set_descriptor(13, reinterpret_cast<void*>(isr13), idt_flags::kernel_interrupt());
        set_descriptor(14, reinterpret_cast<void*>(isr14), idt_flags::kernel_interrupt());

        set_descriptor(32, reinterpret_cast<void*>(isr32), idt_flags::kernel_interrupt()); // Timer
        set_descriptor(33, reinterpret_cast<void*>(isr33), idt_flags::kernel_interrupt()); // Keyboard

        idtr.limit = (sizeof(idt_entry) * 256) - 1;
        idtr.base = reinterpret_cast<uint64_t>(&idt);

        __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the pointer into the CPU register
        __asm__ volatile ("sti"); // tell the CPU to start listening for interrupts
    }
}
