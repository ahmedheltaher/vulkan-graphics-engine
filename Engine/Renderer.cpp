#include "Renderer.hpp"

// std lib headers
#include <array>

namespace Engine {

	Renderer::Renderer(Engine::Window& window, Engine::Device& device) : m_Window{ window }, m_Device{ device }, m_IsFrameStarted{ false }, m_CurrentFrameIndex{ 0 }{
		this->RecreateSwapChain();
		this->CreateCommandBuffers();
	}

	Renderer::~Renderer() {
		this->FreeCommandBuffers();
	}


	void Renderer::RecreateSwapChain() {
		auto extent = this->m_Window.GetExtent();

		while (extent.width == 0 || extent.height == 0) {
			extent = this->m_Window.GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(this->m_Device.GetDevice());

		if (this->m_SwapChain == nullptr)
			this->m_SwapChain = std::make_unique<Engine::SwapChain>(this->m_Device, extent);
		else {
			std::shared_ptr<Engine::SwapChain> oldSwapChain = std::move(this->m_SwapChain);
			this->m_SwapChain = std::make_unique<Engine::SwapChain>(this->m_Device, extent, oldSwapChain);

			if (!oldSwapChain->CompareSwapFormats(*this->m_SwapChain.get())) {
				std::runtime_error("Swap chain image or depth format has changed");
			}

		}

	}

	void Renderer::CreateCommandBuffers() {
		this->m_CommandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = this->m_Device.GetCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(this->m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(this->m_Device.GetDevice(), &allocInfo, this->m_CommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void Renderer::FreeCommandBuffers() {
		vkFreeCommandBuffers(this->m_Device.GetDevice(), this->m_Device.GetCommandPool(), static_cast<uint32_t>(this->m_CommandBuffers.size()), this->m_CommandBuffers.data());
		this->m_CommandBuffers.clear();
	}


	VkCommandBuffer Renderer::BeginFrame() {
		assert(!this->m_IsFrameStarted && "Cannot begin frame when one is already in progress!");

		auto result = this->m_SwapChain->AcquireNextImage(&this->m_CurrentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			this->RecreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		this->m_IsFrameStarted = true;
		auto commandBuffer = this->GetCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return commandBuffer;

	};
	void Renderer::EndFrame() {
		assert(this->m_IsFrameStarted && "Cannot end frame when one is not in progress!");

		auto commandBuffer = this->GetCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = this->m_SwapChain->SubmitCommandBuffers(&commandBuffer, &this->m_CurrentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || this->m_Window.WasWindowResized()) {
			this->m_Window.ResetWindowResizedFlag();
			this->RecreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		this->m_IsFrameStarted = false;
		this->m_CurrentFrameIndex = (this->m_CurrentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
	};

	void Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(this->m_IsFrameStarted && "Cannot begin render pass when one is not in progress!");
		assert(commandBuffer == this->GetCurrentCommandBuffer() && "Cannot begin render pass with a command buffer that is not the current command buffer!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = this->m_SwapChain->GetRenderPass();
		renderPassInfo.framebuffer = this->m_SwapChain->GetFrameBuffer(this->m_CurrentImageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = this->m_SwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(this->m_SwapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(this->m_SwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{ { 0, 0 }, this->m_SwapChain->GetSwapChainExtent() };
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}
	void Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(this->m_IsFrameStarted && "Cannot end render pass when one is not in progress!");
		assert(commandBuffer == this->GetCurrentCommandBuffer() && "Cannot end render pass with a command buffer that is not the current command buffer!");

		vkCmdEndRenderPass(commandBuffer);
	};


}