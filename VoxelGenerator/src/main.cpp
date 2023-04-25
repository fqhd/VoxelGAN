#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glad.h"
#include <vector>

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
constexpr int CHUNK_WIDTH = 16;

struct Vertex {
    Vertex(GLubyte _x, GLubyte _y, GLubyte _z, GLubyte _r, GLubyte _g, GLubyte _b) {
        x = _x;
		y = _y;
		z = _z;
		r = _r;
		g = _g;
		b = _b;
    }
    GLubyte x, y, z, r, g, b;
};

struct Block {
    Block() {
        r = 0;
        g = 0;
        b = 0;
        a = 0;
    }
    Block(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }
    unsigned char r, g, b, a;
};

static GLuint meshVAO;
static GLuint meshVBO;
static std::vector<Vertex> vertices;
static Block* data;

void generateVoxels();
void updateVAO();
void createShader();
void addTopFace(GLubyte x, GLubyte y, GLubyte z, GLubyte r, GLubyte g, GLubyte b);
void addBottomFace(GLubyte x, GLubyte y, GLubyte z, GLubyte r, GLubyte g, GLubyte b);
void addRightFace(GLubyte x, GLubyte y, GLubyte z, GLubyte r, GLubyte g, GLubyte b);
void addLeftFace(GLubyte x, GLubyte y, GLubyte z, GLubyte r, GLubyte g, GLubyte b);
void addFrontFace(GLubyte x, GLubyte y, GLubyte z, GLubyte r, GLubyte g, GLubyte b);
void addBackFace(GLubyte x, GLubyte y, GLubyte z, GLubyte r, GLubyte g, GLubyte b);
void addBlock(GLubyte x, GLubyte y, GLubyte z, GLubyte r, GLubyte g, GLubyte b);



int main() {
    data = new Block[CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_WIDTH];
    if(!glfwInit()){
        std::cout << "Failed to initialize glfw" << std::endl;
    }
	glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Window", nullptr, nullptr);    
    if(!window) {
        std::cout << "Failed to create window" << std::endl;
    }
    glfwMakeContextCurrent(window);
    if(!gladLoadGL()) {
        std::cout << "Failed to initialize glad" << std::endl;
    }

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

    glGenVertexArrays(1, &meshVAO);
    glBindVertexArray(meshVAO);

    glGenBuffers(1, &meshVBO);
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribIPointer(0, 3, GL_UNSIGNED_BYTE, sizeof(Vertex), 0);
    glVertexAttribIPointer(1, 3, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)3);
    createShader();

    generateVoxels();
    
    updateVAO();
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());

        glfwSwapBuffers(window);
    }

    return 0;
}

bool isBlockInLocalWorld(int _x, int _y, int _z){
	if(_x < 0 || _x >= CHUNK_WIDTH || _z < 0 || _z >= CHUNK_WIDTH || _y < 0 || _y >= CHUNK_WIDTH) return false;
	return true;
}

Block getBlock(int _x, int _y, int _z){
	if (!isBlockInLocalWorld(_x, _y, _z)) {
		return Block();
	}
	return data[(_y * CHUNK_WIDTH * CHUNK_WIDTH) + (_z * CHUNK_WIDTH) + _x];
}

