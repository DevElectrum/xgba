#!/usr/bin/env python3
"""Guard the XGBA SDL fast-forward contract against accidental regressions."""

from pathlib import Path


def require(text, needle, explanation):
    if needle not in text:
        raise SystemExit(f"Fast-forward regression: {explanation}")


def forbid(text, needle, explanation):
    if needle in text:
        raise SystemExit(f"Fast-forward regression: {explanation}")


def main():
    root = Path(__file__).resolve().parent.parent
    main_source = (root / "src/platform/sdl/main.c").read_text()
    events = (root / "src/platform/sdl/sdl-events.c").read_text()
    software = (root / "src/platform/sdl/sw-sdl2.c").read_text()
    opengl = (root / "src/platform/sdl/gl-common.c").read_text()

    require(main_source, ".audioSync = true", "normal speed must be audio-paced")
    require(main_source, ".videoSync = false", "video sync must not cap fast-forward")
    require(events, "if (keycode == SDLK_TAB)", "Tab fast-forward key is missing")
    require(events, "context->impl->sync.audioWait = event->type != SDL_KEYDOWN",
            "press/release must disable/restore audio pacing")
    require(events, "mCoreSyncLockAudio(&context->impl->sync)",
            "audio pacing must change under the synchronization lock")
    require(events, "mCoreSyncUnlockAudio(&context->impl->sync)",
            "audio synchronization lock must be released")
    require(software, "SDL_SetRenderVSync(renderer->sdlRenderer, 0)",
            "SDL3 VSync must be disabled for turbo")
    require(software, "SDL_RENDERER_ACCELERATED",
            "SDL2 accelerated rendering must remain enabled")
    forbid(software, "SDL_RENDERER_PRESENTVSYNC",
           "SDL2 presentation VSync throttles turbo")
    require(opengl, "SDL_GL_SetSwapInterval(0)",
            "SDL2/OpenGL swap interval must be zero")
    require(opengl, "SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0)",
            "legacy OpenGL swap control must be off")

    tab = events.index("if (keycode == SDLK_TAB)")
    mapping = events.index("key = mInputMapKey(sdlContext->bindings, SDL_BINDING_KEY, keycode)")
    if tab > mapping:
        raise SystemExit("Fast-forward regression: Tab must override GBA key bindings")

    print("PASS: SDL Tab fast-forward contract (key priority, audio pacing, VSync)")


if __name__ == "__main__":
    main()
