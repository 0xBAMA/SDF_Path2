#include "engine.h"

bool engine::mainLoop() {
  render();                    // render with the current mode
  postprocess();               // accumulatorTexture -> displayTexture
  mainDisplayBlit();           // fullscreen triangle copying the image
  imguiPass();                 // do all the GUI stuff
  SDL_GL_SwapWindow( window ); // swap the double buffers to present
  handleEvents();              // handle input events

  return !pQuit;               // break loop in main.cc when pQuit turns true
}


void engine::render() {
  // different rendering modes - preview until pathtrace is triggered
  switch ( mode ) {
    case renderMode::preview:   raymarch();  break;
    case renderMode::pathtrace: pathtrace(); break;
    default: break;
  }
}

void engine::raymarch() {
  glUseProgram( raymarchShader );
  // fullscreen pass
}

void engine::pathtrace() {
  glUseProgram( pathtraceShader );

  // now I can just call getTile() to get an offset, whenever I need it
    // abstracting this out makes this function's implementation much cleaner

  // get an initial time - this has to be done with gl timer queries, not std::chrono
  // loop
    // get a glm::ivec2
    // render the specified tile
    // check time, break if duration exceeds some specified timing
}

void engine::postprocess() {
  glUseProgram( postprocessShader );
  // tonemapping and dithering, as configured in the GUI
}

void engine::mainDisplayBlit() {
  // clear the screen
  glClearColor( clearColor.x, clearColor.y, clearColor.z, clearColor.w );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // texture display
  glUseProgram( displayShader );
  glBindVertexArray( displayVAO );
  glBindBuffer( GL_ARRAY_BUFFER, displayVBO );

  ImGuiIO &io = ImGui::GetIO();
  glUniform2f( glGetUniformLocation( displayShader, "resolution" ), io.DisplaySize.x, io.DisplaySize.y );
  glDrawArrays( GL_TRIANGLES, 0, 3 );
}

void engine::imguiPass() {
  // start the imgui frame
  imguiFrameStart();

  // show the demo window
  static bool showDemoWindow = true;
  if ( showDemoWindow )
    ImGui::ShowDemoWindow( &showDemoWindow );

  // show quit confirm window
  quitConf( &quitConfirm );

  // finish up the imgui stuff and put it in the framebuffer
  imguiFrameEnd();
}


void engine::handleEvents() {
  SDL_Event event;
  while ( SDL_PollEvent( &event ) ) {
    // imgui event handling
    ImGui_ImplSDL2_ProcessEvent( &event );

    if ( event.type == SDL_QUIT )
      pQuit = true;

    if ( event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID( window ) )
      pQuit = true;

    if ( ( event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE) || ( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_X1 ) )
      quitConfirm = !quitConfirm; // x1 is browser back on the mouse

    if ( event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE && SDL_GetModState() & KMOD_SHIFT )
      pQuit = true; // force quit on shift+esc ( bypasses confirm window )
  }
}

glm::ivec2 engine::getTile() {
  static std::vector< glm::ivec2 > offsets;
  static int listOffset = 0;
  static bool firstTime = true;
  std::random_device rd;
  std::mt19937 rngen( rd() );

  if ( firstTime ) { // construct the tile list
    firstTime = false;
    for( int x = 0; x < WIDTH; x+= 32 ) {
      for( int y = 0; y < HEIGHT; y+= 32 ) {
        offsets.push_back( glm::ivec2( x, y ) );
      }
    }
  } else {
    if ( ++listOffset == int( offsets.size() ) ) {
      listOffset = 0;
    }
  }

  // shuffle when listOffset is zero ( first iteration, and any subsequent resets )
  if ( !listOffset ) std::shuffle( offsets.begin(), offsets.end(), rngen );
  return offsets[ listOffset ];
}

void engine::screenShot() {

}