void generateVoxels() {
    for(int y = 0; y < CHUNK_WIDTH; y++) {
        for(int z = 0; z < CHUNK_WIDTH; z++) {
            for(int x = 0; x < CHUNK_WIDTH; x++) {
                if(y == 0) {
                    data[y * CHUNK_WIDTH * CHUNK_WIDTH + z * CHUNK_WIDTH + x].r = 255;
                    data[y * CHUNK_WIDTH * CHUNK_WIDTH + z * CHUNK_WIDTH + x].g = 255;
                    data[y * CHUNK_WIDTH * CHUNK_WIDTH + z * CHUNK_WIDTH + x].b = 255;
                    data[y * CHUNK_WIDTH * CHUNK_WIDTH + z * CHUNK_WIDTH + x].a = 255;
                }else{
                    data[y * CHUNK_WIDTH * CHUNK_WIDTH + z * CHUNK_WIDTH + x].r = 0;
                    data[y * CHUNK_WIDTH * CHUNK_WIDTH + z * CHUNK_WIDTH + x].g = 0;
                    data[y * CHUNK_WIDTH * CHUNK_WIDTH + z * CHUNK_WIDTH + x].b = 0;
                    data[y * CHUNK_WIDTH * CHUNK_WIDTH + z * CHUNK_WIDTH + x].a = 0;
                }
            }
        }
    }
	data[1 * CHUNK_WIDTH * CHUNK_WIDTH + 6 * CHUNK_WIDTH + 5].r = 255;
	data[1 * CHUNK_WIDTH * CHUNK_WIDTH + 6 * CHUNK_WIDTH + 5].g = 0;
	data[1 * CHUNK_WIDTH * CHUNK_WIDTH + 6 * CHUNK_WIDTH + 5].b = 0;
	data[1 * CHUNK_WIDTH * CHUNK_WIDTH + 6 * CHUNK_WIDTH + 5].a = 255;

}

void updateVAO() {
    vertices.clear();

    for(unsigned int y = 0; y < CHUNK_WIDTH; y++){
		for(unsigned int z = 0; z < CHUNK_WIDTH; z++){
			for(unsigned int x = 0; x < CHUNK_WIDTH; x++){
				Block block = data[y * CHUNK_WIDTH * CHUNK_WIDTH + z * CHUNK_WIDTH + x];

				if(block.a){
					addBlock(x, y, z, block.r, block.g, block.b);
				}
			}
		}
	}

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}


void addBlock(GLubyte x, GLubyte y, GLubyte z, GLubyte r, GLubyte g, GLubyte b){
	addTopFace(x, y, z, r, g, b);
	addBottomFace(x, y, z, r * 0.90, g * 0.90, b * 0.90);
	addLeftFace(x, y, z, r * 0.94, g * 0.94, b * 0.94);
	addRightFace(x, y, z, r * 0.98, g * 0.98, b * 0.98);
	addFrontFace(x, y, z, r * 0.96, g * 0.96, b * 0.96);
	addBackFace(x, y, z, r * 0.92, g * 0.92, b * 0.92);
}

unsigned int calcAO(bool side1, bool side2, bool corner, bool face){
	if(face) return 2;
	if(side1 && side2){
		return 0;
	}
	return 3 - (side1 + side2 + corner);
}

float map(float ao) {
	return 0.6 + ao * 0.4;
}

void addTopFace(GLubyte x, GLubyte y, GLubyte z, GLubyte r, GLubyte g, GLubyte b){
	Block adjacentBlock = getBlock(x, y + 1, z);
	if(adjacentBlock.a) return;

	float a00 = calcAO(getBlock(x, y + 1, z - 1).a, getBlock(x - 1, y + 1, z).a, getBlock(x - 1, y + 1, z - 1).a, adjacentBlock.a) / 3.0f;
	float a01 = calcAO(getBlock(x - 1, y + 1, z).a, getBlock(x, y + 1, z + 1).a, getBlock(x - 1, y + 1, z + 1).a, adjacentBlock.a) / 3.0f;
	float a11 = calcAO(getBlock(x, y + 1, z + 1).a, getBlock(x + 1, y + 1, z).a, getBlock(x + 1, y + 1, z + 1).a, adjacentBlock.a) / 3.0f;
	float a10 = calcAO(getBlock(x, y + 1, z - 1).a, getBlock(x + 1, y + 1, z).a, getBlock(x + 1, y + 1, z - 1).a, adjacentBlock.a) / 3.0f;
	a00 = map(a00);
	a01 = map(a01);
	a11 = map(a11);
	a10 = map(a10);

	if(a00 + a11 > a01 + a10) {
		// Generate normal quad
		vertices.emplace_back(x, y + 1, z, a00*r, a00*g, a00*b);
		vertices.emplace_back(x, y + 1, z + 1, a01*r, a01*g, a01*b);
		vertices.emplace_back(x + 1, y + 1, z + 1, a11*r, a11*g, a11*b);
		vertices.emplace_back(x, y + 1, z, a00*r, a00*g, a00*b);
		vertices.emplace_back(x + 1, y + 1, z + 1, a11 * r, a11 * g, a11 * b);
		vertices.emplace_back(x + 1, y + 1, z, a10 * r, a10 * g, a10 * b);
	} else {
		// Generate flipped quad
		vertices.emplace_back(x + 1, y + 1, z, a10 * r, a10 * g, a10 * b);
		vertices.emplace_back(x, y + 1, z, a00 * r, a00 * g, a00 * b);
		vertices.emplace_back(x, y + 1, z + 1, a01 * r, a01 * g, a01 * b);
		vertices.emplace_back(x + 1, y + 1, z, a10 * r, a10 * g, a10 * b);
		vertices.emplace_back(x, y + 1, z + 1, a01 * r, a01 * g, a01 * b);
		vertices.emplace_back(x + 1, y + 1, z + 1, a11 * r, a11 * g, a11 * b);
	}
}

