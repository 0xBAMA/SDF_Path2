#include "engine.h"

void engine::startMessage() {
  cout << endl << T_YELLOW << BOLD << "NQADE - Not Quite A Demo Engine" << endl;
  cout << " By Jon Baker ( 2020 - 2021 ) " << RESET << endl;
  cout << "  https://jbaker.graphics/ " << endl << endl;
}

void engine::createWindowAndContext() {
  cout << T_BLUE << "    Initializing SDL2" << RESET << " ................................ ";

  if ( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
    cout << "Error: " << SDL_GetError() << endl;

  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER,       1 );
  SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE,           8 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE,         8 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE,          8 );
  SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE,         8 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE,        24 );
  SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE,       8 );
  SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, MSAACount );

  // query the screen resolution
  SDL_DisplayMode dm;
  SDL_GetDesktopDisplayMode( 0, &dm );

  cout << T_GREEN << "done." << RESET << endl;

  cout << T_BLUE << "    Creating window" << RESET << " .................................. ";
  // auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_BORDERLESS;
  auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE;
  window = SDL_CreateWindow( "NQADE", 0, 0, dm.w, dm.h, flags );

  // if init takes some time, don't show the window before it's done
  SDL_ShowWindow( window );

  cout << T_GREEN << "done." << RESET << endl;

  cout << T_BLUE << "    Setting up OpenGL context" << RESET << " ........................ ";
  // initialize OpenGL 4.3 + GLSL version 430
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, 0 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
  GLcontext = SDL_GL_CreateContext( window );
  SDL_GL_MakeCurrent( window, GLcontext );
  SDL_GL_SetSwapInterval( 1 ); // Enable vsync

  // load OpenGL functions
  if ( gl3wInit() != 0 ) cout << "Failed to initialize OpenGL loader!" << endl;

  // basic OpenGL Config
  glEnable( GL_DEPTH_TEST );
  glEnable( GL_LINE_SMOOTH );
  glPointSize( 3.0 );
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  cout << T_GREEN << "done." << RESET << endl;
}


void engine::displaySetup() {
  // some info on your current platform
  const GLubyte *renderer = glGetString( GL_RENDERER ); // get renderer string
  const GLubyte *version = glGetString( GL_VERSION );   // version as a string

  cout << T_BLUE << "    Platform Info:" << RESET << endl;
  cout << T_RED << "      Renderer: " << T_CYAN << renderer << RESET << endl;
  cout << T_RED << "      OpenGL version supported: " << T_CYAN << version << RESET << endl << endl;

  // create the shader for the triangles to cover the screen
  displayShader = Shader( "resources/engine_code/shaders/blit.vs.glsl", "resources/engine_code/shaders/blit.fs.glsl" ).Program;

  // Based on this, one triangle is significantly faster than two
  // https://michaldrobot.com/2014/04/01/gcn-execution-patterns-in-full-screen-passes/
  // main idea there is that there is coherency along the diagonal, which does not
  // exist shen you split the screen with two triangles, e.g. if the execution blocks
  // straddle the diagonal, they have to consider two primitives instead of one - so
  // even with the clipping that happens around the viewport, the rasterization process
  // is still cheaper, since there is no divergence there. Long story short, good for
  // a small speedup and it is Good Practice.

  glm::vec3 points[] = {
    glm::vec3(-1, -1, 0.5), // A
    glm::vec3( 3, -1, 0.5), // B
    glm::vec3(-1,  3, 0.5)  // C
  };

  // VAO, VBO
  cout << T_BLUE << "    Setting up VAO, VBO for display geometry" << RESET << " ......... ";
  glGenVertexArrays( 1, &displayVAO );
  glBindVertexArray( displayVAO );

  glGenBuffers( 1, &displayVBO );
  glBindBuffer( GL_ARRAY_BUFFER, displayVBO );
  cout << T_GREEN << "done." << RESET << endl;

  // buffer the data
  cout << T_BLUE << "    Buffering vertex data" << RESET << " ............................ ";
  glBufferData( GL_ARRAY_BUFFER, sizeof( glm::vec3 ) * 3, NULL, GL_DYNAMIC_DRAW );
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( glm::vec3 ) * 3, &points[ 0 ] );
  cout << T_GREEN << "done." << RESET << endl;

  // set up attributes
  cout << T_BLUE << "    Setting up attributes in display shader" << RESET << " .......... ";
  GLuint pointsAttrib = glGetAttribLocation( displayShader, "vPosition" );
  glEnableVertexAttribArray( pointsAttrib );
  glVertexAttribPointer( pointsAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0 );
  cout << T_GREEN << "done." << RESET << endl;

  // replace this with real image data
  std::vector<uint8_t> imageData;
  imageData.resize( WIDTH * HEIGHT * 4 );

  // fill with random values
  std::default_random_engine gen;
  std::uniform_int_distribution<uint8_t> dist( 150, 255 );
  std::uniform_int_distribution<uint8_t> dist2( 12, 45 );

  for ( auto it = imageData.begin(); it != imageData.end(); it++ )
    *it = dist( gen );

  // create the image textures
  glGenTextures( 1, &displayTexture );
  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, displayTexture );

  // texture parameters
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  // glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  // glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

  // buffer the image data to the GPU
  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, displayTexture );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, &imageData[ 0 ] );
  glBindImageTexture( 0, displayTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI );
}


