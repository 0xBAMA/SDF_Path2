#version 430 core
layout( local_size_x = 32, local_size_y = 32, local_size_z = 1 ) in;

layout( binding = 0, rgba8ui ) uniform uimage2D display;
layout( binding = 1, rgba32f ) uniform image2D accumulator;

void main() {
  ivec2 location = ivec2( gl_GlobalInvocationID.xy );
  vec4 toStore = imageLoad( accumulator, location );
  // do the postprocessing
  imageStore( display, location, uvec4( toStore * 255. ) );
}
