#include <windows.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "cpuCore.h"
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
    MMU mmu(timers, ppu, rom_name);
    CpuCore cpu(mmu);
    cpu.reset();

    Renderer renderer{};

    if (!renderer.initiate()) {
        std::cerr << "unable to initiate the renderer";
        return -1;
    }

    constexpr std::chrono::microseconds target_frame_time(16742);
    unsigned int total_cycle{};

    while (1) {
        auto frame_begin = std::chrono::steady_clock::now();

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
        renderer.render(ppu);

        auto frame_end = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(frame_end - frame_begin);

        if (elapsed_time < target_frame_time) {
            auto to_wait_time = target_frame_time - elapsed_time;

            if (to_wait_time > std::chrono::milliseconds(4)) {
                std::this_thread::sleep_for(to_wait_time - std::chrono::milliseconds(4));
            }

            while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() -
                                                                         frame_begin) < target_frame_time) {
            }

            auto full_frame =
                std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - frame_begin);
        }
    }
    timeEndPeriod(1);
    return 0;
}