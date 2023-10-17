#include <stdio.h>

#include "cpu.h"
#include "instructions.h"
#include "base_helper.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

void run_code() {
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
		c = (char)getchar();
		if (c == '\n') {
			cpu_step(cpu);
			cpu_debug(cpu);
			cpu_viewMemoryAt(cpu, cpu->registers[CPU_REG_IP]);
			cpu_viewMemoryAt(cpu, 0xffff - 1 - 6);
		}
		else if (c == 'q')
		break;

		while (getchar() != '\n');
	}
}

void view_memory_at(cpu_t *cpu, u16 address, int lines = 1) {

	for(int i = 0; i < lines; i++) {
		ImGui::Text("0x%04X: ", address);
		ImGui::SameLine();
		for(int j = 0; j < 10; j++) {
			ImGui::Text("0x%02X", cpu->memory[address++]);
			ImGui::SameLine();
		}	
		ImGui::Text("\n");
	}	
}

int main(int argc, char **argv) {
	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window *window = SDL_CreateWindow("Low Level 16-bit Virtual Machine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		SDL_Log("Error creating SDL_Renderer!");
		return 0;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.0f);

	bool done = false;
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

	while (!done) {
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
			ImGui::Begin("CPU Registers");
			ImGui::Text("IP: 0x%04X\n", cpu->registers[CPU_REG_IP]);
			ImGui::Text("ACC: 0x%04X\n", cpu->registers[CPU_REG_ACC]);
			ImGui::Text("R1: 0x%04X\n", cpu->registers[CPU_REG_R1]);
			ImGui::Text("R2: 0x%04X\n", cpu->registers[CPU_REG_R2]);
			ImGui::Text("R3: 0x%04X\n", cpu->registers[CPU_REG_R3]);
			ImGui::Text("R4: 0x%04X\n", cpu->registers[CPU_REG_R4]);
			ImGui::Text("R5: 0x%04X\n", cpu->registers[CPU_REG_R5]);
			ImGui::Text("R6: 0x%04X\n", cpu->registers[CPU_REG_R6]);
			ImGui::Text("R7: 0x%04X\n", cpu->registers[CPU_REG_R7]);
			ImGui::Text("R8: 0x%04X\n", cpu->registers[CPU_REG_R8]);
			ImGui::Text("SP: 0x%04X\n", cpu->registers[CPU_REG_SP]);
			ImGui::Text("FP: 0x%04X\n", cpu->registers[CPU_REG_FP]);
			ImGui::End();
		}

		{
			ImGui::Begin("Memory");
				view_memory_at(cpu, cpu->registers[CPU_REG_IP]);					
			ImGui::End();
		}

		{
			ImGui::Begin("Stack");
				view_memory_at(cpu, 0xffff - 1 - 6);					
			ImGui::End();
		}

	ImGui::Render();
		SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(renderer);
	}

	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();


	return 0;
}
