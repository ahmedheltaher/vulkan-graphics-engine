#include "./SimpleRenderSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


// std lib headers
#include <array>

namespace App {
	struct SimplePushConstantData {
		glm::mat2 Transform{ 1.0f };
		glm::vec2 Offset;
		alignas(16) glm::vec3 Color;
	};

	SimpleRenderSystem::SimpleRenderSystem(Engine::Device& device, VkRenderPass renderPass) : m_Device{ device } {
		this->CreatePipelineLayout();
		this->CreatePipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem() {
		vkDestroyPipelineLayout(this->m_Device.GetDevice(), this->m_PipelineLayout, nullptr);
	}


	void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass) {
		assert(this->m_PipelineLayout != nullptr && "Can't create pipeline without pipeline layout");

		Engine::PipelineConfigurationInfo config{};
		Engine::Pipeline::DefaultPipelineConfigurationInfo(config);
		config.RenderPass = renderPass;
		config.PipelineLayout = this->m_PipelineLayout;
		m_Pipeline = std::make_unique<Engine::Pipeline>(this->m_Device, config, "./Shaders/SimpleShader.vert.spv", "./Shaders/SimpleShader.frag.spv");
	}

	void SimpleRenderSystem::CreatePipelineLayout() {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);


		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(this->m_Device.GetDevice(), &pipelineLayoutInfo, nullptr, &this->m_PipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}


	void SimpleRenderSystem::RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<Engine::GameObject>& gameObjects) {
		this->m_Pipeline->Bind(commandBuffer);
		int i = 0;

		for (auto& obj : gameObjects) {
			obj.Transform.Rotation = glm::mod<float>(obj.Transform.Rotation + 0.001f * i, 2.f * glm::pi<float>());

			SimplePushConstantData pushConstantData{};
			pushConstantData.Offset = obj.Transform.Translation;
			pushConstantData.Color = obj.Color;
			pushConstantData.Transform = obj.Transform.GetTransformMatrix();

			vkCmdPushConstants(commandBuffer, this->m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &pushConstantData);

			obj.Model->Bind(commandBuffer);
			obj.Model->Draw(commandBuffer);
			
			i++;
		}
	}
}