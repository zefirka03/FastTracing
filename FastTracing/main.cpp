#include "globalGL.h"
#include "Window.h"
#include "Shader.h"
#include "VAO.h"
#include "Chunk.h"
#include "World.h"
#include "Camera.h"
#include "Texture.h"
#include "CollisionInfo.h"

#define WIN_X 1440
#define WIN_Y 900

int p_block_id = 1;

const char* curr_map = "worlds/first.wrld";

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	p_block_id += yoffset;
	if (p_block_id > 17) p_block_id = 17;
	if (p_block_id < 1) p_block_id = 1;
}


int main() {
	Window win(WIN_X, WIN_Y, "fck", 0);
	glfwSetScrollCallback(win.getGLFWWindow(), scroll_callback);
	World w;
	//scene1

	//w.loadFromHeightMapToFile("heightmaps/mountains512-1.png", "worlds/map16x8x16.wrld");

	//scene2
	//w.loadFromHeightMap("heightmaps/0qXhFa.png");

	//scene3
	//w.loadFromHeightMap("heightmaps/pole.png");

	//scene4
	//w.loadFromHeightMap("heightmaps/Untitled.png");
	// 
	//scene5
	//w.loadFromHeightMap("heightmaps/Untitled1.png");
	w.loadFromFile(curr_map);
	
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

	Shader sh;
	sh.Init("shaders/tracing3_1.shader", 0);
	sh.setUniformV2("resolution", glm::vec2(WIN_X, WIN_Y));
	sh.setUniform1i("sz_x", w.getSize().x);
	sh.setUniform1i("sz_y", w.getSize().y);
	sh.setUniform1i("sz_z", w.getSize().z);

	Texture texture_pack;
	texture_pack.loadFromFile("textures/texture_pack_min.jpg");

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
	while (!win.ShouldClose()) {
		win.frameStart();

		if (glfwGetKey(win.getGLFWWindow(), GLFW_KEY_1)) {
			sh.Init("shaders/tracing.shader", 0);
			sh.setUniformV2("resolution", glm::vec2(WIN_X, WIN_Y));
			sh.setUniform1i("sz_x", w.getSize().x);
			sh.setUniform1i("sz_y", w.getSize().y);
			sh.setUniform1i("sz_z", w.getSize().z);
		}
		if (glfwGetKey(win.getGLFWWindow(), GLFW_KEY_2)) {
			sh.Init("shaders/tracing2.shader", 0);
			sh.setUniformV2("resolution", glm::vec2(WIN_X, WIN_Y));
			sh.setUniform1i("sz_x", w.getSize().x);
			sh.setUniform1i("sz_y", w.getSize().y);
			sh.setUniform1i("sz_z", w.getSize().z);
		}
		if (glfwGetKey(win.getGLFWWindow(), GLFW_KEY_3)) {
			sh.Init("shaders/tracing3.shader", 0);
			sh.setUniformV2("resolution", glm::vec2(WIN_X, WIN_Y));
			sh.setUniform1i("sz_x", w.getSize().x);
			sh.setUniform1i("sz_y", w.getSize().y);
			sh.setUniform1i("sz_z", w.getSize().z);
		}
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
			light_ratio += 3 * win.get_deltaTime();
			sh.setUniform1f("light_ratio", light_ratio);
		}
		if (glfwGetKey(win.getGLFWWindow(), GLFW_KEY_DOWN)) {
			light_ratio -= 3 * win.get_deltaTime();
			sh.setUniform1f("light_ratio", light_ratio);
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
		}

		cam.update(win);
		sh.setCamera(cam);

		sh.setUniform1f("tr",timer);
		texture_pack.bind();
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);
		sh.use();

		main_vao.draw();
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		
		timer += win.get_deltaTime();
		win.frameEnd();
	}

	glDeleteBuffers(1, &ssbo);
	
	
	return 0;
}