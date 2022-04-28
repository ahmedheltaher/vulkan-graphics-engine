#pragma once

#include "../Engine/Window.hpp"
#include "../Engine/Device.hpp"
#include "../Engine/SwapChain.hpp"

#include "../Engine/Utils/NonMoveable.hpp"
#include "../Engine/Utils/NonCopyable.hpp"

// std lib headers
#include <memory>
#include <vector>
#include <cassert>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


namespace Engine {

	class Renderer : public NonMoveable, public NonCopyable {
	public:

		Renderer(Engine::Window&, Engine::Device&);
		~Renderer();

		inline VkRenderPass GetSwapChainRenderPass() const { return this->m_SwapChain->GetRenderPass(); }
		inline bool IsFrameInProgress() const { return this->m_IsFrameStarted; }
		inline VkCommandBuffer GetCurrentCommandBuffer() const {
			assert(this->m_IsFrameStarted && "Cannot get current command buffer when frame is not in progress");
			return this->m_CommandBuffers[this->m_CurrentFrameIndex];
		}
		int GetCurrentFrameIndex() const {
			assert(this->m_IsFrameStarted && "Cannot get current frame Index when frame is not in progress");
			return this->m_CurrentFrameIndex;
		}


		VkCommandBuffer BeginFrame();
		void EndFrame();

		void BeginSwapChainRenderPass(VkCommandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer);


	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapChain();


		Engine::Window& m_Window;
		Engine::Device& m_Device;
		std::unique_ptr <Engine::SwapChain> m_SwapChain;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		uint32_t m_CurrentImageIndex;
		int m_CurrentFrameIndex;
		bool m_IsFrameStarted;
	};
}