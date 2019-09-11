#include <windows.h>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cstring>

struct point
{
public:
    short x;
    short y;
};

const int w = 15;
const int h = 30;

const point obj[7][4] =
{
    {{0,0},{1,0},{-1,0},{0,1}},
    {{0,0},{1,0},{1,1},{0,1}},
    {{0,-1},{0,0},{0,1},{0,2}},
    {{0,0},{-1,0},{0,1},{1,1}},
    {{0,0},{1,0},{0,1},{-1,1}},
    {{0,0},{-1,0},{0,-1},{0,-2}},
    {{0,0},{1,0},{0,-1},{0,-2}}
};

const COLORREF clrTable[6] = {RGB(90,90,90),RGB(0,255,0),RGB(255,0,0),RGB(0,0,255),RGB(255,255,0),RGB(0,255,255)};

point cur[4];

WORD screenW,screenH;
RECT workSpace,ClnArea;
int rectsize;

point fieldCords[w][h];
BYTE field[w][h] = {};
BYTE figNum = 0;
BYTE curClr = 1;
int points = 0, maxpoints;

HWND hWnd;

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);


namespace Drawing
{
void resizeField();
void drawRect(HDC hdc,short x, short y);
void drawEl(HDC hdc,WORD x, WORD y);
void setZone(RECT*,short,short,short,short);
void setZonePos(RECT*, short,short);
void fillrct(HDC hdc,short x, short y,HBRUSH hb);
}


namespace Controls
{
void MoveDown();
void MoveLeft();
void MoveRight();
void rotateF();
}

void newFig();
void Copy(point*,point*);
void UpdateScore();
void NewGame();
void read();
void write();
void formOutPut(char*);

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wc = {};

    wc.cbSize=sizeof(wc);
    wc.hInstance=hThisInstance;
    wc.lpszClassName="MainWindow";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadCursor(NULL, IDC_NO);
    wc.lpfnWndProc=WindowProcedure;
    wc.hbrBackground  = (HBRUSH)(COLOR_WINDOWTEXT);

    if(!RegisterClassEx(&wc))
        MessageBox(NULL,"Can't register window class","Error",MB_OK | MB_ICONERROR);

    hwnd = CreateWindowEx(0,
                          "MainWindow",
                          "Tetris",
                          WS_OVERLAPPEDWINDOW,
                          0,0,500,500,NULL,NULL,hThisInstance,NULL);

    if(!hwnd)
        MessageBox(NULL,"Can't create window","Error",MB_OK | MB_ICONERROR);

    hWnd = hwnd;
    ShowWindow(hwnd,nCmdShow);
    UpdateWindow(hwnd);

    while(GetMessage(&msg,hwnd,NULL,NULL))
    {
        DispatchMessage(&msg);
    }

    return msg.message;
}

short xc,yc,hill = 0;
point last[4];
short pensize = 1;

