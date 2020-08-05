#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include <vector>

void mataCarro(int);
void carroDireita(int);
void carroEsquerda(int);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

class Modelo
{
public:
	Modelo();

	//transform
	float transX;
	float transY;
	float transZ;
	float scaleD;
	float animaX;
	float rotate;
	
	//animation
	bool animate;
	bool resetAnima;
	float animaTransX;

	bool animaCarroDireita;
	bool animaCarroEsquerda;
	
	//timing
	float tempo;
	float tempoInicio;

	//model draw
	unsigned int tipo;
};

Modelo::Modelo()
{
	transX = 0.0f;
	transY = -1.0f;
	transZ = 0.0f;
	scaleD = 0.3f;
	animaX = 0.0f;
	rotate = 0.0f;
	
	//animation
	animate = false;
	resetAnima = false;
	animaTransX = 0.0f;
	
	animaCarroDireita = false;
	animaCarroEsquerda = false;

	//timing
	tempo = 0.0f;
	tempoInicio = 0.0f;

	//model draw
	tipo = 0;

}

vector<Modelo> modelor;


unsigned int quantLuz = 10;//se mudar aqui, tem que mudar no fragment shader
unsigned int quant = 0;

unsigned int ripModelo;
bool assassino = false;

bool zSolto = true;
bool xSolto = true;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, 12.5f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//luz
glm::vec3 lightPos(0.0f, 2.0f, 0.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

    // build and compile shaders
    // -------------------------
    Shader ourShader(FileSystem::getPath("resources/cg_ufpel.vs").c_str(), FileSystem::getPath("resources/cg_ufpel.fs").c_str());


    // load models
    // -----------
	Model ourModel1(FileSystem::getPath("resources/objects/cyborg/cyborg.obj"));//pessoa
	Model ourModel2(FileSystem::getPath("resources/objects/Road_Set/Road_Set.obj"));//estrada
	Model ourModel3(FileSystem::getPath("resources/objects/rock/rock.obj"));//poste
	Model ourModel4(FileSystem::getPath("resources/objects/planet/planet.obj"));//carro


	glm::vec3 pointLightPositions[] = { //numero igual ao quant luz
		glm::vec3(10000.0f,10000.0f,10000.0f),
		glm::vec3(10000.0f,10000.0f,10000.0f),
		glm::vec3(10000.0f,10000.0f,10000.0f),
		glm::vec3(10000.0f,10000.0f,10000.0f),
		glm::vec3(10000.0f,10000.0f,10000.0f),
		glm::vec3(10000.0f,10000.0f,10000.0f),
		glm::vec3(10000.0f,10000.0f,10000.0f),
		glm::vec3(10000.0f,10000.0f,10000.0f),
		glm::vec3(10000.0f,10000.0f,10000.0f),
		glm::vec3(10000.0f,10000.0f,10000.0f)
	};


	quant++;
	modelor.resize(quant);
	modelor.push_back(Modelo());
	modelor[quant].tipo = 1;

	quant++;
	modelor.resize(quant);
	modelor.push_back(Modelo());
	modelor[quant].tipo = 2;

	quant++;
	modelor.resize(quant);
	modelor.push_back(Modelo());
	modelor[quant].tipo = 3;


    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();
		ourShader.setVec3("viewPos", camera.Position);
		ourShader.setFloat("material.shininess", 32.0f);


		//spot
		ourShader.setVec3("spotLight.position", lightPos);
		ourShader.setVec3("spotLight.direction", 0.0f, -1.0f, 0.0f);
		ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		ourShader.setVec3("spotLight.diffuse", 0.5f, 0.5f, 0.5f);
		ourShader.setVec3("spotLight.specular", 2.0f, 2.0f, 2.0f);
		ourShader.setFloat("spotLight.constant", 1.0f);
		ourShader.setFloat("spotLight.linear", 0.09);
		ourShader.setFloat("spotLight.quadratic", 0.032);
		ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
		ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(45.0f)));

		//  light points
		for (int i = 0; i < quantLuz; i++) {
			if (i<modelor.size()-4){
				pointLightPositions[i] = glm::vec3(modelor[i+4].transX+modelor[i+4].animaTransX, modelor[i + 4].transY+0.5f, modelor[i + 4].transZ);
			}else {
				pointLightPositions[i] = glm::vec3(10000.0f, 10000.0f, 10000.0f);
			}
			ourShader.setVec3("pointLights[" + to_string(i) + "].position", pointLightPositions[i]);
			ourShader.setVec3("pointLights[" + to_string(i) + "].ambient", 0.05f, 0.05f, 0.05f);
			ourShader.setVec3("pointLights[" + to_string(i) + "].diffuse", 2.0f, 2.0f, 2.0f);
			ourShader.setVec3("pointLights[" + to_string(i) + "].specular", 0.7f, 0.7f, 0.7f);
			ourShader.setFloat("pointLights[" + to_string(i) + "].constant", 0.5f);
			ourShader.setFloat("pointLights[" + to_string(i) + "].linear", 0.09f);
			ourShader.setFloat("pointLights[" + to_string(i) + "].quadratic", 0.1f);
		}
		

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
		for (int i = 0; i < modelor.size(); i++) {
			glm::mat4 model;
			model = glm::translate(model, glm::vec3(modelor[i].transX+ modelor[i].animaTransX, modelor[i].transY, modelor[i].transZ)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(modelor[i].scaleD, modelor[i].scaleD, modelor[i].scaleD));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(modelor[i].rotate), glm::vec3(0.0f, 1.0f, 0.0f));

			if (modelor[i].animate) {
				modelor[i].tempoInicio = glfwGetTime();
				modelor[i].tempo = 0.0f;
				modelor[i].resetAnima = true;
				modelor[i].animate = false;
			}

			if (modelor[i].tempo < 3.5f && modelor[i].resetAnima) {
				if (modelor[i].animaCarroDireita)
					carroDireita(i);
				if (modelor[i].animaCarroEsquerda)
					carroEsquerda(i);
			}
			else
				if (modelor[i].resetAnima) {
					mataCarro(i);
					modelor[i].resetAnima = false;
				}

			//-------PARTE FIXA----------------------------
			if (modelor[i].tipo == 1) {//desenhar pessoa
				ourShader.setMat4("model", model);
				ourModel1.Draw(ourShader);
			}
			if (modelor[i].tipo == 2) {//desenhar estrada
				model = glm::scale(model, glm::vec3(20.0f, 1.0f, 30.0f));
				modelor[i].rotate = 90.0f;
				modelor[i].transZ = 26.5f;
				ourShader.setMat4("model", model);
				ourModel2.Draw(ourShader);
			}
			if (modelor[i].tipo == 3) {//desenhar poste
				modelor[i].transY = 2.0f;
				modelor[i].scaleD = 0.15;
				ourShader.setMat4("model", model);
				ourModel3.Draw(ourShader);
			}
			//-------FIM PARTE FIXA------------------------


			//-------PARTE MOVEL----------------------------
			if (modelor[i].tipo == 4) {//desenhar carro
				ourShader.setMat4("model", model);
				ourModel4.Draw(ourShader);
			}
			//-------FIM PARTE MOVEL------------------------

		}
		if (assassino) {
			modelor.erase(modelor.begin()+ripModelo);
			quant--;
			assassino = false;
		}

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

