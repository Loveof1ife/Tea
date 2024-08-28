#include "descriptorSet.h"

namespace Tea::Wrapper { 
	DescriptorSet::DescriptorSet(
		const Device::Ptr& device,
		const std::vector<UniformParameter::Ptr>& params,
		const DescriptorPool::Ptr& pool,
		const DescriptorSetLayout::Ptr& layout,
		int frameCount
	){
		mDevice = device;
		//we will create one descriptor set for each frame in flight, all with the same layout.
		std::vector<VkDescriptorSetLayout> layouts(frameCount, layout->getLayout());

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pool->getPool();
		allocInfo.descriptorSetCount = static_cast<uint32_t>(frameCount);
		allocInfo.pSetLayouts = layouts.data();

		mDescriptorSets.resize(frameCount);
		if (vkAllocateDescriptorSets(mDevice->getDevice(), &allocInfo, mDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
		//The descriptor sets have been allocated now,
		// but the descriptors within still need to be configured.
		for (int i = 0; i < frameCount; ++i) {
			std::vector<VkWriteDescriptorSet> descriptorSetWrites{};

			for (const auto& param : params) {
				VkWriteDescriptorSet descriptorSetWrite{};
				descriptorSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorSetWrite.dstSet = mDescriptorSets[i];
				descriptorSetWrite.dstArrayElement = 0;
				descriptorSetWrite.descriptorType = param->mDescriptorType;
				descriptorSetWrite.descriptorCount = param->mCount;
				descriptorSetWrite.dstBinding = param->mBinding;

				if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
					descriptorSetWrite.pBufferInfo = &param->mBuffers[i]->getBufferInfo();
				}

				if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
					descriptorSetWrite.pImageInfo = &param->mTexture->getImageInfo();
				}

				descriptorSetWrites.push_back(descriptorSetWrite);
			}
			vkUpdateDescriptorSets(mDevice->getDevice(), static_cast<uint32_t>(descriptorSetWrites.size()), descriptorSetWrites.data(), 0, nullptr);
		}
	}

	DescriptorSet::~DescriptorSet(){
	}
}



