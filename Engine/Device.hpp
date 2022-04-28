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

		Device(Window&);
		~Device();


		inline VkCommandPool GetCommandPool() { return this->m_CommandPool; }
		inline VkDevice GetDevice() { return this->m_Device; }
		inline VkSurfaceKHR Surface() { return this->m_Surface; }
		inline VkQueue GraphicsQueue() { return this->m_GraphicsQueue; }
		inline VkQueue PresentQueue() { return this->m_PresentQueue; }

		inline SwapChainSupportDetails GetSwapChainSupport() { return this->QuerySwapChainSupport(this->m_PhysicalDevice); }
		inline QueueFamilyIndices FindPhysicalQueueFamilies() { return this->FindQueueFamilies(this->m_PhysicalDevice); }


		uint32_t FindMemoryType(uint32_t, VkMemoryPropertyFlags);
		VkFormat FindSupportedFormat(const std::vector<VkFormat>&, VkImageTiling, VkFormatFeatureFlags);

		// Buffer Helper Functions
		void CreateBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, VkBuffer&, VkDeviceMemory&);
		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer);
		void CopyBuffer(VkBuffer, VkBuffer, VkDeviceSize);
		void CopyBufferToImage(VkBuffer, VkImage, uint32_t, uint32_t, uint32_t);

		void CreateImageWithInfo(const VkImageCreateInfo&, VkMemoryPropertyFlags, VkImage&, VkDeviceMemory&);

		VkPhysicalDeviceProperties properties;

	private:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateCommandPool();

		// helper functions
		bool IsDeviceSuitable(VkPhysicalDevice);
		std::vector<const char*> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice);
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&);
		void HasGlfwRequiredInstanceExtensions();
		bool CheckDeviceExtensionSupport(VkPhysicalDevice);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice);

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