#include "./FirstApp.hpp"
#include "./SimpleRenderSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


// std lib headers
#include <array>

namespace App {

	FirstApp::FirstApp() {
		this->LoadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::Run() {
		SimpleRenderSystem renderSystem{ this->m_Device,this->m_Renderer.GetSwapChainRenderPass() };

		while (!m_Window.IsClosed()) {
			this->m_Window.Update();

			if (auto commandBuffer = m_Renderer.BeginFrame()) {
				this->m_Renderer.BeginSwapChainRenderPass(commandBuffer);
				renderSystem.RenderGameObjects(commandBuffer, this->m_GameObjects);
				this->m_Renderer.EndSwapChainRenderPass(commandBuffer);
				this->m_Renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(this->m_Device.GetDevice());
	}
	

	void FirstApp::LoadGameObjects() {
		std::vector<Engine::Model::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};
		std::vector<glm::vec3> colors{
			{1.f, .7f, .73f},
			{1.f, .87f, .73f},
			{1.f, 1.f, .73f},
			{.73f, 1.f, .8f},
			{.73, .88f, 1.f}
		};
		for (auto& color : colors) color = glm::pow(color, glm::vec3{ 2.2f });

		auto model = std::make_shared<Engine::Model>(this->m_Device, vertices);

		for (int i = 0; i < 40; i++) {
			auto triangle = Engine::GameObject::CreateGameObject();
			triangle.Model = model;
			triangle.Transform.Scale = glm::vec2(.5f) + i * .025f;
			triangle.Transform.Rotation = i * glm::two_pi<float>() * .025f;
			triangle.Color = colors[i % colors.size()];

			this->m_GameObjects.push_back(std::move(triangle));
		}
	}



	void FirstApp::Sierpinski(
		std::vector<Engine::Model::Vertex>& vertices,
		int depth,
		glm::vec2 left,
		glm::vec2 right,
		glm::vec2 top) {
		if (depth <= 0) {
			vertices.push_back({ top });
			vertices.push_back({ right });
			vertices.push_back({ left });
		}
		else {
			auto leftTop = 0.5f * (left + top);
			auto rightTop = 0.5f * (right + top);
			auto leftRight = 0.5f * (left + right);
			Sierpinski(vertices, depth - 1, left, leftRight, leftTop);
			Sierpinski(vertices, depth - 1, leftRight, right, rightTop);
			Sierpinski(vertices, depth - 1, leftTop, rightTop, top);
		}
	}
}