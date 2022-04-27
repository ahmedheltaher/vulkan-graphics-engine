#pragma once

#include "Model.hpp"


// std lib headers
#include <memory>

namespace Engine {

	struct Transform2DComponent {
		glm::vec2 Translation{}; // position offset
		glm::vec2 Scale{ 1.0f, 1.0f }; // scale
		float Rotation; // rotation in radians

		glm::mat2 GetTransformMatrix() const {
			const float sinR = glm::sin(Rotation);
			const float cosR = glm::cos(Rotation);
			
			glm::mat2 rotationMatrix{ { cosR, sinR }, { -sinR, cosR } };
			glm::mat2 scaleMatrix{ { Scale.x, .0f }, { .0f, Scale.y } };

			return rotationMatrix * scaleMatrix ;
		}
	};

	class GameObject {
	public:
		using IdType = unsigned int;

		static GameObject CreateGameObject() {
			static IdType currentId = 0;
			return GameObject{ currentId++ };
		};

		inline IdType GetId() const { return m_Id; }

		std::shared_ptr<Engine::Model> Model{};
		glm::vec3 Color{};
		Transform2DComponent Transform{};
	private:
		GameObject(IdType id) : m_Id{ id } {};
		IdType m_Id;
	};

}