void engine::imguiSetup() {
  cout << T_BLUE << "    Configuring dearImGUI" << RESET << " ............................ ";

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

  // enable docking
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  // Setup Platform/Renderer bindings
  ImGui_ImplSDL2_InitForOpenGL( window, GLcontext );
  const char *glsl_version = "#version 430";
  ImGui_ImplOpenGL3_Init( glsl_version );

  // initial value for clear color
  clearColor = ImVec4( 0.295f, 0.295f, 0.295f, 0.5f );

  glClearColor( clearColor.x, clearColor.y, clearColor.z, clearColor.w );
  glClear( GL_COLOR_BUFFER_BIT );
  SDL_GL_SwapWindow( window ); // show clear color

  // setting custom font, if desired
  // io.Fonts->AddFontFromFileTTF("resources/fonts/star_trek/titles/TNG_Title.ttf", 16);

  ImGui::StyleColorsDark();

  ImVec4 *colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text] = ImVec4(0.67f, 0.50f, 0.16f, 1.00f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.33f, 0.27f, 0.16f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.05f, 0.00f, 1.00f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.23f, 0.17f, 0.02f, 0.05f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.30f, 0.12f, 0.06f, 0.94f);
  colors[ImGuiCol_Border] = ImVec4(0.25f, 0.18f, 0.09f, 0.33f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.33f, 0.15f, 0.02f, 0.17f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.561f, 0.082f, 0.04f, 0.17f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.09f, 0.02f, 0.17f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.25f, 0.12f, 0.01f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.33f, 0.15f, 0.02f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.25f, 0.12f, 0.01f, 1.00f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.07f, 0.02f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.13f, 0.10f, 0.08f, 0.53f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.33f, 0.15f, 0.02f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.69f, 0.45f, 0.11f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.18f, 0.06f, 1.00f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.36f, 0.22f, 0.06f, 1.00f);
  colors[ImGuiCol_Button] = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.33f, 0.15f, 0.02f, 1.00f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
  colors[ImGuiCol_Header] = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.33f, 0.15f, 0.02f, 1.00f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
  colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.18f, 0.06f, 0.37f);
  colors[ImGuiCol_SeparatorHovered] = ImVec4(0.33f, 0.15f, 0.02f, 0.17f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(0.42f, 0.18f, 0.06f, 0.17f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.33f, 0.15f, 0.02f, 1.00f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
  colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
  colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.15f, 0.02f, 1.00f);
  colors[ImGuiCol_TabActive] = ImVec4(0.34f, 0.14f, 0.01f, 1.00f);
  colors[ImGuiCol_TabUnfocused] = ImVec4(0.33f, 0.15f, 0.02f, 1.00f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.42f, 0.18f, 0.06f, 1.00f);
  colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.06f, 0.03f, 0.01f, 0.78f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(0.64f, 0.42f, 0.09f, 0.90f);
  colors[ImGuiCol_NavHighlight] = ImVec4(0.64f, 0.42f, 0.09f, 0.90f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

  ImGuiStyle &style = ImGui::GetStyle();

  style.TabRounding = 2;
  style.FrameRounding = 2;
  style.WindowPadding.x = 0;
  style.WindowPadding.y = 0;
  style.FramePadding.x = 1;
  style.FramePadding.y = 0;
  style.IndentSpacing = 8;
  style.WindowRounding = 3;
  style.ScrollbarSize = 10;

  cout << T_GREEN << "done." << RESET << endl << endl;
}


void engine::computeShaderCompile() {

  // compile any compute shaders here, store handles in engine class member function variables

}
