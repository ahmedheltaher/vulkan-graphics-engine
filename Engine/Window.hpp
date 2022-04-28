#pragma once

// std lib headers
#include <iostream>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "./Utils/NonMoveable.hpp"
#include "./Utils/NonCopyable.hpp"

namespace Engine {
	class Window : public NonMoveable, public NonCopyable {
	public:
		Window(std::string, int, int);
		~Window();


		void Update();

		bool IsClosed() const;

		void SetVSync(bool);

		inline int GetWidth() const { return this->m_Data.Width; }
		inline int GetHeight() const { return this->m_Data.Height; }
		inline bool WasWindowResized() const { return this->m_Data.FramebufferResized; }

		inline void ResetWindowResizedFlag() { this->m_Data.FramebufferResized = false; }

		void CreateWindowSurface(VkInstance, VkSurfaceKHR*);
		VkExtent2D GetExtent() { return { static_cast<uint32_t>(this->m_Data.Width), static_cast<uint32_t>(this->m_Data.Height) }; }
	private:
		static void FramebufferResizeCallback(GLFWwindow*, int, int);

		struct WindowData {
			std::string Title;
			int Width, Height;
			bool VSync, FramebufferResized;

			GLFWwindow* Window;
		};

		void Init();

		WindowData m_Data;
	};
};