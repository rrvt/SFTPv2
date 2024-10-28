// Path Transform
//   * remote separators to local separators
//   * local separtors to remote separators
//   * full path to relative
//   * relative to full path



#pragma once



class PathXform {

String s;
String root;

public:


  PathXform() { }
 ~PathXform() { }

  void    clear() {root.clear();   s.clear();}
  bool    isEmpty() {return root.isEmpty();}
  void    set(TCchar* rootPath);                    // Set root
  String& get() {return root;}                      // get root

  String& normalize( TCchar* path);
  String& trmntDir(  TCchar* path);

  String& toLocal(   TCchar* path);
  String& toRemote(  TCchar* path);
  String& toFull(    TCchar* path);
  String& toRelative(TCchar* path);
  };

