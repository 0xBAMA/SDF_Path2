#version 430 core
layout( binding = 0 ) uniform sampler2D current;

uniform vec2 resolution;
out vec4 fragmentOutput;

void main() {
  // fragmentOutput = vec4( vec3( int( gl_FragCoord.x ) ^ int( gl_FragCoord.y ) ) / 1023., 1.0 );
	fragmentOutput = texture( current, gl_FragCoord.xy / resolution );
}
