#include <stdio.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "lib/dashgl.h"

#define WIDTH 854
#define HEIGHT 480

GLuint program;
GLint attribute_coord2d, attribute_texcoord;
GLuint player_tex, vbo_player_coords, ibo_player_indices;
GLint uniform_mvp, uniform_mytexture;

bool init_resources();
void main_loop(SDL_Window *window);
void logic();
void render(SDL_Window *window);
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
	// glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	main_loop(window);
	free_resources();

	return EXIT_SUCCESS;

}

bool init_resources() {
	
	GLfloat player_vertices[] = {
		-20.0f, -20.0f, 0.0, 0.0,
		-20.0f,  20.0f, 0.0, 1.0,
		 20.0f,  20.0f, 1.0, 1.0,
		 20.0f, -20.0f, 1.0, 0.0
	};
	
	/*
	GLfloat player_vertices[] = {
		-0.8f, -0.8f, 0.0, 0.0,
		-0.8f,  0.8f, 0.0, 1.0,
		 0.8f,  0.8f, 1.0, 1.0,
		 0.8f, -0.8f, 1.0, 0.0
	};
	*/

	GLushort player_indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	glGenBuffers(1, &vbo_player_coords);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_player_coords);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(player_vertices),
		player_vertices,
		GL_STATIC_DRAW
	);

	glGenBuffers(1, &ibo_player_indices);
	glBindBuffer(GL_ARRAY_BUFFER, ibo_player_indices);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(player_indices),
		player_indices,
		GL_STATIC_DRAW
	);
	
	program = dash_create_program("sdr/vertex.glsl", "sdr/fragment.glsl");
	if(program == 0) {
		fprintf(stderr, "Could not create shader program\n");
		return false;
	}
	glLinkProgram(program);

	player_tex = dash_texture_load("tex/playerShip1_red.png");

	const char *attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if(attribute_coord2d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return false;
	}

	attribute_name = "texcoord";
	attribute_texcoord = glGetAttribLocation(program, attribute_name);
	if(attribute_texcoord == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return false;
	}

	const char *uniform_name = "orthographic";
	GLint uniform_orthograph = glGetUniformLocation(program, uniform_name);
	if(uniform_orthograph == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return false;
	}

	uniform_name = "mvp";
	uniform_mvp = glGetUniformLocation(program, uniform_name);
	if(uniform_mvp == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return false;
	}

	uniform_name = "mytexture";
	uniform_mytexture = glGetUniformLocation(program, uniform_name);
	if(uniform_mvp == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return false;
	}

	glUseProgram(program);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	
	mat4 orthographic;
	
	//mat4_identity(orthographic);
	
	mat4_orthographic(0, WIDTH, HEIGHT, 0, orthographic);
	glUniformMatrix4fv(uniform_orthograph, 1, GL_FALSE, orthographic);

	return true;

}

void main_loop(SDL_Window *window) {

	while(true) {
		
		SDL_Event evt;

		while(SDL_PollEvent(&evt)) {
			
			if(evt.type == SDL_QUIT) {
				return;
			}
			
			logic();
			render(window);
		}

	}

}

void logic() {


}

void render(SDL_Window *window) {

	int size;

	vec3 pos = { 100.0, 100.0, 0.0 };
	mat4 mvp;
	mat4_translate(pos, mvp);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, mvp);

	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(uniform_mytexture, 0);
	glBindTexture(GL_TEXTURE_2D, player_tex);
	
	glEnableVertexAttribArray(attribute_coord2d);
	glEnableVertexAttribArray(attribute_texcoord);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_player_coords);

	glVertexAttribPointer(
		attribute_coord2d,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float)*4,
		0
	);

	glVertexAttribPointer(
		attribute_texcoord,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float)*4,
		(void*)(sizeof(float)*2)
	);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_player_indices);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(attribute_coord2d);
	glDisableVertexAttribArray(attribute_texcoord);

	SDL_GL_SwapWindow(window);

}

void free_resources() {

	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo_player_coords);
	glDeleteBuffers(1, &ibo_player_indices);

}