bool pressedUP=false;
bool pressedLEFT=false;
bool pressedRIGHT=false;
bool pressedDOWN=false;
bool fullscreen = false;
bool stars = false;

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_ERASEBKGND:
    {
        HDC hdc = GetDC(hwnd);
        FillRect(hdc,&ClnArea,(HBRUSH)COLOR_CAPTIONTEXT);
        HBRUSH brush = CreateSolidBrush(clrTable[0]);
        FillRect(hdc,&workSpace,brush);
        HGDIOBJ orig = SelectObject(hdc,brush);
        for(int i=0; i<h; i++)
            for(int j=0; j<w; j++)
                if(field[j][i])
                {
                    brush = CreateSolidBrush(clrTable[field[j][i]]);
                    DeleteObject(SelectObject(hdc,brush));
                    Drawing::drawRect(hdc,j,i);
                }
        DeleteObject(SelectObject(hdc,orig));
        SwapBuffers(hdc);
        ReleaseDC(hwnd,hdc);
        return 0;
        break;
    }

    case WM_CREATE:
    {
        newFig();
        read();
        SetTimer(hwnd,1,200,NULL);
        SetTimer(hwnd,2,50,NULL);
        return 0;
        break;
    }
    case WM_SIZE:
    {
        screenW = LOWORD(lParam);
        screenH = HIWORD(lParam);

        if(screenW/w*h>screenH)
            rectsize = screenH/h;
        else
            rectsize = screenW/w;

        pensize = rectsize/10;

        SetRect(&ClnArea,0,0,screenW,screenH);

        Drawing::resizeField();
        return 0;
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd,&ps);

        HBRUSH brush = CreateSolidBrush(clrTable[0]);
        HGDIOBJ orig = SelectObject(hdc,brush);
        for(int i=0; i<4; i++)
            Drawing::fillrct(hdc,last[i].x,last[i].y,brush);

        DeleteObject(SelectObject(hdc,orig));

        Drawing::drawEl(hdc,xc,yc);

        char buff[50] = "Score: ";
        formOutPut(buff);
        DrawText(hdc,buff,-1,&ClnArea,0);

        SwapBuffers(hdc);
        EndPaint(hwnd,&ps);

        for(int i=0; i<4; i++)
        {
            last[i].x = cur[i].x+xc;
            last[i].y = cur[i].y+yc;
        }

        return 0;
        break;
    }
    case WM_KEYUP:
    {
        switch(wParam)
        {
        case VK_UP:
            pressedUP=false;
            break;
        case VK_DOWN:
            pressedDOWN=false;
            break;
        case VK_LEFT:
            pressedLEFT=false;
            break;
        case VK_RIGHT:
            pressedRIGHT=false;
            break;
        }
        return 0;
        break;
    }
    case WM_KEYDOWN:
    {
        WORD lol = HIWORD(lParam);

        if(wParam==VK_UP)
            if(!pressedUP)
            {
                Controls::rotateF();
                pressedUP=true;
            }

        switch(wParam)
        {
        case VK_DOWN:
        {
            pressedDOWN=true;
            break;
        }
        case VK_LEFT:
        {
            if(lol>500)pressedLEFT=true;
            if(!pressedLEFT)Controls::MoveLeft();
            break;
        }
        case VK_RIGHT:
        {
            if(lol>500)pressedRIGHT=true;
            if(!pressedRIGHT)Controls::MoveRight();
            break;
        }
        case VK_CONTROL:
        {
            NewGame();
            break;
        }
        case VK_F1:
            {
                stars = !stars;
                break;
            }
        case VK_ESCAPE:
        {
            PostQuitMessage(0);
            break;
        }
        case VK_BACK:
        {
            fullscreen=!fullscreen;
            if(fullscreen)
            {
                SetWindowLong(hwnd,GWL_STYLE,WS_POPUP);
                ShowWindow(hwnd,SW_SHOWMAXIMIZED);
            }
            else
            {
                SetWindowLong(hwnd,GWL_STYLE,WS_OVERLAPPEDWINDOW);
                ShowWindow(hwnd,SW_SHOWNORMAL);
                SetWindowPos(hwnd,NULL,0,0,300,300,0);
            }
        }
        }
        return 0;
        break;
    }

    case WM_TIMER:
    {
        switch(wParam)
        {
        case 1:
        {
            if(!pressedDOWN)
                Controls::MoveDown();
            break;
        }
        case 2:
        {
            if(pressedDOWN)
            {
                Controls::MoveDown();
                points++;
            }
            if(pressedLEFT)Controls::MoveLeft();
            if(pressedRIGHT)Controls::MoveRight();


            if(stars)
            {
               HDC hdc = GetDC(hwnd);
               for(int i=0;i<workSpace.left;i++)
               SetPixel(hdc,i,50,RGB(255,255,0));
               ReleaseDC(hwnd,hdc);
            }
        break;
        }
        }
        return 0;
        break;
    }

    case WM_CLOSE:
    {
        if(points>maxpoints)
            maxpoints = points;
        write();
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
        break;
    }

}

void read()
{
    FILE *fp;

    fp = fopen("score.data", "rb");
    fread(&maxpoints,4,1,fp);
    fclose(fp);
}

void write()
{
    FILE *fp;

    fp = fopen("score.data", "wb");
    fwrite(&maxpoints,4,1,fp);
    fclose(fp);
}

void NewGame()
{
    for(int i=0; i<h; i++)
        for(int j=0; j<w; j++)
            field[j][i] = 0;
    if(points>maxpoints)
        maxpoints = points;
    write();
    points = 0;
    newFig();
    InvalidateRect(hWnd,NULL,true);
}

void formOutPut(char* buff)
{
    char score[10];
    char maxscore[10];
    itoa(points,score,10);
    itoa(maxpoints,maxscore,10);
    strcat(buff,score);
    strcat(buff," Record: ");
    strcat(buff,maxscore);
}

inline bool checkLine(short y)
{
    for(int j=0; j<w; j++)
        if(!field[j][y])
            return false;
    return true;
}

void checkForPoints()
{
    short high = 0;
    bool filled = true;
    bool redraw = false;

    while(filled)
    {
        filled = false;
        for(int i=high; i<h; i++)
            if(checkLine(i))
            {
                high = i;
                filled = true;
            }
        if(filled)
        {
            for(int i=high; i>0; i--)
                for(int j=0; j<w; j++)
                    field[j][i] = field[j][i-1];
            points += 100;
            redraw = true;
        }
    }
    InvalidateRect(hWnd,NULL,redraw);
}

