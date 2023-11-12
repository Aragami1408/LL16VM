#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "SDL_messagebox.h"
#include "SDL_render.h"
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

constexpr int SCREEN_WIDTH = 1600;
constexpr int SCREEN_HEIGHT = 900;

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

static void panic_and_abort(const char *title, const char *text) {
	fprintf(stderr, "PANIC: %s ... %s\n", title, text);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, NULL);
	SDL_Quit();
	exit(1);
}

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
} render_state_t;

int init_sdl(render_state_t *rs) {

#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        panic_and_abort("SDL_Init failed", SDL_GetError());
        return -1;
    }

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);
	rs->window = SDL_CreateWindow(
			"Low-level 16bit Virtual Machine", 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			SCREEN_WIDTH, SCREEN_HEIGHT, 
			window_flags);
	if (rs->window == nullptr) {
		panic_and_abort("SDL_CreateWindow failed", SDL_GetError());
		return -1;
	}

	SDL_RendererFlags render_flags = (SDL_RendererFlags)(SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	rs->renderer = SDL_CreateRenderer(rs->window, -1, render_flags);
	if (rs->renderer == nullptr) {
		panic_and_abort("SDL_CreateRenderer failed", SDL_GetError());
		return -1;	
	}

	rs->texture = SDL_CreateTexture(rs->renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, 32, 32);
	if (rs->texture == nullptr) {
		panic_and_abort("SDL_CreateTexture failed", SDL_GetError());
	}

	return 0;
}

void free_sdl(render_state_t *rs) {
	SDL_DestroyTexture(rs->texture);
	SDL_DestroyRenderer(rs->renderer);
	SDL_DestroyWindow(rs->window);
	SDL_Quit();
}

int main(int argc, char **argv) {

	render_state_t rs;

	init_sdl(&rs);

	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      
	io.FontGlobalScale = 1.4f;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForSDLRenderer(rs.window, rs.renderer);
    ImGui_ImplSDLRenderer2_Init(rs.renderer);

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
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(rs.window))
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
					u8 current_value = memmapper_readU8(mm, address);
					if (address == cpu->registers[CPU_REG_IP]) {
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "0x%02X", current_value);
					}
					else if (current_value != 0x00) {
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "0x%02X", current_value);
					}
					else {
						ImGui::Text("0x%02X", current_value);
					}
					ImGui::SameLine();
					address++;
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
					u8 current_value = memmapper_readU8(mm, address);
					if (current_value != 0x00) {
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "0x%02X", current_value);
					}
					else if (address == cpu->registers[CPU_REG_SP]) {
						ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "0x%02X", current_value);
					}
					else {
						ImGui::Text("0x%02X", current_value);
					}
					ImGui::SameLine();
					address--;
				}
				ImGui::Text("\n");	
			}
			ImGui::End();
		}

		{
			ImGui::Begin("Execution");
			static char program_filename[32] = ""; ImGui::InputText("Program Filename", program_filename, 32);	
			static char program_buffer[1024] = "Program Source Code will show here";
			
			static bool is_loaded = false;
			static bool run_successfully = false;
			if (ImGui::Button("Load Program")) {
				FILE *program_file = fopen(program_filename, "r");
				if (program_file == NULL) {
					is_loaded = false;	
				}
				else {
					// Read program file to program buffer
					fseek(program_file, 0, SEEK_END);
					long length = ftell(program_file);
					fseek(program_file, 0, SEEK_SET);
					strncpy(program_buffer, "", 1024);
					fread(program_buffer, 1, (size_t) length, program_file);
					fclose(program_file);

					// Read process output to hex output buffer
					FILE *command_process;
					char command[50];
					char str_output[1024];
					snprintf(command, 50, "./ll16vm_assembler %s", program_filename);
					command_process = popen(command, "r");
					if (!command_process) {
						run_successfully = false;
					}
					else {
						while(fgets(str_output, 1024, command_process) != NULL) {}
						if (parse_hex_array(str_output, ram.memory)) {
							run_successfully = true;
						}
						else {
							run_successfully = false;
						}
					}
					pclose(command_process);

					is_loaded = true;
				}
			}

			ImGui::InputTextMultiline("##source", program_buffer, IM_ARRAYSIZE(program_buffer), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_ReadOnly);

			if (is_loaded) ImGui::Text("Program loaded successfully");
			else ImGui::Text("Program failed to load");

			if (run_successfully) ImGui::Text("Program executed successfully");
			else ImGui::Text("Program failed to execute");

			if (ImGui::Button("Step")) {
				cpu_step(cpu);
			}
			ImGui::SameLine();
			if (ImGui::Button("Run")) {

				cpu_run(cpu);
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset")) {
				cpu_reset(cpu);
			}
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		SDL_RenderSetScale(rs.renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(rs.renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(rs.renderer);

        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(rs.renderer);
	}

	memmapper_free(mm);
	cpu_free(cpu);

	ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

	free_sdl(&rs);

	return 0;
}
