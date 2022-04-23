#pragma once

#include "../engine/window.hpp"

namespace App {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void Run();

	private:
		Engine::Window m_Window{ "FirstApp", WIDTH, HEIGHT };
	};
}