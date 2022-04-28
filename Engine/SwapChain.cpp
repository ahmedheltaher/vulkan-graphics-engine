#include "./SwapChain.hpp"

// std lib headers
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace Engine {
	SwapChain::SwapChain(Device& deviceReference, VkExtent2D extent)
		: m_Device{ deviceReference }, m_WindowExtent{ extent } {
		this->Init();
	}

	SwapChain::SwapChain(Device& deviceReference, VkExtent2D extent, std::shared_ptr<SwapChain> previousSwapChain)
		: m_Device{ deviceReference }, m_WindowExtent{ extent }, m_PreviousSwapChain{ previousSwapChain } {
		this->Init();

		// Clean up the previous swap chain
		this->m_PreviousSwapChain = nullptr;
	}

	void SwapChain::Init() {
		this->CreateSwapChain();
		this->CreateImageViews();
		this->CreateRenderPass();
		this->CreateDepthResources();
		this->CreateFramebuffers();
		this->CreateSyncObjects();
	}

	SwapChain::~SwapChain() {
		for (auto imageView : this->m_SwapChainImageViews) {
			vkDestroyImageView(this->m_Device.GetDevice(), imageView, nullptr);
		}
		this->m_SwapChainImageViews.clear();

		if (this->m_SwapChain != nullptr) {
			vkDestroySwapchainKHR(this->m_Device.GetDevice(), this->m_SwapChain, nullptr);
			this->m_SwapChain = nullptr;
		}

		for (int i = 0; i < this->m_DepthImages.size(); i++) {
			vkDestroyImageView(this->m_Device.GetDevice(), this->m_DepthImageViews[i], nullptr);
			vkDestroyImage(this->m_Device.GetDevice(), this->m_DepthImages[i], nullptr);
			vkFreeMemory(this->m_Device.GetDevice(), this->m_DepthImageMemories[i], nullptr);
		}

		for (auto framebuffer : this->m_SwapChainFramebuffers) {
			vkDestroyFramebuffer(this->m_Device.GetDevice(), framebuffer, nullptr);
		}

		vkDestroyRenderPass(this->m_Device.GetDevice(), this->m_RenderPass, nullptr);

		// cleanup synchronization objects
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(this->m_Device.GetDevice(), this->m_RenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(this->m_Device.GetDevice(), this->m_ImageAvailableSemaphores[i], nullptr);
			vkDestroyFence(this->m_Device.GetDevice(), this->m_InFlightFences[i], nullptr);
		}
	}

	VkResult SwapChain::AcquireNextImage(uint32_t* imageIndex) {
		vkWaitForFences(
			this->m_Device.GetDevice(),
			1,
			&this->m_InFlightFences[m_CurrentFrame],
			VK_TRUE,
			std::numeric_limits<uint64_t>::max());

		VkResult result = vkAcquireNextImageKHR(
			this->m_Device.GetDevice(),
			this->m_SwapChain,
			std::numeric_limits<uint64_t>::max(),
			this->m_ImageAvailableSemaphores[this->m_CurrentFrame], // must be a not signaled semaphore
			VK_NULL_HANDLE,
			imageIndex);

		return result;
	}

	VkResult SwapChain::SubmitCommandBuffers(
		const VkCommandBuffer* buffers, uint32_t* imageIndex) {
		if (this->m_ImagesInFlight[*imageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences(this->m_Device.GetDevice(), 1, &this->m_ImagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
		}
		this->m_ImagesInFlight[*imageIndex] = this->m_InFlightFences[this->m_CurrentFrame];

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { this->m_ImageAvailableSemaphores[this->m_CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = buffers;

		VkSemaphore signalSemaphores[] = { this->m_RenderFinishedSemaphores[this->m_CurrentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(this->m_Device.GetDevice(), 1, &this->m_InFlightFences[this->m_CurrentFrame]);
		if (vkQueueSubmit(this->m_Device.GraphicsQueue(), 1, &submitInfo, this->m_InFlightFences[this->m_CurrentFrame]) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { this->m_SwapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = imageIndex;

		auto result = vkQueuePresentKHR(this->m_Device.PresentQueue(), &presentInfo);

		this->m_CurrentFrame = (this->m_CurrentFrame + 1) % this->MAX_FRAMES_IN_FLIGHT;

		return result;
	}

	void SwapChain::CreateSwapChain() {
		SwapChainSupportDetails swapChainSupport = this->m_Device.GetSwapChainSupport();

		VkSurfaceFormatKHR surfaceFormat = this->ChooseSwapSurfaceFormat(swapChainSupport.Formats);
		VkPresentModeKHR presentMode = this->ChooseSwapPresentMode(swapChainSupport.PresentModes);
		VkExtent2D extent = this->ChooseSwapExtent(swapChainSupport.Capabilities);

		uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
		if (swapChainSupport.Capabilities.maxImageCount > 0 &&
			imageCount > swapChainSupport.Capabilities.maxImageCount) {
			imageCount = swapChainSupport.Capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = this->m_Device.Surface();

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = this->m_Device.FindPhysicalQueueFamilies();
		uint32_t queueFamilyIndices[] = { indices.GraphicsFamily, indices.PresentFamily };

		if (indices.GraphicsFamily != indices.PresentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;      // Optional
			createInfo.pQueueFamilyIndices = nullptr;  // Optional
		}

		createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = this->m_PreviousSwapChain == nullptr ? VK_NULL_HANDLE : this->m_PreviousSwapChain->m_SwapChain;

		if (vkCreateSwapchainKHR(this->m_Device.GetDevice(), &createInfo, nullptr, &this->m_SwapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		// we only specified a minimum number of images in the swap chain, so the implementation is
		// allowed to create a swap chain with more. That's why we'll first query the final number of
		// images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
		// retrieve the handles.
		vkGetSwapchainImagesKHR(this->m_Device.GetDevice(), this->m_SwapChain, &imageCount, nullptr);
		this->m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(this->m_Device.GetDevice(), this->m_SwapChain, &imageCount, this->m_SwapChainImages.data());

		this->m_SwapChainImageFormat = surfaceFormat.format;
		this->m_SwapChainExtent = extent;
	}

	void SwapChain::CreateImageViews() {
		m_SwapChainImageViews.resize(this->m_SwapChainImages.size());
		for (size_t i = 0; i < this->m_SwapChainImages.size(); i++) {
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = this->m_SwapChainImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = this->m_SwapChainImageFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(this->m_Device.GetDevice(), &viewInfo, nullptr, &this->m_SwapChainImageViews[i]) !=
				VK_SUCCESS) {
				throw std::runtime_error("failed to create texture image view!");
			}
		}
	}

	void SwapChain::CreateRenderPass() {
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = this->FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = this->GetSwapChainImageFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcAccessMask = 0;
		dependency.srcStageMask =
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

		dependency.dstSubpass = 0;
		dependency.dstStageMask =
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask =
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(this->m_Device.GetDevice(), &renderPassInfo, nullptr, &this->m_RenderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void SwapChain::CreateFramebuffers() {
		this->m_SwapChainFramebuffers.resize(this->ImageCount());
		for (size_t i = 0; i < this->ImageCount(); i++) {
			std::array<VkImageView, 2> attachments = { this->m_SwapChainImageViews[i], this->m_DepthImageViews[i] };

			VkExtent2D swapChainExtent = this->GetSwapChainExtent();
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = this->m_RenderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(this->m_Device.GetDevice(), &framebufferInfo, nullptr, &this->m_SwapChainFramebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	void SwapChain::CreateDepthResources() {
		VkFormat depthFormat = this->FindDepthFormat();
		this->m_SwapChainDepthFormat = depthFormat;

		VkExtent2D swapChainExtent = this->GetSwapChainExtent();

		this->m_DepthImages.resize(this->ImageCount());
		this->m_DepthImageMemories.resize(this->ImageCount());
		this->m_DepthImageViews.resize(this->ImageCount());

		for (int i = 0; i < this->m_DepthImages.size(); i++) {
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = this->m_SwapChainExtent.width;
			imageInfo.extent.height = this->m_SwapChainExtent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = depthFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.flags = 0;

			this->m_Device.CreateImageWithInfo(
				imageInfo,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				this->m_DepthImages[i],
				this->m_DepthImageMemories[i]);

			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = this->m_DepthImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = depthFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(this->m_Device.GetDevice(), &viewInfo, nullptr, &this->m_DepthImageViews[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create texture image view!");
			}
		}
	}

	void SwapChain::CreateSyncObjects() {
		this->m_ImageAvailableSemaphores.resize(this->MAX_FRAMES_IN_FLIGHT);
		this->m_RenderFinishedSemaphores.resize(this->MAX_FRAMES_IN_FLIGHT);
		this->m_InFlightFences.resize(this->MAX_FRAMES_IN_FLIGHT);
		this->m_ImagesInFlight.resize(this->ImageCount(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < this->MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(this->m_Device.GetDevice(), &semaphoreInfo, nullptr, &this->m_ImageAvailableSemaphores[i]) !=
				VK_SUCCESS ||
				vkCreateSemaphore(this->m_Device.GetDevice(), &semaphoreInfo, nullptr, &this->m_RenderFinishedSemaphores[i]) !=
				VK_SUCCESS ||
				vkCreateFence(this->m_Device.GetDevice(), &fenceInfo, nullptr, &this->m_InFlightFences[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}

	VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		//for (const auto& availablePresentMode : availablePresentModes) {
		//	if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
		//		std::cout << "Present mode: Mailbox" << std::endl;
		//		return availablePresentMode;
		//	}
		//}

		//for (const auto& availablePresentMode : availablePresentModes) {
		//	if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
		//		std::cout << "Present mode: Immediate" << std::endl;
		//		return availablePresentMode;
		//	}
		//}

		std::cout << "Present mode: V-Sync" << std::endl;
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			VkExtent2D actualExtent = this->m_WindowExtent;
			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	VkFormat SwapChain::FindDepthFormat() {
		return this->m_Device.FindSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}
}