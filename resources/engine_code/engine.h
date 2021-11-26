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

  // called from main
  bool mainLoop();

private:
  // application handles + basic data
	SDL_Window * window;
	SDL_GLContext GLcontext;
  int totalScreenWidth, totalScreenHeight;
	ImVec4 clearColor;

  // OpenGL data handles
    // render
  GLuint accumulatorTexture;
  GLuint raymarchShader;
  GLuint pathtraceShader;
    // present
  GLuint displayTexture;
  GLuint displayShader;
	GLuint displayVAO;
	GLuint displayVBO;

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
  void quitConf( bool *open );

  // rendering functions
  void raymarch();    // preview render
  void pathtrace();   // accumulate samples
  void postprocess(); // tonemap, dither

  // shutdown procedures
  void imguiQuit();
  void SDLQuit();
	void quit();

  // program flags
	bool quitConfirm = false;
	bool pQuit = false;

  // render mode
  renderMode mode = renderMode::preview;
};

#endif
