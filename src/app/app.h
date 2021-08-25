#pragma once

#ifdef _WIN32
#include <SDL.h>
#elif __linux__
#include <SDL2/SDL.h>
#endif

#include <string>

class Clock
{
public:
    Clock();

    float restart();

private:
    Uint32 m_start, m_end;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

class App
{
private:
    App();
    ~App();

public:
    static App& Get();
    static void Init(const char* title, int width, int height);
    static void Run();

    static void ClearColor(int r, int g, int b, int a);

    static int ScreenWidth();
    static int ScreenHeight();

    static SDL_Window* GetWindow();

    static bool GetFocus();
    static Uint8 GetKey(SDL_Scancode code);
    static const Uint8* GetKeys();

    static void FPSCounter(bool fpscounter);
    static void WireFrame(bool wireframe);
    static void VSync(bool vsync);
    static void Culling(bool cull);
    static void ShowCursor(bool cursor);

    static bool MouseHold(int key);

private:
    void IInit(const char* title, int width, int height);
    void IRun();

    void IClearColor(int r, int g, int b, int a);

    int IScreenWidth();
    int IScreenHeight();

    SDL_Window* IGetWindow();

    bool IGetFocus();
    Uint8 IGetKey(SDL_Scancode code);
    const Uint8* IGetKeys();

    void IFPSCounter(bool fpscounter);
    void IWireFrame(bool wireframe);
    void IVSync(bool vsync);
    void ICulling(bool cull);
    void IShowCursor(bool cursor);

    bool IMouseHold(int key);

protected:
    SDL_Window* m_window;
    SDL_GLContext m_maincontext;

    int m_screenWidth, m_screenHeight;
    std::string m_title;
    bool m_bFPSCounter;
    bool m_bFocus;

    const Uint8 *m_keys;

    Clock m_clock;

    void sdl_die(const char* message);
    void init_screen(const char* title);
};