void addBottomFace(GLubyte x, GLubyte y, GLubyte z, GLubyte r, GLubyte g, GLubyte b){
	Block adjacentBlock = getBlock(x, y - 1, z);
	if(adjacentBlock.a) return;

	float a00 = calcAO(getBlock(x, y - 1, z - 1).a, getBlock(x - 1, y - 1, z).a, getBlock(x - 1, y - 1, z - 1).a, adjacentBlock.a) / 3.0f;
	float a01 = calcAO(getBlock(x - 1, y - 1, z).a, getBlock(x, y - 1, z + 1).a, getBlock(x - 1, y - 1, z + 1).a, adjacentBlock.a) / 3.0f;
	float a10 = calcAO(getBlock(x, y - 1, z - 1).a, getBlock(x + 1, y - 1, z).a, getBlock(x + 1, y - 1, z - 1).a, adjacentBlock.a) / 3.0f;
	float a11 = calcAO(getBlock(x, y - 1, z + 1).a, getBlock(x + 1, y - 1, z).a, getBlock(x + 1, y - 1, z + 1).a, adjacentBlock.a) / 3.0f;
	a00 = map(a00);
	a01 = map(a01);
	a11 = map(a11);
	a10 = map(a10);

	if(a00 + a11 > a01 + a10) {
		// Generate normal quad
		vertices.emplace_back(x, y, z, a00 * r, a00 * g, a00 * b);
		vertices.emplace_back(x + 1, y, z + 1, a11 * r, a11 * g, a11 * b);
		vertices.emplace_back(x, y, z + 1, a01 * r, a01 * g, a01 * b);
		vertices.emplace_back(x, y, z, a00 * r, a00 * g, a00 * b);
		vertices.emplace_back(x + 1, y, z, a10 * r, a10 * g, a10 * b);
		vertices.emplace_back(x + 1, y, z + 1, a11 * r, a11 * g, a11 * b);

	} else {
		// Generate flipped quad
		vertices.emplace_back(x + 1, y, z, a10 * r, a10 * g, a10 * b);
		vertices.emplace_back(x, y, z + 1, a01 * r, a01 * g, a01 * b);
		vertices.emplace_back(x, y, z, a00 * r, a00 * g, a00 * b);
		vertices.emplace_back(x + 1, y, z, a10 * r, a10 * g, a10 * b);
		vertices.emplace_back(x + 1, y, z + 1, a11 * r, a11 * g, a11 * b);
		vertices.emplace_back(x, y, z + 1, a01 * r, a01 * g, a01 * b);
	}
}

