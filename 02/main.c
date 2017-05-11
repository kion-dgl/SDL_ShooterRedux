#include <stdio.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "lib/dashgl.h"

#define WIDTH 854
#define HEIGHT 480

GLuint program;
GLint attribute_coord2d, attribute_texcoord;
GLint uniform_mvp, uniform_mytexture;

struct {
	vec3 pos;
	int size;
	GLuint tex, vbo, ibo;
	float dx, dy;
	bool left_down, right_down;
} player;

bool init_resources();
void main_loop(SDL_Window *window);
void evt_keydown(int key);
void evt_keyup(int key);
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
	
	GLushort player_indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	
	player.size = sizeof(player_indices)/sizeof(GLushort);
	player.pos[0] = 40.0f;
	player.pos[1] = 40.0f;
	player.pos[2] = 0.0f;
	player.dx = 4.0f;
	player.dy = 4.0f;

	glGenBuffers(1, &player.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, player.vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(player_vertices),
		player_vertices,
		GL_STATIC_DRAW
	);

	glGenBuffers(1, &player.ibo);
	glBindBuffer(GL_ARRAY_BUFFER, player.ibo);
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

	player.tex = dash_texture_load("tex/playerShip1_red.png");

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
	
	mat4_orthographic(0, WIDTH, HEIGHT, 0, orthographic);
	glUniformMatrix4fv(uniform_orthograph, 1, GL_FALSE, orthographic);

	return true;

}

void main_loop(SDL_Window *window) {

	while(true) {
		
		SDL_Event evt;

		while(SDL_PollEvent(&evt)) {
			
			switch(evt.type) {
				case SDL_QUIT:

					return;

				break;
				case SDL_KEYDOWN:
					
					evt_keydown(evt.key.keysym.sym);

				break;
				case SDL_KEYUP:

					evt_keyup(evt.key.keysym.sym);

				break;
			}
			
			logic();
			render(window);
		}

	}

}

void evt_keydown(int key) {

	switch(key) {
		case SDLK_RIGHT:

			player.right_down = true;

		break;
		case SDLK_LEFT:

			player.left_down = true;

		break;
	}

}

void evt_keyup(int key) {

	switch(key) {
		case SDLK_RIGHT:

			player.right_down = false;

		break;
		case SDLK_LEFT:

			player.left_down = false;

		break;
	}

}

void logic() {

	if(player.left_down) {
		player.pos[0] -= player.dx;
	}

	if(player.right_down) {
		player.pos[0] += player.dx;
	}
	
	if(player.pos[0] < 0) {
		player.pos[0] = 0.0f;
	} else if(player.pos[0] > (float)WIDTH) {
		player.pos[0] = (float)WIDTH;
	}

}


void render(SDL_Window *window) {

	mat4 mvp;
	mat4_translate(player.pos, mvp);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, mvp);

	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(uniform_mytexture, 0);
	glBindTexture(GL_TEXTURE_2D, player.tex);
	
	glEnableVertexAttribArray(attribute_coord2d);
	glEnableVertexAttribArray(attribute_texcoord);

	glBindBuffer(GL_ARRAY_BUFFER, player.vbo);

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
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, player.ibo);
	glDrawElements(GL_TRIANGLES, player.size, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(attribute_coord2d);
	glDisableVertexAttribArray(attribute_texcoord);

	SDL_GL_SwapWindow(window);

}

void free_resources() {

	glDeleteProgram(program);
	glDeleteBuffers(1, &player.vbo);
	glDeleteBuffers(1, &player.ibo);

}
