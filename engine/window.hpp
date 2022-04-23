#pragma once

// std
#include <iostream>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace Engine {
	class Window {
	public:
		Window(std::string title, int width, int height);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		void Update();

		bool IsClosed() const;

		void SetVSync(bool enabled);

		inline int GetWidth() const { return m_Data.Width; }
		inline int GetHeight() const { return m_Data.Height; }

	private:
		struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			GLFWwindow* Window;
		};

		void Init();

		WindowData m_Data;
	};
};