void addRightFace(GLubyte x, GLubyte y, GLubyte z, GLubyte r, GLubyte g, GLubyte b){
	Block adjacentBlock = getBlock(x - 1, y, z);
	if(adjacentBlock.a) return;

	float a00 = calcAO(getBlock(x - 1, y, z - 1).a, getBlock(x - 1, y - 1, z).a, getBlock(x - 1, y - 1, z - 1).a, adjacentBlock.a) / 3.0f;
	float a01 = calcAO(getBlock(x - 1, y, z - 1).a, getBlock(x - 1, y + 1, z).a, getBlock(x - 1, y + 1, z - 1).a, adjacentBlock.a) / 3.0f;
	float a10 = calcAO(getBlock(x - 1, y, z + 1).a, getBlock(x - 1, y - 1, z).a, getBlock(x - 1, y - 1, z + 1).a, adjacentBlock.a) / 3.0f;
	float a11 = calcAO(getBlock(x - 1, y + 1, z).a, getBlock(x - 1, y, z + 1).a, getBlock(x - 1, y + 1, z + 1).a, adjacentBlock.a) / 3.0f;
	a00 = map(a00);
	a01 = map(a01);
	a11 = map(a11);
	a10 = map(a10);

	if(a00 + a11 > a01 + a10) {
		// Generate normal quad
		vertices.emplace_back(x, y, z, a00 * r, a00 * g, a00 * b);
		vertices.emplace_back(x, y + 1, z + 1, a11 * r, a11 * g, a11 * b);
		vertices.emplace_back(x, y + 1, z, a01 * r, a01 * g, a01 * b);
		vertices.emplace_back(x, y, z, a00 * r, a00 * g, a00 * b);
		vertices.emplace_back(x, y, z + 1, a10 * r, a10 * g, a10 * b);
		vertices.emplace_back(x, y + 1, z + 1, a11 * r, a11 * g, a11 * b);
	} else {
		// Generate flipped quad
		vertices.emplace_back(x, y, z + 1, a10 * r, a10 * g, a10 * b);
		vertices.emplace_back(x, y + 1, z, a01 * r, a01 * g, a01 * b);
		vertices.emplace_back(x, y, z, a00 * r, a00 * g, a00 * b);
		vertices.emplace_back(x, y, z + 1, a10 * r, a10 * g, a10 * b);
		vertices.emplace_back(x, y + 1, z + 1, a11 * r, a11 * g, a11 * b);
		vertices.emplace_back(x, y + 1, z, a01 * r, a01 * g, a01 * b);
	}
}

void addLeftFace(GLubyte x, GLubyte y, GLubyte z, GLubyte r, GLubyte g, GLubyte b){
	Block adjacentBlock = getBlock(x + 1, y, z);
	if(adjacentBlock.a) return;

	float a00 = calcAO(getBlock(x + 1, y, z - 1).a, getBlock(x + 1, y - 1, z).a, getBlock(x + 1, y - 1, z - 1).a, adjacentBlock.a) / 3.0f;
	float a01 = calcAO(getBlock(x + 1, y, z - 1).a, getBlock(x + 1, y + 1, z).a, getBlock(x + 1, y + 1, z - 1).a, adjacentBlock.a) / 3.0f;
	float a10 = calcAO(getBlock(x + 1, y, z + 1).a, getBlock(x + 1, y - 1, z).a, getBlock(x + 1, y - 1, z + 1).a, adjacentBlock.a) / 3.0f;
	float a11 = calcAO(getBlock(x + 1, y + 1, z).a, getBlock(x + 1, y, z + 1).a, getBlock(x + 1, y + 1, z + 1).a, adjacentBlock.a) / 3.0f;
	a00 = map(a00);
	a01 = map(a01);
	a11 = map(a11);
	a10 = map(a10);

	if(a00 + a11 > a01 + a10) {
		// Generate normal quad
		vertices.emplace_back(x + 1, y, z, a00 * r, a00 * g, a00 * b);
		vertices.emplace_back(x + 1, y + 1, z, a01 * r, a01 * g, a01 * b);
		vertices.emplace_back(x + 1, y + 1, z + 1, a11 * r, a11 * g, a11 * b);
		vertices.emplace_back(x + 1, y, z, a00 * r, a00 * g, a00 * b);
		vertices.emplace_back(x + 1, y + 1, z + 1, a11 * r, a11 * g, a11 * b);
		vertices.emplace_back(x + 1, y, z + 1, a10 * r, a10 * g, a10 * b);
	} else {
		// Generate flipped quad
		vertices.emplace_back(x + 1, y + 1, z, a01 * r, a01 * g, a01 * b);
		vertices.emplace_back(x + 1, y + 1, z + 1, a11 * r, a11 * g, a11 * b);
		vertices.emplace_back(x + 1, y, z + 1, a10 * r, a10 * g, a10 * b);
		vertices.emplace_back(x + 1, y + 1, z, a01 * r, a01 * g, a01 * b);
		vertices.emplace_back(x + 1, y, z + 1, a10 * r, a10 * g, a10 * b);
		vertices.emplace_back(x + 1, y, z, a00 * r, a00 * g, a00 * b);
	}
}

