# SDF_Path2
Rewrite of SDF_Path using NQADE

Some areas to improve / mess around with:
- Timing on tile rendering loop, to keep things responsive - this wasn't working on the last implementation, needs work - probably use OpenGL timing queries instead of std::chrono, that might be all I need to do
- Finish implementing dither logic - port [this color space](https://bottosson.github.io/posts/colorpicker/) and see if there's any other interesting ones
- The glitch logic that I was thinking about before:
  - Pull image data to CPU or do in a shader and write back to the accumulator texture
    - Add to it by glyph stamps
    - Mask for noise application ( + / - )
    - Blurs
    - Clears ( every other row / checkerboard )
    - Dither inside tile
    - Reset averaging ( sample count )
    - Use std::vector + shuffle to on an array of floats cast to a byte array - make a mess of the data
