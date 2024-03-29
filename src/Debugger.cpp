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

  // Add buttons for running and pausing execution
  if (ImGui::Button("Start")) {
    paused = false;
  }

  if (ImGui::Button("Pause")) {
    paused = true;
  }

  if (ImGui::Button("Step")) {
    stepped = true;
  }

  // Display registers
  if (ImGui::CollapsingHeader("Registers")) {
    if (ImGui::BeginTable("register_table", 2)) {
      for (uint8_t i = 0; i < 16; i += 2) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text(fmt::format("V{:X}: {:#x}", i, cpu->V.at(i)).c_str());

        ImGui::TableSetColumnIndex(1);
        ImGui::Text(fmt::format("V{:X}: {:#x}", i+1, cpu->V.at(i+1)).c_str());
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

  // Display stack
  if (ImGui::CollapsingHeader("Stack")) {
    if (ImGui::BeginTable("stack_table", 2, ImGuiTableFlags_SizingFixedFit)) {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Position");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("Value");
      
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Separator();
      ImGui::TableSetColumnIndex(1);
      ImGui::Separator();

      for (uint16_t i = 0; i < cpu->stack.size(); ++i) {
          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0);
          ImGui::Selectable(fmt::format("{:#x}", i).c_str(), true);
          ImGui::TableSetColumnIndex(1);
          ImGui::Selectable(fmt::format("{:#x}", cpu->stack.at(i)).c_str(), true);
      }

      ImGui::EndTable();
    }
  }

  // Display memory
  if (ImGui::CollapsingHeader("Memory")) {
    ImGui::BeginChild("ScrollingMemory");
    if (ImGui::BeginTable("memory_table", 3, ImGuiTableFlags_SizingFixedFit))
    {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Address");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("Value");
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
          if (breakpoints.contains(i))
            ImGui::Bullet();

          if (ImGui::Selectable(fmt::format("{:#x}", i).c_str(), true)) {
            if (!breakpoints.contains(i))
              breakpoints.insert(i);
            else
              breakpoints.erase(i);
          }
          ImGui::TableSetColumnIndex(1);
          ImGui::Selectable(fmt::format("{:#x}", opcode).c_str(), true);
          ImGui::TableSetColumnIndex(2);
          ImGui::Selectable(fmt::format("{}", disassembled).c_str(), true);
        } else {
          ImGui::TableSetColumnIndex(0);
          if (breakpoints.contains(i))
            ImGui::Bullet();
          if (ImGui::Selectable(fmt::format("{:#x}", i).c_str(), true)) {
            if (!breakpoints.contains(i))
              breakpoints.insert(i);
            else
              breakpoints.erase(i);
          }
          ImGui::TableSetColumnIndex(1);
          ImGui::Text(fmt::format("{:#x}", opcode).c_str(), false);
          ImGui::TableSetColumnIndex(2);
          ImGui::Text(fmt::format("{}", disassembled).c_str(), false);
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
        return fmt::format("JP {:#x}", opcode & 0x0FFF);
      case 0x2:
        // 0x2NNN
        return fmt::format("CALL {:#x}", opcode & 0x0FFF);
      case 0x3:
        // 0x3XNN
        return fmt::format("SE V{:X}, {:#x}", (opcode & 0x0F00) >> 8, opcode & 0x00FF);
      case 0x4:
        // 0x4XNN
        return fmt::format("SNE V{:X}, {:#x}", (opcode & 0x0F00)>> 8, opcode & 0x00FF);
      case 0x5:
        // 0x5XY0
        return fmt::format("SE V{:X}, V{:X}", (opcode & 0x0F00) >> 8, opcode & 0x00F0 >> 4);
      case 0x6:
        // 0x6XNN
        return fmt::format("LD V{:X}, {:#x}", (opcode & 0x0F00) >> 8, opcode & 0x00FF);
      case 0x7:
        // 0x7XNN
        return fmt::format("ADD V{:X}, {:#x}", (opcode & 0x0F00) >> 8, opcode & 0x00FF);
      case 0xA:
        // 0xANNN
        return fmt::format("LD I, {:#x}", opcode & 0x0FFF);
      case 0xB:
        // 0xBNNN
        return fmt::format("JP V0, {:#x}", opcode & 0x0FFF);
      case 0xC:
        // 0xCXNN
        return fmt::format("RND V{:X}, {:#x}", (opcode & 0x0F00) >> 8, opcode & 0x00FF);
      case 0xD:
        // 0xDXYN
        return fmt::format("DRAW V{:X}, V{:X}, {:#x}", (opcode & 0x0F00) >> 8, opcode & 0x00F0 >> 4, opcode & 0x000F);
      default:
        // NULL
        return "-";
    }
  } else if (MSB_id == 0x8) {
    switch (opcode & 0x000F) {
      case 0x0:
        // 0x8XY0
        return fmt::format("LD V{:X}, V{:X}", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
      case 0x1:
        // 0x8XY1
        return fmt::format("OR V{:X}, V{:X}", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
      case 0x2:
        // 0x8XY2
        return fmt::format("AND V{:X}, V{:X}", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
      case 0x3:
        // 0x8XY3
        return fmt::format("XOR V{:X}, V{:X}", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
      case 0x4:
        // 0x8XY4
        return fmt::format("ADD V{:X}, V{:X}", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
      case 0x5:
        // 0x8XY5
        return fmt::format("SUB V{:X}, V{:X}", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
      case 0x6:
        // 0x8XY6
        return fmt::format("SHR V{:X} {{, V{:X} }}", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
      case 0x7:
        // 0x8XY7
        return fmt::format("SUBN V{:X}, V{:X}", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
      case 0xE:
        // 0x8XYE
        return fmt::format("SHL V{:X} {{, V{:X} }}", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
      default:
        // NULL
        return "-";
    }
  } else if ((MSB_id == 0xE) || (MSB_id == 0x9)) {
    if (MSB_id == 0xE) {
      switch (opcode & 0x000F) {
        case 0x1:
          // 0xEXA1
          return fmt::format("SKNP V{:X}", (opcode & 0x0F00) >> 12);
        case 0xE:
          // 0xEX9E
          return fmt::format("SKP V{:X}", (opcode & 0x0F00) >> 12);
        default:
          // null
          return "-";
      }
    } else {
      // 9XY0
      return fmt::format("SNE V{:X}, V{:X}", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
    }
  } else if (MSB_id == 0xF) {
    switch (opcode & 0x00FF) {
      case 0x07:
        // 0xFX07
        return fmt::format("LD V{:X}, DT", (opcode & 0x0F00) >> 8);
      case 0x0A:
        // 0xFX0A
        return fmt::format("LD V{:X}, K", (opcode & 0x0F00) >> 8);
      case 0x15:
        // 0xFX15
        return fmt::format("LD DT, V{:X}", (opcode & 0x0F00) >> 8);
      case 0x18:
        // 0xFX18
        return fmt::format("LD ST, V{:X}", (opcode & 0x0F00) >> 8);
      case 0x1E:
        // 0xFX1E
        return fmt::format("ADD I, V{:X}", (opcode & 0x0F00) >> 8);
      case 0x29:
        // 0xFX29
        return fmt::format("LD F, V{:X}", (opcode & 0x0F00) >> 8);
      case 0x33:
        // 0xFX33
        return fmt::format("LD B, V{:X}", (opcode & 0x0F00) >> 8);
      case 0x55:
        // 0xFX55
        return fmt::format("LD [I], V{:X}", (opcode & 0x0F00) >> 8);
      case 0x65:
        // 0xFX65
        return fmt::format("LD V{:X}, [I]", (opcode & 0x0F00) >> 8);
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

Uint32 Debugger::get_window_id()
{
  return SDL_GetWindowID(window_);
}