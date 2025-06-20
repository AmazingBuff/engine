//
// Created by AmazingBuff on 2025/6/9.
//

#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <astd/astd.h>
#include "geometry/mesh.h"

AMAZING_NAMESPACE_BEGIN

Scene load_model(const char* path);
void release_model(const Scene& model);


AMAZING_NAMESPACE_END

#endif //MODEL_LOADER_H
