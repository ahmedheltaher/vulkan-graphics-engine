#pragma once

#include "../Engine/Pipeline.hpp"
#include "../Engine/Device.hpp"
#include "../Engine/GameObject.hpp"

#include "../Engine/Utils/NonMoveable.hpp"
#include "../Engine/Utils/NonCopyable.hpp"

// std lib headers
#include <memory>
#include <vector>

namespace App {
	class SimpleRenderSystem : public NonMoveable, public NonCopyable {
	public:
		SimpleRenderSystem(Engine::Device&, VkRenderPass);
		~SimpleRenderSystem();

		void RenderGameObjects(VkCommandBuffer, std::vector<Engine::GameObject>&);

	private:
		void CreatePipeline(VkRenderPass);
		void CreatePipelineLayout();


		Engine::Device& m_Device;

		std::unique_ptr<Engine::Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}