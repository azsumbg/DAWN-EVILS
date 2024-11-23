#include "framework.h"
#include "DAWN EVILS.h"
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include "ErrH.h"
#include "FCheck.h"
#include "D2BMPLOADER.h"
#include "gifresizer.h"
#include "GameAux.h"
#include <chrono>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "errh.lib")
#pragma comment(lib, "fcheck.lib")
#pragma comment(lib, "d2bmploader.lib")
#pragma comment(lib, "gifresizer.lib")
#pragma comment(lib, "gameaux.lib")

constexpr wchar_t bWinClassName[]{ L"MyDawnAdventure" };
constexpr char tmp_file[]{ ".\\res\\data\\temp.dat" };
constexpr wchar_t Ltmp_file[]{ L".\\res\\data\\temp.dat" };
constexpr wchar_t save_file[]{ L".\\res\\data\\save.dat" };
constexpr wchar_t record_file[]{ L".\\res\\data\\record.dat" };
constexpr wchar_t help_file[]{ L".\\res\\data\\help.dat" };
constexpr wchar_t sound_file[]{ L".\\res\\snd\\sound.wav" };

constexpr int mNew{ 1001 };
constexpr int mLvl{ 1002 };
constexpr int mExit{ 1003 };
constexpr int mLoad{ 1004 };
constexpr int mSave{ 1005 };
constexpr int mHoF{ 1006 };

constexpr int record{ 2001 };
constexpr int first_record{ 2002 };
constexpr int rno_ecord{ 2003 };

WNDCLASS bWin{};
HINSTANCE bIns = nullptr;
HWND bHwnd{ nullptr };
HICON Icon = nullptr;
HCURSOR mainCur = nullptr;
HCURSOR outCur = nullptr;
HMENU bBar = nullptr;
HMENU bMain = nullptr;
HMENU bStore = nullptr;
HDC PaintDC = nullptr;
PAINTSTRUCT bPaint{};

MSG bMsg{};
UINT bRet = 0;

POINT curPos{};
UINT bTimer = -1;

wchar_t current_player[16] = L"ONE CAPTAIN";

D2D1_RECT_F b1Rect{ 20.0f, 0, scr_width / 3 - 50.0f, 50.0f };
D2D1_RECT_F b2Rect{ scr_width / 3 + 20.0f, 0, scr_width * 2 / 3 - 50.0f, 50.0f };
D2D1_RECT_F b3Rect{ scr_width * 2 / 3 + 20.0f, 0, scr_width, 50.0f };

dll::RANDOMIZER RandGenerator{};

bool pause = false;
bool sound = true;
bool name_set = false;
bool in_client = true;
bool show_help = false;
bool b1Hglt = false;
bool b2Hglt = false;
bool b3Hglt = false;

int level = 1;
int score = 0;
int mins = 0;
int secs = 290;

//////////////////////////////////////////

ID2D1Factory* iFactory{ nullptr };
ID2D1HwndRenderTarget* Draw{ nullptr };
ID2D1RadialGradientBrush* bckgBrush{ nullptr };
ID2D1SolidColorBrush* txtBrush{ nullptr };
ID2D1SolidColorBrush* inactBrush{ nullptr };
ID2D1SolidColorBrush* hgltBrush{ nullptr };

IDWriteFactory* iWriteFactory{ nullptr };
IDWriteTextFormat* nrmTextFormat{ nullptr };
IDWriteTextFormat* midTextFormat{ nullptr };
IDWriteTextFormat* bigTextFormat{ nullptr };

ID2D1Bitmap* bmpField{ nullptr };
ID2D1Bitmap* bmpTree1{ nullptr };
ID2D1Bitmap* bmpTree2{ nullptr };
ID2D1Bitmap* bmpTree3{ nullptr };
ID2D1Bitmap* bmpRIP{ nullptr };

ID2D1Bitmap* bmpHeroL[9]{ nullptr };
ID2D1Bitmap* bmpHeroR[9]{ nullptr };

ID2D1Bitmap* bmpZombie1L[3]{ nullptr };
ID2D1Bitmap* bmpZombie1R[3]{ nullptr };

