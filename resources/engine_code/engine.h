#ifndef ENGINE
#define ENGINE

#include "includes.h"

enum class renderMode {
  none, preview, pathtrace
};

class engine {
public:
	engine()  { init(); }
	~engine() { quit(); }

  // called from main()
  bool mainLoop();

private:
  // application handles + basic data
	SDL_Window * window;
	SDL_GLContext GLcontext;
	ImVec4 clearColor;

  // program control flags
  bool quitConfirm = false;
  bool pQuit       = false;
  bool filter      = false;
  renderMode mode  = renderMode::preview;

  // initialization
  void init();
  void startMessage();
	void createWindowAndContext();
  void displaySetup();
  void computeShaderCompile();
  void imguiSetup();

  // main loop functions
  void mainDisplayBlit();
  void handleEvents();
  void imguiPass();
  void imguiFrameStart();
  void imguiFrameEnd();
  void controlsWindow();
  void drawTextEditor();
  void screenShot();
  void quitConf( bool *open );

  // rendering functions
  void render();      // wrapper
  void raymarch();    // preview render
  void pathtrace();   // accumulate samples
  void postprocess(); // tonemap, dither

  // shutdown procedure
  void imguiQuit();
  void SDLQuit();
	void quit();

  // OpenGL data handles
    // render
  GLuint accumulatorTexture;
  GLuint raymarchShader;
  GLuint pathtraceShader;
  GLuint postprocessShader;
    // present
  GLuint displayTexture;
  GLuint displayShader;
	GLuint displayVAO;
	GLuint displayVBO;
};

#endif
