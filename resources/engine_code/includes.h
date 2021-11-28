#ifndef INCLUDES
#define INCLUDES

#include <stdio.h>

// stl includes
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <vector>

// iostream stuff
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::flush;

// pi definition - definitely sufficient precision
constexpr double pi = 3.14159265358979323846;

// MSAA count - effects OpenGL geometry evaluation
constexpr int MSAACount = 1;

// vector math library GLM
#define GLM_FORCE_SWIZZLE
#define GLM_SWIZZLE_XYZW
#include "../glm/glm.hpp"                  //general vector types
#include "../glm/gtc/matrix_transform.hpp" // for glm::ortho
#include "../glm/gtc/type_ptr.hpp"         //to send matricies gpu-side
#include "../glm/gtx/rotate_vector.hpp"
#include "../glm/gtx/transform.hpp"

// not sure as to the utility of this
#define GLX_GLEXT_PROTOTYPES

// OpenGL Loader
#include "../ocornut_imgui/gl3w.h"

// GUI library (dear ImGUI)
#include "../ocornut_imgui/TextEditor.h"
#include "../ocornut_imgui/imgui.h"
#include "../ocornut_imgui/imgui_impl_sdl.h"
#include "../ocornut_imgui/imgui_impl_opengl3.h"

// SDL includes - windowing, gl context, system info
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// png loading library - very powerful
#include "../lodev_lodePNG/lodepng.h"

// wrapper for TinyOBJLoader
#include "../TinyOBJLoader/objLoader.h"

// shader compilation wrapper
#include "shader.h"

// coloring of CLI output
#include "colors.h"

// diamond square heightmap generation
#include "../mafford_diamond_square/diamond_square.h"

// Brent Werness' Voxel Automata Terrain
#include "../VAT/VAT.h"

// Niels Lohmann - JSON for Modern C++
#include "../nlohmann_JSON/json.hpp"
using json = nlohmann::json;

// #define WIDTH 640
// #define HEIGHT 480

#define WIDTH  1920
#define HEIGHT 1080

// not sure about this, going to need to do some testing, to maximize amount of work per timer query
#define TILESIZE 128


struct coreParameters {
  glm::ivec2 tileOffset;
  glm::ivec2 noiseOffset; // update once a frame, offset blue noise read
  int maxSteps = 300;
  int maxBounces = 10;
  float maxDistance = 5.;
  float epsilon = 0.001;
  float exposure = 1.0;
  int normalMethod = 0;
  float FoV = 0.152;
  glm::vec3 basicDiffuse;
  glm::vec3 viewerPosition;
  float rotationAboutX = 0.;
  float rotationAboutY = 0.;
  float rotationAboutZ = 0.;
  // calculated from the above 3 params
  glm::vec3 basisX;
  glm::vec3 basisY;
  glm::vec3 basisZ;
};

struct lensParameters {
  float lensScaleFactor;
  float lensRadius1;
  float lensRadius2;
  float lensThickness;
  float lensRotate;
  float lensIOR;
};

struct postParameters {
  int ditherMode;
  int ditherMethod;
  int ditherPattern;
  int tonemapMode;
};





#endif
