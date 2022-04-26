#pragma once

#include "Device.hpp"
#include "./Utils/NonMoveable.hpp"
#include "./Utils/NonCopyable.hpp"

// std lib headers
#include <string>
#include <vector>

namespace Engine {
	struct PipelineConfigurationInfo {
		VkPipelineViewportStateCreateInfo ViewportInfo;
		VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo RasterizationInfo;
		VkPipelineMultisampleStateCreateInfo MultisampleInfo;
		VkPipelineColorBlendAttachmentState ColorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo ColorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;
		std::vector<VkDynamicState> DynamicStateEnables;
		VkPipelineDynamicStateCreateInfo DynamicStateInfo;

		VkPipelineLayout PipelineLayout = nullptr;
		VkRenderPass RenderPass = nullptr;
		uint32_t Subpass = 0;
	};



	class Pipeline : public NonMoveable {
	public:
		Pipeline(Device&, const PipelineConfigurationInfo&, const std::string&, const std::string&);
		~Pipeline();

		void Bind(VkCommandBuffer);
		static void DefaultPipelineConfigurationInfo(PipelineConfigurationInfo&);
	private:
		static std::vector<char> ReadFile(const std::string&);

		void CreateGraphicsPipeline(const std::string&, const std::string&, const PipelineConfigurationInfo&);

		void CreateShaderModule(const std::vector<char>&, VkShaderModule*);

		Device& m_Device;
		VkPipeline m_Pipeline;
		VkShaderModule m_VertexShaderModule;
		VkShaderModule m_FragmentShaderModule;

	};
}