//
// Created by AmazingBuff on 2025/5/6.
//

#ifndef VK_MACRO_H
#define VK_MACRO_H

#if defined(_DEBUG) || defined(DEBUG)
#define VK_CHECK_RESULT(expr)																		\
{																									\
    VkResult res = expr;																			\
    if (res != VK_SUCCESS)																			\
    {																								\
        const char* message = Amazing::transfer_result(res);										\
        std::string format = std::format(															\
        "[vulkan, fatal, result]: VkResult is \"{}\" in line: {}, function: {}, file: {}.",         \
        message, __LINE__, __FUNCTION__, __FILE__);												    \
        std::cerr << format << std::endl;															\
        throw std::runtime_error(message);															\
    }																								\
}
#else
#define VK_CHECK_RESULT(expr) {(expr);}
#endif

#endif //VK_MACRO_H
