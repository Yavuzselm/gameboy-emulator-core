#include <iostream>
#include <string>

#include "cpuCore.h"
#include "mmu.h"
#include "timers.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "unable to locate rom" << "\n";
        return 1;
    }

    std::string rom_name = argv[1];

    Timers timers{};
    MMU mmu(timers);

    mmu.load_rom(rom_name);
    CpuCore cpu(mmu);
    cpu.reset();

    while (1) {
        unsigned int cycle{};

        cpu.step(cycle);
        if (timers.step_timers(cycle)) {
            mmu.request_interrupt(0x04);
        }
    }

    return 0;
}