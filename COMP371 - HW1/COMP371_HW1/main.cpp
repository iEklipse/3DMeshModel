/*
	COMP 371 - ASSIGNMENT 1
	TRI-LUONG STEVEN DIEN
	27415281
*/

// GLEW
#include "glew.h"

// GLFW
#include "glfw3.h"

// GLM Mathematics
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "gtc/constants.hpp"

// Various headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cctype>

using namespace std;

#define M_PI        3.14159265358979323846264338327950288   /* pi */
#define DEG_TO_RAD	M_PI/180.0f

// Function prototypes
void window_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void do_movement();
void rotateAround();
void moveInAndOut();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void readFile();
void putIndices(int numberPoints1, int numberPoints2, int sweepType);
bool initialize();
bool cleanUp();
GLuint loadShaders(std::string vertex_shader_path, std::string fragment_shader_path);

// GLFWwindow pointer
GLFWwindow* window = 0x00;

// Window dimensions
int width = 800, height = 800;

// Program ID
GLuint shader_program = 0;

// View, Model and Projection matrices ID
GLuint view_matrix_id = 0;
GLuint model_matrix_id = 0;
GLuint proj_matrix_id = 0;

// Transformations matrices
glm::mat4 proj_matrix;
glm::mat4 view_matrix;
glm::mat4 model_matrix;

// Vertex Buffer Object, Vertex Array Object and Element Buffer Object
GLuint VBO, VAO, EBO;

// Model
GLfloat angle = 0.0f;
int renderingMode = 2;

// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;
GLfloat lastX = width / 2.0;
GLfloat lastY = height / 2.0;
GLfloat oldY;
GLfloat newY;
GLfloat fov = 45.0f;
bool firstMouse = true;
bool mouseFocused = false;
bool keys[1024];
bool buttons[1024];

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// File's input
int sweepType;
int numberOfPoints;
int points_1;
int points_2;
int trajectoryIndex = 3;
int numberOfSpans;
float spansAngle;

// Vectors for the profile points, trajectory points and indices
vector<GLfloat> profilePoints;
vector<GLfloat> trajectoryPoints;
vector<GLuint> indices;

// Variable to be used to increase the size of the points
GLfloat point_size = 3.0f;

// Is called whenever the window got resized via GLFW
void window_size_callback(GLFWwindow* window, int newWidth, int newHeight)
{
	width = newWidth;
	height = newHeight;

	glViewport(0, 0, width, height);
	proj_matrix = glm::perspective(glm::radians(fov), (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		renderingMode = 0;
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		renderingMode = 1;
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
		renderingMode = 2;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
			angle = 0.0f;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
			angle = 0.0f;
		}
	}
}

