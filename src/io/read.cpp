//
// Created by AmazingBuff on 2025/4/30.
//

#include "io/read.h"

AMAZING_NAMESPACE_BEGIN

Vector<char> read_file(const String& filename)
{
    std::ifstream file(filename.c_str(), std::ios::ate | std::ios::binary);

    if (!file.is_open())
        LOG_ERROR("Read File", "failed to open file!");

    size_t file_size = file.tellg();
    Vector<char> buffer(file_size);
    file.seekg(0);
    file.read(buffer.data(), static_cast<int64_t>(file_size));
    file.close();
    return buffer;
}

AMAZING_NAMESPACE_END