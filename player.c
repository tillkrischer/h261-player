#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#include "video.h"

int32_t WinMain(HINSTANCE, HINSTANCE, LPSTR, int32_t);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
struct window_dimension GetWindowDimension(HWND);
void paint(HDC, uint32_t, uint32_t, struct offscreen_buffer *);
void initBuffer(struct offscreen_buffer *, uint32_t, uint32_t);
void renderBuffer(struct offscreen_buffer *, struct picture *);

struct offscreen_buffer
{
    void *Memory;
    BITMAPINFO Info;
    uint32_t BytesPerPixel;
    uint32_t Width;
    uint32_t Height;
};

struct window_dimension
{
    uint32_t width;
    uint32_t height;
};

uint8_t running = 1;
struct offscreen_buffer globalBuffer;

int32_t WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int32_t nShowCmd)
{
    MSG msg;
    WNDCLASSW wc = {0};

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszClassName = L"Pixels";
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);

    RegisterClassW(&wc);
    HWND hwnd = CreateWindowW(wc.lpszClassName, L"Pixels", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 400, 300, NULL, NULL, hInstance, NULL);

    initBuffer(&globalBuffer, 352, 288);

    uint32_t lastTick, newTick, framecount;
    newTick = lastTick = 0;
    framecount = 0;

    initTables();

    struct reader r;
    initReader(&r, "akiyo.h261");

    struct picture *pic, *prev;
    pic = (struct picture *)malloc(sizeof(struct picture));
    prev = (struct picture *)malloc(sizeof(struct picture));

    while (running)
    {
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (framecount < 299 && ((newTick = GetTickCount()) - lastTick) * 30 > 1001)
        {
            lastTick = newTick;
            readPicture(&r, pic, prev);
            *prev = *pic;
            framecount++;
        }

        renderBuffer(&globalBuffer, pic);
        HDC hdc = GetDC(hwnd);
        struct window_dimension dim = GetWindowDimension(hwnd);
        paint(hdc, dim.width, dim.height, &globalBuffer);
        ReleaseDC(hwnd, hdc);
    }

    free(pic);
    free(prev);

    close(r.fd);

    return (uint32_t)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    switch (msg)
    {
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        struct window_dimension dim = GetWindowDimension(hwnd);
        paint(hdc, dim.width, dim.height, &globalBuffer);
        EndPaint(hwnd, &ps);
        break;

    case WM_CLOSE:
        running = 0;
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

struct window_dimension GetWindowDimension(HWND hwnd)
{
    struct window_dimension Result;
    RECT ClientRect;
    GetClientRect(hwnd, &ClientRect);
    Result.width = ClientRect.right - ClientRect.left;
    Result.height = ClientRect.bottom - ClientRect.top;
    return Result;
}

void paint(HDC hdc, uint32_t width, uint32_t height, struct offscreen_buffer *Buffer)
{
    StretchDIBits(
        hdc,
        0, 0, width, height,
        // 0, 0, Buffer->Width, Buffer->Height,
        0, 0, Buffer->Width, Buffer->Height,
        Buffer->Memory,
        &Buffer->Info,
        DIB_RGB_COLORS,
        SRCCOPY);
}

void initBuffer(struct offscreen_buffer *Buffer, uint32_t Width, uint32_t Height)
{
    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;

    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = Buffer->BytesPerPixel * 8;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    uint32_t BitmapSize = Buffer->Width * Buffer->Height * Buffer->BytesPerPixel;

    Buffer->Memory = malloc(BitmapSize);
}

void YCbCrToRGB(double Y, double Cb, double Cr, int *R, int *G, int *B)
{
    // Cr = Cr - 128;
    // Cb = Cb - 128;
    // *R = Y + 45 * Cr / 32;
    // *G = Y - (11 * Cb + 23 * Cr) / 32;
    // *B = Y + 113 * Cb / 64;

    *R = (int)(Y + 1.40200 * (Cr - 0x80));
    *G = (int)(Y - 0.34414 * (Cb - 0x80) - 0.71414 * (Cr - 0x80));
    *B = (int)(Y + 1.77200 * (Cb - 0x80));

    *R = MAX(0, MIN(255, *R));
    *G = MAX(0, MIN(255, *G));
    *B = MAX(0, MIN(255, *B));
}

void renderBuffer(struct offscreen_buffer *buffer, struct picture *pic)
{
    uint8_t *m = (uint8_t *)buffer->Memory;

    for (int Y = 0; Y < pic->height; Y++)
    {
        for (int X = 0; X < pic->width; X++)
        {
            uint32_t Red, Green, Blue;
            YCbCrToRGB(pic->Y[Y][X], pic->Cb[Y / 2][X / 2], pic->Cr[Y / 2][X / 2], &Red, &Green, &Blue);
            *((uint32_t *)(m + (Y * buffer->Width + X) * buffer->BytesPerPixel)) = Red << 16 | Green << 8 | Blue;
        }
    }
}