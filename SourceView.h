
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2008/03/30 Waync created.
//

#pragma once

class CSourceView :
  public CWindowImpl<CSourceView, CEdit>,
  public CEditCommands<CSourceView>
{
public:

  CCursor mCursor;

  BEGIN_MSG_MAP_EX(CSourceView)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_SETCURSOR(OnSetCursor)
  ALT_MSG_MAP(1)
    CHAIN_MSG_MAP_ALT(CEditCommands<CSourceView>, 1)
  END_MSG_MAP()

  //
  // Message handler.
  //

  int OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
    mCursor.LoadCursor(IDC_POINTER);

    SetMargins(16,0);

    SetMsgHandled(FALSE);
    return 0;
  } // OnCreate

  BOOL OnSetCursor(CWindow wnd, UINT nHitTest, UINT message)
  {
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(&p);

    if (16 > p.x) {
      SetCursor(mCursor);
      return TRUE;
    }

    SetMsgHandled(FALSE);
    return 0;
  } // OnSetCursor
};

// end of SourceView.h
