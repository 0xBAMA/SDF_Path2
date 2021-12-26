#include "engine.h"

bool engine::mainLoop() {
  // renderer config
  static coreParameters core;
  static lensParameters lens;
  static postParameters post;

  render();                     // render with the current mode
  postprocess();                // accumulatorTexture -> displayTexture
  mainDisplayBlit();            // fullscreen triangle copying the image
  imguiPass();                  // do all the GUI stuff
  SDL_GL_SwapWindow( window );  // swap the double buffers to present
  handleEvents();               // handle input events

  return !pQuit;                // break loop in main.cc when pQuit turns true
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
  // do a fullscreen pass with simple shading
}

void engine::pathtrace() {
  glUseProgram( pathtraceShader );

  GLuint64 startTime, checkTime;
  GLuint queryID[ 2 ];
  glGenQueries( 2, queryID );
  glQueryCounter( queryID[ 0 ], GL_TIMESTAMP );

  // get startTime
  GLint startTimeAvailable = 0;
  while( !startTimeAvailable )
    glGetQueryObjectiv( queryID[ 0 ], GL_QUERY_RESULT_AVAILABLE, &startTimeAvailable );
  glGetQueryObjectui64v( queryID[ 0 ], GL_QUERY_RESULT, &startTime );

  int tilesCompleted = 0;
  float looptime = 0.;
  while( 1 ) {
    // get a tile offset + send it
    glm::ivec2 tile = getTile();
    glUniform2i( glGetUniformLocation( pathtraceShader, "tileOffset" ), tile.x, tile.y );

    // render the specified tile - send uniforms and dispatch
    glDispatchCompute( TILESIZE / 32, TILESIZE / 32, 1 );
    glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
    tilesCompleted++;

    // check time, wait for query to be ready
    glQueryCounter( queryID[ 1 ], GL_TIMESTAMP );
    GLint checkTimeAvailable = 0;
    while( !checkTimeAvailable )
      glGetQueryObjectiv( queryID[ 1 ], GL_QUERY_RESULT_AVAILABLE, &checkTimeAvailable );
    glGetQueryObjectui64v( queryID[ 1 ], GL_QUERY_RESULT, &checkTime );

    // break if duration exceeds 16 ms - query units are nanoseconds
    looptime = ( checkTime - startTime ) / 1e6; // get milliseconds
    if( looptime > 16. ) {
      cout << tilesCompleted << " tiles in " << looptime << " ms, avg " << looptime / tilesCompleted << " ms/tile" << endl;
      break;
    }
  }
}

void engine::postprocess() {
  // tonemapping and dithering, as configured in the GUI
  glUseProgram( postprocessShader );
  glDispatchCompute( std::ceil( WIDTH / 32. ), std::ceil( HEIGHT / 32. ), 1 );
  glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT ); // sync
}

void engine::mainDisplayBlit() {
  // clear the screen
  glClearColor( clearColor.x, clearColor.y, clearColor.z, clearColor.w );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // texture display
  glUseProgram( displayShader );
  glBindVertexArray( displayVAO );

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
    for( int x = 0; x <= WIDTH; x += TILESIZE ) {
      for( int y = 0; y <= HEIGHT; y += TILESIZE ) {
        offsets.push_back( glm::ivec2( x, y ) );
      }
    }
  } else { // check if the offset needs to be reset
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
