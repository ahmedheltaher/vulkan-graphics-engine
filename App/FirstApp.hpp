#pragma once

#include "../Engine/Window.hpp"
#include "../Engine/Pipeline.hpp"
#include "../Engine/Device.hpp"
#include "../Engine/SwapChain.hpp"
#include "../Engine/Model.hpp"

#include "../Engine/Utils/NonMoveable.hpp"
#include "../Engine/Utils/NonCopyable.hpp"

// std lib headers
#include <memory>
#include <vector>

namespace App {
	class FirstApp : public NonMoveable, public NonCopyable {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();


		void Run();

	private:
		void LoadModels();
		void Sierpinski(std::vector<Engine::Model::Vertex>& vertices,
			int depth,
			glm::vec2 left,
			glm::vec2 right,
			glm::vec2 top);
		void CreatePipeline();
		void CreatePipelibeLayout();
		void CreateCommandBuffers();
		void DrawFrame();

		Engine::Window m_Window{ "FirstApp", WIDTH, HEIGHT };
		Engine::Device m_Device{ m_Window };
		Engine::SwapChain m_SwapChain{ m_Device, m_Window.GetExtent() };
		std::unique_ptr<Engine::Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		std::unique_ptr<Engine::Model> m_Model;
	};
}