//
// Created by AmazingBuff on 2025/4/15.
//

#ifndef DX12DESCRIPTORHEAP_H
#define DX12DESCRIPTORHEAP_H

#include "rendering/api.h"
#include "rendering/rhi/enumeration.h"

AMAZING_NAMESPACE_BEGIN

class DX12DescriptorHeap
{
public:
    explicit DX12DescriptorHeap(ID3D12Device* device);
    ~DX12DescriptorHeap();

    void create_cbv(ID3D12Device* device, D3D12_CONSTANT_BUFFER_VIEW_DESC const& cbv_desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const;
    void create_srv(ID3D12Device* device, ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC const& srv_desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const;
    void create_uav(ID3D12Device* device, ID3D12Resource* resource, ID3D12Resource* counter_resource, D3D12_UNORDERED_ACCESS_VIEW_DESC const& uav_desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const;
    void create_rtv(ID3D12Device* device, ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC const& rtv_desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const;
    void create_dsv(ID3D12Device* device, ID3D12Resource* resource, D3D12_DEPTH_STENCIL_VIEW_DESC const& dsv_desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const;
private:
    struct D3D12DescriptorHandle
    {
        D3D12_CPU_DESCRIPTOR_HANDLE cpu;
        D3D12_GPU_DESCRIPTOR_HANDLE gpu;
    };

    struct D3D12DescriptorHeap
    {
        ID3D12DescriptorHeap* descriptor_heap;
        // handle
        D3D12DescriptorHandle start_handle;
        // size
        uint32_t descriptor_size;   // every descriptor size for corresponding type
        uint32_t descriptor_count;
        // used
        uint32_t used_descriptor_count;
        // need to free
        Vector<D3D12DescriptorHandle> free_list;
    };

    struct D3D12NullDescriptors
    {
        D3D12_CPU_DESCRIPTOR_HANDLE sampler;
        D3D12_CPU_DESCRIPTOR_HANDLE buffer_cbv;
        D3D12_CPU_DESCRIPTOR_HANDLE buffer_srv;
        D3D12_CPU_DESCRIPTOR_HANDLE buffer_uav;
        D3D12_CPU_DESCRIPTOR_HANDLE texture_srv[GPU_Texture_Type_Count];
        D3D12_CPU_DESCRIPTOR_HANDLE texture_uav[GPU_Texture_Type_Count];
    };
private:
    static D3D12DescriptorHeap* create_descriptor_heap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_DESC const& desc);
    static D3D12DescriptorHandle consume_descriptor_handle(D3D12DescriptorHeap* heap, uint32_t descriptor_count);
    static void return_descriptor_handle(D3D12DescriptorHeap* heap, D3D12_CPU_DESCRIPTOR_HANDLE handle, uint32_t descriptor_count);
    static void copy_descriptor_handle(ID3D12Device* device, D3D12DescriptorHeap* heap, D3D12_CPU_DESCRIPTOR_HANDLE const& src_handle, uint64_t const& dst_handle, uint32_t index);
private:
    // cpu
    D3D12DescriptorHeap* m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    // gpu
    D3D12DescriptorHeap* m_gpu_cbv_srv_uav_heaps[GPU_Node_Count];
    D3D12DescriptorHeap* m_gpu_sampler_heaps[GPU_Node_Count];
    // null
    D3D12NullDescriptors* m_null_descriptors;


    friend class DX12CommandBuffer;
    friend class DX12Buffer;
    friend class DX12TextureView;
    friend class DX12Sampler;
    friend class DX12DescriptorSet;
};

AMAZING_NAMESPACE_END



#endif //DX12DECRIPTOR_HEAP_H
