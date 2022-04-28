#pragma once

#include "../Engine/Window.hpp"
#include "../Engine/Device.hpp"
#include "../Engine/GameObject.hpp"
#include "../Engine/Renderer.hpp"

#include "../Engine/Utils/NonMoveable.hpp"
#include "../Engine/Utils/NonCopyable.hpp"

// std lib headers
#include <memory>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


namespace App {
	class FirstApp : public NonMoveable, public NonCopyable {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();


		void Run();

	private:
		void LoadGameObjects();
		void Sierpinski(std::vector<Engine::Model::Vertex>&, int, glm::vec2, glm::vec2, glm::vec2);


		Engine::Window m_Window{ "FirstApp", WIDTH, HEIGHT };
		Engine::Device m_Device{ m_Window };
		Engine::Renderer m_Renderer{ m_Window, m_Device };

		std::vector<Engine::GameObject> m_GameObjects;
	};
}