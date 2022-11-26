#include "Debugger.h"

Debugger::Debugger() {
  // Set OpenGL version
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  // Create Window with OpenGl context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(
      SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
  window_ = SDL_CreateWindow("CHIP-8 Debugger", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, 600, 900, window_flags);
  if (window_ == nullptr)
    throw std::string(
        fmt::format("Failure to create window: {}", SDL_GetError()));

  gl_context_ = SDL_GL_CreateContext(window_);
  if (gl_context_ == nullptr)
    throw std::string(
        fmt::format("Failure to create OpenGL context: {}", SDL_GetError()));

  SDL_GL_MakeCurrent(window_, gl_context_);
  SDL_GL_SetSwapInterval(1);  // Enables V-Sync

  // Setup ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io_ = &ImGui::GetIO();

  // Setup ImGui style
  ImGui::StyleColorsDark();

  // Setup renderer backends
  ImGui_ImplSDL2_InitForOpenGL(window_, gl_context_);
  ImGui_ImplOpenGL3_Init(glsl_version_);

  ImGui::SetNextWindowSize(ImVec2(600, 900), ImGuiCond_FirstUseEver);
}

void Debugger::place_widgets(CPU* cpu) {
  ImGui::Begin("Debugger", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

  // Display registers
  if (ImGui::CollapsingHeader("Registers")) {
    if (ImGui::BeginTable("register_table", 2)) {
      for (uint8_t i = 0; i < 16; i += 2) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text(fmt::format("V{}: {:#x}", i, cpu->V.at(i)).c_str());

        ImGui::TableSetColumnIndex(1);
        ImGui::Text(fmt::format("V{}: {:#x}", i+1, cpu->V.at(i+1)).c_str());
      }

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Separator();
      ImGui::TableSetColumnIndex(1);
      ImGui::Separator();

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text(fmt::format("delay timer: {:#x}", cpu->delay_timer).c_str());

      ImGui::TableSetColumnIndex(1);
      ImGui::Text(fmt::format("sound timer: {:#x}", cpu->sound_timer).c_str());

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text(fmt::format("draw flag: {:#x}", cpu->draw_flag).c_str());

      ImGui::TableSetColumnIndex(1);
      ImGui::Text(fmt::format("I: {:#x}", cpu->index_register).c_str());

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text(fmt::format("PC: {:#x}", cpu->program_counter).c_str());

      ImGui::TableSetColumnIndex(1);
      ImGui::Text(fmt::format("SP: {:#x}", cpu->stack_ptr).c_str());

      ImGui::EndTable();
    }

  }

  // Display memory
  if (ImGui::CollapsingHeader("Memory")) {
    ImGui::BeginChild("Scrolling");
    if (ImGui::BeginTable("memory_table", 3))
    {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Address");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("Opcode");
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("Disassembled Opcode");

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Separator();
      ImGui::TableSetColumnIndex(1);
      ImGui::Separator();
      ImGui::TableSetColumnIndex(2);
      ImGui::Separator();

      uint16_t opcode;
      std::string disassembled;
      for (uint16_t i = 0; i < cpu->memory.size() - 1; i += 2) {
        ImGui::TableNextRow();
        opcode = (cpu->memory.at(i) << 8) | cpu->memory.at(i + 1);
        disassembled = disassemble(opcode);
        if (i == cpu->program_counter) {
          ImGui::TableSetColumnIndex(0);
          ImGui::Selectable(fmt::format("{:#x}", i).c_str(), true);
          ImGui::TableSetColumnIndex(1);
          ImGui::Selectable(fmt::format("{:#x}", opcode).c_str(), true);
          ImGui::TableSetColumnIndex(2);
          ImGui::Selectable(fmt::format("{}", disassembled).c_str(), true);
        } else {
          ImGui::TableSetColumnIndex(0);
          ImGui::Text(fmt::format("{:#x}", i).c_str());
          ImGui::TableSetColumnIndex(1);
          ImGui::Text(fmt::format("{:#x}", opcode).c_str());
          ImGui::TableSetColumnIndex(2);
          ImGui::Text(fmt::format("{}", disassembled).c_str());
        }
      }
      ImGui::EndTable();
      ImGui::EndChild();
    }
  }

  ImGui::End();
}

