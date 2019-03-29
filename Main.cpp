#include <iostream>

#include "ShaderClass.h"
#include "objLoader.h"
#include "Square.h"
#include "Cube.h"

#include <random>
#include <vector>

// // GLEW - OpenGL Extension Wrangler - http://glew.sourceforge.net/
// // NOTE: include before SDL.h
#ifndef GLEW_H
#define GLEW_H
//#include <GL/glew.h>

// SDL - Simple DirectMedia Layer - https://www.libsdl.org/
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

// // - OpenGL Mathematics - https://glm.g-truc.net/
#define GLM_FORCE_RADIANS // force glm to use radians
// // NOTE: must do before including GLM headers
// // NOTE: GLSL uses radians, so will do the same, for consistency
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// https://opengameart.org/content/puzzle-game-art - game art for bricks and paddle
// https://opengameart.org/content/heart-1616 - game art for the Heart life indicator 
// https://opengameart.org/content/numbers-blocks-set-01 - game art for the numbers






// ***********************************************************************************


int main(int argc, char *argv[]) 
{

	// SDL initialise
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s\n",
			SDL_GetError());
		return 1;
	}

	SDL_Log("SDL initialised OK!\n");

	// Window Creation - modified for OpenGL
	SDL_Window *win = nullptr;

	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	int width = dm.w;
	int height = dm.h;

	win = SDL_CreateWindow("Ross Mills - 14589844 | Module - Graphics - GCP2012M", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width / 2, height / 2, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (win == nullptr) 
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			"SDL_CreateWindow init error: %s\n", SDL_GetError());
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	// Create an OpenGL context associated with the window.
	SDL_GLContext glcontext = SDL_GL_CreateContext(win);

	std::cout << "OpenGL version is" << glGetString(GL_VERSION) << std::endl;

	//create shaders
	Shader vSh("..//src//shader_projection_lighting_ADS.vert");
	Shader fSh("..//src//shader_projection_lighting_ADS.frag");

	//OpenGL specific values
	//****************************
	// OpenGL calls.
	GLenum err = glewInit();

	//compile the shader code
	//1 for vertex, 2 for fragment
	vSh.getShader(1);
	fSh.getShader(2);

	//Create and link the Shader program
	GLuint shaderProgram;

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vSh.shaderID);
	glAttachShader(shaderProgram, fSh.shaderID);
	glLinkProgram(shaderProgram);

	//delete shaders
	glDeleteShader(vSh.shaderID);
	glDeleteShader(fSh.shaderID);

	//set uniform variables
	int modelLocation;
	int viewLocation;
	int projectionLocation;

	glUseProgram(shaderProgram);

	
	//tranform matrices 
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	glm::mat4 testMove;
	testMove = glm::translate(testMove, glm::vec3(0.2f, 0.0f, 0.0f));

	//set view matrix to identity
	//matrix equivalent of '1'
	viewMatrix = glm::mat4(1.0f);

	//orthographic projection
	//left, right, bottom, top, near clip plane, far clip plane
	//projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 100.0f);
	//projectionMatrix = glm::ortho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 100.0f);

	projectionMatrix = glm::perspective(glm::radians(45.0f), (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, -2.0f));

	SDL_Event event;
	bool windowOpen = true;

// ******************    LOADING OBJ FILE    ********************************

	
	

// **************************************************************************

	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	Square test;
	test.setBuffers();

	while (windowOpen)
	{
		glUseProgram(shaderProgram);

		//background colour
		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		modelLocation = glGetUniformLocation(shaderProgram, "uModel");
		viewLocation = glGetUniformLocation(shaderProgram, "uView");
		projectionLocation = glGetUniformLocation(shaderProgram, "uProjection");

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		test.render();

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:

				switch (event.key.keysym.sym)
				{
				case SDLK_LEFT:
					//update x position
					cameraPos.x -= 0.01f;
					break;
				case SDLK_RIGHT:
					//update x position
					cameraPos.x += 0.01f;
					break;
				case SDLK_UP:
					//update z position
					cameraPos.z -= 0.01f;
					break;
				case SDLK_DOWN:
					//update z position
					cameraPos.z += 0.01f;
					break;
				}

			default:
				break;
			}
		}

		SDL_Log("%f", cameraPos.x);
		viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	}
	// Clean up
	SDL_Log("Finished. Cleaning up and closing down\n");

	// Once finished with OpenGL functions, the SDL_GLContext can be deleted.
	SDL_GL_DeleteContext(glcontext);

	SDL_Quit();
	return 0;
}
#endif





// *****************************************************************************************



// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

//objLoader loader;

// Read our .obj file


/*std::vector <glm::vec3> vertices;
std::vector <glm::vec2> uvs;
std::vector <glm::vec3> normals;
bool res = loadOBJ("..\\src\\TexturedCube.obj", vertices, uvs, normals);

glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);*/





// *****************************************************************************************





//bool loadOBJ(const char * path, std::vector < glm::vec3 > & out_vertices, std::vector < glm::vec2 > & out_uvs, std::vector < glm::vec3 > & out_normals)
//{
//	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
//	std::vector< glm::vec3 > temp_vertices;
//	std::vector< glm::vec2 > temp_uvs;
//	std::vector< glm::vec3 > temp_normals;
//
//	FILE * file = fopen(path, "r");
//	if (file == NULL) {
//		printf("Impossible to open the file !\n");
//		return false;
//	}
//
//
//	while (1)
//	{
//		char lineHeader[128];
//		// read the first word of the line
//		int res = fscanf(file, "%s", lineHeader);
//
//		if (res == EOF)
//		{
//			break; // EOF = End Of File. Quit the loop.
//		}
//		// else : parse lineHeader
//
//		if (strcmp(lineHeader, "v") == 0)
//		{
//			glm::vec3 vertex;
//			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
//			temp_vertices.push_back(vertex);
//
//		}
//		else if (strcmp(lineHeader, "vt") == 0)
//		{
//			glm::vec2 uv;
//			fscanf(file, "%f %f\n", &uv.x, &uv.y);
//			temp_uvs.push_back(uv);
//
//		}
//		else if (strcmp(lineHeader, "vn") == 0)
//		{
//			glm::vec3 normal;
//			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
//			temp_normals.push_back(normal);
//
//		}
//		else if (strcmp(lineHeader, "f") == 0)
//		{
//			std::string vertex1, vertex2, vertex3;
//			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
//			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
//			if (matches != 9)
//			{
//				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
//				return false;
//			}
//
//			vertexIndices.push_back(vertexIndex[0]);
//			vertexIndices.push_back(vertexIndex[1]);
//			vertexIndices.push_back(vertexIndex[2]);
//			uvIndices.push_back(uvIndex[0]);
//			uvIndices.push_back(uvIndex[1]);
//			uvIndices.push_back(uvIndex[2]);
//			normalIndices.push_back(normalIndex[0]);
//			normalIndices.push_back(normalIndex[1]);
//			normalIndices.push_back(normalIndex[2]);
//
//			// For each vertex of each triangle
//			for (unsigned int i = 0; i < vertexIndices.size(); i++)
//			{
//
//				unsigned int vertexIndex = vertexIndices[i];
//				glm::vec3 vertex = temp_vertices[vertexIndex - 1];
//				out_vertices.push_back(vertex);
//
//				unsigned int uvIndex = uvIndices[i];
//				glm::vec2 uv = temp_uvs[uvIndex - 1];
//				out_uvs.push_back(uv);
//
//				unsigned int normalIndex = normalIndices[i];
//				glm::vec3 normal = temp_normals[normalIndex - 1];
//				out_normals.push_back(normal);
//			}
//		}
//	}
//}