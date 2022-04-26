#include "./Pipeline.hpp"
#include "Model.hpp"

// std lib headers
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cassert>


namespace Engine {

	Pipeline::Pipeline(Device& device, const PipelineConfigurationInfo& config, const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
		: m_Device(device) {

		CreateGraphicsPipeline(vertexShaderPath, fragmentShaderPath, config);
	}

	Pipeline::~Pipeline() {
		vkDestroyShaderModule(m_Device.GetDevice(), m_VertexShaderModule, nullptr);
		vkDestroyShaderModule(m_Device.GetDevice(), m_FragmentShaderModule, nullptr);
		vkDestroyPipeline(m_Device.GetDevice(), m_Pipeline, nullptr);
	};

	std::vector<char> Pipeline::ReadFile(const std::string& filePath) {
		std::ifstream file(filePath, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file: " + filePath);
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	};

	void Pipeline::CreateGraphicsPipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const PipelineConfigurationInfo& config) {
		assert(config.PipelineLayout != VK_NULL_HANDLE &&
			"Cannot create graphics pipeline: no PipelineLayout provided in config");
		assert(config.RenderPass != VK_NULL_HANDLE &&
			"Cannot create graphics pipeline: no RenderPass provided in config");

		auto vertexShaderCode = ReadFile(vertexShaderPath);
		auto fragmentShaderCode = ReadFile(fragmentShaderPath);

		CreateShaderModule(vertexShaderCode, &m_VertexShaderModule);
		CreateShaderModule(fragmentShaderCode, &m_FragmentShaderModule);

		VkPipelineShaderStageCreateInfo saderStageInfo[2];
		saderStageInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		saderStageInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		saderStageInfo[0].module = m_VertexShaderModule;
		saderStageInfo[0].pName = "main";
		saderStageInfo[0].flags = 0;
		saderStageInfo[0].pNext = nullptr;
		saderStageInfo[0].pSpecializationInfo = nullptr;

		saderStageInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		saderStageInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		saderStageInfo[1].module = m_FragmentShaderModule;
		saderStageInfo[1].pName = "main";
		saderStageInfo[1].flags = 0;
		saderStageInfo[1].pNext = nullptr;
		saderStageInfo[1].pSpecializationInfo = nullptr;

		auto bindingDescription = Model::Vertex::GetBindingDescriptions();
		auto attributeDescriptions = Model::Vertex::GetAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
		vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();

		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = saderStageInfo;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &config.InputAssemblyInfo;
		pipelineInfo.pViewportState = &config.ViewportInfo;
		pipelineInfo.pRasterizationState = &config.RasterizationInfo;
		pipelineInfo.pMultisampleState = &config.MultisampleInfo;
		pipelineInfo.pDepthStencilState = &config.DepthStencilInfo;
		pipelineInfo.pColorBlendState = &config.ColorBlendInfo;
		pipelineInfo.pDynamicState = &config.DynamicStateInfo;

		pipelineInfo.layout = config.PipelineLayout;
		pipelineInfo.renderPass = config.RenderPass;
		pipelineInfo.subpass = config.Subpass;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(m_Device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create graphics pipeline!");
		}

	};

	void Pipeline::Bind(VkCommandBuffer commandBuffer) {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
	};


	void Pipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(m_Device.GetDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module!");
		}
	};


	void Pipeline::DefaultPipelineConfigurationInfo(PipelineConfigurationInfo& configInfo) {
		configInfo.InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		configInfo.ViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		configInfo.ViewportInfo.viewportCount = 1;
		configInfo.ViewportInfo.pViewports = nullptr;
		configInfo.ViewportInfo.scissorCount = 1;
		configInfo.ViewportInfo.pScissors = nullptr;

		configInfo.RasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.RasterizationInfo.depthClampEnable = VK_FALSE;
		configInfo.RasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		configInfo.RasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.RasterizationInfo.lineWidth = 1.0f;
		configInfo.RasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		configInfo.RasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		configInfo.RasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.RasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
		configInfo.RasterizationInfo.depthBiasClamp = 0.0f;           // Optional
		configInfo.RasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

		configInfo.MultisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.MultisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.MultisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.MultisampleInfo.minSampleShading = 1.0f;           // Optional
		configInfo.MultisampleInfo.pSampleMask = nullptr;             // Optional
		configInfo.MultisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		configInfo.MultisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

		configInfo.ColorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		configInfo.ColorBlendAttachment.blendEnable = VK_FALSE;
		configInfo.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		configInfo.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

		configInfo.ColorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.ColorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.ColorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		configInfo.ColorBlendInfo.attachmentCount = 1;
		configInfo.ColorBlendInfo.pAttachments = &configInfo.ColorBlendAttachment;
		configInfo.ColorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		configInfo.ColorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		configInfo.ColorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		configInfo.ColorBlendInfo.blendConstants[3] = 0.0f;  // Optional

		configInfo.DepthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.DepthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.DepthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.DepthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.DepthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.DepthStencilInfo.minDepthBounds = 0.0f;  // Optional
		configInfo.DepthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		configInfo.DepthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.DepthStencilInfo.front = {};  // Optional
		configInfo.DepthStencilInfo.back = {};   // Optional

		configInfo.DynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		configInfo.DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		configInfo.DynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.DynamicStateEnables.size());
		configInfo.DynamicStateInfo.pDynamicStates = configInfo.DynamicStateEnables.data();
		configInfo.DynamicStateInfo.flags = 0;
	};

}