// Rotate the model around
void rotateAround()
{
	GLfloat rotationSpeed = 0.5f * deltaTime;

	if (keys[GLFW_KEY_UP])
	{
		angle -= rotationSpeed;
		model_matrix = glm::rotate(model_matrix, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	if (keys[GLFW_KEY_DOWN])
	{
		angle += rotationSpeed;
		model_matrix = glm::rotate(model_matrix, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	if (keys[GLFW_KEY_LEFT])
	{
		angle += rotationSpeed;
		model_matrix = glm::rotate(model_matrix, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (keys[GLFW_KEY_RIGHT])
	{
		angle -= rotationSpeed;
		model_matrix = glm::rotate(model_matrix, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
	}
}

// Is called whenever a mouse button is pressed/released via GLFW
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button >= 0 && button < 1024)
	{
		if (action == GLFW_PRESS)
			buttons[button] = true;
		else if (action == GLFW_RELEASE)
			buttons[button] = false;
	}
}

// Move around the model
void do_movement()
{
	if (keys[GLFW_KEY_M])
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		mouseFocused = true;
	}

	if (mouseFocused)
	{
		// Camera controls
		GLfloat cameraSpeed = 5.0f * deltaTime;
		if (keys[GLFW_KEY_R])
			cameraPos += cameraSpeed * cameraFront;
		if (keys[GLFW_KEY_F])
			cameraPos -= cameraSpeed * cameraFront;
		if (keys[GLFW_KEY_D])
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (keys[GLFW_KEY_G])
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
		
	if (keys[GLFW_KEY_N] && mouseFocused)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPos(window, width / 2, height / 2);
		mouseFocused = false;

		cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
		
}

// Move the camera in/out of the scene
void moveInAndOut()
{
	if (buttons[GLFW_MOUSE_BUTTON_LEFT] && newY > oldY && mouseFocused == false)
	{
		cameraPos += glm::vec3(0.0f, 0.0f, 0.1f);
		view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		oldY = newY;
	}
	if (buttons[GLFW_MOUSE_BUTTON_LEFT] && newY < oldY && mouseFocused == false)
	{
		cameraPos -= glm::vec3(0.0f, 0.0f, 0.1f);
		view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		oldY = newY;
	}
}

// Is called whenever the mouse moved via GLFW
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		newY = ypos;

		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	newY = ypos;

	if (!buttons[GLFW_MOUSE_BUTTON_LEFT] && mouseFocused)
	{
		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left
		lastX = xpos;
		lastY = ypos;

		GLfloat sensitivity = 0.05;	// Change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);
	}

}

// Read the input file
void readFile()
{
	// Read the file's input
	ifstream input("\input_a1.txt");

	if (input.fail())
	{
		cout << "File does not exist" << endl;
		cout << "Exit program" << endl;
	}

	// Get the sweep type
	input >> sweepType;

	if (sweepType == 0) // Translational Sweep
	{
		input >> points_1; // Get the number of points in the first batch

		// First batch of point's coordinates put inside the profilePoints vector
		for (int i = 0; i < points_1 * 3; i++)
		{
			GLfloat data;
			input >> data;
			profilePoints.push_back(data);
		}

		input >> points_2; // Get the number of points in the second batch

		// Update the total number of points that will be added in the vector
		numberOfPoints = points_1 * points_2;

		// Second batch of point's coordinates to be use to find the translation vector
		for (int i = 0; i < points_2 * 3; i++)
		{
			GLfloat data;
			input >> data;
			trajectoryPoints.push_back(data);
		}

		// Calculate the sequence of points after successive translations and add it to the profilePoints vector
		for (int i = points_1 * 3; i < numberOfPoints * 3; i += 3)
		{
			glm::vec4 vec(profilePoints[i - points_1 * 3], profilePoints[i - points_1 * 3 + 1], profilePoints[i - points_1 * 3 + 2], 1.0f);
			glm::mat4 trans;
			trans = glm::translate(trans, glm::vec3(trajectoryPoints[trajectoryIndex] - trajectoryPoints[trajectoryIndex - 3],
				trajectoryPoints[trajectoryIndex + 1] - trajectoryPoints[trajectoryIndex - 2],
				trajectoryPoints[trajectoryIndex + 2] - trajectoryPoints[trajectoryIndex - 1]));
			vec = trans * vec;

			profilePoints.push_back(vec.x);
			profilePoints.push_back(vec.y);
			profilePoints.push_back(vec.z);

			if ((i + 3) % (points_1 * 3) == 0)
				trajectoryIndex += 3;
		}
	}
	else // Rotational Sweep
	{
		input >> numberOfSpans; // Get the number of spans
		input >> numberOfPoints; // Get the number of points

		points_1 = numberOfPoints; // To be use for the indices vectors, number of points
		points_2 = numberOfSpans + 1; // To be use for the indices vectors, number of spans + 1 (We have to connect the last set of points with the first set)

		// Put the coordinates into the profilePoints vector
		for (int i = 0; i < numberOfPoints * 3; i++)
		{
			GLfloat data;
			input >> data;
			profilePoints.push_back(data);
		}

		// Calculate the angle to rotate each coordinate
		spansAngle = glm::radians(360.0f / numberOfSpans);

		// Transformation matrix with the correct angle to rate
		glm::mat4 trans;
		trans = glm::rotate(trans, spansAngle, glm::vec3(0.0, 1.0, 0.0));

		// Calculate the sequence of points after successive rotations and add it to the profilePoints vector
		for (int i = numberOfPoints * 3; i < numberOfPoints * (numberOfSpans + 1) * 3; i += 3)
		{
			glm::vec4 vec(profilePoints[i - numberOfPoints * 3], profilePoints[i - numberOfPoints * 3 + 1], profilePoints[i - numberOfPoints * 3 + 2], 1.0f);
			vec = trans * vec;

			profilePoints.push_back(vec.x);
			profilePoints.push_back(vec.y);
			profilePoints.push_back(vec.z);
		}

	}

	input.close();
}

// Calculate the correct order to put the indices in order to get a triangle mesh of the sweep surface
void putIndices(int numberPoints1, int numberPoints2, int sweepType)
{
	int a = 0;
	int b = a + 1;
	int c = a + numberPoints1 + 1;
	int d = a;
	int e = a + numberPoints1;
	int f = c;

	for (int i = 0; i < numberPoints2 - 1; i++)
	{
		for (int j = 0; j < numberPoints1 - 1; j++)
		{
			for (int k = 0; k < 6; k++)
				switch (k)
				{
				case 0:
					indices.push_back(a);
					break;
				case 1:
					indices.push_back(b);
					break;
				case 2:
					indices.push_back(c);
					break;
				case 3:
					indices.push_back(d);
					break;
				case 4:
					indices.push_back(e);
					break;
				case 5:
					indices.push_back(f);
					break;
				}

			if (j != numberPoints1 - 2)
			{
				a++;
				b = a + 1;
				c = a + numberPoints1 + 1;
				d = a;
				e = a + numberPoints1;
				f = c;
			}
		}

		a += 2;
		b = a + 1;
		c = a + numberPoints1 + 1;
		d = a;
		e = a + numberPoints1;
		f = c;
	}
}

bool initialize() {
	// Initialize GL context and O/S window using the GLFW helper library
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

	// Create a window of size 800x800 and with title "Lecture 2: First Triangle"
	window = glfwCreateWindow(width, height, "COMP371: Assignment 1", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// At initialization, set the mouse cursor at the center of the window
	glfwSetCursorPos(window, width / 2, height / 2);

	// Set the required callback functions
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// Initialize GLEW extension handler
	glewExperimental = GL_TRUE;	// Needed to get the latest version of OpenGL
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, width, height);

	// Get the current OpenGL version
	const GLubyte* renderer = glGetString(GL_RENDERER); // Get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // Version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	// Enable the depth test i.e. draw a pixel if it's closer to the viewer
	glEnable(GL_DEPTH_TEST); // Enable depth-testing
	glDepthFunc(GL_LESS);	// The type of testing i.e. a smaller value as "closer"

	return true;
}

bool cleanUp() {
	glDisableVertexAttribArray(0);
	//Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// Close GL context and any other GLFW resources
	glfwTerminate();

	return true;
}

GLuint loadShaders(std::string vertex_shader_path, std::string fragment_shader_path) {
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_shader_path.c_str());
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_shader_path.c_str());
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);

	glBindAttribLocation(ProgramID, 0, "in_Position");

	//appearing in the vertex shader.
	glBindAttribLocation(ProgramID, 1, "in_Color");

	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	//The three variables below hold the id of each of the variables in the shader
	//If you read the vertex shader file you'll see that the same variable names are used.
	view_matrix_id = glGetUniformLocation(ProgramID, "view_matrix");
	model_matrix_id = glGetUniformLocation(ProgramID, "model_matrix");
	proj_matrix_id = glGetUniformLocation(ProgramID, "proj_matrix");

	return ProgramID;
}

int main() {

	initialize();

	// Load the shaders
	shader_program = loadShaders("COMP371_HW1.vs", "COMP371_HW1.fs");

	// Read the file's input
	readFile();

	// Calculate and put the correct order of indices in the vector
	putIndices(points_1, points_2, sweepType);

	// Set up vertex data (and buffer(s)) and attribute pointers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind Vertex Array Object
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	// Copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, profilePoints.size() * sizeof(GLfloat), &profilePoints[0], GL_STATIC_DRAW);
	// Then set our vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	// Copy our elements array in a buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
	
	// Game Loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Wipe the drawing surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
		glPointSize(point_size);

		// Activate shader program
		glUseProgram(shader_program);

		// Camera/View transformation
		view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		// Projection
		proj_matrix = glm::perspective(glm::radians(fov), (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

		//Pass the values of the three matrices to the shaders
		glUniformMatrix4fv(proj_matrix_id, 1, GL_FALSE, glm::value_ptr(proj_matrix));
		glUniformMatrix4fv(view_matrix_id, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, glm::value_ptr(model_matrix));

		glBindVertexArray(VAO);
		switch (renderingMode) // Depending on the rendering mode choosen, the model will be draw with points, lines or triangles
		{
		case 0:
			glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
			break;
		case 1:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			break;
		case 2:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			break;
		}
		glBindVertexArray(0);

		// Update other events like input handling
		glfwPollEvents();
		rotateAround();
		moveInAndOut();
		do_movement();

		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(window);
	}

	cleanUp();
	return 0;
}