#pragma once

#include "../Engine/Window.hpp"
#include "../Engine/Pipeline.hpp"
#include "../Engine/Device.hpp"
#include "../Engine/SwapChain.hpp"
#include "../Engine/GameObject.hpp"

#include "../Engine/Utils/NonMoveable.hpp"
#include "../Engine/Utils/NonCopyable.hpp"

// std lib headers
#include <memory>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


namespace App {

	struct SimplePushConstantData {
		glm::mat2 Transform{ 1.0f };
		glm::vec2 Offset;
		alignas(16) glm::vec3 Color;
	};

	class FirstApp : public NonMoveable, public NonCopyable {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();


		void Run();

	private:
		void LoadGameObjects();
		void Sierpinski(std::vector<Engine::Model::Vertex>&, int, glm::vec2, glm::vec2, glm::vec2);
		void CreatePipeline();
		void CreatePipelibeLayout();
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void DrawFrame();

		void RecreateSwapChain();
		void RecordCommandBuffer(uint32_t);

		void RenderGameObjects(VkCommandBuffer);

		Engine::Window m_Window{ "FirstApp", WIDTH, HEIGHT };
		Engine::Device m_Device{ m_Window };
		std::unique_ptr <Engine::SwapChain> m_SwapChain;
		std::unique_ptr<Engine::Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		std::vector<Engine::GameObject> m_GameObjects;
	};
}