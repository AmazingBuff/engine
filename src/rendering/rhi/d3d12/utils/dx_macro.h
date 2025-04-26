//
// Created by AmazingBuff on 2025/4/11.
//

#ifndef DX_MACRO_H
#define DX_MACRO_H

#if defined(_DEBUG) || defined(DEBUG)
#define DX_CHECK_RESULT(expr)																		\
{																									\
    HRESULT res = expr;																			    \
    if (FAILED(res))																			    \
    {																								\
        std::string format = std::format(															\
        "[dx12, fatal, result]: HRESULT is \"{:x}\" in line: {}, function: {}, file: {}.",          \
        *reinterpret_cast<uint32_t*>(&res), __LINE__, __FUNCTION__, __FILE__);						\
        std::cerr << format << std::endl;															\
        throw std::runtime_error(format);															\
    }																								\
}
#else
#define DX_CHECK_RESULT(expr) (expr)
#endif


#define DX_FREE(ptr) if(ptr) {(ptr)->Release(); (ptr) = nullptr;}

#endif //DX_MACRO_H
