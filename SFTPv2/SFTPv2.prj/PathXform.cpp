// Path Transform
//   * remote separators to local separators
//   * local separtors to remote separators
//   * full path to relative
//   * relative to full path


#include "pch.h"
#include "PathXform.h"
#include "AppUtilities.h"


void PathXform::set(TCchar* rootPath) {root = trmntDir(rootPath);}


String& PathXform::normalize(TCchar* path) {return toRelative(toLocal(path));}


String& PathXform::trmntDir(TCchar* path) {
Tchar ch;

  s = path;   ch =  s[s.length() - 1];

  if (ch != _T('\\') && ch != _T('/')) s += _T('\\');

  return s;
  }


String& PathXform::toLocal(TCchar* path) {
int    n;
int    i;

  s = path;

  for (i = 0, n = s.length(); i < n; i++) if (s[i] == _T('/')) s[i] = _T('\\');

  return s;
  }


String& PathXform::toRemote(TCchar* path) {
int i;
int n;

  s = path;

  for (i = 0, n = s.length(); i < n; i++) if (s[i] == _T('\\')) s[i] = _T('/');

  return s;

  }


String& PathXform::toRelative(TCchar* path){

  s = path;

  if (!s.find(root)) s = s.substr(root.length());

  return s;
  }


String& PathXform::toFull(TCchar* path) {

  s = path;

  if (s.find(root)) s = root + path;

  return s;
  }

