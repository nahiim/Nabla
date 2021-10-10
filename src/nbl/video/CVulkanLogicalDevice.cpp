#include "CVulkanLogicalDevice.h"

#include "nbl/video/CVulkanPhysicalDevice.h"
#include "nbl/video/CVulkanCommandBuffer.h"
#include "nbl/video/CVulkanEvent.h"

namespace nbl::video
{

core::smart_refctd_ptr<IGPUEvent> CVulkanLogicalDevice::createEvent(IGPUEvent::E_CREATE_FLAGS flags)
{
    VkEventCreateInfo vk_createInfo = { VK_STRUCTURE_TYPE_EVENT_CREATE_INFO };
    vk_createInfo.pNext = nullptr;
    vk_createInfo.flags = static_cast<VkEventCreateFlags>(flags);

    VkEvent vk_event;
    if (m_devf.vk.vkCreateEvent(m_vkdev, &vk_createInfo, nullptr, &vk_event) == VK_SUCCESS)
        return core::make_smart_refctd_ptr<CVulkanEvent>(
            core::smart_refctd_ptr<const CVulkanLogicalDevice>(this), flags, vk_event);
    else
        return nullptr;
};

IGPUEvent::E_STATUS CVulkanLogicalDevice::getEventStatus(const IGPUEvent* _event)
{
    if (!_event || _event->getAPIType() != EAT_VULKAN)
        return IGPUEvent::E_STATUS::ES_FAILURE;

    VkEvent vk_event = static_cast<const CVulkanEvent*>(_event)->getInternalObject();
    VkResult retval = m_devf.vk.vkGetEventStatus(m_vkdev, vk_event);
    switch (retval)
    {
    case VK_EVENT_SET:
        return IGPUEvent::ES_SET;
    case VK_EVENT_RESET:
        return IGPUEvent::ES_RESET;
    default:
        return IGPUEvent::ES_FAILURE;
    }
}

IGPUEvent::E_STATUS CVulkanLogicalDevice::resetEvent(IGPUEvent* _event)
{
    if (!_event || _event->getAPIType() != EAT_VULKAN)
        return IGPUEvent::E_STATUS::ES_FAILURE;

    VkEvent vk_event = static_cast<const CVulkanEvent*>(_event)->getInternalObject();
    if (m_devf.vk.vkResetEvent(m_vkdev, vk_event) == VK_SUCCESS)
        return IGPUEvent::ES_RESET; // weird return value alert!
    else
        return IGPUEvent::ES_FAILURE;
}

IGPUEvent::E_STATUS CVulkanLogicalDevice::setEvent(IGPUEvent* _event)
{
    if (!_event || _event->getAPIType() != EAT_VULKAN)
        return IGPUEvent::E_STATUS::ES_FAILURE;

    VkEvent vk_event = static_cast<const CVulkanEvent*>(_event)->getInternalObject();
    if (m_devf.vk.vkSetEvent(m_vkdev, vk_event) == VK_SUCCESS)
        return IGPUEvent::ES_SET; // weird return value alert!
    else
        return IGPUEvent::ES_FAILURE;
}

core::smart_refctd_ptr<IDriverMemoryAllocation> CVulkanLogicalDevice::allocateDeviceLocalMemory(
    const IDriverMemoryBacked::SDriverMemoryRequirements& additionalReqs)
{
    IDriverMemoryBacked::SDriverMemoryRequirements memoryReqs = getDeviceLocalGPUMemoryReqs();
    memoryReqs.vulkanReqs.alignment = core::max(memoryReqs.vulkanReqs.alignment, additionalReqs.vulkanReqs.alignment);
    memoryReqs.vulkanReqs.size = core::max(memoryReqs.vulkanReqs.size, additionalReqs.vulkanReqs.size);
    memoryReqs.vulkanReqs.memoryTypeBits &= additionalReqs.vulkanReqs.memoryTypeBits;
    memoryReqs.mappingCapability = additionalReqs.mappingCapability;
    memoryReqs.memoryHeapLocation = IDriverMemoryAllocation::ESMT_DEVICE_LOCAL;
    memoryReqs.prefersDedicatedAllocation = additionalReqs.prefersDedicatedAllocation;
    memoryReqs.requiresDedicatedAllocation = additionalReqs.requiresDedicatedAllocation;

    return allocateGPUMemory(additionalReqs);
}

core::smart_refctd_ptr<IDriverMemoryAllocation> CVulkanLogicalDevice::allocateSpilloverMemory(
    const IDriverMemoryBacked::SDriverMemoryRequirements& additionalReqs)
{
    if (additionalReqs.memoryHeapLocation == IDriverMemoryAllocation::ESMT_DEVICE_LOCAL)
        return nullptr;

    IDriverMemoryBacked::SDriverMemoryRequirements memoryReqs = getSpilloverGPUMemoryReqs();
    memoryReqs.vulkanReqs.alignment = core::max(memoryReqs.vulkanReqs.alignment, additionalReqs.vulkanReqs.alignment);
    memoryReqs.vulkanReqs.size = core::max(memoryReqs.vulkanReqs.size, additionalReqs.vulkanReqs.size);
    memoryReqs.vulkanReqs.memoryTypeBits &= additionalReqs.vulkanReqs.memoryTypeBits;
    memoryReqs.mappingCapability = additionalReqs.mappingCapability;
    memoryReqs.memoryHeapLocation = additionalReqs.memoryHeapLocation;
    memoryReqs.prefersDedicatedAllocation = additionalReqs.prefersDedicatedAllocation;
    memoryReqs.requiresDedicatedAllocation = additionalReqs.requiresDedicatedAllocation;

    return allocateGPUMemory(memoryReqs);
}

core::smart_refctd_ptr<IDriverMemoryAllocation> CVulkanLogicalDevice::allocateUpStreamingMemory(
    const IDriverMemoryBacked::SDriverMemoryRequirements& additionalReqs)
{
    if (getUpStreamingMemoryReqs().memoryHeapLocation != additionalReqs.memoryHeapLocation)
        return nullptr;

    IDriverMemoryBacked::SDriverMemoryRequirements memoryReqs = getUpStreamingMemoryReqs();
    memoryReqs.vulkanReqs.alignment = core::max(memoryReqs.vulkanReqs.alignment, additionalReqs.vulkanReqs.alignment);
    memoryReqs.vulkanReqs.size = core::max(memoryReqs.vulkanReqs.size, additionalReqs.vulkanReqs.size);
    memoryReqs.vulkanReqs.memoryTypeBits &= additionalReqs.vulkanReqs.memoryTypeBits;
    memoryReqs.mappingCapability = additionalReqs.mappingCapability;
    memoryReqs.memoryHeapLocation = additionalReqs.memoryHeapLocation;
    memoryReqs.prefersDedicatedAllocation = additionalReqs.prefersDedicatedAllocation;
    memoryReqs.requiresDedicatedAllocation = additionalReqs.requiresDedicatedAllocation;

    return allocateGPUMemory(memoryReqs);
}

core::smart_refctd_ptr<IDriverMemoryAllocation> CVulkanLogicalDevice::allocateDownStreamingMemory(
    const IDriverMemoryBacked::SDriverMemoryRequirements& additionalReqs)
{
    if (getDownStreamingMemoryReqs().memoryHeapLocation != additionalReqs.memoryHeapLocation)
        return nullptr;

    IDriverMemoryBacked::SDriverMemoryRequirements memoryReqs = getDownStreamingMemoryReqs();
    memoryReqs.vulkanReqs.alignment = core::max(memoryReqs.vulkanReqs.alignment, additionalReqs.vulkanReqs.alignment);
    memoryReqs.vulkanReqs.size = core::max(memoryReqs.vulkanReqs.size, additionalReqs.vulkanReqs.size);
    memoryReqs.vulkanReqs.memoryTypeBits &= additionalReqs.vulkanReqs.memoryTypeBits;
    memoryReqs.mappingCapability = additionalReqs.mappingCapability;
    memoryReqs.memoryHeapLocation = additionalReqs.memoryHeapLocation;
    memoryReqs.prefersDedicatedAllocation = additionalReqs.prefersDedicatedAllocation;
    memoryReqs.requiresDedicatedAllocation = additionalReqs.requiresDedicatedAllocation;

    return allocateGPUMemory(memoryReqs);
}

core::smart_refctd_ptr<IDriverMemoryAllocation> CVulkanLogicalDevice::allocateCPUSideGPUVisibleMemory(
    const IDriverMemoryBacked::SDriverMemoryRequirements& additionalReqs)
{
    if (additionalReqs.memoryHeapLocation != IDriverMemoryAllocation::ESMT_NOT_DEVICE_LOCAL)
        return nullptr;

    IDriverMemoryBacked::SDriverMemoryRequirements memoryReqs = getCPUSideGPUVisibleGPUMemoryReqs();
    memoryReqs.vulkanReqs.alignment = core::max(memoryReqs.vulkanReqs.alignment, additionalReqs.vulkanReqs.alignment);
    memoryReqs.vulkanReqs.size = core::max(memoryReqs.vulkanReqs.size, additionalReqs.vulkanReqs.size);
    memoryReqs.vulkanReqs.memoryTypeBits &= additionalReqs.vulkanReqs.memoryTypeBits;
    memoryReqs.mappingCapability = additionalReqs.mappingCapability;
    memoryReqs.memoryHeapLocation = additionalReqs.memoryHeapLocation;
    memoryReqs.prefersDedicatedAllocation = additionalReqs.prefersDedicatedAllocation;
    memoryReqs.requiresDedicatedAllocation = additionalReqs.requiresDedicatedAllocation;

    return allocateGPUMemory(memoryReqs);
}

core::smart_refctd_ptr<IDriverMemoryAllocation> CVulkanLogicalDevice::allocateGPUMemory(
    const IDriverMemoryBacked::SDriverMemoryRequirements& reqs)
{
    VkMemoryPropertyFlags desiredMemoryProperties = static_cast<VkMemoryPropertyFlags>(0u);

    if (reqs.memoryHeapLocation == IDriverMemoryAllocation::ESMT_DEVICE_LOCAL)
        desiredMemoryProperties |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    if ((reqs.mappingCapability & IDriverMemoryAllocation::EMCF_CAN_MAP_FOR_READ) ||
        (reqs.mappingCapability & IDriverMemoryAllocation::EMCF_CAN_MAP_FOR_WRITE))
        desiredMemoryProperties |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

    if (reqs.mappingCapability & IDriverMemoryAllocation::EMCF_COHERENT)
        desiredMemoryProperties |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    if (reqs.mappingCapability & IDriverMemoryAllocation::EMCF_CACHED)
        desiredMemoryProperties |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;

    const IPhysicalDevice::SMemoryProperties& memoryProperties = m_physicalDevice->getMemoryProperties();

    uint32_t compatibleMemoryTypeCount = 0u;
    uint32_t compatibleMemoryTypeIndices[VK_MAX_MEMORY_TYPES];

    for (uint32_t i = 0u; i < memoryProperties.memoryTypeCount; ++i)
    {
        const bool memoryTypeSupportedForResource = (reqs.vulkanReqs.memoryTypeBits & (1 << i));

        const bool memoryHasDesirableProperties = (memoryProperties.memoryTypes[i].propertyFlags
            & desiredMemoryProperties) == desiredMemoryProperties;

        if (memoryTypeSupportedForResource && memoryHasDesirableProperties)
            compatibleMemoryTypeIndices[compatibleMemoryTypeCount++] = i;
    }

    for (uint32_t i = 0u; i < compatibleMemoryTypeCount; ++i)
    {
        // Todo(achal): Make use of requiresDedicatedAllocation and prefersDedicatedAllocation

        VkMemoryAllocateInfo vk_allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        vk_allocateInfo.pNext = nullptr; // No extensions for now
        vk_allocateInfo.allocationSize = reqs.vulkanReqs.size;
        vk_allocateInfo.memoryTypeIndex = compatibleMemoryTypeIndices[i];

        VkDeviceMemory vk_deviceMemory;
        if (m_devf.vk.vkAllocateMemory(m_vkdev, &vk_allocateInfo, nullptr, &vk_deviceMemory) == VK_SUCCESS)
        {
            // Todo(achal): Change dedicate to not always be false
            return core::make_smart_refctd_ptr<CVulkanMemoryAllocation>(this, reqs.vulkanReqs.size, false, vk_deviceMemory);
        }
    }

    return nullptr;
}

bool CVulkanLogicalDevice::createCommandBuffers_impl(IGPUCommandPool* cmdPool, IGPUCommandBuffer::E_LEVEL level,
    uint32_t count, core::smart_refctd_ptr<IGPUCommandBuffer>* outCmdBufs)
{
    constexpr uint32_t MAX_COMMAND_BUFFER_COUNT = 1000u;

    if (cmdPool->getAPIType() != EAT_VULKAN)
        return false;

    auto vulkanCommandPool = static_cast<CVulkanCommandPool*>(cmdPool)->getInternalObject();

    assert(count <= MAX_COMMAND_BUFFER_COUNT);
    VkCommandBuffer vk_commandBuffers[MAX_COMMAND_BUFFER_COUNT];

    VkCommandBufferAllocateInfo vk_allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    vk_allocateInfo.pNext = nullptr; // this must be NULL
    vk_allocateInfo.commandPool = vulkanCommandPool;
    vk_allocateInfo.level = static_cast<VkCommandBufferLevel>(level);
    vk_allocateInfo.commandBufferCount = count;

    if (m_devf.vk.vkAllocateCommandBuffers(m_vkdev, &vk_allocateInfo, vk_commandBuffers) == VK_SUCCESS)
    {
        for (uint32_t i = 0u; i < count; ++i)
        {
            outCmdBufs[i] = core::make_smart_refctd_ptr<CVulkanCommandBuffer>(
                core::smart_refctd_ptr<ILogicalDevice>(this), level, vk_commandBuffers[i],
                cmdPool);
        }

        return true;
    }
    else
    {
        return false;
    }
}

core::smart_refctd_ptr<IGPUImage> CVulkanLogicalDevice::createGPUImage(asset::IImage::SCreationParams&& params)
{
    VkImageCreateInfo vk_createInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    vk_createInfo.pNext = nullptr; // there are a lot of extensions
    vk_createInfo.flags = static_cast<VkImageCreateFlags>(params.flags);
    vk_createInfo.imageType = static_cast<VkImageType>(params.type);
    vk_createInfo.format = getVkFormatFromFormat(params.format);
    vk_createInfo.extent = { params.extent.width, params.extent.height, params.extent.depth };
    vk_createInfo.mipLevels = params.mipLevels;
    vk_createInfo.arrayLayers = params.arrayLayers;
    vk_createInfo.samples = static_cast<VkSampleCountFlagBits>(params.samples);
    vk_createInfo.tiling = static_cast<VkImageTiling>(params.tiling);
    vk_createInfo.usage = static_cast<VkImageUsageFlags>(params.usage.value);
    vk_createInfo.sharingMode = static_cast<VkSharingMode>(params.sharingMode);
    vk_createInfo.queueFamilyIndexCount = params.queueFamilyIndexCount;
    vk_createInfo.pQueueFamilyIndices = params.queueFamilyIndices;
    vk_createInfo.initialLayout = static_cast<VkImageLayout>(params.initialLayout);

    VkImage vk_image;
    if (m_devf.vk.vkCreateImage(m_vkdev, &vk_createInfo, nullptr, &vk_image) == VK_SUCCESS)
    {
        VkImageMemoryRequirementsInfo2 vk_memReqsInfo = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2 };
        vk_memReqsInfo.pNext = nullptr;
        vk_memReqsInfo.image = vk_image;

        VkMemoryDedicatedRequirements vk_memDedReqs = { VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS };
        VkMemoryRequirements2 vk_memReqs = { VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2 };
        vk_memReqs.pNext = &vk_memDedReqs;

        m_devf.vk.vkGetImageMemoryRequirements2(m_vkdev, &vk_memReqsInfo, &vk_memReqs);

        IDriverMemoryBacked::SDriverMemoryRequirements imageMemReqs = {};
        imageMemReqs.vulkanReqs.alignment = vk_memReqs.memoryRequirements.alignment;
        imageMemReqs.vulkanReqs.size = vk_memReqs.memoryRequirements.size;
        imageMemReqs.vulkanReqs.memoryTypeBits = vk_memReqs.memoryRequirements.memoryTypeBits;
        imageMemReqs.memoryHeapLocation = 0u; // doesn't matter, would get overwritten during memory allocation for this resource anyway
        imageMemReqs.mappingCapability = 0u; // doesn't matter, would get overwritten during memory allocation for this resource anyway
        imageMemReqs.prefersDedicatedAllocation = vk_memDedReqs.prefersDedicatedAllocation;
        imageMemReqs.requiresDedicatedAllocation = vk_memDedReqs.requiresDedicatedAllocation;

        return core::make_smart_refctd_ptr<CVulkanImage>(
            core::smart_refctd_ptr<CVulkanLogicalDevice>(this), std::move(params),
            vk_image, imageMemReqs);
    }
    else
    {
        return nullptr;
    }
}

