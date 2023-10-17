#include <stdio.h>

#include "cpu.h"
#include "instructions.h"
#include "base_helper.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

int main(int argc, char **argv) {
        // Setup SDL
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
        {
                printf("Error: %s\n", SDL_GetError());
                return -1;
        }

        printf("Welcome to LL16VM64. Press Enter to step through instructions, press q to quit\n");

        u8 writable_bytes[] = {
                // mov $5151, r1
                MOV_LIT_REG,
                0x51,
                0x51,
                CPU_REG_R1,
                // mov $4242, 42
                MOV_LIT_REG,
                0x42,
                0x42,
                CPU_REG_R2,

                // psh r1
                PSH_REG,
                CPU_REG_R1,
                // psh r2
                PSH_REG,
                CPU_REG_R2,

                // pop r1
                POP,
                CPU_REG_R1,
                // pop r2
                POP,
                CPU_REG_R2,

        };

        cpu_t *cpu = cpu_init(writable_bytes, ArrayCount(writable_bytes));

        cpu_debug(cpu);
        cpu_viewMemoryAt(cpu, cpu->registers[CPU_REG_IP]);
        cpu_viewMemoryAt(cpu, 0xffff - 1 - 6);

        char c;
        while (1) {
                c = (char) getchar();
                if (c == '\n') {
                        cpu_step(cpu);
                        cpu_debug(cpu);
                        cpu_viewMemoryAt(cpu, cpu->registers[CPU_REG_IP]);
                        cpu_viewMemoryAt(cpu, 0xffff - 1 - 6);
                }
                else if(c == 'q') break;

                while (getchar() != '\n');
        }


        return 0;
}
