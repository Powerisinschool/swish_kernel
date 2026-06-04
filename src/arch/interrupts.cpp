#include <arch/interrupts.h>
#include <terminal.h>
#include <drivers/pic.h>
#include <drivers/keyboard.h>

extern "C" void interrupt_handler(struct cpu_status* regs)
{
    // A simple switch statement to route the events
    switch (regs->int_no)
    {
        case 0:
            cout << "Division by Zero" << "\r\n";
            PIC::send_eoi(0);
            break;

        case 8: // Double Fault
            cout << "Double Fault!\r\n";
            // Halt execution completely
            // ReSharper disable once CppDFAEndlessLoop
            while (true) {
                __asm__ volatile("hlt");
            }
            break;

        case 32: // System Timer (PIT)
            PIC::send_eoi(0);
            break;

        case 33: // Keyboard Interrupt
            Keyboard::handle_interrupt();
            // cout << "Keyboard Interrupted!\r\n";
            PIC::send_eoi(1);
            break;

        default:
            // Unhandled interrupt
            if (regs->int_no < 32) {
                cout << "CPU Exception Caught: " << static_cast<int64_t>(regs->int_no) << "\r\n";
                // Halt the CPU on critical exceptions
                // ReSharper disable once CppDFAEndlessLoop
                while (true) {
                    __asm__ volatile("hlt");
                }
            }
            break;
    }
}
