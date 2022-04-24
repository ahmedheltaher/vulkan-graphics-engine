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
		Window(std::string title, int width, int height);
		~Window();


		void Update();

		bool IsClosed() const;

		void SetVSync(bool enabled);

		inline int GetWidth() const { return m_Data.Width; }
		inline int GetHeight() const { return m_Data.Height; }

		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		VkExtent2D GetExtent() { return { static_cast<uint32_t>(m_Data.Width), static_cast<uint32_t>(m_Data.Height) }; }
	private:
		struct WindowData {
			std::string Title;
			int Width, Height;
			bool VSync;

			GLFWwindow* Window;
		};

		void Init();

		WindowData m_Data;
	};
};