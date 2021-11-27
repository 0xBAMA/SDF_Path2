#version 430 core
layout( local_size_x = 32, local_size_y = 32, local_size_z = 1 ) in;

layout( binding = 1, rgba32f ) uniform image2D accumulator;
layout( binding = 2, rgba8ui ) uniform uimage2D blueNoise;

uniform ivec2 tileOffset;

bool check( ivec2 loc ) {
  ivec2 bounds = ivec2( imageSize( accumulator ) ).xy;
  return ( loc.x < bounds.x && loc.y < bounds.y );
}

void main() {
  ivec2 location = ivec2( gl_GlobalInvocationID.xy ) + tileOffset;
  imageStore( accumulator, location, vec4( 1., 1., 0., 1. ) );
}
