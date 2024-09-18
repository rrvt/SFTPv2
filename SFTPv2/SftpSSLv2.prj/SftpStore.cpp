// Store for data received from server


#include "pch.h"
#include "SftpStore.h"
#include "SftpDataIter.h"


SftpStore& SftpStore::operator -= (SftpStore& store) {

  fileType  = store.fileType;   fileType = NilFlTyp;
  firstResp = store.firstResp;  firstResp.clear();
  lastResp  = store.lastResp;   lastResp.clear();
  lastCh    = store.lastCh;     lastCh = 0;
  data     -= store.data;
  return *this;
  }


bool SftpStore::find(int code) {
SftpDataIter iter(*this);
String*      line;
uint         pos;
int          v;
bool         codeSeen = false;

  for (line = iter(); line; line = iter++) {
    String& t = *line;

    v = t.stoi(pos);   if (pos > 4) continue;

    if (v == code) {

      if (!codeSeen) {codeSeen |= true;   firstResp = t;   lastCh = t[0];}

      lastResp = t;
      }
    }
  return codeSeen;
  }


bool SftpStore::find(Tchar ch) {
SftpDataIter iter(*this);
String*      line;
ulong        v;
uint         nxt;
bool         rslt = false;


  for (line = iter(); line; line = iter++) {
    String& t = *line;

    v = t.stoul(nxt);    if (nxt > 4) continue;

    if (t[0] == ch) {

      if (!rslt) {rslt = true;   firstResp = t;   lastCh = t[0];}

      lastResp = t;
      }
    }
  return rslt;
  }


void SftpStore::getResp(String& s) {
SftpDataIter iter(*this);
String*      line;

  for (s.clear(), line = iter(); line; line = iter++) s += *line + _T('\n');
  }

