//
// Created by AmazingBuff on 2025/4/18.
//

#ifndef DX12BUFFER_H
#define DX12BUFFER_H

#include "rendering/rhi/common/buffer.h"

AMAZING_NAMESPACE_BEGIN

class DX12Buffer final : public GPUBuffer
{
public:
    DX12Buffer();
    ~DX12Buffer() override;

    AResult initialize() override;
private:
};

AMAZING_NAMESPACE_END

#endif //DX12BUFFER_H