void Debugger::render(CPU* cpu) {
  SDL_GL_MakeCurrent(window_, gl_context_);
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  place_widgets(cpu);

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  ImGui::Render();
  glViewport(0, 0, static_cast<int>(io_->DisplaySize.x),
             static_cast<int>(io_->DisplaySize.y));
  glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
               clear_color.z * clear_color.w, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  SDL_GL_SwapWindow(window_);
}

std::string Debugger::disassemble(uint16_t opcode) {
  uint8_t MSB_id = (opcode & 0xF000) >> 12;

  if (MSB_id == 0x0) {
    switch (opcode & 0x00FF) {
      case 0xE0:
        // 0x00E0
        return "CLS";
      case 0xEE:
        // 0x00EE
        return "RET";
      default:
        // NULL
        return "-";
    }
  } else if (((MSB_id >= 1) && (MSB_id <= 7)) ||
             ((MSB_id >= 0xA) && (MSB_id <= 0xD))) {
    switch (MSB_id) {
      case 0x1:
        // 0x1NNN
        return "JP addr";
      case 0x2:
        // 0x2NNN
        return "CALL adrr";
      case 0x3:
        // 0x3XNN
        return "SE Vx, byte";
      case 0x4:
        // 0x4XNN
        return "SNE Vx, byte";
      case 0x5:
        // 0x5XY0
        return "SE Vx, Vy";
      case 0x6:
        // 0x6XNN
        return "LD Vx, byte";
      case 0x7:
        // 0x7XNN
        return "ADD Vx, byte";
      case 0xA:
        // 0xANNN
        return "LD I, addr";
      case 0xB:
        // 0xBNNN
        return "JP V0, addr";
      case 0xC:
        // 0xCXNN
        return "RND Vx, byte";
      case 0xD:
        // 0xDXYN
        return "DRAW Vx, Vy, nibble";
      default:
        // NULL
        return "-";
    }
  } else if (MSB_id == 0x8) {
    switch (opcode & 0x000F) {
      case 0x0:
        // 0x8XY0
        return "LD Vx, Vy";
      case 0x1:
        // 0x8XY1
        return "OR Vx, Vy";
      case 0x2:
        // 0x8XY2
        return "AND Vx, Vy";
      case 0x3:
        // 0x8XY3
        return "XOR Vx, Vy";
      case 0x4:
        // 0x8XY4
        return "ADD Vx, Vy";
      case 0x5:
        // 0x8XY5
        return "SUB Vx, Vy";
      case 0x6:
        // 0x8XY6
        return "SHR Vx {, Vy}";
      case 0x7:
        // 0x8XY7
        return "SUBN Vx, Vy";
      case 0xE:
        // 0x8XYE
        return "SHL Vx, {, Vy}";
      default:
        // NULL
        return "-";
    }
  } else if ((MSB_id == 0xE) || (MSB_id == 0x9)) {
    if (MSB_id == 0xE) {
      switch (opcode & 0x000F) {
        case 0x1:
          // 0xEXA1
          return "SKNP Vx";
        case 0xE:
          // 0xEX9E
          return "SKP Vx";
        default:
          // null
          return "-";
      }
    } else {
      // 9XY0
      return "SNE Vx, Vy";
    }
  } else if (MSB_id == 0xF) {
    switch (opcode & 0x00FF) {
      case 0x07:
        // 0xFX07
        return "LD Vx, DT";
      case 0x0A:
        // 0xFX0A
        return "LD Vx, K";
      case 0x15:
        // 0xFX15
        return "LD DT, Vx";
      case 0x18:
        // 0xFX18
        return "LD ST, Vx";
      case 0x1E:
        // 0xFX1E
        return "ADD I, Vx";
      case 0x29:
        // 0xFX29
        return "LD F, Vx";
      case 0x33:
        // 0xFX33
        return "LD B, Vx";
      case 0x55:
        // 0xFX55
        return "LD [I], Vx";
      case 0x65:
        // 0xFX65
        return "LD Vx, [I]";
      default:
        // NULL
        return "-";
    }
  }

  return "unrecognized opcode";
}

void Debugger::cleanup() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context_);
  SDL_DestroyWindow(window_);
  SDL_Quit();

  window_ = nullptr;
  io_ = nullptr;
}