//--------------------------------------------------------------
	unsigned int tempo = 500; //1000 milisegundos -> 1 segundo
	
	
	if ((glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)&& xSolto) {
		quant++;
		modelor.resize(quant);
		modelor.push_back(Modelo());
		modelor[quant].tipo = 4;
		modelor[quant].transX = 8.0f;
		modelor[quant].transZ = 2.5f;
		modelor[quant].animate = true;
		modelor[quant].animaCarroDireita = true;
		xSolto = false;
	}
	
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE)
		xSolto = true;

	if ((glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)&& zSolto) {
		quant++;
		modelor.resize(quant);
		modelor.push_back(Modelo());
		modelor[quant].tipo = 4;
		modelor[quant].transX = -8.0f;
		modelor[quant].transZ = 5.6f;
		modelor[quant].animate = true;
		modelor[quant].animaCarroEsquerda = true;
		zSolto = false;
	}

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)
		zSolto = true;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void carroDireita(int atual) {
	modelor[atual].animaTransX = modelor[atual].tempo * -5.0f;
	modelor[atual].tempo = glfwGetTime() - modelor[atual].tempoInicio;
}

void carroEsquerda(int atual) {
	modelor[atual].animaTransX = modelor[atual].tempo * 5.0f;
	modelor[atual].tempo = glfwGetTime() - modelor[atual].tempoInicio;
}

void mataCarro(int atual) {
	assassino = true;
	ripModelo = atual;
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
