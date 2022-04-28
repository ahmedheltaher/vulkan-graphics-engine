#pragma once

#include "./Device.hpp"

#include "./Utils/NonMoveable.hpp"
#include "./Utils/NonCopyable.hpp"


// std lib headers
#include <memory>
#include <string>
#include <vector>

namespace Engine {

	class SwapChain : public NonMoveable, public NonCopyable {
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		SwapChain(Device&, VkExtent2D);
		SwapChain(Device&, VkExtent2D, std::shared_ptr<SwapChain>);
		~SwapChain();


		inline VkFramebuffer GetFrameBuffer(int index) { return this->m_SwapChainFramebuffers[index]; }
		inline VkRenderPass GetRenderPass() { return this->m_RenderPass; }
		inline VkImageView GetImageView(int index) { return this->m_SwapChainImageViews[index]; }
		inline size_t ImageCount() { return this->m_SwapChainImages.size(); }
		inline VkFormat GetSwapChainImageFormat() { return this->m_SwapChainImageFormat; }
		inline VkExtent2D GetSwapChainExtent() { return this->m_SwapChainExtent; }
		inline uint32_t GetWidth() { return this->m_SwapChainExtent.width; }
		inline uint32_t GetHeight() { return this->m_SwapChainExtent.height; }

		float ExtentAspectRatio() {
			return static_cast<float>(this->m_SwapChainExtent.width) / static_cast<float>(this->m_SwapChainExtent.height);
		}
		VkFormat FindDepthFormat();

		VkResult AcquireNextImage(uint32_t*);
		VkResult SubmitCommandBuffers(const VkCommandBuffer*, uint32_t*);

		inline bool CompareSwapFormats(const SwapChain& other) const {
			return this->m_SwapChainImageFormat == other.m_SwapChainImageFormat &&
				this->m_SwapChainDepthFormat == other.m_SwapChainDepthFormat;
		};

	private:
		void Init();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateDepthResources();
		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateSyncObjects();

		// Helper functions
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
			const std::vector<VkSurfaceFormatKHR>&);
		VkPresentModeKHR ChooseSwapPresentMode(
			const std::vector<VkPresentModeKHR>&);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR&);

		VkFormat m_SwapChainImageFormat;
		VkFormat m_SwapChainDepthFormat;
		VkExtent2D m_SwapChainExtent;

		std::vector<VkFramebuffer> m_SwapChainFramebuffers;
		VkRenderPass m_RenderPass;

		std::vector<VkImage> m_DepthImages;
		std::vector<VkDeviceMemory> m_DepthImageMemories;
		std::vector<VkImageView> m_DepthImageViews;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;

		Device& m_Device;
		VkExtent2D m_WindowExtent;

		VkSwapchainKHR m_SwapChain;

		std::shared_ptr<SwapChain> m_PreviousSwapChain;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;
		std::vector<VkFence> m_ImagesInFlight;
		size_t m_CurrentFrame = 0;
	};
}