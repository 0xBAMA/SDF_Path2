# SDF_Path2
Rewrite of SDF_Path using NQADE

Some areas to improve / mess around with:
- Timing on tile loop, to keep things responsive
- Finish implementing dither logic - port [this color space](https://bottosson.github.io/posts/colorpicker/) and see if there's any other interesting ones
- The glitch logic that I was thinking about before:
  - Pull image data to CPU
    - Add to it by glyph stamps
    - Mask for noise application ( + / - )
    - Blurs
    - Clears ( every other row / checkerboard )
    - Dither inside tile
    - Reset averaging ( sample count )
    - Use std::vector + shuffle to on an array of floats cast to a byte array - make a mess of the data
