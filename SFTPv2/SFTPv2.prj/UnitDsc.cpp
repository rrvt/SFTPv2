// Unit Descriptor -- file path or directory path plus some attributes



#include "pch.h"
#include "UnitDsc.h"
#include "Filename.h"
#include "CSVLex.h"
#include "CSVOut.h"


bool UnitDsc::load(CSVLex& lex) {
CSVtokCode code;
CSVtokCode code1;
int        i;
uint       x;

  for (code = lex.get_token(), i = 0; code != EOFToken; code = lex.get_token(), i++) {

    if (code == EolToken) {lex.accept_token(); return true;}

    CSVtok& tok = lex.token;   code1 = lex.token1.code;

    if (code != StringToken || code1 != CommaToken)
         {notePad << _T("Unable to find field in line: ") << *tok.psource << nCrlf;  return false;}

    switch (i) {
      case 0  : name = tok.name; break;

      case 1  : key.dir = tok.name.stoi(x); break;

      case 2  : key.path = tok.name; relPath = ::getPath(key.path); break;

      case 3  : size = tok.name.stoi(x); break;

      case 4  : {ToDate lcldt(tok.name);  date = lcldt();} break;

      default : notePad << _T("Whoops!") << nCrlf;
      }

    lex.accept_two_tokens();
    }

  return false;
  }



void UnitDsc::save(CSVOut& csvOut) {

  csvOut << name     << Comma;
  csvOut << key.dir  << Comma;
  csvOut << key.path << Comma;
  csvOut << size     << Comma;
  csvOut << date     << Comma;
  csvOut << vCrlf;
  }


void UnitDsc::display() {
String d  = date;

  notePad << name << nTab << size << nTab << d << nCrlf;
  }


void UnitDsc::set(TCchar* relPath, bool dir, UnitOp op) {
String& path = key.path;

  normalize(relPath);   key.dir = dir;   unitOp = op;

  this->relPath = ::getPath(path);

  name = dir ? path.substr(0, path.length()-1) : path;   name = removePath(name);
  }


String& UnitDsc::normalize(TCchar* relPath) {
String& path = key.path;
int     n;
int     i;
int     pos;
String  prefix;

  path = relPath;

  for (i = 0, n = path.length(); i < n; i++) if (path[i] == _T('/')) path[i] = _T('\\');

  pos = path.find(_T("\\\\"));

  if (pos >= 0) {prefix = path.substr(0, pos+1);   path = prefix + path.substr(pos+2);}

  return path;
  }


bool UnitDsc::operator<  (UnitKey& s) {return s >  key;}
bool UnitDsc::operator<= (UnitKey& s) {return s >= key;}             // Required for Binary Search
bool UnitDsc::operator== (UnitKey& s) {return s == key;}             // Required for Binary Search
bool UnitDsc::operator!= (UnitKey& s) {return s != key;}
bool UnitDsc::operator>  (UnitKey& s) {return s <  key;}
bool UnitDsc::operator>= (UnitKey& s) {return s <= key;}


bool UnitKey::operator== (UnitKey& k) {return  dir == k.dir && _tcsicmp(path, k.path) == 0;}
bool UnitKey::operator!= (UnitKey& k) {return  dir != k.dir || _tcsicmp(path, k.path) != 0;}

bool UnitKey::operator<  (UnitKey& k)
                          {return  dir > k.dir || (dir == k.dir && _tcsicmp(path, k.path) <  0);}
bool UnitKey::operator<= (UnitKey& k)
                          {return (dir > k.dir || (dir == k.dir && _tcsicmp(path, k.path) <= 0));}

bool UnitKey::operator>  (UnitKey& k)
                          {return  dir < k.dir || (dir == k.dir && _tcsicmp(path, k.path) >  0);}
bool UnitKey::operator>= (UnitKey& k)
                          {return (dir < k.dir || (dir == k.dir && _tcsicmp(path, k.path) >= 0));}