void addFrontFace(GLubyte x, GLubyte y, GLubyte z, GLubyte r, GLubyte g, GLubyte b){
	Block adjacentBlock = getBlock(x, y, z - 1);
	if(adjacentBlock.a) return;

	float a00 = calcAO(getBlock(x - 1, y, z - 1).a, getBlock(x, y - 1, z - 1).a, getBlock(x - 1, y - 1, z - 1).a, adjacentBlock.a) / 3.0f;
	float a01 = calcAO(getBlock(x - 1, y, z - 1).a, getBlock(x, y + 1, z - 1).a, getBlock(x - 1, y + 1, z - 1).a, adjacentBlock.a) / 3.0f;
	float a10 = calcAO(getBlock(x, y - 1, z - 1).a, getBlock(x + 1, y, z - 1).a, getBlock(x + 1, y - 1, z - 1).a, adjacentBlock.a) / 3.0f;
	float a11 = calcAO(getBlock(x, y + 1, z - 1).a, getBlock(x + 1, y, z - 1).a, getBlock(x + 1, y + 1, z - 1).a, adjacentBlock.a) / 3.0f;
	a00 = map(a00);
	a01 = map(a01);
	a11 = map(a11);
	a10 = map(a10);

	if(a00 + a11 > a01 + a10) {
		// Generate normal quad
		vertices.emplace_back(x, y, z, a00 * r, a00 * g, a00 * b);
		vertices.emplace_back(x, y + 1, z, a01 * r, a01 * g, a01 * b);
		vertices.emplace_back(x + 1, y + 1, z, a11 * r, a11 * g, a11 * b);
		vertices.emplace_back(x, y, z, a00 * r, a00 * g, a00 * b);
		vertices.emplace_back(x + 1, y + 1, z, a11 * r, a11 * g, a11 * b);
		vertices.emplace_back(x + 1, y, z, a10 * r, a10 * g, a10 * b);
	} else {
		// Generate flipped quad
		vertices.emplace_back(x + 1, y, z, a10 * r, a10 * g, a10 * b);
		vertices.emplace_back(x, y, z, a00 * r, a00 * g, a00 * b);
		vertices.emplace_back(x, y + 1, z, a01 * r, a01 * g, a01 * b);
		vertices.emplace_back(x + 1, y, z, a10 * r, a10 * g, a10 * b);
		vertices.emplace_back(x, y + 1, z, a01 * r, a01 * g, a01 * b);
		vertices.emplace_back(x + 1, y + 1, z, a11 * r, a11 * g, a11 * b);
	}
}

