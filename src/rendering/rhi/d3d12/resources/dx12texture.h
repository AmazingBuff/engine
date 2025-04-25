//
// Created by AmazingBuff on 2025/4/17.
//

#ifndef DX12TEXTURE_H
#define DX12TEXTURE_H

#include "rendering/rhi/common/texture.h"
#include "rendering/rhi/d3d12/d3d12ma/D3D12MemAlloc.h"

AMAZING_NAMESPACE_BEGIN

class DX12Texture final : public GPUTexture
{
public:
    DX12Texture();
    ~DX12Texture();

    AResult initialize(GPUDevice const* device, GPUTextureCreateInfo const& info) override;

private:
    static D3D12_RESOURCE_DIMENSION transfer_resource_dimension(GPUTextureCreateInfo const& info);
    static D3D12_TEXTURE_LAYOUT transfer_resource_layout(GPUTextureCreateInfo const& info);
    static D3D12_RESOURCE_FLAGS transfer_resource_flags(GPUTextureCreateInfo const& info);
    static GPUResourceState transfer_states(GPUTextureCreateInfo const& info);
    static uint32_t transfer_sampler_count(ID3D12Device* device, DXGI_FORMAT format, uint32_t sample_count);
private:
    ID3D12Resource*  m_resource;
    D3D12MA::Allocation* m_allocation;

    struct D3D12AliasingTextureInfo : GPUTextureInfo
    {
        String name;
        D3D12_RESOURCE_DESC resource_desc;
    };

    struct D3D12TiledTextureInfo : GPUTextureInfo
    {
        struct D3D12TiledPackingMipMapping
        {
            D3D12MA::Allocation* allocation;
            uint32_t count;
        };

        struct D3D12TiledSubresourceMapping
        {
            D3D12MA::Allocation* allocation;
            GPUTiledSubresourceInfo subresource_info;
        };

        GPUTiledTextureInfo tiled_info;
        Vector<D3D12TiledSubresourceMapping> subresources;
        Vector<D3D12TiledPackingMipMapping> packings;
    };

    friend class DX12SwapChain;
    friend class DX12TextureView;
};

AMAZING_NAMESPACE_END

#endif //DX12TEXTURE_H
