#include "Model.hpp"


// std lib headers
#include <cassert>
#include <cstring>

namespace Engine {
	Model::Model(Device& device, const std::vector<Vertex>& vertices) : m_Device{ device } {
		CreateVertexBuffer(vertices);
	}

	Model::~Model() {
		if (m_VertexBuffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(m_Device.GetDevice(), m_VertexBuffer, nullptr);
		}
		if (m_VertexBufferMemory != VK_NULL_HANDLE) {
			vkFreeMemory(m_Device.GetDevice(), m_VertexBufferMemory, nullptr);
		}
	}

	void Model::Draw(VkCommandBuffer commandBuffer) {
		vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
	}

	void Model::Bind(VkCommandBuffer commandBuffer) {
		VkBuffer vertexBuffers[] = { m_VertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	}

	void Model::CreateVertexBuffer(const std::vector<Vertex>& vertices) {
		m_VertexCount = static_cast<uint32_t>(vertices.size());
		assert(m_VertexCount >= 3 && "Model must have at least 3 vertices");

		VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
		m_Device.CreateBuffer(bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_VertexBuffer,
			m_VertexBufferMemory);

		void* data;
		vkMapMemory(m_Device.GetDevice(), m_VertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_Device.GetDevice(), m_VertexBufferMemory);

	}

	std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = 0;

		return attributeDescriptions;
	}
}