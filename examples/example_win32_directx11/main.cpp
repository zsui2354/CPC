// Dear ImGui: standalone example application for DirectX 11

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <windows.h>
#include <iostream>
#include "imconfig.h"
#include <cstdint>


//所需环境
#include <tlhelp32.h>
#include <vector>
#include "lib_function.h"
#include "External_API.h"

#include <processthreadsapi.h>

// Data
static ID3D11Device*            g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions     辅助函数的前向声明
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char**)
{
    // Create application window创建应用程序窗口
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowExW(0,wc.lpszClassName, L"王果冻的课堂工具箱", WS_OVERLAPPEDWINDOW, 100, 100, 350, 440, nullptr, nullptr, wc.hInstance, nullptr);
    HWND Console_hwnd = GetConsoleWindow();

    //WS_EX_LAYERED             WS_POPUP                       WS_OVERLAPPEDWINDOW  Dear ImGui DirectX11 Example

    // Initialize Direct3D  初始化 Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context  设置 Dear ImGui 上下文
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd); 
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f); 
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);
       //ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\simfang.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    //ImGuiIO& io = ImGui::GetIO();
    
    

    ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull()); 
           
    bool show_demo_window = false;
    bool show_another_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    External_API mapi;
    bool SHOW_STATUS = true;
    #define HOTKEY_ID 1 
    #define HOTKEY_ID_show 2



    if (!RegisterHotKey(NULL, HOTKEY_ID, MOD_CONTROL, 'F')) {
        std::cerr << "无法注册全局热键！" << std::endl;
    }
    if (!RegisterHotKey(NULL, HOTKEY_ID_show,  MOD_SHIFT  ,'S')) {
        std::cerr << "无法注册全局热键！" << std::endl;
    }
    ::ShowWindow(Console_hwnd, SW_HIDE);


    MessageBox(NULL, L"本程序初衷是为了与控屏程序争夺计算机的控制权，不要滥用它在课堂打游戏，要好好学习哦！！！\n\n 一开始我真没想到这极域这么废，我高中时期的学生端被我搞了之后开发团队后续都更新免杀补丁了,但目前我看 学校的学生端甚至用的是破解版\n 那也就是说后续大概不会有更新的版本支持了，那么这个程序能够一直稳定奔放\n\n\n by.王果冻", L"温馨提示：不要让它成为求学路上的障碍", MB_OK);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;


            if (msg.message == WM_HOTKEY) {
                if (msg.wParam == HOTKEY_ID){       // 检查热键 ID1
                    std::cout << "Global HotKey" << std::endl;
                    mapi.pid = GetPID(L"StudentMain.exe");
                    if (mapi.pid == 0)
                    {
                        mapi.pid = GetPID(L"Student.exe");
                    }
                    mapi.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, mapi.pid);
                    if (!TerminateProcess(mapi.hProcess, 0))
                    {
                        if (DebugActiveProcess(mapi.pid))
                        {
                            exit(0); 
                        }
                    }
                }

                if (msg.wParam == HOTKEY_ID_show) {  // 检查热键 ID2
                    std::cout << "Shift + S 热键被触发！" << std::endl;
                    if (SHOW_STATUS == true)
                    {
                        ::ShowWindow(hwnd, SW_HIDE);
                        SHOW_STATUS = false;
                    }
                    else {
                        ::ShowWindow(hwnd, SW_SHOWDEFAULT);
                        SHOW_STATUS = true;
                    }
                }
            }
        }

        if (done)
            break;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();



        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;



            ImGuiStyle* style = &ImGui::GetStyle();
            style->Colors[ImGuiCol_Button] = ImColor(84, 255, 159, 255);
            style->Colors[ImGuiCol_ButtonActive] = ImColor(255, 0, 0, 255);

            
            style->Colors[ImGuiCol_TitleBg] = ImColor(84, 255, 159, 255);
            style->Colors[ImGuiCol_TitleBgActive] = ImColor(255, 0, 0, 255);
            style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0, 130);

            
            style->Colors[ImGuiCol_Separator] = ImColor(70, 70, 70, 255);
            style->Colors[ImGuiCol_SeparatorActive] = ImColor(76, 76, 76, 255);
            style->Colors[ImGuiCol_SeparatorHovered] = ImColor(76, 76, 76, 255);

            
            style->Colors[ImGuiCol_FrameBg] = ImColor(255, 171, 255, 255); 
            style->Colors[ImGuiCol_FrameBgActive] = ImColor(255, 0, 29, 250);  
            style->Colors[ImGuiCol_FrameBgHovered] = ImColor(66, 124, 250, 157);

            
            style->Colors[ImGuiCol_Header] = ImColor(84, 255, 159, 255);
            style->Colors[ImGuiCol_HeaderActive] = ImColor(84, 255, 159, 104);
            style->Colors[ImGuiCol_HeaderHovered] = ImColor(84, 255, 159, 104);

            
            style->Colors[ImGuiCol_Tab] = ImColor(255, 0, 0, 255);
            style->Colors[ImGuiCol_TabActive] = ImColor(84, 255, 159, 255);
            style->Colors[ImGuiCol_TabHovered] = ImColor(84, 255, 159, 255);
            

            //ImGui::Begin("王果冻的小面板");                          // Create a window called "Hello, world!" and append into it.
            // 

            //ImGui::Text("测试阶段软件");               // Display some text (you can use a format strings too)
            //ImGui::Checkbox("示例窗口", &show_demo_window);      // Edit bools storing our window open/close state
            //ImGui::Checkbox("扩展功能窗口", &show_another_window);

          //  ImGui::GetWindowDrawList()->AddRect({ 100.f,100.f }, { 200.f,200.f }, ImColor(255, 96, 0, 255));   

            //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            //if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)  计数器
            //    counter++;
            //ImGui::SameLine();
            //ImGui::Text("counter = %d", counter);

            //ImGui::Text("平均应用率 %.3f ms/frame       帧数：(%.1f FPS)", 1000.0f / io.Framerate, io.Framerate); 
            //ImGui::End();

            //ImGui::GetWindowDrawList()->AddRect({ 100.f,100.f }, { 200.f,200.f }, ImColor(255, 96, 0, 255)); 
            
            //监控键盘输入 INS 则隐藏程序
            //if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
            //    bool value_cache = show_demo_window;      
            //    show_demo_window = !value_cache;
            //    Sleep(150);
            //    
            //}
            //监控键盘输入 ESC 则退出程序
            //if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            //    break;
            //}
            //扩展功能按钮
            //if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
            //    bool value_cache = show_another_window;
            //    show_another_window = !value_cache;
            //    Sleep(150);
            //}
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("王果冻的课堂工具箱", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar(" ", tab_bar_flags))
            {
                if (ImGui::BeginTabItem("主菜单"))
                {
                    if (ImGui::Button("王果冻的博客网站")) {
                        ShellExecute(0, 0, L"https://zsui2354.github.io", 0, 0, SW_SHOWNORMAL);
                    }
                    if (ImGui::Button("王果冻的SSH聊天室")) {
                        system("start ssh apache.vyantaosheweining.top -p 8080");
                    }
                    if (ImGui::Button("我的空间下载列表")) {
                        ShellExecute(0, 0, L"http://site.vyantaosheweining.top/page/Download.html", 0, 0, SW_SHOWNORMAL);
                    }
                    if (ImGui::Button("我的开放节点列表")) {
                        ShellExecute(0, 0, L"https://zsui2354.github.io/nd-Guodong/", 0, 0, SW_SHOWNORMAL);
                    }
                    if (ImGui::Button("打开Command 终端")) {
                        system("start cd /");
                    }
                    ImGui::Separator();  
                    if (ImGui::Button("极域 学生端（冻结进程）"))
                    {
                        mapi.pid = GetPID(L"StudentMain.exe");
                        mapi.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, mapi.pid);
                        if (mapi.hProcess == NULL)
                        {
                            std::cout << "[ Error ] 获取极域学生端进程句柄失败" << std::endl;
                        }
                        if (DebugActiveProcess(mapi.pid))
                        {
                            MessageBox(NULL, L"[ succeed ] 挂起进程成功", L"提示", MB_OK);
                        }
                    }
                    if (ImGui::Button("极域 学生端（恢复进程）"))
                    {
                        mapi.pid = GetPID(L"StudentMain.exe");
                        mapi.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, mapi.pid);
                        if (mapi.hProcess == NULL)
                        {
                            std::cout << "[ Error ] 获取极域学生端进程句柄失败" << std::endl;
                        }
                        if (DebugActiveProcessStop(mapi.pid))
                        {
                            MessageBox(NULL, L"[ succeed ] 恢复进程成功", L"提示", MB_OK);
                        }
                    }
                    ImGui::Separator(); 
                    if (ImGui::Button("奥易 学生端（冻结进程）"))
                    {
                        mapi.pid = GetPID(L"Student.exe");
                        mapi.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, mapi.pid);
                        if (mapi.hProcess == NULL)
                        {
                            std::cout << "[ Error ] Failed to get the process handle of the AOYI student side" << std::endl;
                        }
                        if (DebugActiveProcess(mapi.pid))
                        {
                            MessageBox(NULL, L"[ succeed ] 挂起进程成功", L"提示", MB_OK);
                        }
                    }
                    if (ImGui::Button("奥易 学生端（恢复进程）"))
                    {
                        mapi.pid = GetPID(L"Student.exe");
                        mapi.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, mapi.pid);
                        if (mapi.hProcess == NULL)
                        {
                            std::cout << "[ Error ] Failed to get the process handle of the AOYI student side" << std::endl;
                        }
                        if (DebugActiveProcessStop(mapi.pid))
                        {
                            MessageBox(NULL, L"[ succeed ] 恢复进程成功", L"提示", MB_OK);
                        }
                    }
                    ImGui::Separator();
                    ImGui::Text("快捷键Ctrl + F   :杀死 学生端进程(对极域，奥易兼容)");
                    ImGui::Text("快捷键Shift + S  :将窗口程序隐藏");

                    ImGui::Text("平均应用率 %.3f ms/frame       帧数：(%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                    ImGui::EndTabItem();
                }
                ImGui::EndTabItem(); 
            }

            if (ImGui::Button("关闭程序")) {
                CloseHandle(mapi.hProcess);
                UnregisterHotKey(NULL, HOTKEY_ID);
                exit(0);
            }
            ImGui::End();
        }


        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w }; 
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr); 
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        Sleep(10); //锁帧数
        g_pSwapChain->Present(1, 0); // Present with vsync 垂直同步
        //g_pSwapChain->Present(0, 0); // Present without vsync 
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
