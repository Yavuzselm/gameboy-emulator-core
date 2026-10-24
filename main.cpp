#include <windows.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "cpuCore.h"
#include "joypad.h"
#include "mmu.h"
#include "ppu.h"
#include "renderer.h"
#include "timers.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "unable to locate rom" << "\n";
        return 1;
    }

    timeBeginPeriod(1);

    std::string rom_name = argv[1];

    PPU ppu{};
    Timers timers{};
    Joypad joypad{};
    MMU mmu(timers, ppu, joypad, rom_name);
    CpuCore cpu(mmu);
    cpu.reset();

    Renderer renderer{};

    if (!renderer.initiate()) {
        std::cerr << "unable to initiate the renderer";
        return -1;
    }

    auto previous_time = std::chrono::steady_clock::now();
    std::chrono::microseconds acumulator{};
    constexpr std::chrono::microseconds target_frame_time(16742);
    unsigned int total_cycle{};

    bool is_running{true};

    while (is_running) {
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(current_time - previous_time);
        previous_time = current_time;
        acumulator += elapsed_time;

        if (acumulator >= target_frame_time) {
            joypad.handle_input(is_running);
            while (total_cycle <= 70224) {
                unsigned int cycle{};
                cpu.step(cycle);
                total_cycle += cycle;

                if (timers.step_timers(cycle)) {
                    mmu.request_interrupt(0x04);
                }

                uint8_t ppu_interrupt = ppu.step(cycle);
                if (ppu_interrupt != 0) {
                    mmu.request_interrupt(ppu_interrupt);
                }
            }

            total_cycle -= 70224;
            acumulator -= target_frame_time;
            renderer.render(ppu);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    timeEndPeriod(1);
    return 0;
}