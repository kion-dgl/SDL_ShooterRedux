#include <stdio.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "lib/dashgl.h"

#define WIDTH 854
#define HEIGHT 480

GLint attribute_coord2d, attribute_texcoord;
GLuint texture_id, vbo_player_coords, ibo_player_indices;
GLint uniform_mvp, uniform_mytexture;

bool init_resources();
void main_loop(SDL_Window *window);
void free_resources();

int main(int argc, char *argv[]) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow(
		"DashGL - SpaceRedux",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WIDTH,
		HEIGHT,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
	);

	if(window == NULL) {
		fprintf(stderr, "Error can't create window %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);

	if(SDL_GL_CreateContext(window) == NULL) {
		fprintf(stderr, "Error can't create context %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	GLenum glew_status = glewInit();
	if(glew_status != GLEW_OK) {
		fprintf(stderr, "Error glewInit: %s\n", glewGetErrorString(glew_status));
		return EXIT_FAILURE;
	}

	if(!GLEW_VERSION_2_0) {
		fprintf(stderr, "Your graphics card does not support OpenGL 2.0\n");
		return EXIT_FAILURE;
	}
	
	if(!init_resources()) {
		fprintf(stderr, "Could not initialize OpenGL resources\n");
		return EXIT_FAILURE;
	}


	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	main_loop(window);
	free_resources();

	return EXIT_SUCCESS;

}

bool init_resources() {

}

void main_loop(SDL_Window *window) {


}

void free_resources() {

}