void addBackFace(GLubyte x, GLubyte y, GLubyte z, GLubyte r, GLubyte g, GLubyte b){
	Block adjacentBlock = getBlock(x, y, z + 1);
	if(adjacentBlock.a) return;

	float a00 = calcAO(getBlock(x - 1, y, z + 1).a, getBlock(x, y - 1, z + 1).a, getBlock(x - 1, y - 1, z + 1).a, adjacentBlock.a) / 3.0f;
	float a01 = calcAO(getBlock(x - 1, y, z + 1).a, getBlock(x, y + 1, z + 1).a, getBlock(x - 1, y + 1, z + 1).a, adjacentBlock.a) / 3.0f;
	float a10 = calcAO(getBlock(x, y - 1, z + 1).a, getBlock(x + 1, y, z + 1).a, getBlock(x + 1, y - 1, z + 1).a, adjacentBlock.a) / 3.0f;
	float a11 = calcAO(getBlock(x, y + 1, z + 1).a, getBlock(x + 1, y, z + 1).a, getBlock(x + 1, y + 1, z + 1).a, adjacentBlock.a) / 3.0f;
	a00 = map(a00);
	a01 = map(a01);
	a11 = map(a11);
	a10 = map(a10);

	if(a00 + a11 > a01 + a10) {
		// Generate normal quad
		vertices.emplace_back(x, y, z + 1, a00 * r, a00 * g, a00 * b);
		vertices.emplace_back(x + 1, y + 1, z + 1, a11 * r, a11 * g, a11 * b);
		vertices.emplace_back(x, y + 1, z + 1, a01 * r, a01 * g, a01 * b);
		vertices.emplace_back(x, y, z + 1, a00 * r, a00 * g, a00 * b);
		vertices.emplace_back(x + 1, y, z + 1, a10 * r, a10 * g, a10 * b);
		vertices.emplace_back(x + 1, y + 1, z + 1, a11 * r, a11 * g, a11 * b);
	} else {
		// Generate a flipped quad
		vertices.emplace_back(x + 1, y, z + 1, a10 * r, a10 * g, a10 * b);
		vertices.emplace_back(x, y + 1, z + 1, a01 * r, a01 * g, a01 * b);
		vertices.emplace_back(x, y, z + 1, a00 * r, a00 * g, a00 * b);
		vertices.emplace_back(x + 1, y, z + 1, a10 * r, a10 * g, a10 * b);
		vertices.emplace_back(x + 1, y + 1, z + 1, a11 * r, a11 * g, a11 * b);
		vertices.emplace_back(x, y + 1, z + 1, a01 * r, a01 * g, a01 * b);
	}
}

void createShader(){
    GLuint program = glCreateProgram();
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

    const char* vsCode =
        "\n"
        "#version 330 core\n"
        "layout (location = 0) in uvec3 aPosition;\n"
        "layout (location = 1) in uvec3 aColor;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "out vec3 color;\n"
        "void main(){\n"
        "   gl_Position = projection * view * vec4(aPosition, 1.0);\n"
        "   color = vec3(aColor) / 255.0;\n"
        "}\n";

    const char* fsCode =
        "#version 330 core\n"
        "out vec4 outColor;\n"
        "in vec3 color;\n"
        "void main(){\n"
        "   outColor = vec4(color, 1.0);\n"
        "}\n";
    
    glShaderSource(vs, 1, &vsCode, 0);
    glShaderSource(fs, 1, &fsCode, 0);

    GLint vsCompileStatus, fsCompileStatus;

    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &vsCompileStatus);
    if(!vsCompileStatus){
        std::cout << "Failed to compile vertex shader" << std::endl;
        std::cout << vsCode << std::endl;
    }

    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &fsCompileStatus);
	if(!fsCompileStatus){
		std::cout << "Failed to compile fragment shader" << std::endl;
	}

    glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

    glUseProgram(program);

    glm::mat4 projection = glm::perspective(glm::radians(70.0f), WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(-CHUNK_WIDTH/2, CHUNK_WIDTH*1.25, -CHUNK_WIDTH/2), glm::vec3(CHUNK_WIDTH/2, 0.0, CHUNK_WIDTH/2), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);
}