ID2D1Bitmap* bmpZombie2L[12]{ nullptr };
ID2D1Bitmap* bmpZombie2R[12]{ nullptr };

ID2D1Bitmap* bmpZombie3L[8]{ nullptr };
ID2D1Bitmap* bmpZombie3R[8]{ nullptr };

ID2D1Bitmap* bmpIntro[75]{ nullptr };
ID2D1Bitmap* bmpShot[33]{ nullptr };

//////////////////////////////////////////////////

// GAME VARS ************************************

dll::creature_ptr Hero{ nullptr };
std::vector<dll::creature_ptr>vZombies;
std::vector<dll::creature_ptr>vBullets;
std::vector<dll::PROTON>vTrees;

bool stop_hero = false;

///////////////////////////////////////////////////
template<typename T>concept InHeap = requires(T check_var)
{
    check_var.Release();
};
template<InHeap T> bool ClearHeap(T** var)
{
    if (*var)
    {
        (*var)->Release();
        (*var) = nullptr;
        return true;
    }
    return false;
}
void LogError(LPCWSTR what)
{
    std::wofstream log(L".\\res\\data\\error.log", std::ios::app);
    log << what << L" Time stamp: " << std::chrono::system_clock::now() << std::endl;
    log.close();
}
void ClearResources()
{
    if (!ClearHeap(&iFactory))LogError(L"Error releasing iFactory !");
    if (!ClearHeap(&Draw))LogError(L"Error releasing Draw !");
    if (!ClearHeap(&bckgBrush))LogError(L"Error releasing bckgBrush !");
    if (!ClearHeap(&txtBrush))LogError(L"Error releasing txtBrush !");
    if (!ClearHeap(&inactBrush))LogError(L"Error releasing inactBrush !");

    if (!ClearHeap(&iWriteFactory))LogError(L"Error releasing iWriteFactory !");
    if (!ClearHeap(&nrmTextFormat))LogError(L"Error releasing nrmTextFormat !");
    if (!ClearHeap(&midTextFormat))LogError(L"Error releasing midTextFormat !");
    if (!ClearHeap(&bigTextFormat))LogError(L"Error releasing bigTextFormat !");

    if (!ClearHeap(&bmpField))LogError(L"Error releasing bmpField !");
    if (!ClearHeap(&bmpTree1))LogError(L"Error releasing bmpTree1 !");
    if (!ClearHeap(&bmpTree2))LogError(L"Error releasing bmpTree2 !");
    if (!ClearHeap(&bmpTree3))LogError(L"Error releasing bmpTree3 !");
    if (!ClearHeap(&bmpRIP))LogError(L"Error releasing bmpRIP !");

    for (int i = 0; i < 9; i++)if (!ClearHeap(&bmpHeroL[i]))LogError(L"Error releasing bmpHeroL !");
    for (int i = 0; i < 9; i++)if (!ClearHeap(&bmpHeroR[i]))LogError(L"Error releasing bmpHeroR !");

    for (int i = 0; i < 3; i++)if (!ClearHeap(&bmpZombie1L[i]))LogError(L"Error releasing bmpZombie1L !");
    for (int i = 0; i < 3; i++)if (!ClearHeap(&bmpZombie1R[i]))LogError(L"Error releasing bmpZombie1R !");

    for (int i = 0; i < 12; i++)if (!ClearHeap(&bmpZombie2L[i]))LogError(L"Error releasing bmpZombie2L !");
    for (int i = 0; i < 12; i++)if (!ClearHeap(&bmpZombie2R[i]))LogError(L"Error releasing bmpZombie2R !");

    for (int i = 0; i < 8; i++)if (!ClearHeap(&bmpZombie3L[i]))LogError(L"Error releasing bmpZombie3L !");
    for (int i = 0; i < 8; i++)if (!ClearHeap(&bmpZombie3R[i]))LogError(L"Error releasing bmpZombie3R !");

    for (int i = 0; i < 75; i++)if (!ClearHeap(&bmpIntro[i]))LogError(L"Error releasing bmpIntro !");
    for (int i = 0; i < 33; i++)if (!ClearHeap(&bmpShot[i]))LogError(L"Error releasing bmpShot !");
}
void ErrExit(int what)
{
    MessageBeep(MB_ICONERROR);
    MessageBox(NULL, ErrHandle(what), L"КРИТИЧНА ГРЕШКА !", MB_OK | MB_APPLMODAL | MB_ICONERROR);

    ClearResources();
    std::remove(tmp_file);
    exit(1);
}

