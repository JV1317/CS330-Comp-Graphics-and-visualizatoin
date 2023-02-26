/*

Jonathan Vest
CS330 Computer graphics and visualation
Module 4 Milestone

*/


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

#include <iostream>

#include "cylinder.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(7.0f, 13.0f, 20.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;
bool orthographic = false;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Jonathan Vest. Final Project", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("shaderfiles/7.3.camera.vs", "shaderfiles/7.3.camera.fs");

	// build and compile our shader zprogram
	// ------------------------------------
	//Shader lightingShader("2.1.basic_lighting.vs", "2.1.basic_lighting.fs");
	Shader lightCubeShader("6.light_cube.vs", "6.light_cube.fs");



	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		//Position			//Normals				//Texture
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	0.0f, 0.0f,		

	    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
	    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	0.0f, 0.0f,	

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,	1.0f, 0.0f,


		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	0.0f, 1.0f
	};

	unsigned int VBO, VAO;
	unsigned int VBO2, VAO2;
	unsigned int VBO3, VAO3;
	unsigned int lightCubeVAO;

	//Configure VBO and VAO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)	
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Configure VAO and VBO for plane
	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glBindVertexArray(VAO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)768);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);


	// load and create a texture 
	// -------------------------
	unsigned int texture1, texture2, texture3, texture4, texture5, texture6;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load("images/background.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	
	// texture 2
	// ---------
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("images/cube.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

// texture 3
// ---------
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data2 = stbi_load("images/metal1.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data2);

	// texture 4
	// ---------
	glGenTextures(1, &texture4);
	glBindTexture(GL_TEXTURE_2D, texture4);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("images/pencil.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// texture 5
	// ---------
	glGenTextures(1, &texture5);
	glBindTexture(GL_TEXTURE_2D, texture5);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("images/buds.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	// texture 6
	// ---------
	glGenTextures(1, &texture6);
	glBindTexture(GL_TEXTURE_2D, texture6);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("images/budsSide.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);
	ourShader.setInt("texture3", 2);
	ourShader.setInt("texture4", 3);
	ourShader.setInt("texture5", 4);
	ourShader.setInt("texture6", 5);

	glm::mat4 model;
	float angle;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// pass projection matrix to shader
		glm::mat4 projection;
		if (orthographic) {
			projection = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, 0.1f, 100.0f);
		}
		else {
			projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		}

		// camera/view transformation
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		ourShader.setMat4("view", view);		

		// pass projection matrix to shader (note that in this case it could change every frame) 
		ourShader.setMat4("projection", projection); 

		//Plane
		glBindTexture(GL_TEXTURE_2D, texture1);
		glBindVertexArray(VAO3);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, 30.0f, 0.0f));
		model = glm::scale(model, glm::vec3(45.0, 50.0, 50.0));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		//Box
		glBindTexture(GL_TEXTURE_2D, texture2);
		glBindVertexArray(VAO);

		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(4.0f, 6.6f, -2.0f));
		model = glm::scale(model, glm::vec3(3.0, 3.0, 3.0));
		angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Calculate model matrix for metal cylinder
		glBindTexture(GL_TEXTURE_2D, texture3);
		glBindVertexArray(VAO2);

		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(1.0f, 7.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8, 0.8, 0.8));
		ourShader.setMat4("model", model);

		//Draw Cylinder
		static_meshes_3D::Cylinder C(1, 100, 5, true, true, true);
		C.render();

		//Calculate model matrix for pincel 
		glBindTexture(GL_TEXTURE_2D, texture4);
		glBindVertexArray(VAO2);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(3.0f, 8.0f, -0.2f));
		model = glm::scale(model, glm::vec3(0.8, 0.8, 0.8));
		angle = -55.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		ourShader.setMat4("model", model);

		//Draw pincel
		static_meshes_3D::Cylinder C1(0.2, 5, 12, true, true, true);
		C1.render();

		//Calculate model matrix for bud cylinder Sids
		glBindTexture(GL_TEXTURE_2D, texture6);
		glBindVertexArray(VAO2);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(8.5f, 6.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8, 0.8, 0.8));
		ourShader.setMat4("model", model);

		//Draw bud sides
		static_meshes_3D::Cylinder C2(3.0, 4, 2, true, true, true);
		C2.render();

		//Calculate model matrix for bud cylinder Top
		glBindTexture(GL_TEXTURE_2D, texture5);
		glBindVertexArray(VAO2);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(8.5f, 6.8f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8, 0.8, 0.8));
		ourShader.setMat4("model", model);

		//Draw bud Top
		static_meshes_3D::Cylinder C3(3.0, 4, 0.01, true, true, true);
		C3.render();

		// also draw the lamp object
		/*lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lightCubeShader.setMat4("model", model);

		glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);*/

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);

	glDeleteVertexArrays(1, &VAO3);
	glDeleteBuffers(1, &VBO3);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 3.0 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		orthographic = !orthographic; 
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}