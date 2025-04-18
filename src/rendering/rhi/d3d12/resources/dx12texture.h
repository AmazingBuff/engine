//
// Created by AmazingBuff on 2025/4/17.
//

#ifndef DX12TEXTURE_H
#define DX12TEXTURE_H

#include "rendering/rhi/common/texture.h"

AMAZING_NAMESPACE_BEGIN

class DX12Texture final : public GPUTexture
{
public:
    DX12Texture();
    ~DX12Texture();

    AResult initialize(GPUDevice const* device, GPUTextureCreateInfo const& info) override;

private:

    friend class DX12SwapChain;
};

AMAZING_NAMESPACE_END

#endif //DX12TEXTURE_H
