#pragma once

#include "./Device.hpp"
#include "Utils/NonCopyable.hpp"
#include "Utils/NonMoveable.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std lib headers
#include <vector>

namespace Engine {
	class Model : public NonCopyable, public NonMoveable {
	public:
		struct Vertex {
			glm::vec2 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};

		Model(Device&, const std::vector<Vertex>&);
		~Model();

		void Bind(VkCommandBuffer);
		void Draw(VkCommandBuffer);

	private:
		void CreateVertexBuffer(const std::vector<Vertex>&);

		Device& m_Device;
		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexBufferMemory;
		uint32_t m_VertexCount;
	};
}