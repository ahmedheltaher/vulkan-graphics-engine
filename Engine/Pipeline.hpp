#pragma once

#include "Device.hpp"
#include "./Utils/NonMoveable.hpp"
#include "./Utils/NonCopyable.hpp"

// std lib headers
#include <string>
#include <vector>

namespace Engine {

	struct PipelineConfigurationInfo {
		VkViewport Viewport;
		VkRect2D Scissor;
		VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo RasterizationInfo;
		VkPipelineMultisampleStateCreateInfo MultisampleInfo;
		VkPipelineColorBlendAttachmentState ColorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo ColorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;
		VkPipelineLayout PipelineLayout = nullptr;
		VkRenderPass RenderPass = nullptr;
		uint32_t Subpass = 0;
	};



	class Pipeline : public NonMoveable, public NonCopyable {
	public:
		Pipeline(Device& device, const PipelineConfigurationInfo& config, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		~Pipeline();

		void Bind(VkCommandBuffer commandBuffer);
		static PipelineConfigurationInfo DefaultPipelineConfigurationInfo(uint32_t width, uint32_t height);
	private:
		static std::vector<char> ReadFile(const std::string& filePath);

		void CreateGraphicsPipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const PipelineConfigurationInfo& config);

		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		Device& m_Device;
		VkPipeline m_Pipeline;
		VkShaderModule m_VertexShaderModule;
		VkShaderModule m_FragmentShaderModule;

	};
}