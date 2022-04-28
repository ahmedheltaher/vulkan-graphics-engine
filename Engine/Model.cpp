#include "Model.hpp"


// std lib headers
#include <cassert>
#include <cstring>

namespace Engine {
	Model::Model(Device& device, const std::vector<Vertex>& vertices) : m_Device{ device } {
		this->CreateVertexBuffer(vertices);
	}

	Model::~Model() {
		if (this->m_VertexBuffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(this->m_Device.GetDevice(), this->m_VertexBuffer, nullptr);
		}
		if (this->m_VertexBufferMemory != VK_NULL_HANDLE) {
			vkFreeMemory(this->m_Device.GetDevice(), this->m_VertexBufferMemory, nullptr);
		}
	}

	void Model::Draw(VkCommandBuffer commandBuffer) {
		vkCmdDraw(commandBuffer, this->m_VertexCount, 1, 0, 0);
	}

	void Model::Bind(VkCommandBuffer commandBuffer) {
		VkBuffer vertexBuffers[] = { this->m_VertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	}

	void Model::CreateVertexBuffer(const std::vector<Vertex>& vertices) {
		this->m_VertexCount = static_cast<uint32_t>(vertices.size());
		assert(this->m_VertexCount >= 3 && "Model must have at least 3 vertices");

		VkDeviceSize bufferSize = sizeof(vertices[0]) * this->m_VertexCount;
		this->m_Device.CreateBuffer(bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			this->m_VertexBuffer,
			this->m_VertexBufferMemory);

		void* data;
		vkMapMemory(this->m_Device.GetDevice(), this->m_VertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(this->m_Device.GetDevice(), this->m_VertexBufferMemory);

	}

	std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions() {
		return {
			{ 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
		};
	}

	std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions() {
		return {
			{ 0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, position) },
			{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) }
		};
	}
}