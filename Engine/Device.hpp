#pragma once

#include "Window.hpp"
#include "./Utils/NonMoveable.hpp"
#include "./Utils/NonCopyable.hpp"

// std lib headers
#include <string>
#include <vector>

namespace Engine {
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	struct QueueFamilyIndices {
		uint32_t GraphicsFamily = -1;
		uint32_t PresentFamily = -1;

		bool IsComplete() {
			return GraphicsFamily >= 0 && PresentFamily >= 0;
		}
	};

	class Device : public NonMoveable, public NonCopyable {
	public:
#ifdef NDEBUG
		const bool EnableValidationLayers = false;
#else
		const bool EnableValidationLayers = true;
#endif

		Device(Window& window);
		~Device();


		VkCommandPool GetCommandPool() { return m_CommandPool; }
		VkDevice GetDevice() { return m_Device; }
		VkSurfaceKHR Surface() { return m_Surface; }
		VkQueue GraphicsQueue() { return m_GraphicsQueue; }
		VkQueue PresentQueue() { return m_PresentQueue; }

		SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(m_PhysicalDevice); }
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(m_PhysicalDevice); }
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		// Buffer Helper Functions
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

		void CreateImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

		VkPhysicalDeviceProperties properties;

	private:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateCommandPool();

		// helper functions
		bool IsDeviceSuitable(VkPhysicalDevice device);
		std::vector<const char*> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void HasGlfwRequiredInstanceExtensions();
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		Window& m_Window;
		VkCommandPool m_CommandPool;

		VkDevice m_Device;
		VkSurfaceKHR m_Surface;
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;

		const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};
}