void GameOver()
{
    PlaySound(NULL, NULL, NULL);
    KillTimer(bHwnd, bTimer);


    bMsg.message = WM_QUIT;
    bMsg.wParam = 0;
}
void InitGame()
{
    level = 1;
    score = 0;
    secs = 290 + level * 10;
    wcscpy_s(current_player, L"ONE CAPTAIN");
    name_set = false;

    ClearHeap(&Hero);
    Hero = dll::CreatureFactory(hero_flag, (float)(RandGenerator(0, (int)(scr_width - 100.0f))), (int)(ground - 80.0f));
    if (!vZombies.empty())
        for (int i = 0; i < vZombies.size(); i++)ClearHeap(&vZombies[i]);
    vZombies.clear();
    if (!vBullets.empty())
        for (int i = 0; i < vBullets.size(); i++)ClearHeap(&vBullets[i]);
    vBullets.clear();
   
    vTrees.clear();

    for (float rows = 100.0f; rows < ground - 100.0f; rows += 100.0f)
    {
        for (float cols = 50.0f; cols < scr_width - 100.0f; cols += 100.0f)
        {
            if (RandGenerator(0, 4) == 1)
            {
                vTrees.push_back(dll::PROTON(cols, rows));
                switch (RandGenerator(0, 2))
                {
                case 0:
                    vTrees.back().SetFlag(tree1_flag);
                    vTrees.back().NewDims(47.0f, 80.0f);
                    break;

                case 1:
                    vTrees.back().SetFlag(tree2_flag);
                    vTrees.back().NewDims(78.0f, 86.0f);
                    break;

                case 3:
                    vTrees.back().SetFlag(tree3_flag);
                    vTrees.back().NewDims(98.0f, 90.0f);
                    break;
                }
            }
        }
    }
}

INT_PTR CALLBACK bDlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_INITDIALOG:
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)(Icon));
        return true;

    case WM_CLOSE:
        EndDialog(hwnd, IDCANCEL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            break;

        case IDOK:
            if (GetDlgItemText(hwnd, IDC_NAME, current_player, 16 < 1))
            {
                wcscpy_s(current_player, L"ONE CAPTAIN");
                if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
                MessageBox(bHwnd, L"Името си ли забрави ?", L"Забраватор !", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
                EndDialog(hwnd, IDCANCEL);
                break;
            }

            EndDialog(hwnd, IDCANCEL);
        }
        break;
    }

    return (INT_PTR)(FALSE);
}
LRESULT CALLBACK bWinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_CREATE:
        SetTimer(hwnd, bTimer, 1000, NULL);
        bBar = CreateMenu();
        bMain = CreateMenu();
        bStore = CreateMenu();
        AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
        AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Меню за данни");
        AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
        AppendMenu(bMain, MF_STRING, mLvl, L"Следващо ниво");
        AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bMain, MF_STRING, mExit, L"Изход");
        AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
        AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
        AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");
        SetMenu(hwnd, bBar);
        InitGame();
        break;

    case WM_CLOSE:
        pause = true;
        if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
        if (MessageBox(hwnd, L"Ако излезеш, губиш тази игра !\n\n Наистина ли излизаш ?",
            L"Изход !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
        {
            pause = false;
            break;
        }
        GameOver();
        break;

    case WM_PAINT:
        PaintDC = BeginPaint(hwnd, &bPaint);
        FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(100, 100, 100)));
        EndPaint(hwnd, &bPaint);
        break;

    case WM_TIMER:
        if (pause)break;
        --secs;
        mins = secs / 60;
        break;

    case WM_SETCURSOR:
        GetCursorPos(&curPos);
        ScreenToClient(hwnd, &curPos);
        if (LOWORD(lParam) == HTCLIENT)
        {
            if (!in_client)
            {
                in_client = true;
                pause = false;
            }

            if (curPos.y <= 50)
            {
                if (curPos.x >= b1Rect.left && curPos.x <= b1Rect.right)
                {
                    if (!b1Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = true;
                        b2Hglt = false;
                        b3Hglt = false;
                    }
                }
                if (curPos.x >= b2Rect.left && curPos.x <= b2Rect.right)
                {
                    if (!b2Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = false;
                        b2Hglt = true;
                        b3Hglt = false;
                    }
                }
                if (curPos.x >= b3Rect.left && curPos.x <= b3Rect.right)
                {
                    if (!b3Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = false;
                        b2Hglt = false;
                        b3Hglt = true;
                    }
                }
                SetCursor(outCur);
                return true;
            }

            if (b1Hglt || b2Hglt || b3Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b1Hglt = false;
                b2Hglt = false;
                b3Hglt = false;
            }
            SetCursor(mainCur);
            return true;
        }
        else
        {
            if (in_client)
            {
                in_client = false;
                pause = true;
            }
            if (b1Hglt || b2Hglt || b3Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b1Hglt = false;
                b2Hglt = false;
                b3Hglt = false;
            }
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return true;
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case mNew:
            pause = true;
            if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
            if (MessageBox(hwnd, L"Ако рестартираш, губиш тази игра !\n\n Наистина ли рестартираш ?",
                L"Рестарт !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            InitGame();
            break;


        case mExit:
            SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            break;



        }
        break;

    case WM_LBUTTONDOWN:
        if (HIWORD(lParam <= 50))
        {

        }
        else
        {
            stop_hero = false;
            if (Hero)Hero->Move(true, LOWORD(lParam), HIWORD(lParam));
        }
        break;

    case WM_RBUTTONDOWN:
        stop_hero = true;
        break;



    default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
    }

    return (LRESULT)(FALSE);
}

