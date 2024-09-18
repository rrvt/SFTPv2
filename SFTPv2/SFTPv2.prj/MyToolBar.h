// My ToolBar


#pragma once
#include "TBMenu.h"
#include "ToolBarBase.h"

struct CbxItem;


class MyToolBar : public ToolBarBase {


TBMenu    saveMenu;

public:

  MyToolBar();
 ~MyToolBar() { }


  bool    addMenu(uint id, int idr, int index);         // uses icon at index on toolbar
  };




///--------------------

#ifdef Examples
#include "TBButton.h"
#include "TBCboBx.h"
#include "TBEditBox.h"
#endif
#ifdef Examples
TBButton  button;
TBEditBox editBox;
TBMenu    menu;
TBMenu    menu1;
TBCboBx   cboBx;
#endif
#ifdef Examples

  bool    addButton( uint id, TCchar* caption);
  bool    addEditBox(uint id, int     noChars);
  CString getText(uint id);

  bool    addMenu(   uint id, int     idr, TCchar* caption);

  bool    addCBx(    uint id);
  bool    addCBx(    uint id, int     idr, TCchar* caption);
  bool    addCbxItem(uint id, TCchar* txt, int data = 0);
  bool    addCBx(    uint id, CbxItem cbxItem[], int n, TCchar* caption);
  bool    addCbxItemSorted(uint id, TCchar* txt, int data = 0);
  void    setCaption(uint id, TCchar* caption);
  void    setWidth(  uint id);
  void    setHeight( uint id);
  bool    getCurSel( uint id, String& s, int& data);

#endif

