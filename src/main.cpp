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

void draw_memory_at(cpu_t *cpu, u16 address, int rows = 10, int cols = 1) {

	for(int i = 0; i < cols; i++) {
		ImGui::Text("0x%04X: ", address);
		ImGui::SameLine();
		for(int j = 0; j < rows; j++) {
			ImGui::Text("0x%02X", cpu->memory[address++]);
			ImGui::SameLine();
		}	
		ImGui::Text("\n");
	}	
}

void draw_stack(cpu_t *cpu, int rows = 10, int cols = 1) {
	u16 address = 0xffff;
	for(int i = 0; i < cols; i++) {
		ImGui::Text("0x%04X: ", address);
		ImGui::SameLine();
		for(int j = 0; j < rows; j++) {
			ImGui::Text("0x%02X", cpu->memory[address--]);
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
	SDL_Window *window = SDL_CreateWindow("Low Level 16-bit Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
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

	const u16 subroutine_address = 0x3000;
	u8 writable_bytes[256*256] = {
		// psh 0x3333
		PSH_LIT,
		0x33,
		0x33,
		// psh 0x2222
		PSH_LIT,
		0x22,
		0x22,
		// psh 0x1111
		PSH_LIT,
		0x11,
		0x11,

		// mov 0x1234, r1
		MOV_LIT_REG,
		0x12,
		0x34,
		CPU_REG_R1,

		// mov 0x5678, r4
		MOV_LIT_REG,
		0x56,
		0x78,
		CPU_REG_R4,

		// psh 0x0000
		PSH_LIT,
		0x00,
		0x00,

		// cal subroutine_address:
		CAL_LIT,
		(subroutine_address & 0xff00) >> 8,
		(subroutine_address & 0x00ff),

		// psh 0x4444
		PSH_LIT,
		0x44,
		0x44,
	};

	int current_address = subroutine_address;
	// psh 0x0102
	writable_bytes[current_address++] = PSH_LIT;
	writable_bytes[current_address++] = 0x01;
	writable_bytes[current_address++] = 0x02;
	// psh 0x0304
	writable_bytes[current_address++] = PSH_LIT;
	writable_bytes[current_address++] = 0x03;
	writable_bytes[current_address++] = 0x04;
	// psh 0x0506
	writable_bytes[current_address++] = PSH_LIT;
	writable_bytes[current_address++] = 0x05;
	writable_bytes[current_address++] = 0x06;
	// mov 0x0708, r1
	writable_bytes[current_address++] = MOV_LIT_REG;
	writable_bytes[current_address++] = 0x07;
	writable_bytes[current_address++] = 0x08;
	writable_bytes[current_address++] = CPU_REG_R1;
	// mov 0x090A, r8
	writable_bytes[current_address++] = MOV_LIT_REG;
	writable_bytes[current_address++] = 0x09;
	writable_bytes[current_address++] = 0x0A;
	writable_bytes[current_address++] = CPU_REG_R8;
	// ret
	writable_bytes[current_address++] = RET;
	

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
			ImGui::Begin("CPU Debugger");
				ImGui::SeparatorText("CPU REGISTERS:");
				ImGui::BulletText("IP: 0x%04X\n", cpu->registers[CPU_REG_IP]);
				ImGui::BulletText("ACC: 0x%04X\n", cpu->registers[CPU_REG_ACC]);
				ImGui::BulletText("R1: 0x%04X\n", cpu->registers[CPU_REG_R1]);
				ImGui::BulletText("R2: 0x%04X\n", cpu->registers[CPU_REG_R2]);
				ImGui::BulletText("R3: 0x%04X\n", cpu->registers[CPU_REG_R3]);
				ImGui::BulletText("R4: 0x%04X\n", cpu->registers[CPU_REG_R4]);
				ImGui::BulletText("R5: 0x%04X\n", cpu->registers[CPU_REG_R5]);
				ImGui::BulletText("R6: 0x%04X\n", cpu->registers[CPU_REG_R6]);
				ImGui::BulletText("R7: 0x%04X\n", cpu->registers[CPU_REG_R7]);
				ImGui::BulletText("R8: 0x%04X\n", cpu->registers[CPU_REG_R8]);
				ImGui::BulletText("SP: 0x%04X\n", cpu->registers[CPU_REG_SP]);
				ImGui::BulletText("FP: 0x%04X\n", cpu->registers[CPU_REG_FP]);

				ImGui::SeparatorText("INSTRUCTIONS:");
				draw_memory_at(cpu, cpu->registers[CPU_REG_IP], 10, 5);					

				ImGui::SeparatorText("STACK:");
				// draw_memory_at(cpu, 0xffff - 1 - 42, 44);					
				draw_stack(cpu, 10, 5);

				ImGui::SeparatorText("COMMANDS:");
				bool step_btn = ImGui::Button("Step");
				ImGui::SetItemTooltip("You can also press [Spacebar]");	
				if (step_btn) {
					cpu_step(cpu);
				}
				
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
