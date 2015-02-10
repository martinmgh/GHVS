#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "importer.h"

#define MAX_BONES 32
#define MODEL_FILE "monkey_with_bones_y_up.dae"

/* Shaders */
const GLchar* vertexSource =
"#version 410 core\n"
"layout(location = 0) in vec3 vpos;"
"layout(location = 1) in vec3 vnormal;"
"layout(location = 2) in vec2 vtexcoord;"
"layout(location = 3) in int bone_id;"
"out vec3 normal;"
"out vec2 st;"

"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 proj;"
"uniform mat4 bone_matrices[32];"

"void main(){"
"	st = vtexcoord;"
"	normal = vnormal;"
"	gl_Position = proj * view * model * bone_matrices[bone_id] * vec4(vpos, 1.0);"
"}";

const GLchar* fragmentSource =
"#version 410 core\n"
"in vec3 normal;"
"in vec2 st;"
"out vec4 outColor;"

"void main(){"
"	outColor = vec4(normal, 1.0);"
"}";

GLFWwindow* initGLFW(int width, int weight, char* title);
void initGLEW();
GLuint createShader(GLenum type, const GLchar* src);

int main(){
	float rot1 = 0.0f;
	float rot2 = 0.0f;
	int width = 1024;
	int height = 768;
	GLFWwindow* window = initGLFW(width, height, "PACT");
	glfwMakeContextCurrent(window);
	initGLEW();
	glEnable(GL_DEPTH_TEST);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/* Appel du loader */
	int point_ctr = 0;
	int bone_ctr = 0;
	glm::mat4 bone_offset_matrices[MAX_BONES];
	loadModel(MODEL_FILE, &vao, &point_ctr, bone_offset_matrices, &bone_ctr);
	printf("\nNombre de bones : %i\n", bone_ctr);

	/* Gestion des shaders */
	GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentSource);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	/* Initialisation des matrices de bones */
	glm::mat4 identity = glm::mat4(1.0f);
	int bone_matrices_loc[MAX_BONES];
	char name[64];
	for (int i = 0; i < MAX_BONES; i++){
		sprintf(name, "bone_matrices[%i]", i);
		bone_matrices_loc[i] = glGetUniformLocation(shaderProgram, name);
		glUniformMatrix4fv(bone_matrices_loc[i], 1, GL_FALSE, glm::value_ptr(identity));
	}

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::lookAt(
		glm::vec3(3.0f, 3.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 proj = glm::perspective(45.0f, 1024.0f / 768.0f, 0.1f, 100.0f);

	GLint uniModel = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	glm::mat4 leftEarMat;
	glm::mat4 rightEarMat;
	glm::mat4 headMat;
	float theta1 = 0.0f;
	float theta2 = 0.0f;
	float theta3 = 0.0f;


	while (!glfwWindowShouldClose(window)){
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);

		/* Taille de la fenetre */
		glfwGetWindowSize(window, &width, &height);
		glm::mat4 proj = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
		glUseProgram(shaderProgram);
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
		glViewport(0, 0, width, height);

		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Gere l'oreille gauche */
		leftEarMat = glm::mat4(1.0f);
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
			theta1 += 0.07f;
			leftEarMat = glm::inverse(bone_offset_matrices[0]) * glm::rotate(leftEarMat, glm::radians(theta1), glm::vec3(0.0f, 1.0f, 0.0f)) * bone_offset_matrices[0];
			glUseProgram(shaderProgram);
			glUniformMatrix4fv(bone_matrices_loc[0], 1, GL_FALSE, glm::value_ptr(leftEarMat));
		}

		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
			theta1 -= 0.07f;
			leftEarMat = glm::inverse(bone_offset_matrices[0]) * glm::rotate(leftEarMat, glm::radians(theta1), glm::vec3(0.0f, 1.0f, 0.0f)) * bone_offset_matrices[0];
			glUseProgram(shaderProgram);
			glUniformMatrix4fv(bone_matrices_loc[0], 1, GL_FALSE, glm::value_ptr(leftEarMat));
		}

		/* Gere la tete */
		headMat = glm::mat4(1.0f);
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS){
			theta3 += 0.07f;
			headMat = glm::inverse(bone_offset_matrices[2]) * glm::rotate(headMat, glm::radians(theta3), glm::vec3(1.0f, 0.0f, 0.0f)) * bone_offset_matrices[2];
			glUseProgram(shaderProgram);
			glUniformMatrix4fv(bone_matrices_loc[2], 1, GL_FALSE, glm::value_ptr(headMat));
		}
		if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS){
			theta3 -= 0.07f;
			headMat = glm::inverse(bone_offset_matrices[2]) * glm::rotate(headMat, glm::radians(theta3), glm::vec3(1.0f, 0.0f, 0.0f)) * bone_offset_matrices[2];
			glUseProgram(shaderProgram);
			glUniformMatrix4fv(bone_matrices_loc[2], 1, GL_FALSE, glm::value_ptr(headMat));
		}

		/* Gere l'oreille droite */
		rightEarMat = glm::mat4(1.0f);
		if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS){
			theta2 += 0.07f;
			rightEarMat = glm::inverse(bone_offset_matrices[1]) * glm::rotate(rightEarMat, glm::radians(theta2), glm::vec3(0.0f, 1.0f, 0.0f)) * bone_offset_matrices[1];
			glUseProgram(shaderProgram);
			glUniformMatrix4fv(bone_matrices_loc[1], 1, GL_FALSE, glm::value_ptr(rightEarMat));
		}

		if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS){
			theta2 -= 0.07f;
			rightEarMat = glm::inverse(bone_offset_matrices[1]) * glm::rotate(rightEarMat, glm::radians(theta2), glm::vec3(0.0f, 1.0f, 0.0f)) * bone_offset_matrices[1];
			glUseProgram(shaderProgram);
			glUniformMatrix4fv(bone_matrices_loc[1], 1, GL_FALSE, glm::value_ptr(rightEarMat));
		}

		/* Rotation du mod�le */
		rot1 = 0.0f;
		rot2 = 0.0f;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			rot1 += 0.07f;

		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			rot1 -= 0.07f;

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			rot2 += 0.07f;

		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			rot2 -= 0.07f;

		model = glm::rotate(model, glm::radians(rot2), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rot1), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0,point_ctr);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteVertexArrays(1, &vao);

	glfwTerminate();

	return 0;
}

GLFWwindow* initGLFW(int width, int height, char* title){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	return glfwCreateWindow(width, height, title, NULL, NULL);
}

void initGLEW(){
	glewExperimental = GL_TRUE;
	glewInit();
}

GLuint createShader(GLenum type, const GLchar* src){
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	return shader;
}