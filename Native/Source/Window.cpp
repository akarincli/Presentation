//
// Created by 刘予顺 on 2019-04-02.
//

#include "Window.h"
#include "SDL2/SDL.h"

AK_PUBLIC uintptr_t AK_CALL akCreateWindow(
        const char* caption,
        int32_t display, int32_t positionX, int32_t positionY, int32_t width, int32_t height
) noexcept {
    return reinterpret_cast<uintptr_t>(SDL_CreateWindow(caption, positionX, positionY, width, height, SDL_WINDOW_VULKAN));
}

AK_PUBLIC void AK_CALL akShowWindow(uintptr_t handle) noexcept {
    SDL_ShowWindow(reinterpret_cast<SDL_Window*>(handle));
}

AK_PUBLIC void AK_CALL akHideWindow(uintptr_t handle) noexcept{
    SDL_HideWindow(reinterpret_cast<SDL_Window*>(handle));
}

AK_PUBLIC void AK_CALL akMinimizeWindow(uintptr_t handle) noexcept{
    SDL_MinimizeWindow(reinterpret_cast<SDL_Window*>(handle));
}

AK_PUBLIC void AK_CALL akMaximizeWindow(uintptr_t handle) noexcept{
    SDL_MaximizeWindow(reinterpret_cast<SDL_Window*>(handle));
}

AK_PUBLIC void AK_CALL akDestroyWindow(uintptr_t handle) noexcept{
    SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(handle));
}
