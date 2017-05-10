attribute vec2 coord2d;
attribute vec2 texcoord;
varying vec2 f_texcoord;

uniform mat4 orthographic, mvp;

void main(void) {

	gl_Position = orthographic * mvp * vec4(coord2d, 0.0, 1.0);
	f_texcoord = texcoord;

}
