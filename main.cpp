#include <iostream>
#include <string>

#include "cpuCore.h"
#include "mmu.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "unable to locate rom" << "\n";
        return 1;
    }

    std::string rom_name = argv[1];

    MMU mmu{};
    mmu.load_rom(rom_name);
    CpuCore cpu(mmu);
    cpu.reset();

    unsigned int cycle{};

    while (1) {
        cpu.step(cycle);
    }

    return 0;
}