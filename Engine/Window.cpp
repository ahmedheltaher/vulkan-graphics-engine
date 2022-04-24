#include "./Window.hpp"

namespace Engine {
	Window::Window(std::string title, int width, int height)
		: m_Data{ title, width, height, false, nullptr } {
		Init();
	}

	Window::~Window() {
		glfwDestroyWindow(m_Data.Window);
		glfwTerminate();
	}

	void Window::Update() {
		glfwPollEvents();
	}

	bool Window::IsClosed() const {
		return glfwWindowShouldClose(m_Data.Window);
	}
	

	void Window::SetVSync(bool enabled) {
		m_Data.VSync = enabled;
		glfwSwapInterval(enabled);
	}

	void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, m_Data.Window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	void Window::Init() {
		if (!glfwInit()) {
			throw std::runtime_error("Failed to initialize GLFW!");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_Data.Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_Data.Window, &m_Data);
		SetVSync(m_Data.VSync);
		glfwSwapBuffers(m_Data.Window);
	}
}