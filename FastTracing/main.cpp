#include "globalGL.h"
#include "Window.h"
#include "Shader.h"
#include "VAO.h"
#include "Chunk.h"
#include "World.h"
#include "Camera.h"
#include "Texture.h"
#include "CollisionInfo.h"
#include "FBO.h"

#define WIN_X 1920
#define WIN_Y 1080

int p_block_id = 1;

const char* curr_map = "worlds/first.wrld";

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	p_block_id += yoffset;
	if (p_block_id > 16) p_block_id = 16;
	if (p_block_id < 1) p_block_id = 1;
}

int main() {
	Window win(WIN_X, WIN_Y, "fck", 1);
	glfwSetScrollCallback(win.getGLFWWindow(), scroll_callback);
	World w("worlds/first.wrld");
	//World w;

	//scene1

	//w.loadFromHeightMapToFile("heightmaps/map16.png", "worlds/map512.wrld");

	//scene2
	//w.loadFromHeightMap("heightmaps/0qXhFa.png");

	//scene3
	//w.loadFromHeightMap("heightmaps/pole.png");

	//scene4
	//w.loadFromHeightMap("heightmaps/Untitled.png");
	// 
	//scene5
	//w.loadFromHeightMap("heightmaps/Untitled1.png");
	
	float light_ratio = 1.f;

	Camera cam;
	
	std::vector<float> dt = { 1,  1,
		-1,  1,
		-1, -1,
		 1,  1,
		 1, -1,
		-1, -1 };
	std::vector<GLfloat> tx_coord = {
		 1,  1,
		0,  1,
		0, 0,
		1,  1,
		 1, 0,
		0, 0
	};

	FBO main_fbo, pipe1_fbo, temp_fbo;
	main_fbo.initTexture(WIN_X, WIN_Y);
	pipe1_fbo.initTexture(WIN_X, WIN_Y);
	temp_fbo.initTexture(WIN_X, WIN_Y);

	Shader sh, pipe_1, pipe_2;
	sh.Init("shaders/tracing3_1.shader", 0);
	sh.setUniformV2("resolution", glm::vec2(WIN_X, WIN_Y));
	sh.setUniform1i("sz_x", w.getSize().x);
	sh.setUniform1i("sz_y", w.getSize().y);
	sh.setUniform1i("sz_z", w.getSize().z);

	pipe_1.Init("shaders/pipe_1.shader", 0);
	pipe_2.Init("shaders/pipe_2.shader", 0);

	Texture texture_pack;
	texture_pack.loadFromFile("textures/texture_pack.png");

	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	
	glBufferData(GL_SHADER_STORAGE_BUFFER, w.getDataSize()*sizeof(ui), w.getData(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	VAO main_vao;
	main_vao.vboInit();
	main_vao.setShader(sh);
	main_vao.vboRedata(dt.size() * sizeof(float), dt.data(), dt.size() / 2, data_type::pos);
	main_vao.vboRedata(tx_coord.size() * sizeof(float), tx_coord.data(), tx_coord.size() / 2, data_type::tex);
	
	bool r_pressed=false, l_pressed = false;
	float timer = 0;
	int sample = 1;
	while (!win.ShouldClose()) {
		win.frameStart();

		if (glfwGetKey(win.getGLFWWindow(), GLFW_KEY_4)) {
			sh.Init("shaders/tracing3_1.shader", 0);
			sh.setUniformV2("resolution", glm::vec2(WIN_X, WIN_Y));
			sh.setUniform1i("sz_x", w.getSize().x);
			sh.setUniform1i("sz_y", w.getSize().y);
			sh.setUniform1i("sz_z", w.getSize().z);
		}
		if (glfwGetKey(win.getGLFWWindow(), GLFW_KEY_5)) {
			sh.Init("shaders/tracing3_2.shader", 0);
			sh.setUniformV2("resolution", glm::vec2(WIN_X, WIN_Y));
			sh.setUniform1i("sz_x", w.getSize().x);
			sh.setUniform1i("sz_y", w.getSize().y);
			sh.setUniform1i("sz_z", w.getSize().z);
			sh.setUniform1f("light_ratio", light_ratio);
		}
		if (glfwGetKey(win.getGLFWWindow(), GLFW_KEY_UP)) {
			light_ratio += 0.5 * win.get_deltaTime();
			sh.setUniform1f("light_ratio", light_ratio);
			sample = 1;
		}
		if (glfwGetKey(win.getGLFWWindow(), GLFW_KEY_DOWN)) {
			light_ratio -= 0.5 * win.get_deltaTime();
			if (light_ratio < 0) light_ratio = 0;
			sh.setUniform1f("light_ratio", light_ratio);
			sample = 1;
		}
		if (glfwGetMouseButton(win.getGLFWWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) l_pressed = false;
		if (glfwGetMouseButton(win.getGLFWWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) r_pressed = false;
		if (glfwGetMouseButton(win.getGLFWWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && (!l_pressed || glfwGetKey(win.getGLFWWindow(), GLFW_KEY_LEFT_ALT))) {
			CollisionInfo cl;
			Ray mid;
			mid.rayOrig = cam.cameraPos;
			mid.rayDir = cam.cameraFront;

			if (cl.DDA_chunks(mid, w)) {
				w.setBlock(cl.voxel_pos.x, cl.voxel_pos.y, cl.voxel_pos.z, 0);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
				auto itt = w.get_change_que();
				for (int i = 0; i < itt.size(); i++) {
					ui dati = itt[i].second;
					glBufferSubData(GL_SHADER_STORAGE_BUFFER, itt[i].first * sizeof(ui), sizeof(ui), &dati);
				}
			}
			l_pressed = true;
			sample = 1;
		}
		if (glfwGetMouseButton(win.getGLFWWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && (!r_pressed || glfwGetKey(win.getGLFWWindow(), GLFW_KEY_LEFT_ALT))) {
			CollisionInfo cl;
			Ray mid;
			mid.rayOrig = cam.cameraPos;
			mid.rayDir = cam.cameraFront;

			if (cl.DDA_chunks(mid, w)) {
				w.setBlock(cl.voxel_pos.x + int(cl.normal.x), cl.voxel_pos.y + int(cl.normal.y), cl.voxel_pos.z + int(cl.normal.z), p_block_id);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
				auto itt = w.get_change_que();
				for (int i = 0; i < itt.size(); i++) {
					ui dati = itt[i].second;
					glBufferSubData(GL_SHADER_STORAGE_BUFFER, itt[i].first * sizeof(ui), sizeof(ui), &dati);
				}
			}
			r_pressed = true;
			sample = 1;
		}
		if (cam.update(win)) sample = 1;

		sh.setCamera(cam);

		sh.setUniform1f("tr",timer);
		texture_pack.bind();
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);


		//pipeline

		temp_fbo.bind();
		main_vao.setShader(sh);
		sh.setUniform1i("curr_sample", sample);
		main_vao.draw();
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		temp_fbo.unbind();

		main_fbo.bind();
		main_vao.setShader(pipe_1);
		pipe_1.setUniform1i("curr_sample", sample);
		pipe_1.setUniform1i("tex1", 0);
		glActiveTexture(GL_TEXTURE0);
		main_fbo.bindFBO_Texture();
		pipe_1.setUniform1i("tex2", 1);
		glActiveTexture(GL_TEXTURE1);
		temp_fbo.bindFBO_Texture();
		main_vao.draw();

		FBO::bindDefault();
		main_vao.setShader(pipe_2);
		glActiveTexture(GL_TEXTURE0);
		main_fbo.bindFBO_Texture();
		main_vao.draw();
		
		timer += win.get_deltaTime();
		win.frameEnd();
		sample++;
	}

	glDeleteBuffers(1, &ssbo);
	
	
	return 0;
}