
#pragma once

sw2::RECT_t<int> mBounding;
sw2::RECT_t<int> mOutterBounding;

template<class ObjMgrT>
class MyObj : public stge::Object
{
public:

  bool init(ObjMgrT& fac, int idAction, int idNewObj)
  {
    //
    // Return false if init failed, and clear this object.
    //

    return true;
  } // init

  template<typename PlayerT>
  bool update(float fElapsed, ObjMgrT& om, PlayerT& player)
  {
    //
    // Return false, and clear this object.
    //

    stge::Object::update(fElapsed, om, player);

    return mOutterBounding.ptInRect(sw2::POINT_t<int>((int)x, (int)y));
  } // update
};

class MyObjectManager : public stge::ObjectManager<MyObjectManager, MyObj<MyObjectManager> >
{
public:
  static int getWindowWidth()
  {
    return 400;
  }

  static int getWindowHeight()
  {
    return 400;
  }
};

class CStgeView :
  public CWindowImpl<CStgeView>,
  public CIdleHandler,
  public COpenGL<CStgeView>
{
public:
  DECLARE_WND_CLASS(NULL)

  enum { FPS = 60 };

  DWORD mNextTime;
  float mDepth;

  stge::ScriptManager mScm;
  MyObjectManager mOm;
  stge::Object mPlayer;

  CStgeView()
  {
    mOm.init(&mScm);
  } // CStgeView

  virtual BOOL OnIdle()
  {
    //
    // Update.
    //

    RECT rc;
    GetClientRect(&rc);

    POINT p;
    GetCursorPos(&p);
    ScreenToClient(&p);

    mPlayer.x = (float)(p.x - rc.right/2);
    mPlayer.y = (float)(p.y - rc.bottom/2);

    mOm.update(1/(float)FPS, mPlayer);

    //
    // Render.
    //

    RedrawWindow();

    ::Sleep(TimeLeft());

    return FALSE;
  } // OnIdle

  BEGIN_MSG_MAP_EX(CStgeView)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_SHOWWINDOW(OnShowWindow)
    CHAIN_MSG_MAP(COpenGL<CStgeView>)
  END_MSG_MAP()

  //
  // Message handler.
  //

  int OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
    mBounding = sw2::RECT_t<int>(-200,-200,200,200);

    mOutterBounding = mBounding;
    mOutterBounding.inflate(mBounding.width()/10, mBounding.height()/10);

    SetMsgHandled(FALSE);
    return 0;
  } // OnCreate

  void OnShowWindow(BOOL bShow, UINT nStatus)
  {
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);

    if (bShow) {
      mNextTime = ::GetTickCount() + (int)(1000/(float)FPS);
      pLoop->AddIdleHandler(this);
    } else {
      pLoop->RemoveIdleHandler(this);
    }

    SetMsgHandled(FALSE);
  } // OnShowWindow

  //
  // COpenGL<>
  //

  void OnInit(void)
  {
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    //
    // Load font texture.
    //

    {
      CBitmap bmFont;
      bmFont.LoadBitmap(IDB_BITMAP1);

      CClientDC dc(m_hWnd);

      BITMAPINFO bmi = {0};
      bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
      bmi.bmiHeader.biWidth  = 256;
      bmi.bmiHeader.biHeight = 256;
      bmi.bmiHeader.biPlanes   = 1;
      bmi.bmiHeader.biBitCount = 24;
      bmi.bmiHeader.biCompression = BI_RGB;

      unsigned char* pPixel;

      CBitmap bmTmp;
      bmTmp = ::CreateDIBSection(dc, &bmi, DIB_RGB_COLORS, (void**)&pPixel, NULL, 0);

      CDC memdc1, memdc2;

      memdc1.CreateCompatibleDC(dc);
      memdc1.SelectBitmap(bmFont);

      memdc2.CreateCompatibleDC(dc);
      memdc2.SelectBitmap(bmTmp);

      memdc2.BitBlt(0, 0, 256, 256, memdc1, 0, 0, SRCCOPY);

      GLuint texture = 0;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pPixel);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    }
  } // OnInit

  void OnRender(void)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    // Bounding.
    //

    glLoadIdentity();
    glTranslatef(0, 0, -mDepth);
    glScalef(mBounding.width()/2.0f, mBounding.height()/2.0f, 1);
    glColor3ub(255, 255, 255);
    DrawRect();

    glLoadIdentity();
    glTranslatef(0, 0, -mDepth);
    glScalef(mOutterBounding.width()/2.0f, mOutterBounding.height()/2.0f, 1);
    glColor3ub(128, 128, 128);
    DrawRect();

    //
    // Draw bullet.
    //

    for (int i = mOm.objects.first(); -1 != i; i = mOm.objects.next(i)) {

      stge::Object const& o = mOm.objects[i];
      sw2::POINT_t<int> pt((int)o.x, (int)o.y);

      if (!mOutterBounding.ptInRect(pt)) {
        continue;
      }

      glLoadIdentity();
      glTranslatef(o.x, -o.y, -mDepth);
      glScalef(3, 3, 3);

      if (mBounding.ptInRect(pt)) {
        glColor3ub(255, 255, 255);
      } else {
        glColor3ub(128, 128, 128);
      }

      DrawRect();
    }

    //
    // Draw stats.
    //

    RECT rc;
    GetClientRect(&rc);

    std::stringstream ss;
    ss << "a" << mOm.actions.size() << " o" << mOm.objects.size();
    DrawString(-rc.right/2.0f, rc.bottom/2.0f, ss.str());
  } // OnRender

  void OnResize(int width, int height)
  {
    if (height == 0) {
      height = 1;
    }

    glViewport(0,0,width,height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //
    // Calculate The Aspect Ratio Of The Window.
    //

    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 1000.0f);

    float deg2pi = 3.1415926f / 180.0f;
    mDepth = (height / 2.0f) / tanf((45.0f/2.0f) * deg2pi);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  } // OnResize

  //
  // Helper.
  //

  void DrawChar(int c)
  {
    int idx = c - ' ';

    int cw = 12, ch = 24;
    int cch = 15;

    int x = cw * (idx % cch);
    int y = ch * (idx / cch);

    float x0 = x / 256.0f, y0 = (256 - y) / 256.0f;
    float x1 = (x + cw) / 256.0f, y1 = (256 - y - ch) / 256.0f;

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_QUADS);
      glTexCoord2f(x0,y1);
      glVertex3f(-1,-1,0);
      glTexCoord2f(x1,y1);
      glVertex3f(1,-1,0);
      glTexCoord2f(x1,y0);
      glVertex3f(1,1,0);
      glTexCoord2f(x0,y0);
      glVertex3f(-1,1,0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
  } // DrawChar

  void DrawRect()
  {
    glBegin(GL_LINE_LOOP);
      glVertex2f(-1, 1);
      glVertex2f(1, 1);
      glVertex2f(1, -1);
      glVertex2f(-1, -1);
    glEnd();
  } // DrawRect

  void DrawString(float x, float y, std::string const& str)
  {
    x += 8, y -= 8;
    for (size_t i = 0; i < str.size(); ++i) {
      glLoadIdentity();
      glTranslatef(x + 10 * i, y, -mDepth);
      glScalef(5, 6, 1);
      glColor3ub(255, 255, 255);
      DrawChar(str[i]);
    }
  } // DrawString

  DWORD TimeLeft()
  {
    DWORD now = ::GetTickCount();

    DWORD left = 0;
    if (mNextTime > now) {
      left = mNextTime - now;
    }

    mNextTime += (int)(1000/(float)FPS);

    return left;
  } // TimeLeft
};