void CreateResources()
{
    int result = 0;
    CheckFile(Ltmp_file, &result);
    if (result == FILE_EXIST)ErrExit(eStarted);
    else
    {
        std::wofstream started(Ltmp_file);
        started << L"Game started at:" << std::chrono::system_clock::now();
        started.close();
    }

    int win_x = (int)(GetSystemMetrics(SM_CXSCREEN) / 2 - (int)(scr_width / 2));
    if (win_x + scr_width > GetSystemMetrics(SM_CXSCREEN) || GetSystemMetrics(SM_CYSCREEN) < scr_height + 10)ErrExit(eScreen);

    Icon = (HICON)(LoadImage(NULL, L".\\res\\main.ico", IMAGE_ICON, 255, 255, LR_LOADFROMFILE));
    if (!Icon)ErrExit(eIcon);
    mainCur = LoadCursorFromFileW(L".\\res\\main.ani");
    outCur = LoadCursorFromFileW(L".\\res\\out.ani");
    if (!mainCur || !outCur)ErrExit(eCursor);

    bWin.lpszClassName = bWinClassName;
    bWin.hInstance = bIns;
    bWin.lpfnWndProc = &bWinProc;
    bWin.hbrBackground = CreateSolidBrush(RGB(100, 100, 100));
    bWin.hIcon = Icon;
    bWin.hCursor = mainCur;
    bWin.style = CS_DROPSHADOW;

    if(!RegisterClass(&bWin))ErrExit(eClass);

    bHwnd = CreateWindow(bWinClassName, L"ОЦЕЛЯВАНЕ НА ЗДРАЧАВАНЕ !", WS_CAPTION | WS_SYSMENU, win_x, 10, (int)(scr_width), 
        (int)(scr_height), NULL, NULL, bIns, NULL);
    if (!bHwnd)ErrExit(eWindow);
    else
    {
        ShowWindow(bHwnd, SW_SHOWDEFAULT);

        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
        if (hr != S_OK)
        {
            LogError(L"Error creating D2D1 Factory !");
            ErrExit(eD2D);
        }

        if (iFactory)
        {
            hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(bHwnd,
                D2D1::SizeU((UINT32)(scr_width), (UINT32)(scr_height))), &Draw);
            if (hr != S_OK)
            {
                LogError(L"Error creating D2D1 HwndRenderTarget !");
                ErrExit(eD2D);
            }

            if (Draw)
            {
                D2D1_GRADIENT_STOP gStops[2]{};
                ID2D1GradientStopCollection* GSCol = nullptr;

                gStops[0].position = 0;
                gStops[0].color = D2D1::ColorF(D2D1::ColorF::DarkBlue);
                gStops[1].position = 1.0f;
                gStops[1].color = D2D1::ColorF(D2D1::ColorF::AliceBlue);

                hr = Draw->CreateGradientStopCollection(gStops, 2, &GSCol);
                if (hr != S_OK)
                {
                    LogError(L"Error creating D2D1 GradientStopCollection !");
                    ErrExit(eD2D);
                }

                if (GSCol)
                {
                    hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(scr_width / 2, 25.0f),
                        D2D1::Point2F(0, 0), scr_width / 2, 25.0f), GSCol, &bckgBrush);
                    if (hr != S_OK)
                    {
                        LogError(L"Error creating D2D1 RadialGradientBrush !");
                        ErrExit(eD2D);
                    }
                    ClearHeap(&GSCol);
                }
                
                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightSeaGreen), &txtBrush);
                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &hgltBrush);
                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::SlateGray), &inactBrush);

                if (hr != S_OK)
                {
                    LogError(L"Error creating D2D1 Text Brushes !");
                    ErrExit(eD2D);
                }

                bmpField = Load(L".\\res\\img\\field.png", Draw);
                if (!bmpField)
                {
                    LogError(L"Error loading bmpField !");
                    ErrExit(eD2D);
                }
                bmpTree1 = Load(L".\\res\\img\\tree1.png", Draw);
                if (!bmpTree1)
                {
                    LogError(L"Error loading bmpTree1 !");
                    ErrExit(eD2D);
                }
                bmpTree2 = Load(L".\\res\\img\\tree2.png", Draw);
                if (!bmpTree2)
                {
                    LogError(L"Error loading bmpTree2 !");
                    ErrExit(eD2D);
                }
                bmpTree3 = Load(L".\\res\\img\\tree3.png", Draw);
                if (!bmpTree3)
                {
                    LogError(L"Error loading bmpTree3 !");
                    ErrExit(eD2D);
                }
                bmpRIP = Load(L".\\res\\img\\rip.png", Draw);
                if (!bmpRIP)
                {
                    LogError(L"Error loading bmpRIP !");
                    ErrExit(eD2D);
                }

                for (int i = 0; i < 9; i++)
                {
                    wchar_t name[75] = L".\\res\\img\\hero_l\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpHeroL[i] = Load(name, Draw);
                    if (!bmpHeroL[i])
                    {
                        LogError(L"Error loading bmpHeroL !");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 9; i++)
                {
                    wchar_t name[75] = L".\\res\\img\\hero_r\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpHeroR[i] = Load(name, Draw);
                    if (!bmpHeroR[i])
                    {
                        LogError(L"Error loading bmpHeroR !");
                        ErrExit(eD2D);
                    }
                }

                for (int i = 0; i < 3; i++)
                {
                    wchar_t name[75] = L".\\res\\img\\zombie1_l\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpZombie1L[i] = Load(name, Draw);
                    if (!bmpZombie1L[i])
                    {
                        LogError(L"Error loading bmpZombie1L !");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 3; i++)
                {
                    wchar_t name[75] = L".\\res\\img\\zombie1_r\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpZombie1R[i] = Load(name, Draw);
                    if (!bmpZombie1R[i])
                    {
                        LogError(L"Error loading bmpZombie1R !");
                        ErrExit(eD2D);
                    }
                }

                for (int i = 0; i < 12; i++)
                {
                    wchar_t name[75] = L".\\res\\img\\zombie2_l\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpZombie2L[i] = Load(name, Draw);
                    if (!bmpZombie2L[i])
                    {
                        LogError(L"Error loading bmpZombie2L !");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 12; i++)
                {
                    wchar_t name[75] = L".\\res\\img\\zombie2_r\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpZombie2R[i] = Load(name, Draw);
                    if (!bmpZombie2R[i])
                    {
                        LogError(L"Error loading bmpZombie2R !");
                        ErrExit(eD2D);
                    }
                }

                for (int i = 0; i < 8; i++)
                {
                    wchar_t name[75] = L".\\res\\img\\zombie3_l\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpZombie3L[i] = Load(name, Draw);
                    if (!bmpZombie3L[i])
                    {
                        LogError(L"Error loading bmpZombie3L !");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 8; i++)
                {
                    wchar_t name[75] = L".\\res\\img\\zombie3_r\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpZombie3R[i] = Load(name, Draw);
                    if (!bmpZombie3R[i])
                    {
                        LogError(L"Error loading bmpZombie3R !");
                        ErrExit(eD2D);
                    }
                }
                
                for (int i = 0; i < 75; i++)
                {
                    wchar_t name[75] = L".\\res\\img\\intro\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpIntro[i] = Load(name, Draw);
                    if (!bmpIntro[i])
                    {
                        LogError(L"Error loading bmpIntro !");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 33; i++)
                {
                    wchar_t name[75] = L".\\res\\img\\shot\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpShot[i] = Load(name, Draw);
                    if (!bmpShot[i])
                    {
                        LogError(L"Error loading bmpShot !");
                        ErrExit(eD2D);
                    }
                }
            }
        }

        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&iWriteFactory));
        if (hr != S_OK)
        {
            LogError(L"Error creating D2D1 Write Factory !");
            ErrExit(eD2D);
        }

        if (iWriteFactory)
        {
            hr = iWriteFactory->CreateTextFormat(L"SEGOE", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_OBLIQUE,
                DWRITE_FONT_STRETCH_NORMAL, 24, L"", &nrmTextFormat);
            hr = iWriteFactory->CreateTextFormat(L"SEGOE", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_OBLIQUE,
                DWRITE_FONT_STRETCH_NORMAL, 36, L"", &midTextFormat);
            hr = iWriteFactory->CreateTextFormat(L"SEGOE", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_OBLIQUE,
                DWRITE_FONT_STRETCH_NORMAL, 72, L"", &bigTextFormat);

            if (hr != S_OK)
            {
                LogError(L"Error creating D2D1 Write Text Formats !");
                ErrExit(eD2D);
            }
        }

    }

    if (Draw && bigTextFormat && txtBrush)
    {
        int intro_frame = 0;

        for (int i = 0; i < 80; i++)
        {
            Draw->BeginDraw();
            Draw->DrawBitmap(bmpIntro[intro_frame], D2D1::RectF(0, 0, scr_width, scr_height));
            ++intro_frame;
            if (intro_frame > 74)intro_frame = 0;
            if (RandGenerator(0, 3) == 1)
            {
                Draw->DrawTextW(L"ЗОМБИТА В АТАКА !\n\ndev. Daniel !", 33, bigTextFormat, D2D1::RectF(100.0f, 150.0f,
                    scr_width, scr_height), txtBrush);
                mciSendString(L"play .\\res\\snd\\buzz.wav", NULL, NULL, NULL);
                Sleep(150);
            }
            Draw->EndDraw();
        }

        Draw->BeginDraw();
        Draw->DrawBitmap(bmpIntro[intro_frame], D2D1::RectF(0, 0, scr_width, scr_height));
        Draw->DrawTextW(L"ЗОМБИТА В АТАКА !\n\ndev. Daniel !", 33, bigTextFormat, D2D1::RectF(100.0f, 150.0f,
            scr_width, scr_height), txtBrush);
        Draw->EndDraw();
        PlaySound(L".\\res\\snd\\boom.wav", NULL, SND_SYNC);
    }
}

