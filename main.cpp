#include <vector>
#include <time.h>
#include <unistd.h>
#include "load.hpp"

#ifdef EMSCRIPTEN
	#include <emscripten.h>
	#include "key_mouse.c"
#endif


using namespace glm;

int height = 9*50;
int width = 16*50;

GLFWwindow* window;
GLuint programID;
GLuint Texture;
std::vector<unsigned short> indices;
std::vector< glm::vec3 > vertices;
std::vector< glm::vec2 > uvs;
std::vector< glm::vec3 > normals;

// get locations/handles
GLuint vPosition_m;
GLuint vertexUV;
GLuint vNormal_m;
GLuint vMVP;
GLuint V;
GLuint M;
GLuint TextureID;
GLuint LightID;
GLuint LightID2;

class camera{
	glm::mat4 View;
	glm::mat4 Projection;
	public:
	camera(glm::vec3 location, glm::vec3 view, glm::vec3 up){
		View = glm::lookAt(location, view, up);
		Projection = glm::perspective(45.0f, float(width)/float(height), 0.1f, 100.0f);
	}
	void update(){}
	glm::mat4 getView(){return View;}
	glm::mat4 getProj(){return Projection;}
};

camera maincamera(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0));
glm::vec3 lightPos = glm::vec3(4,4,4);

void initGL(){
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	programID = glCreateProgram ( );
	LoadShader (programID, GL_VERTEX_SHADER, "vShader" );
	LoadShader (programID, GL_FRAGMENT_SHADER, "fShader" );
	glLinkProgram ( programID );
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	Texture = loadDDS("uvmap.DDS");
	std::vector<glm::vec3> unindexed_vertices;
	std::vector<glm::vec2> unindexed_uvs;
	std::vector<glm::vec3> unindexed_normals;
	loadOBJ("suzanne.obj", unindexed_vertices, unindexed_uvs, unindexed_normals);
	indexVBO(unindexed_vertices, unindexed_uvs, unindexed_normals, indices, vertices, uvs, normals);

	// setup handles
	vPosition_m = glGetAttribLocation (programID, "vPosition_m");
	vertexUV = glGetAttribLocation(programID, "vertexUV");
	vNormal_m = glGetAttribLocation (programID, "vNormal_m");
	vMVP = glGetUniformLocation(programID, "vMVP");
	V = glGetUniformLocation(programID, "V");
	M = glGetUniformLocation(programID, "M");
	TextureID  = glGetUniformLocation(programID, "myTextureSampler");
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	LightID2 = glGetUniformLocation(programID, "LightPosition_worldspace2");
}

void mainloop(){
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram ( programID );

//	std::cout << glfwGetTime() <<"\n";
	// camera
	glm::mat4 Model      = glm::mat4(1.0f);
	glm::mat4 View       = maincamera.getProj() * maincamera.getView();
	glm::mat4 Matrix     = View * Model;
	// send matrix to buffer
	glUniformMatrix4fv(M, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(V, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(vMVP, 1, GL_FALSE, &Matrix[0][0]);
	// send lightsource to buffer
	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(LightID2, lightPos.x, lightPos.y, lightPos.z);

	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	// create uniform buffer
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	// create normal buffer
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	// create index buffer
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);
	// texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(TextureID, 0);

	// bind vertex buffer
	glEnableVertexAttribArray(vPosition_m);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer( vPosition_m, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// attribute, size, type, normalized?, stride, array buffer offset
	// bind UV buffer
	glEnableVertexAttribArray(vertexUV);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer( vertexUV, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	// bind normal buffer
	glEnableVertexAttribArray(vNormal_m);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer( vNormal_m, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	// bind element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		indices.size(),    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);

	glDisableVertexAttribArray(vPosition_m);
	glDisableVertexAttribArray(vertexUV);
	glDisableVertexAttribArray(vNormal_m);

	glfwSwapBuffers(window);
	return;
}


int main(){
	window = init(width, height);
	#ifdef EMSCRIPTEN
	init_keyboard_and_mouse(width, height);
	#endif
	initGL();

	#ifdef EMSCRIPTEN
	emscripten_set_main_loop(mainloop, 30, 1);
	#else
	double wait = glfwGetTime();
	double temp;
	while(!glfwWindowShouldClose(window)){
		temp = glfwGetTime()-wait;
		if(temp < .3){usleep((.3-temp)*1000000);}
		wait = glfwGetTime();
		mainloop();
	}
	#endif

	// cleanup
}
