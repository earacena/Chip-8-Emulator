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

  SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  window_ = SDL_CreateWindow("CHIP-8 Debugger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 900, window_flags);
  if (window_ == nullptr)
    throw std::string(fmt::format("Failure to create window: {}", SDL_GetError()));

  gl_context_ = SDL_GL_CreateContext(window_);
  if (gl_context_ == nullptr)
    throw std::string(fmt::format("Failure to create OpenGL context: {}", SDL_GetError()));

  SDL_GL_MakeCurrent(window_, gl_context_);
  SDL_GL_SetSwapInterval(1); // Enables V-Sync

  // Setup ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io_ = &ImGui::GetIO();

  // Setup ImGui style
  ImGui::StyleColorsDark();

  // Setup renderer backends
  ImGui_ImplSDL2_InitForOpenGL(window_, gl_context_);
  ImGui_ImplOpenGL3_Init(glsl_version_);
}

void Debugger::place_widgets() {
  ImGui::Begin("Debugger");
  ImGui::Text("Testing text");
  if (ImGui::Button("Press Me"))
    ImGui::Text("Pressed");
  ImGui::End();
}

void Debugger::render(CPU * cpu) {
  SDL_GL_MakeCurrent(window_, gl_context_);
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  place_widgets();

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  ImGui::Render();
  glViewport(0, 0, static_cast<int>(io_->DisplaySize.x), static_cast<int>(io_->DisplaySize.y));
  glClearColor(
    clear_color.x * clear_color.w,
    clear_color.y * clear_color.w,
    clear_color.z * clear_color.w,
    clear_color.w
  );
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  SDL_GL_SwapWindow(window_); 
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