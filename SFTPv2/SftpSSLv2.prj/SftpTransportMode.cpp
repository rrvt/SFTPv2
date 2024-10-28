// Transport Mode -- A combination of Transport command and file type (ascii or image)


#include "pch.h"
#include "SftpTransportMode.h"
#include "FileName.h"



static TCchar* asciiExt[] = {_T("htm"), _T("html"), _T("css"), _T("dwt"),
                             _T("txt"), _T("cpp"),  _T("h")
                             };

// The first row includes all the ascii commands and the second row contains all the image commands
// However, List is a special case

static TransportMode modes[2][3] = {{ListMd, PutAMd, GetAMd},   // The order is dependent on the
                                    {ListMd, PutIMd, GetIMd}};  // order in the SftpIO enum.


void SftpTransportMode::set(SftpIO io, TCchar* path) {mode = get(io, path);}


TransportMode SftpTransportMode::get(SftpIO io, TCchar* path) {
String ext;
int    i;
int    n;

  if (path) {
    ext = getExtension(path);
    if (!ext.isEmpty()) {
      for (i = 0, n = noElements(asciiExt); i < n; i++)
                                                    if (ext == asciiExt[i]) return modes[0][io-1];
      }
    }

  return modes[1][io-1];
  }


TCchar* SftpTransportMode::typeStg() {return isAscii() ? _T("A") : _T("I");}


bool SftpTransportMode::isAscii() {return NilMd < mode && mode < PutIMd;}


TCchar* SftpTransportMode::command() {

  switch (mode) {
    case ListMd : return _T("LIST -l -a");
    case PutAMd :
    case PutIMd : return _T("STOR");
    case GetAMd :
    case GetIMd : return _T("RETR");
    default     : break;
    }
  return 0;
  }

