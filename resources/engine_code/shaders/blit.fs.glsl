#version 430 core
layout( binding = 0 ) uniform sampler2D current;
// layout( binding = 2, rgba8ui ) uniform uimage2D blueNoise;

uniform vec2 resolution;
out vec4 fragmentOutput;

void main() {
	fragmentOutput = texture( current, gl_FragCoord.xy / resolution );
	// fragmentOutput = imageLoad( blueNoise, ivec2( gl_FragCoord.xy ) ) / 255.;
}
