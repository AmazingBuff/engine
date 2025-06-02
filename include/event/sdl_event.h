//
// Created by AmazingBuff on 2025/5/21.
//

#ifndef SDL_EVENT_H
#define SDL_EVENT_H

#include "event.h"
#include <SDL3/SDL_events.h>

AMAZING_NAMESPACE_BEGIN

using SDLEventCallback = Functional<void(const SDL_Event&)>;

using SDLEventCallbackHandler = EventCallbackHandler<uint32_t, SDLEventCallback>;

AMAZING_NAMESPACE_END

#endif //SDL_EVENT_H