core::smart_refctd_ptr<IGPUGraphicsPipeline> CVulkanLogicalDevice::createGPUGraphicsPipeline_impl(
    IGPUPipelineCache* pipelineCache,
    IGPUGraphicsPipeline::SCreationParams&& params)
{
    core::smart_refctd_ptr<IGPUGraphicsPipeline> result;
    if (createGPUGraphicsPipelines_impl(pipelineCache, { &params, &params + 1 }, &result))
        return result;
    else
        return nullptr;
}

bool CVulkanLogicalDevice::createGPUGraphicsPipelines_impl(
    IGPUPipelineCache* pipelineCache,
    core::SRange<const IGPUGraphicsPipeline::SCreationParams> params,
    core::smart_refctd_ptr<IGPUGraphicsPipeline>* output)
{
    IGPUGraphicsPipeline::SCreationParams* creationParams = const_cast<IGPUGraphicsPipeline::SCreationParams*>(params.begin());

    VkPipelineCache vk_pipelineCache = VK_NULL_HANDLE;
    if (pipelineCache && pipelineCache->getAPIType() == EAT_VULKAN)
        vk_pipelineCache = static_cast<const CVulkanPipelineCache*>(pipelineCache)->getInternalObject();

    // Shader stages
    uint32_t shaderStageCount_total = 0u;
    core::vector<VkPipelineShaderStageCreateInfo> vk_shaderStages(params.size() * IGPURenderpassIndependentPipeline::SHADER_STAGE_COUNT);
    uint32_t specInfoCount_total = 0u;
    core::vector<VkSpecializationInfo> vk_specInfos(vk_shaderStages.size());
    constexpr uint32_t MAX_MAP_ENTRIES_PER_SHADER = 100u;
    uint32_t mapEntryCount_total = 0u;
    core::vector<VkSpecializationMapEntry> vk_mapEntries(vk_specInfos.size()*MAX_MAP_ENTRIES_PER_SHADER);

    // Vertex input
    uint32_t vertexBindingDescriptionCount_total = 0u;
    core::vector<VkVertexInputBindingDescription> vk_vertexBindingDescriptions(params.size() * asset::SVertexInputParams::MAX_ATTR_BUF_BINDING_COUNT);
    uint32_t vertexAttribDescriptionCount_total = 0u;
    core::vector<VkVertexInputAttributeDescription> vk_vertexAttribDescriptions(params.size() * asset::SVertexInputParams::MAX_VERTEX_ATTRIB_COUNT);
    core::vector<VkPipelineVertexInputStateCreateInfo> vk_vertexInputStates(params.size());

    // Input Assembly
    core::vector<VkPipelineInputAssemblyStateCreateInfo> vk_inputAssemblyStates(params.size());

    // Viewport State
    uint32_t maxViewportCount = params.size(); // 1 per pipeline
    uint32_t maxScissorCount = params.size(); // 1 per pipeline
    // On a side note: m_physicalDevice->getFeatures are supported features not enabled features!
    // if (<some way to get enabled device features>.multiViewport)
    // {
    //     maxViewportCount = m_physicalDevice->getLimits().maxViewports * params.size();
    //     maxScissorCount = m_physicalDevice->getLimits().maxViewports * params.size();
    // }
    uint32_t viewportCount_total = 0u;
    uint32_t scissorCount_total = 0u;
    core::vector<VkViewport> vk_viewports(maxViewportCount);
    core::vector<VkRect2D> vk_scissors(maxScissorCount);
    core::vector<VkPipelineViewportStateCreateInfo> vk_viewportStates(params.size());

    core::vector<VkPipelineRasterizationStateCreateInfo> vk_rasterizationStates(params.size());

    core::vector<VkPipelineMultisampleStateCreateInfo> vk_multisampleStates(params.size());

    uint32_t colorBlendAttachmentCount_total = 0u;
    core::vector<VkPipelineColorBlendAttachmentState> vk_colorBlendAttachmentStates(params.size() * asset::SBlendParams::MAX_COLOR_ATTACHMENT_COUNT);
    core::vector<VkPipelineColorBlendStateCreateInfo> vk_colorBlendStates(params.size());

    core::vector<VkGraphicsPipelineCreateInfo> vk_createInfos(params.size());
    for (size_t i = 0ull; i < params.size(); ++i)
    {
        const auto& rpIndie = creationParams[i].renderpassIndependent;

        vk_createInfos[i].sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        vk_createInfos[i].pNext = nullptr;
        vk_createInfos[i].flags = static_cast<VkPipelineCreateFlags>(creationParams[i].createFlags.value);

        uint32_t shaderStageCount = 0u;
        for (uint32_t ss = 0u; ss < IGPURenderpassIndependentPipeline::SHADER_STAGE_COUNT; ++ss)
        {
            const IGPUSpecializedShader* shader = rpIndie->getShaderAtIndex(ss);
            if (!shader || shader->getAPIType() != EAT_VULKAN)
                continue;

            const auto* vulkanSpecShader = static_cast<const CVulkanSpecializedShader*>(shader);

            auto& vk_shaderStage = vk_shaderStages[shaderStageCount_total + shaderStageCount];

            vk_shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vk_shaderStage.pNext = nullptr;
            vk_shaderStage.flags = 0u;
            vk_shaderStage.stage = static_cast<VkShaderStageFlagBits>(shader->getStage());
            vk_shaderStage.module = vulkanSpecShader->getInternalObject();
            vk_shaderStage.pName = "main";

            const auto& shaderSpecInfo = vulkanSpecShader->getSpecInfo();

            if (shaderSpecInfo.m_backingBuffer && shaderSpecInfo.m_entries)
            {
                for (uint32_t me = 0u; me < shaderSpecInfo.m_entries->size(); ++me)
                {
                    const auto entry = shaderSpecInfo.m_entries->begin() + me;

                    vk_mapEntries[mapEntryCount_total + me].constantID = entry->specConstID;
                    vk_mapEntries[mapEntryCount_total + me].offset = entry->offset;
                    vk_mapEntries[mapEntryCount_total + me].size = entry->size;
                }

                vk_specInfos[specInfoCount_total].mapEntryCount = static_cast<uint32_t>(shaderSpecInfo.m_entries->size());
                vk_specInfos[specInfoCount_total].pMapEntries = vk_mapEntries.data() + mapEntryCount_total;
                mapEntryCount_total += vk_specInfos[specInfoCount_total].mapEntryCount;
                vk_specInfos[specInfoCount_total].dataSize = shaderSpecInfo.m_backingBuffer->getSize();
                vk_specInfos[specInfoCount_total].pData = shaderSpecInfo.m_backingBuffer->getPointer();

                vk_shaderStage.pSpecializationInfo = vk_specInfos.data() + specInfoCount_total++;
            }
            else
            {
                vk_shaderStage.pSpecializationInfo = nullptr;
            }

            ++shaderStageCount;
        }
        vk_createInfos[i].stageCount = shaderStageCount;
        vk_createInfos[i].pStages = vk_shaderStages.data() + shaderStageCount_total;
        shaderStageCount_total += shaderStageCount;

        // Vertex Input
        {
            vk_vertexInputStates[i].sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vk_vertexInputStates[i].pNext = nullptr;
            vk_vertexInputStates[i].flags = 0u;

            const auto& vertexInputParams = rpIndie->getVertexInputParams();

            // Fill up vertex binding descriptions
            uint32_t offset = vertexBindingDescriptionCount_total;
            uint32_t vertexBindingDescriptionCount = 0u;

            for (uint32_t b = 0u; b < asset::SVertexInputParams::MAX_ATTR_BUF_BINDING_COUNT; ++b)
            {
                if (vertexInputParams.enabledBindingFlags & (1 << b))
                {
                    auto& bndDesc = vk_vertexBindingDescriptions[offset + vertexBindingDescriptionCount++];

                    bndDesc.binding = b;
                    bndDesc.stride = vertexInputParams.bindings[b].stride;
                    bndDesc.inputRate = static_cast<VkVertexInputRate>(vertexInputParams.bindings[b].inputRate);
                }
            }
            vk_vertexInputStates[i].vertexBindingDescriptionCount = vertexBindingDescriptionCount;
            vk_vertexInputStates[i].pVertexBindingDescriptions = vk_vertexBindingDescriptions.data() + offset;
            vertexBindingDescriptionCount_total += vertexBindingDescriptionCount;

            // Fill up vertex attribute descriptions
            offset = vertexAttribDescriptionCount_total;
            uint32_t vertexAttribDescriptionCount = 0u;

            for (uint32_t l = 0u; l < asset::SVertexInputParams::MAX_VERTEX_ATTRIB_COUNT; ++l)
            {
                if (vertexInputParams.enabledAttribFlags & (1 << l))
                {
                    auto& attribDesc = vk_vertexAttribDescriptions[offset + vertexAttribDescriptionCount++];

                    attribDesc.location = l;
                    attribDesc.binding = vertexInputParams.attributes[l].binding;
                    attribDesc.format = getVkFormatFromFormat(static_cast<asset::E_FORMAT>(vertexInputParams.attributes[l].format));
                    attribDesc.offset = vertexInputParams.attributes[l].relativeOffset;
                }
            }
            vk_vertexInputStates[i].vertexAttributeDescriptionCount = vertexAttribDescriptionCount;
            vk_vertexInputStates[i].pVertexAttributeDescriptions = vk_vertexAttribDescriptions.data() + offset;
            vertexAttribDescriptionCount_total += vertexAttribDescriptionCount;
        }
        vk_createInfos[i].pVertexInputState = &vk_vertexInputStates[i];

        // Input Assembly
        {
            const auto& primAssParams = rpIndie->getPrimitiveAssemblyParams();

            vk_inputAssemblyStates[i].sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            vk_inputAssemblyStates[i].pNext = nullptr;
            vk_inputAssemblyStates[i].flags = 0u; // reserved for future use by Vulkan
            vk_inputAssemblyStates[i].topology = static_cast<VkPrimitiveTopology>(primAssParams.primitiveType);
            vk_inputAssemblyStates[i].primitiveRestartEnable = primAssParams.primitiveRestartEnable;
        }
        vk_createInfos[i].pInputAssemblyState = &vk_inputAssemblyStates[i];

        // Tesselation
        vk_createInfos[i].pTessellationState = nullptr;

        // Viewport State
        {
            const auto& viewportParams = creationParams[i].viewportParams;

            vk_viewportStates[i].sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            vk_viewportStates[i].pNext = nullptr;
            vk_viewportStates[i].flags = 0u;

            // Viewports
            {
                assert(viewportParams.viewportCount == viewportParams.scissorCount);

                uint32_t viewportCount = viewportParams.viewportCount;
                assert(viewportCount <= 1u);
                // if (viewportCount > 1u)
                //     assert((<some way to get enabled device features>.multiViewport>.multiViewport)
                //         && (viewportCount <= m_physicalDevice->getLimits().maxViewports));

                uint32_t offset = viewportCount_total;
                memcpy(vk_viewports.data() + offset, &viewportParams.viewport, viewportCount * sizeof(asset::SViewport));
                viewportCount_total += viewportCount;

                vk_viewportStates[i].viewportCount = viewportCount;
                vk_viewportStates[i].pViewports = vk_viewports.data() + offset;
            }

            // Scissors
            {
                uint32_t scissorCount = viewportParams.scissorCount;
                assert(scissorCount <= 1u);
                // if (scissorCount > 1)
                //     assert((<some way to get enabled device features>.multiViewport>.multiViewport)
                //         && (scissorCount <= m_physicalDevice->getLimits().maxViewports));

                uint32_t offset = scissorCount_total;
                memcpy(vk_scissors.data() + offset, &viewportParams.scissor, scissorCount * sizeof(VkRect2D));
                scissorCount_total += scissorCount;

                vk_viewportStates[i].scissorCount = scissorCount;
                vk_viewportStates[i].pScissors = vk_scissors.data() + offset;
            }
        }
        vk_createInfos[i].pViewportState = &vk_viewportStates[i];

        // Rasterization
        {
            const auto& rasterizationParams = rpIndie->getRasterizationParams();

            vk_rasterizationStates[i].sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            vk_rasterizationStates[i].pNext = nullptr;
            vk_rasterizationStates[i].flags = 0u;
            vk_rasterizationStates[i].depthClampEnable = rasterizationParams.depthClampEnable;
            vk_rasterizationStates[i].rasterizerDiscardEnable = rasterizationParams.rasterizerDiscard;
            vk_rasterizationStates[i].polygonMode = static_cast<VkPolygonMode>(rasterizationParams.polygonMode);
            vk_rasterizationStates[i].cullMode = static_cast<VkCullModeFlags>(rasterizationParams.faceCullingMode);
            vk_rasterizationStates[i].frontFace = rasterizationParams.frontFaceIsCCW ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
            vk_rasterizationStates[i].depthBiasEnable = rasterizationParams.depthBiasEnable;
            vk_rasterizationStates[i].depthBiasConstantFactor = rasterizationParams.depthBiasConstantFactor;
            vk_rasterizationStates[i].depthBiasClamp = 0.f;
            vk_rasterizationStates[i].depthBiasSlopeFactor = rasterizationParams.depthBiasSlopeFactor;
            vk_rasterizationStates[i].lineWidth = 1.f;
        }
        vk_createInfos[i].pRasterizationState = &vk_rasterizationStates[i];

        // Multisampling
        {
            const auto& rasterizationParams = rpIndie->getRasterizationParams();

            vk_multisampleStates[i].sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            vk_multisampleStates[i].pNext = nullptr;
            vk_multisampleStates[i].flags = 0u;
            vk_multisampleStates[i].rasterizationSamples = static_cast<VkSampleCountFlagBits>(rasterizationParams.rasterizationSamplesHint);            
            vk_multisampleStates[i].sampleShadingEnable = rasterizationParams.sampleShadingEnable;
            vk_multisampleStates[i].minSampleShading = rasterizationParams.minSampleShading;
            vk_multisampleStates[i].pSampleMask = rasterizationParams.sampleMask;
            vk_multisampleStates[i].alphaToCoverageEnable = rasterizationParams.alphaToCoverageEnable;
            vk_multisampleStates[i].alphaToOneEnable = rasterizationParams.alphaToOneEnable;
        }
        vk_createInfos[i].pMultisampleState = &vk_multisampleStates[i];

        // Depth-stencil
        vk_createInfos[i].pDepthStencilState = nullptr;

        // Color blend
        {
            const auto& blendParams = rpIndie->getBlendParams();
            
            uint32_t offset = colorBlendAttachmentCount_total;

            uint32_t colorBlendAttachmentCount = 0u;
            for (uint32_t as = 0u; as < asset::SBlendParams::MAX_COLOR_ATTACHMENT_COUNT; ++as)
            {
                const auto& attBlendParams = blendParams.blendParams[as];
                if (attBlendParams.attachmentEnabled)
                {
                    auto& attBlendState = vk_colorBlendAttachmentStates[offset + colorBlendAttachmentCount++];

                    attBlendState.blendEnable = attBlendParams.blendEnable;
                    attBlendState.srcColorBlendFactor = static_cast<VkBlendFactor>(attBlendParams.srcColorFactor);
                    attBlendState.dstColorBlendFactor = static_cast<VkBlendFactor>(attBlendParams.dstColorFactor);
                    assert(attBlendParams.colorBlendOp <= asset::EBO_MAX);
                    attBlendState.colorBlendOp = static_cast<VkBlendOp>(attBlendParams.colorBlendOp);
                    attBlendState.srcAlphaBlendFactor = static_cast<VkBlendFactor>(attBlendParams.srcAlphaFactor);
                    attBlendState.dstAlphaBlendFactor = static_cast<VkBlendFactor>(attBlendParams.dstAlphaFactor);
                    assert(attBlendParams.alphaBlendOp <= asset::EBO_MAX);
                    attBlendState.alphaBlendOp = static_cast<VkBlendOp>(attBlendParams.alphaBlendOp);
                    attBlendState.colorWriteMask = static_cast<VkColorComponentFlags>(attBlendParams.colorWriteMask);
                }
            }
            colorBlendAttachmentCount_total += colorBlendAttachmentCount;

            vk_colorBlendStates[i].sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            vk_colorBlendStates[i].pNext = nullptr;
            vk_colorBlendStates[i].flags = 0u;
            vk_colorBlendStates[i].logicOpEnable = blendParams.logicOpEnable;
            vk_colorBlendStates[i].logicOp = static_cast<VkLogicOp>(blendParams.logicOp);
            vk_colorBlendStates[i].attachmentCount = colorBlendAttachmentCount;
            vk_colorBlendStates[i].pAttachments = vk_colorBlendAttachmentStates.data() + offset;
            vk_colorBlendStates[i].blendConstants[0] = 0.0f;
            vk_colorBlendStates[i].blendConstants[1] = 0.0f;
            vk_colorBlendStates[i].blendConstants[2] = 0.0f;
            vk_colorBlendStates[i].blendConstants[3] = 0.0f;
        }
        vk_createInfos[i].pColorBlendState = &vk_colorBlendStates[i];

        // Dynamic state
        vk_createInfos[i].pDynamicState = nullptr;

        vk_createInfos[i].layout = static_cast<const CVulkanPipelineLayout*>(rpIndie->getLayout())->getInternalObject();
        vk_createInfos[i].renderPass = static_cast<const CVulkanRenderpass*>(creationParams[i].renderpass.get())->getInternalObject();
        vk_createInfos[i].subpass = creationParams[i].subpassIx;
        vk_createInfos[i].basePipelineHandle = VK_NULL_HANDLE;
        vk_createInfos[i].basePipelineIndex = 0u;
    }

    core::vector<VkPipeline> vk_pipelines(params.size());
    if (m_devf.vk.vkCreateGraphicsPipelines(m_vkdev, vk_pipelineCache,
        static_cast<uint32_t>(params.size()), vk_createInfos.data(), nullptr, vk_pipelines.data()) == VK_SUCCESS)
    {
        for (size_t i = 0ull; i < params.size(); ++i)
        {
            output[i] = core::make_smart_refctd_ptr<CVulkanGraphicsPipeline>(
                core::smart_refctd_ptr<CVulkanLogicalDevice>(this),
                std::move(creationParams[i]),
                vk_pipelines[i]);
        }
        return true;
    }
    else
    {
        return false;
    }
}

}