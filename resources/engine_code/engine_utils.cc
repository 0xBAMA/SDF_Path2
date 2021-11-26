#include "engine.h"

bool engine::mainLoop() {
  // different rendering modes - preview until pathtrace is triggered
  switch( mode ) {
    case renderMode::preview:   raymarch();  break;
    case renderMode::pathtrace: pathtrace(); break;
    default: break;
  }

  postprocess();               // accumulatorTexture -> displayTexture
  mainDisplayBlit();           // fullscreen triangle copying the image
  imguiPass();                 // do all the gui stuff
  SDL_GL_SwapWindow( window ); // swap the double buffers to present
  handleEvents();              // handle all events

  return pQuit;                // break main loop when pQuit turns true
}

void engine::raymarch() {

}

void engine::pathtrace() {

}

void engine::postprocess() {

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
