// Utility Functions


#pragma once
#include "SendMsg.h"
#include "Resource.h"


String& fixLocalPath( String& path);          // Add '\' to end of path
String& fixRemotePath(String& path);          // Add '/' to end of path

String toLocal( TCchar* path);
String toRemote(TCchar* path);

bool   rmvLastDir(String& path, String& right, Tchar sep);  // Remove last subdir from path and
                                                            // return the path removed in right.
                                                            // The sep character is either '/' or
                                                            // '\'.

inline void sendWdwScroll(bool scroll = true) {sendMsg(ID_SetWdwScroll,  scroll, 0);}
inline void sendDisplayMsg()                  {sendMsg(ID_DisplayMsg,    0,      0);}
inline void sendStepPrgBar()                  {sendMsg(ID_StepPrgBarMsg, 0,      0);}


