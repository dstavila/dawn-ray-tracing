// Copyright 2018 The Dawn Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DAWNNATIVE_VULKAN_RAY_TRACING_ACCELERATION_CONTAINER_H_
#define DAWNNATIVE_VULKAN_RAY_TRACING_ACCELERATION_CONTAINER_H_

#include "common/vulkan_platform.h"
#include "dawn_native/RayTracingAccelerationContainer.h"
#include "dawn_native/ResourceMemoryAllocation.h"
#include "dawn_native/DynamicUploader.h"

#include <vector>

namespace dawn_native { namespace vulkan {

    class Device;

    struct VkAccelerationInstance {
        float transform[12];
        uint32_t instanceId : 24;
        uint32_t mask : 8;
        uint32_t instanceOffset : 24;
        uint32_t flags : 8;
        uint64_t accelerationStructureHandle;
    };

    struct ScratchMemory {
        uint32_t offset = 0;
        VkBuffer buffer = VK_NULL_HANDLE;
        ResourceMemoryAllocation resource;
    };

    struct ScratchMemoryPool {
        ScratchMemory result;
        ScratchMemory build;
        ScratchMemory update;
    };

    class RayTracingAccelerationContainer : public RayTracingAccelerationContainerBase {
      public:
        static ResultOrError<RayTracingAccelerationContainer*> Create(Device* device, const RayTracingAccelerationContainerDescriptor* descriptor);
        ~RayTracingAccelerationContainer();

        MaybeError GetHandle(uint64_t* handle) const;
        VkAccelerationStructureTypeNV GetLevel() const;
        VkBuildAccelerationStructureFlagBitsNV GetFlags() const;
        VkAccelerationStructureNV GetAccelerationStructure() const;
        VkMemoryRequirements2 RayTracingAccelerationContainer::GetMemoryRequirements(
            VkAccelerationStructureMemoryRequirementsTypeNV type) const;
        uint32_t RayTracingAccelerationContainer::GetMemoryRequirementSize(
            VkAccelerationStructureMemoryRequirementsTypeNV type) const;

        VkBuffer GetInstanceBuffer() const;
        
        std::vector<VkGeometryNV>& GetGeometries();
        std::vector<VkAccelerationInstance>& GetInstances();

        ScratchMemoryPool GetScratchMemory() const;

      private:
        using RayTracingAccelerationContainerBase::RayTracingAccelerationContainerBase;

        std::vector<VkGeometryNV> mGeometries;
        std::vector<VkAccelerationInstance> mInstances;

        // AS related
        VkAccelerationStructureTypeNV mLevel;
        VkBuildAccelerationStructureFlagBitsNV mFlags;
        VkAccelerationStructureNV mAccelerationStructure = VK_NULL_HANDLE;

        // instance buffer for top-level containers
        VkBuffer mInstanceBuffer = VK_NULL_HANDLE;
        ResourceMemoryAllocation mInstanceResource;
        UploadHandle mInstanceHandle;

        // scratch memory
        ScratchMemoryPool mScratchMemory;

        MaybeError RayTracingAccelerationContainer::CreateAccelerationStructure(
            const RayTracingAccelerationContainerDescriptor* descriptor);
        MaybeError RayTracingAccelerationContainer::ReserveScratchMemory(
            const RayTracingAccelerationContainerDescriptor* descriptor);

        MaybeError CreateScratchMemory(ScratchMemoryPool* memory,
                                       uint32_t resultSize,
                                       uint32_t buildSize,
                                       uint32_t updateSize);

        MaybeError Initialize(const RayTracingAccelerationContainerDescriptor* descriptor);
    };

}}  // namespace dawn_native::vulkan

#endif  // DAWNNATIVE_VULKAN_RAY_TRACING_ACCELERATION_CONTAINER_H_