void newFig()
{
    xc = w / 2;
    yc = -2;
    srand(time(NULL));
    figNum = rand() % 7;
    curClr = 1+rand() % 5;
    for(int i = 0; i<4; i++)
    {
        cur[i] = obj[figNum][i];
        last[i].x = cur[i].x+xc;
        last[i].y = cur[i].y+yc;
    }
    checkForPoints();
}

bool checkForColl(point* arr,short x,short y)
{
    for(int i=0; i<4; i++)
    {
        if(arr[i].y+y<0)
            return false;
        if(field[arr[i].x+x][arr[i].y+y])
            return true;
    }
    return false;
}

void Drawing::fillrct(HDC hdc,short x,short y,HBRUSH hb)
{
    if(y<0)
        return;
    RECT r;
    WORD left = fieldCords[x][y].x;
    WORD top = fieldCords[x][y].y;
    SetRect(&r,left,top,left+rectsize,top+rectsize);
    FillRect(hdc,&r,hb);
}

void Drawing::resizeField()
{
    WORD width  = rectsize*w;
    WORD height = rectsize*h;

    workSpace.left  = (screenW-width)/2-pensize*2;
    workSpace.top   = (screenH-height)/2;
    workSpace.right = workSpace.left + width;
    workSpace.bottom= workSpace.top + height;

    int x=workSpace.left;
    int y=workSpace.top;

    for(int i=0; i<h; i++)
    {
        for(int j=0; j<w; j++)
        {
            fieldCords[j][i].x = x;
            fieldCords[j][i].y = y;
            x+=rectsize;
        }
        x=workSpace.left;
        y+=rectsize;
    }

    InvalidateRect(hWnd,NULL,true);
}

void fall()
{
    for(int i=0; i<4; i++)
    {
        field[cur[i].x+xc][cur[i].y+yc] = curClr;
        if(cur[i].y+yc<0)
        {
            NewGame();
            break;
        }
    }
    newFig();
}

void Controls::MoveDown()
{
    for(int i=0; i<4; i++)
        if(yc+1+cur[i].y>=h )
        {
            fall();
            return;
        }

    if(checkForColl(cur,xc,yc+1))
    {
        fall();
        return;
    }

    yc++;
    InvalidateRect(hWnd,NULL,false);
}

bool FalloutX(short x)
{
    for(int i=0; i<4; i++)
        if(x+cur[i].x<0 || x+cur[i].x>=w)
            return true;
    return false;
}

void Controls::MoveLeft()
{
    if(!FalloutX(xc-1) && !checkForColl(cur,xc-1,yc))
    {
        xc--;
        InvalidateRect(hWnd,&workSpace,false);
    }

}

void Controls::MoveRight()
{
    if(!FalloutX(xc+1)&& !checkForColl(cur,xc+1,yc))
    {
        xc++;
        InvalidateRect(hWnd,&workSpace,false);
    }
}

void Copy(point* f,point* s)
{
    for(int i=0; i<4; i++)
        f[i] = s[i];
}

void Controls::rotateF()
{
    if(figNum==1)
        return;
    short off = 0;

    point temp[4];

    for(int i=0; i<4; i++)
    {
        temp[i]=cur[i];
        short t = temp[i].x;
        temp[i].x = -temp[i].y;
        temp[i].y = t;
        if(yc+temp[i].y>=h)
            return;
        if(xc+temp[i].x<0 || xc+temp[i].x>=w)
            if(off<abs(temp[i].x))
                off = temp[i].x;
    }

    if(!checkForColl(temp,xc-off,yc))
    {
        xc-=off;
        Copy(cur,temp);
        InvalidateRect(hWnd,&workSpace,false);
    }

}

void Drawing::drawRect(HDC hdc, short x, short y)
{
    if(y<0)
        return;
    WORD left = fieldCords[x][y].x;
    WORD top = fieldCords[x][y].y;
    Rectangle(hdc,left,top,left+rectsize,top+rectsize);
}

void Drawing::drawEl(HDC hdc,WORD x, WORD y)
{
    HBRUSH brush = CreateSolidBrush(clrTable[curClr]);
    HGDIOBJ orig = SelectObject(hdc,brush);
    for(int i=0; i<4; i++)
        Drawing::drawRect(hdc,cur[i].x+x,cur[i].y+y);
    DeleteObject(SelectObject(hdc,orig));
}
