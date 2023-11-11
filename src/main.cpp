#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "cpu.h"
#include "instructions.h"
#include "memory_mapper.h"
#include "ram_device.h"
#include "screen_device.h"
#include "base_helper.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>

#if !SDL_VERSION_ATLEAST(2,0,17)
	#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr int SCREEN_DISPLAY_SIZE = 50;
const int PIXEL_SIZE = SCREEN_WIDTH / SCREEN_DISPLAY_SIZE;

const Uint32 colors[] = {
	0x000000,
	0xFFFFFF,
	0xFF0000,
	0x00FF00,
	0x0000FF
};

const u8 screen_display_buffer[] = {
	0x00, 0x01, 0x02, 0x03, 0x04
};

bool parse_hex_array(char *input, u8 *output) {
	int index = 0;

	char *token = strtok(input, " ");
	while (token != NULL) {
		char *end_ptr;
		long int value = strtol(token, &end_ptr, 16);

		if (*end_ptr != '\0' || value < 0 || value > 255) {
			return false;
		}

		output[index++] = (u8)value;
		token = strtok(NULL, " ");
	}

	return true;
}

int main(int argc, char **argv) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Low-level 16bit Virtual Machine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return 0;
    }

	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      
	io.FontGlobalScale = 1.2f;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	bool done = false;

	memmapper_t *mm = memmapper_create();
	ram_device_t ram = ram_device_init();
	memmapper_map(mm, DEVICE_RAM, &ram, 0x0000, 0xffff, false);

	u8 *writable_bytes = ram.memory;

	cpu_t *cpu = cpu_init(mm);
	
	while (!done) {
		// Input
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
		}

		ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

		{
			ImGui::Begin("Registers");
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "IP: 0x%04X\n", cpu->registers[CPU_REG_IP]);
			ImGui::Text("ACC: 0x%04X\n", cpu->registers[CPU_REG_ACC]);
			ImGui::Text("R1: 0x%04X\n", cpu->registers[CPU_REG_R1]);
			ImGui::Text("R2: 0x%04X\n", cpu->registers[CPU_REG_R2]);
			ImGui::Text("R3: 0x%04X\n", cpu->registers[CPU_REG_R3]);
			ImGui::Text("R4: 0x%04X\n", cpu->registers[CPU_REG_R4]);
			ImGui::Text("R5: 0x%04X\n", cpu->registers[CPU_REG_R5]);
			ImGui::Text("R6: 0x%04X\n", cpu->registers[CPU_REG_R6]);
			ImGui::Text("R7: 0x%04X\n", cpu->registers[CPU_REG_R7]);
			ImGui::Text("R8: 0x%04X\n", cpu->registers[CPU_REG_R8]);
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SP: 0x%04X\n", cpu->registers[CPU_REG_SP]);
			ImGui::Text("FP: 0x%04X\n", cpu->registers[CPU_REG_FP]);
			ImGui::End();
        }

		{
			ImGui::Begin("Working Memory");
			u16 address = 0x00;
			for (int i = 0; i < 20; i++) {
				ImGui::Text("0x%04X: ", address);
				ImGui::SameLine();
				for (int j = 0; j < 8; j++) {
					if (address == cpu->registers[CPU_REG_IP]) {
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "0x%02X", memmapper_readU8(mm, address++));
					}
					else {
						ImGui::Text("0x%02X", memmapper_readU8(mm, address++));
					}
					ImGui::SameLine();
				}
				ImGui::Text("\n");
			}
			ImGui::End();
		}

		{
			ImGui::Begin("Stack Memory");
			u16 address = 0xffff;
			for (int i = 0; i < 20; i++) {
				ImGui::Text("0x%04X: ", address);
				ImGui::SameLine();
				for (int j = 0; j < 8; j++) {
					if (address == cpu->registers[CPU_REG_SP]) {
						ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "0x%02X", memmapper_readU8(mm, address--));
					}
					else {
						ImGui::Text("0x%02X", memmapper_readU8(mm, address--));
					}
					ImGui::SameLine();
				}
				ImGui::Text("\n");	
			}
			ImGui::End();
		}

		{
			ImGui::Begin("PASTE THE PROGRAM HERE");
			static char text[1024] = "";
			static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CtrlEnterForNewLine;
			static bool is_loaded = false;
			ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
			if (ImGui::Button("Load Program")) {
				if (parse_hex_array(text, writable_bytes)) {
					is_loaded = true;
				}
				else {
					is_loaded = false;
				}
			}

			if (is_loaded) ImGui::Text("Program loaded successfully");
			else ImGui::Text("Program failed to load");

			if (ImGui::Button("Step")) {
				cpu_step(cpu);
			}
			ImGui::SameLine();
			if (ImGui::Button("Run")) {

				cpu_run(cpu);
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset")) {
				cpu->registers[CPU_REG_IP] = 0;
			}
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(renderer);

        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
	}

	memmapper_free(mm);
	cpu_free(cpu);

	ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

	return 0;
}
