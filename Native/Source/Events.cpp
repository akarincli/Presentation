#include "Events.h"
#include "SDL2/SDL.h"
#include <atomic>

namespace {
    std::atomic_bool running = false, shouldRun = false;

    void ApplicationWaitEvents(SDL_Event& event) noexcept {
        SDL_WaitEvent(&event);
    }

    void ApplicationProcessEvent(const SDL_Event& event) noexcept {
        switch (event.type) {
            /* Application events */
        case SDL_QUIT: /**< User-requested quit */
            /* These application events have special meaning on iOS, see README-ios.md for details */
        case SDL_APP_TERMINATING:
            /**< The application is being terminated by the OS
              Called on iOS in applicationWillTerminate()
              Called on Android in onDestroy()
            */
            shouldRun = false;
        case SDL_APP_LOWMEMORY:
            /**< The application is low on memory, free memory if possible.
              Called on iOS in applicationDidReceiveMemoryWarning()
              Called on Android in onLowMemory()
            */
        case SDL_APP_WILLENTERBACKGROUND:
            /**< The application is about to enter the background
              Called on iOS in applicationWillResignActive()
              Called on Android in onPause()
            */
        case SDL_APP_DIDENTERBACKGROUND:
            /**< The application did enter the background and may not get CPU for some time
             Called on iOS in applicationDidEnterBackground()
             Called on Android in onPause()
            */
        case SDL_APP_WILLENTERFOREGROUND:
            /**< The application is about to enter the foreground
              Called on iOS in applicationWillEnterForeground()
              Called on Android in onResume()
            */
        case SDL_APP_DIDENTERFOREGROUND:
            /**< The application is now interactive
              Called on iOS in applicationDidBecomeActive()
              Called on Android in onResume()
            */
            /* Window events */
        case SDL_WINDOWEVENT:            /**< Window state change */
        case SDL_SYSWMEVENT:             /**< System specific event */

            /* Keyboard events */
        case SDL_KEYDOWN:                /**< Key pressed */
        case SDL_KEYUP:                  /**< Key released */
        case SDL_TEXTEDITING:            /**< Keyboard text editing (composition) */
        case SDL_TEXTINPUT:              /**< Keyboard text input */
        case SDL_KEYMAPCHANGED:          /**< Keymap changed due to a system event such as an
                                          input language or keyboard layout change.
                                         */

            /* Mouse events */
        case SDL_MOUSEMOTION:            /**< Mouse moved */
        case SDL_MOUSEBUTTONDOWN:        /**< Mouse button pressed */
        case SDL_MOUSEBUTTONUP:          /**< Mouse button released */
        case SDL_MOUSEWHEEL:             /**< Mouse wheel motion */

            /* Touch events */
        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
        case SDL_FINGERMOTION:

            /* Gesture events */
        case SDL_DOLLARGESTURE:
        case SDL_DOLLARRECORD:
        case SDL_MULTIGESTURE:

            /* Clipboard events */
        case SDL_CLIPBOARDUPDATE:          /**< The clipboard changed */

            /* Drag and drop events */
        case SDL_DROPFILE:                 /**< The system requests a file open */
        case SDL_DROPTEXT:                 /**< text/plain drag-and-drop event */
        case SDL_DROPBEGIN:                /**< A new set of drops is beginning (NULL filename) */
        case SDL_DROPCOMPLETE:             /**< Current set of drops is now complete (NULL filename) */

            /* Render events */
        case SDL_RENDER_TARGETS_RESET: /**< The render targets have been reset and their contents need to be updated */
        case SDL_RENDER_DEVICE_RESET: /**< The device has been reset and all textures need to be recreated */
        default:break;
        }
    }

}

AK_PUBLIC void AK_CALL akAppControlHandOver() noexcept {
    running = shouldRun = true;
    SDL_Event event;
    while (shouldRun) {
        ApplicationWaitEvents(event);
        ApplicationProcessEvent(event);
    }
    running = false;
}

AK_PUBLIC void AK_CALL akAppStop() noexcept {
    shouldRun = false;
}
