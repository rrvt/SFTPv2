// Utility Functions


#include "pch.h"
#include "AppUtilities.h"


String& fixLocalPath(String& path)
                          {if (path[path.length() - 1] != _T('\\')) path += _T('\\'); return path;}
String& fixRemotePath(String& path)
                          {if (path[path.length() - 1] != _T('/'))  path += _T('/');  return path;}



String toLocal(TCchar* path) {
String s = path;
int    n = s.length();
int    i;
int    pos;
String prefix;

  for (i = 0; i < n; i++) if (s[i] == _T('/')) s[i] = _T('\\');

  pos = s.find(_T("\\\\"));

  if (pos >= 0) {prefix = s.substr(0, pos+1);   s = prefix + s.substr(pos+2);}

  return s;
  }



String toRemote(TCchar* path) {
String s = path;
int    n = s.length();
int    i;

  for (i = 0; i < n; i++) if (s[i] == _T('\\')) s[i] = _T('/');

  return s;
  }




bool rmvLastDir(String& path, String& right, Tchar sep) {
int pos = path.findLastOf(sep);

  right = path;

  path = path.substr(0, pos);   pos = path.findLastOf(sep);    if (pos <= 0) return false;

  path = path.substr(0, pos+1);   right = right.substr(pos+1);   return true;
  }



//--------------

#if 0
String toRelative(TCchar* path, String& root) {
String s = path;

  if (s.find(root) == 0) s = s.substr(root.length());

  return toLocalPath(s);
  }
#endif

