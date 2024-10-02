// Directory List subject to a commom action


#include "pch.h"
#include "DirList.h"
#include "UnitList.h"


void DirList::add(UnitDsc* dsc)
          {DirItem item;   item.level = pathLevel(dsc->key.path);   item.dsc = dsc;   data = item;}



int DirList::pathLevel(TCchar* path) {
String pth = path;
String sect;
int    lvl = 0;
int    pos;

  for (pos = pth.find(_T('\\')); pos >= 0; pos = pth.find(_T('\\'))) {
    sect = pth.substr(0, pos+1);   pth = pth.substr(pos+1);   if (!sect.isEmpty()) lvl++;
    }

  if (!pth.isEmpty()) lvl++;

  return lvl;
  }