///////////////////////////////////////////////////

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    bIns = hInstance;
    if (!bIns)
    {
        LogError(L"Error in hInstnace !");
        ErrExit(eClass);
    }

    CreateResources();

    while (bMsg.message != WM_QUIT)
    {
        if ((bRet = PeekMessage(&bMsg, bHwnd, NULL, NULL, PM_REMOVE)) != 0)
        {
            if (bRet == -1)ErrExit(eMsg);
            TranslateMessage(&bMsg);
            DispatchMessage(&bMsg);
        }
        if (pause)
        {
            if (show_help) continue;
            if (Draw && bigTextFormat && txtBrush)
            {
                Draw->BeginDraw();
                Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkSlateBlue));
                Draw->DrawTextW(L"ПАУЗА !", 6, bigTextFormat, D2D1::RectF(scr_width / 2 - 100.0f, scr_height / 2 - 80.0f,
                    scr_width, scr_height), txtBrush);
                Draw->EndDraw();
            }
            continue;
        }
        //////////////////////////////////////////////////////

        // GAME ENGINE **************************************

        if (vZombies.size() < 15 + level)
        {
            if (RandGenerator(0, 50) - level == 6)
            {
                int type = RandGenerator(0, 2);

                switch (type)
                {
                case 0:
                    vZombies.push_back(dll::CreatureFactory(zombie1_flag, scr_width, (float)(RandGenerator(50, int(ground - 50)))));
                    break;

                case 1:
                    vZombies.push_back(dll::CreatureFactory(zombie2_flag, scr_width, (float)(RandGenerator(50, int(ground - 50)))));
                    break;

                case 3:
                    vZombies.push_back(dll::CreatureFactory(zombie1_flag, scr_width, (float)(RandGenerator(50, int(ground - 50)))));
                    break;
                }
            }
        }

        if (!vZombies.empty() && Hero)
        {
            for (std::vector<dll::creature_ptr>::iterator evil = vZombies.begin(); evil < vZombies.end(); evil++)
            {
                (*evil)->Move(true, Hero->x, Hero->y);
            }
        }

        if (Hero)
        {
            if (!stop_hero)
            {
                bool obstacle_hit = false;
                if (!vTrees.empty())
                {
                    for (int i = 0; i < vTrees.size(); ++i)
                    {
                        if (!(Hero->x >= vTrees[i].ex || Hero->ex <= vTrees[i].x
                            || Hero->y >= vTrees[i].ey || Hero->ey <= vTrees[i].y))
                        {
                            Hero->ObstacleDetour(vTrees[i]);
                            obstacle_hit = true;
                            break;
                        }
                    }
                }

                if (!obstacle_hit) Hero->Move(false);
            }

            int bull_strenght = Hero->Attack();
            if (bull_strenght > 0 && !vZombies.empty())
            {
                dll::PROT_CONTAINER Targets(vZombies.size());
                for (int i = 0; i < vZombies.size(); ++i)
                {
                    dll::PROTON Dummy{ vZombies[i]->x, vZombies[i]->y };
                    Targets.push_back(Dummy);
                }
                dll::PROTON HeroPosition{ Hero->x,Hero->y };
                Targets.distance_sort(HeroPosition);

                vBullets.push_back(dll::CreatureFactory(bullet_flag, Hero->ex, Hero->y + 20.0f, Targets[0].x, Targets[0].y));
            }
        }

        if (!vBullets.empty())
        {
            for (std::vector<dll::creature_ptr>::iterator bullet = vBullets.begin(); bullet < vBullets.end(); ++bullet)
            {
                (*bullet)->Move(false);
                if ((*bullet)->x <= 0 || (*bullet)->ex >= scr_width || (*bullet)->y <= sky || (*bullet)->ey >= ground)
                {
                    (*bullet)->Release();
                    vBullets.erase(bullet);
                    break;
                }
            }
        }

        if (!vTrees.empty() && !vZombies.empty())
        {
            for (std::vector<dll::creature_ptr>::iterator zombie = vZombies.begin(); zombie < vZombies.end(); zombie++)
            {
                for (int i = 0; i < vTrees.size(); i++)
                {
                    if (!((*zombie)->x >= vTrees[i].ex || (*zombie)->ex <= vTrees[i].x
                        || (*zombie)->y >= vTrees[i].ey || (*zombie)->ey <= vTrees[i].y))
                        (*zombie)->ObstacleDetour(vTrees[i]);
                }
            }
        }

        // DRAW THINGS *********************************************

        if (Draw && nrmTextFormat && txtBrush && inactBrush && hgltBrush && bckgBrush)
        {
            Draw->BeginDraw();
            Draw->FillRectangle(D2D1::RectF(0, 0, scr_width, 50.0f), bckgBrush);
            if (name_set)
                Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmTextFormat, b1Rect, inactBrush);
            else
            {
                if (b1Hglt)
                    Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmTextFormat, b1Rect, hgltBrush);
                else
                    Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmTextFormat, b1Rect, txtBrush);
            }
            if (b2Hglt)
                Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmTextFormat, b2Rect, hgltBrush);
            else
                Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmTextFormat, b2Rect, txtBrush);
            if (b3Hglt)
                Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmTextFormat, b3Rect, hgltBrush);
            else
                Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmTextFormat, b3Rect, txtBrush);
        }

        Draw->DrawBitmap(bmpField, D2D1::RectF(0, 50.0f, scr_width, scr_height));
        
        if (Hero)
        {
            switch (Hero->dir)
            {
            case dirs::left:
                Draw->DrawBitmap(bmpHeroL[Hero->GetFrame()], Resizer(bmpHeroL[Hero->GetFrame()], Hero->x, Hero->y));
                break;

            case dirs::right:
                Draw->DrawBitmap(bmpHeroR[Hero->GetFrame()], Resizer(bmpHeroR[Hero->GetFrame()], Hero->x, Hero->y));
                break;
            }
        }

        if (!vZombies.empty() && Hero)
        {
            for (std::vector<dll::creature_ptr>::iterator evil = vZombies.begin(); evil < vZombies.end(); evil++)
            {
                if ((*evil)->GetFlag(zombie1_flag))
                {
                    if ((*evil)->x >= Hero->ex)
                        Draw->DrawBitmap(bmpZombie1L[(*evil)->GetFrame()], Resizer(bmpZombie1L[(*evil)->GetFrame()],
                            (*evil)->x, (*evil)->y));
                    else
                        Draw->DrawBitmap(bmpZombie1R[(*evil)->GetFrame()], Resizer(bmpZombie1R[(*evil)->GetFrame()],
                            (*evil)->x, (*evil)->y));
                }
                if ((*evil)->GetFlag(zombie2_flag))
                {
                    if ((*evil)->x >= Hero->ex)
                        Draw->DrawBitmap(bmpZombie2L[(*evil)->GetFrame()], Resizer(bmpZombie2L[(*evil)->GetFrame()],
                            (*evil)->x, (*evil)->y));
                    else
                        Draw->DrawBitmap(bmpZombie2R[(*evil)->GetFrame()], Resizer(bmpZombie2R[(*evil)->GetFrame()],
                            (*evil)->x, (*evil)->y));
                }
                if ((*evil)->GetFlag(zombie3_flag))
                {
                    if ((*evil)->x >= Hero->ex)
                        Draw->DrawBitmap(bmpZombie3L[(*evil)->GetFrame()], Resizer(bmpZombie3L[(*evil)->GetFrame()],
                            (*evil)->x, (*evil)->y));
                    else
                        Draw->DrawBitmap(bmpZombie3R[(*evil)->GetFrame()], Resizer(bmpZombie3R[(*evil)->GetFrame()],
                            (*evil)->x, (*evil)->y));
                }
            }
        }

        if (!vBullets.empty())
        {
            for (int i = 0; i < vBullets.size(); ++i)
            {
                int one_frame = vBullets[i]->GetFrame();
                Draw->DrawBitmap(bmpShot[one_frame], Resizer(bmpShot[one_frame], vBullets[i]->x, vBullets[i]->y));
            }
        }

        if (!vTrees.empty())
        {
            for (int i = 0; i < vTrees.size(); i++)
            {
                if (vTrees[i].GetFlag(tree1_flag))
                    Draw->DrawBitmap(bmpTree1, D2D1::RectF(vTrees[i].x, vTrees[i].y, vTrees[i].ex, vTrees[i].ey));
                else if (vTrees[i].GetFlag(tree2_flag))
                    Draw->DrawBitmap(bmpTree2, D2D1::RectF(vTrees[i].x, vTrees[i].y, vTrees[i].ex, vTrees[i].ey));
                else
                    Draw->DrawBitmap(bmpTree3, D2D1::RectF(vTrees[i].x, vTrees[i].y, vTrees[i].ex, vTrees[i].ey));
            }
        }

        //////////////////////////////////////////////////////////

        Draw->EndDraw();
    }

    ClearResources();
    std::remove(tmp_file);
    return (int) bMsg.wParam;
}