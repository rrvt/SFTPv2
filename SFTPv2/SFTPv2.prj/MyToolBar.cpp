// My ToolBar


#include "pch.h"
#include "MyToolBar.h"
#include "Resource.h"


static int NoOfButtonControls =  2;
static int NoOfButtons        = 12;



MyToolBar::MyToolBar() : dspMenu(ID_DisplayList)
                        {toolBarDim.initialize(NoOfButtonControls, NoOfButtons);}




bool MyToolBar::addMenu(uint id, int idr, int index) {
int dspID   = dspMenu.getId();

  if (id == dspID)   return add(dspMenu,  id, idr, index);

  return false;
  }



