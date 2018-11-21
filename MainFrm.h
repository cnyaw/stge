
#pragma once

class CViewDecorate : public CWindowImpl<CViewDecorate>
{
public:
  DECLARE_WND_CLASS_EX(NULL, 0, COLOR_BTNFACE)

  enum { YOFFSET = 2 };

  HWND mView;

  BEGIN_MSG_MAP_EX(CMainFrame)
    MSG_WM_SETFOCUS(OnSetFocus)
    MSG_WM_SHOWWINDOW(OnShowWindow)
    MSG_WM_SIZE(OnSize)
  END_MSG_MAP()

  void OnSetFocus(CWindow wndOld)
  {
    ::SetFocus(mView);
  } // OnSetFocus

  void OnShowWindow(BOOL bShow, UINT nStatus)
  {
    ::ShowWindow(mView, bShow ? SW_SHOW : SW_HIDE);
  } // OnShowWindow

  void OnSize(UINT nType, CSize size)
  {
    ::SetWindowPos(
        mView,
        NULL,
        0,
        YOFFSET,
        size.cx,
        size.cy - 2 * YOFFSET,
        SWP_NOZORDER | SWP_NOACTIVATE);
  } // OnSize
};

class CMainFrame :
  public CFrameWindowImpl<CMainFrame>,
  public CUpdateUI<CMainFrame>,
  public CMessageFilter,
  public CIdleHandler
{
public:
  DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

  CCommandBarCtrl mCmdBar;

  CSplitterWindow mSplit;
  CTabView mTabView;
  CListBox mList;

  CViewDecorate mD1, mD2;

  CStgeView mView;
  CSourceView mSource;

  std::string mFileName;

  virtual BOOL PreTranslateMessage(MSG* pMsg)
  {
    if (WM_MOUSEWHEEL == pMsg->message) {
      POINT pt;
      ::GetCursorPos(&pt);
      ::SendMessage(
          ::WindowFromPoint(pt),
          WM_MOUSEWHEEL,
          pMsg->wParam,
          pMsg->lParam);
      return TRUE;
    }

    if (mTabView.PreTranslateMessage(pMsg)) {
      return TRUE;
    }

    return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
  } // PreTranslateMessage

  virtual BOOL OnIdle()
  {
    int page = mTabView.GetActivePage();

    int s, e;
    mSource.GetSel(s, e);
    BOOL bHasSel = s != e;

    OpenClipboard();
    BOOL bCanPaste = NULL != GetClipboardData(CF_TEXT);
    CloseClipboard();

    UIEnable(ID_FILE_SAVE, mSource.GetModify());
    UIEnable(ID_FILE_GENRATE, !mView.mScm.empty());
    UIEnable(ID_EDIT_CUT, 1 == page && bHasSel);
    UIEnable(ID_EDIT_COPY, 1 == page && bHasSel);
    UIEnable(ID_EDIT_PASTE, 1 == page && bCanPaste);
    UIEnable(ID_EDIT_UNDO, 1 == page && mSource.CanUndo());

    UIUpdateToolBar();
    return FALSE;
  } // OnIdle

  BEGIN_UPDATE_UI_MAP(CMainFrame)
    UPDATE_ELEMENT(ID_FILE_SAVE, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_FILE_GENRATE, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_EDIT_CUT, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_EDIT_COPY, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_EDIT_PASTE, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_EDIT_UNDO, UPDUI_MENUPOPUP)
    UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
    UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
  END_UPDATE_UI_MAP()

  BEGIN_MSG_MAP_EX(CMainFrame)
    MSG_WM_CLOSE(OnClose)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_DESTROY(OnDestroy)
    MSG_WM_ENDSESSION(OnEndSession)
    MSG_WM_QUERYENDSESSION(OnQueryEndSession)
    COMMAND_ID_HANDLER_EX(ID_FILE_NEW, OnFileNew)
    COMMAND_ID_HANDLER_EX(ID_FILE_OPEN, OnFileOpen)
    COMMAND_ID_HANDLER_EX(ID_FILE_SAVE, OnFileSave)
    COMMAND_ID_HANDLER_EX(ID_FILE_SAVE_AS, OnFileSaveAs)
    COMMAND_ID_HANDLER_EX(ID_FILE_COMPLIE, OnComplie)
    COMMAND_ID_HANDLER_EX(ID_FILE_GENRATE, OnGenerateCode)
    COMMAND_ID_HANDLER_EX(ID_APP_EXIT, OnFileExit)
    CHAIN_MSG_MAP_ALT_MEMBER(mSource, 1)
    COMMAND_ID_HANDLER_EX(ID_VIEW_TOOLBAR, OnViewToolBar)
    COMMAND_ID_HANDLER_EX(ID_VIEW_STATUS_BAR, OnViewStatusBar)
    COMMAND_ID_HANDLER_EX(ID_APP_ABOUT, OnAppAbout)
    COMMAND_CODE_HANDLER_EX(LBN_SELCHANGE, OnSelChange)
    CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
    CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
  END_MSG_MAP()

  //
  // Message handler.
  //

  void OnClose()
  {
    OnEndSession(OnQueryEndSession(0,0), 0);
  } // OnClose

  int OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
    //
    // Create command bar window.
    //

    HWND hWndCmdBar = mCmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);

    //
    // Attach menu.
    //

    mCmdBar.AttachMenu(GetMenu());

    //
    // Load command bar images.
    //

    mCmdBar.LoadImages(IDR_MAINFRAME);
    
    //
    // Remove old menu.
    //

    SetMenu(NULL);

    HWND hWndToolBar = CreateSimpleToolBarCtrl(
                         m_hWnd,
                         IDR_MAINFRAME,
                         FALSE,
                         ATL_SIMPLE_TOOLBAR_PANE_STYLE);

    CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
    AddSimpleReBarBand(hWndCmdBar);
    AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

    CreateSimpleStatusBar();

    m_hWndClient = mSplit.Create(
                            m_hWnd,
                            rcDefault,
                            NULL,
                            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

    UIAddToolBar(hWndToolBar);
    UISetCheck(ID_VIEW_TOOLBAR, 1);
    UISetCheck(ID_VIEW_STATUS_BAR, 1);

    //
    // Register object for message filtering and idle updates.
    //

    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);

    //
    // Setup split(list,tab).
    //

    mTabView.Create(
               mSplit,
               rcDefault,
               NULL,
               WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    mTabView.m_tab.SetTooltips(NULL);
    mList.Create(
            mSplit,
            rcDefault,
            NULL,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY,
            WS_EX_CLIENTEDGE);

    mSplit.SetSplitterPanes(mList, mTabView);
    mSplit.SetActivePane(0);

    //
    // Setup tab view(view,source).
    //

    mD1.Create(
          mTabView,
          rcDefault,
          NULL,
          WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CLIPCHILDREN);
    mD1.mView = mView.Create(
                        mD1,
                        rcDefault,
                        NULL,
                        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                        WS_EX_STATICEDGE);
    mTabView.AddPage(mD1.m_hWnd, _T("View"));

    mD2.Create(
          mTabView,
          rcDefault,
          NULL,
          WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    mD2.mView = mSource.Create(
                          mD2,
                          rcDefault,
                          NULL,
                          WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN,
                          WS_EX_CLIENTEDGE);
    mSource.SetFont(AtlGetStockFont(SYSTEM_FIXED_FONT));
    mTabView.AddPage(mD2.m_hWnd, _T("Source"));

    mTabView.SetActivePage(1);

    //
    // Update layout.
    //

    UpdateLayout();
    mSplit.SetSplitterPos(220);

    SetCaption("Untitled");

    SetMsgHandled(FALSE);
    return 0;
  } // OnCreate

  void OnDestroy()
  {
    //
    // Unregister message filtering and idle updates.
    //

    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->RemoveMessageFilter(this);
    pLoop->RemoveIdleHandler(this);

    SetMsgHandled(FALSE);
  } // OnDestroy

  void OnEndSession(BOOL bEnding, UINT uLogOff)
  {
    if (bEnding) {
      DestroyWindow();
    }
  } // OnEndSession

  LRESULT OnQueryEndSession(UINT nSource, UINT uLogOff)
  {
    if (!mSource.GetModify()) {
      return TRUE;
    }

    switch (MessageBox(
              _T("Save changes?"),
              CString((LPCTSTR)IDR_MAINFRAME),
              MB_YESNOCANCEL | MB_ICONQUESTION))
    {
    case IDYES:                         // Do save.
      {
        std::string name(mFileName);

        if (name.empty()) {

            CFileDialog dlg(
                          FALSE,
                          NULL,
                          NULL,
                          OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                          _T("Script Files(*.txt)\0*.*\0"));
            if (IDOK == dlg.DoModal()) {
              name = dlg.m_szFileName;
            } else {
              return FALSE;
            }
        }

        if (!SaveText(name)) {
          MessageBox(
            _T("Save file failed!"),
            CString((LPCTSTR)IDR_MAINFRAME),
            MB_OK | MB_ICONERROR);
          return FALSE;
        }

        mFileName = name;
      }
      break;

    case IDNO:                          // Skip changes.
      break;

    case IDCANCEL:                      // Cancel.
      return FALSE;

    }

    return TRUE;
  } // OnQueryEndSession

  //
  // Command handler.
  //

  void OnAppAbout(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CAboutDlg dlg;
    dlg.DoModal();
  } // OnAppAbout

  void OnComplie(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    std::stringstream ss;
    if (!GetText(ss)) {
      return;
    }

    mView.mOm.reset();
    mView.mScm.clear();
    mList.ResetContent();

    stge::ScriptManager scm;
    if (!stge::Parser::parse(ss, scm)) {
      CString s = stge::Parser::getLastError().c_str();
      MessageBox(s, _T("Error"), MB_OK|MB_ICONERROR);
      return;
    }

    for (stge::ScriptManager::const_iterator it = scm.begin();
         scm.end() != it;
         ++it) {
      mList.AddString((LPCTSTR)it->first.c_str());
    }

    mView.mScm = scm;
  } // OnComplie

  void OnFileExit(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    PostMessage(WM_CLOSE);
  } // OnFileExit

  void OnFileNew(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    if (!OnQueryEndSession(0,0)) {
      return;
    }

    mView.mOm.reset();
    mView.mScm.clear();
    mFileName.clear();
    mList.ResetContent();
    mSource.SetWindowText(_T(""));
    mSource.SetModify(FALSE);
    SetCaption("Untitled");
  } // OnFileNew

  void OnFileOpen(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    if (!OnQueryEndSession(0,0)) {
      return;
    }

    CFileDialog dlg(
                  TRUE,
                  NULL,
                  NULL,
                  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  _T("Script Files(*.txt)\0*.*\0"));
    if (IDOK != dlg.DoModal()) {
      return;
    }

    mTabView.SetActivePage(1);
    if (!LoadText(dlg.m_szFileName)) {
      MessageBox(
        _T("Open file failed!"),
        CString((LPCTSTR)IDR_MAINFRAME),
        MB_OK | MB_ICONERROR);
      return;
    }

    stge::ScriptManager scm;
    if (!stge::Parser::parse((char const*)dlg.m_szFileName, scm)) {
      MessageBox(
        CString(stge::Parser::getLastError().c_str()),
        _T("Error"),
        MB_OK | MB_ICONERROR);
      return;
    }

    mList.ResetContent();
    for (stge::ScriptManager::const_iterator it = scm.begin();
         scm.end() != it;
         ++it) {
      mList.AddString((LPCTSTR)it->first.c_str());
    }

    mView.mScm = scm;
    mSource.SetModify(FALSE);

    mFileName = dlg.m_szFileName;
    SetCaption(mFileName);
  } // OnFileOpen

  void OnFileSave(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    std::string name(mFileName);

    if (name.empty()) {

      CFileDialog dlg(
                    FALSE,
                    NULL,
                    NULL,
                    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                    _T("Script Files(*.txt)\0*.*\0"));
      if (IDOK == dlg.DoModal()) {
        name = dlg.m_szFileName;
      } else {
        return;
      }
    }

    if (!SaveText(name)) {
      MessageBox(
        _T("Save file failed!"),
        CString((LPCTSTR)IDR_MAINFRAME),
        MB_OK | MB_ICONERROR);
      return;
    }

    mFileName = name;
    SetCaption(mFileName);
  } // OnFileSave

  void OnFileSaveAs(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CFileDialog dlg(
                  FALSE,
                  NULL,
                  NULL,
                  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  _T("Script Files(*.txt)\0*.*\0"));
    if (IDOK != dlg.DoModal()) {
      return;
    }

    if (!SaveText(dlg.m_szFileName)) {
      MessageBox(
        _T("Save file failed!"),
        CString((LPCTSTR)IDR_MAINFRAME),
        MB_OK | MB_ICONERROR);
      return;
    }

    mFileName = dlg.m_szFileName;
    SetCaption(mFileName);
  } // OnFileSaveAs

  void OnGenerateCode(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CFileDialog dlg(
                  FALSE,
                  NULL,
                  NULL,
                  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  _T("C++ Files(*.cpp;*.cxx;*.h;*.hxx;*.hpp)\0*.cpp\0*.cxx\0*.h\0*.hxx\0*.hpp\0"));
    if (IDOK != dlg.DoModal()) {
      return;
    }

    if (!stge::CodeGenerator::generate((std::string const&)dlg.m_szFileName, mView.mScm)) {
      MessageBox(_T("Generate code failed!"), CString((LPCTSTR)IDR_MAINFRAME), MB_OK|MB_ICONERROR);
    } else {
      MessageBox(_T("Done!"), CString((LPCTSTR)IDR_MAINFRAME), MB_OK);
    }
  } // OnGenerateCode

  void OnSelChange(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CString s;
    mList.GetText(mList.GetCurSel(), s);

    //
    // Run script.
    //

    mView.mOm.reset();
    mView.mOm.run(std::string(s), 0, 0);
    mTabView.SetActivePage(0);
  } // OnSelChange

  void OnViewToolBar(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    static BOOL bVisible = TRUE;        // Initially visible.
    bVisible = !bVisible;
    CReBarCtrl rebar = m_hWndToolBar;
    int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1); // Toolbar is 2nd added band.
    rebar.ShowBand(nBandIndex, bVisible);
    UISetCheck(ID_VIEW_TOOLBAR, bVisible);
    UpdateLayout();
  } // OnViewToolBar

  void OnViewStatusBar(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
    ::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
    UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
    UpdateLayout();
  } // OnViewStatusBar

  //
  // Helper.
  //

  bool GetText(std::ostream& stream)
  {
    char buff[2048];
    int nl = mSource.GetLineCount();
    for (int i = 0; i < nl; ++i) {
      int n = mSource.GetLine(i, buff, sizeof(buff));
      stream.write(buff, n);
      if (i != nl - 1) {
        stream << "\n";
      }
    }

    return true;
  } // GetText

  bool LoadText(std::string const& fname)
  {
    std::ifstream ifs(fname.c_str());
    if (!ifs) {
      return false;
    }

    std::stringstream ss;
    std::string line;
    while (std::getline(ifs, line)) {
      ss << line << "\r\r\n";
    }

    mSource.SetWindowText(ss.str().c_str());
    mSource.SetModify(FALSE);

    return true;
  } // LoadText

  bool SaveText(std::string const& fname)
  {
    std::stringstream ss;
    if (!GetText(ss)) {
      return false;
    }

    std::ofstream ofs(fname.c_str());
    if (!ofs) {
      return false;
    }

    ofs << ss.rdbuf();
    ofs.close();

    mSource.SetModify(FALSE);
    return true;
  } // SaveText

  void SetCaption(std::string const& sub)
  {
    CString cap;
    cap.Format(_T("%s - %s"), CString((LPCTSTR)IDR_MAINFRAME), sub.c_str());
    SetWindowText(cap);
  } // SetCaption
};
