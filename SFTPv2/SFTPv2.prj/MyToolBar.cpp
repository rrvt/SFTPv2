// My ToolBar


#include "pch.h"
#include "MyToolBar.h"
#include "Resource.h"


static int NoOfButtonControls =  1;
static int NoOfButtons        =  7;



MyToolBar::MyToolBar() :
                         saveMenu(ID_TBSaveMenu)
                        {toolBarDim.initialize(NoOfButtonControls, NoOfButtons);}




bool MyToolBar::addMenu(uint id, int idr, int index) {
int saveID  = saveMenu.getId();

  if (id == saveID)  return add(saveMenu, id, idr, index);

  return false;
  }





///-------------

#ifdef Examples
static int NoOfButtonControls =  6;
static int NoOfButtons        = 12;
#else
#endif
#ifdef Examples
                         button(ID_Button), editBox(ID_EditBox),     menu(ID_Menu),
                         menu1(ID_Menu1), cboBx(ID_CboBx),
#endif
#ifdef Examples

bool MyToolBar::addButton( uint id, TCchar* caption) {
  return add(button,  id, caption);
  }


bool MyToolBar::addEditBox(uint id, int noChars)              {
  return add(editBox, id, noChars);
  }


CString MyToolBar::getText(uint id) {

  if (id == editBox.getId()) {return ToolBarBase::getText(editBox);}

  return 0;
  }


bool MyToolBar::addMenu(uint id, int idr, TCchar* caption) {
int menuID  = menu.getId();
int menu1ID = menu1.getId();
int saveID  = saveMenu.getId();

  if (id == menuID)  return add(menu,     id, idr, caption);
  if (id == menu1ID) return add(menu1,    id, idr, caption);
  if (id == saveID)  return add(saveMenu, id, idr, caption);

  return false;
  }


bool MyToolBar::addMenu(uint id, int idr, int index) {
int menuID  = menu.getId();
int menu1ID = menu1.getId();
int saveID  = saveMenu.getId();

  if (id == menuID)  return add(menu,     id, idr, index);
  if (id == menu1ID) return add(menu1,    id, idr, index);
  if (id == saveID)  return add(saveMenu, id, idr, index);

  return false;
  }


bool MyToolBar::addCBx(uint id) {
  return add(cboBx,  id, 10);
  }


bool MyToolBar::addCBx(uint id, int idr, TCchar* caption) {
  return add(cboBx,  id, idr, caption);
  }


bool MyToolBar::addCbxItem(uint id, TCchar* txt, int data) {
int cbxID = cboBx.getId();

  if (id == cbxID) {return addItem(cboBx, txt, data);}
  return false;
  }


bool MyToolBar::addCbxItemSorted(uint id, TCchar* txt, int data) {
int cbxID = cboBx.getId();

  if (id == cbxID) {return addItemSorted(cboBx, txt, data);}
  return false;
  }


void MyToolBar::setCaption(uint id, TCchar* caption) {
int cbxID = cboBx.getId();

  if (id == cbxID) ToolBarBase::setCaption(cboBx, caption);
  }


void MyToolBar::setWidth(uint id) {
int cbxID = cboBx.getId();

  if (id == cbxID) ToolBarBase::setWidth(cboBx);
  }


void MyToolBar::setHeight( uint id) {
int cbxID = cboBx.getId();

  if (id == cbxID) ToolBarBase::setHeight(cboBx);
  }



bool MyToolBar::getCurSel( uint id, String& s, int& data) {
int cbxID = cboBx.getId();

  if (id == cbxID) return ToolBarBase::getCurSel(cboBx, s, data);
  return false;
  }


#else
#endif

