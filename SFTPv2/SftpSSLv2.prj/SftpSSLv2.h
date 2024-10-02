// SSH FTP Interface


#pragma once
#include "SftpDataIter.h"

class Archive;
class Date;


class SftpSSL {
WSADATA wsa;

public:

       SftpSSL();
      ~SftpSSL() {WSACleanup();}

  // SFTP Commands

  bool open(TCchar* host);
  bool login(TCchar* userId, TCchar* password);

  bool avbl(String& avail);
  bool stat(String& rslt);
  bool noop();

  bool pwd( String& rslt);
  bool cwd( TCchar* dir);                                 // Change Working Directory
  bool mkd( TCchar* dir);                                 // Make Directory
  bool rmd( TCchar* dir);                                 // Remove Directory
  bool rmda(TCchar* dir);                                 // Remove Directory Tree
  bool del(TCchar* webPath);                              // Delete File

  bool list(TCchar* path, TCchar* args, SftpStore& store);// Load directory for path into Transport
                                                          // Store, args may be  -l and/or -a
  bool size(TCchar* path, int&  size);                    // File size when true
  bool date(TCchar* path, Date& val);                     // File last modified date when true


  // Sending a file to the web and Retrieving a file from the web is performed in three steps:
  //   *  Loading a local file into a transport buffer or
  //   *  Retrieving a remote file into a transport buffer
  //
  //   *  Storing the content of the buffer in the remote host or
  //   *  Storing the content of the buffer in the local PC
  //
  //   *  Closing the Transport

  void load(Archive& ar);                                 // load transport buffer from a local
                                                          // file
  bool stor(TCchar* webPath);                             // copy ransport buffer to web host file
  bool stou(TCchar* webPath, String& fileName);           // copy transport buffer to unique file
                                                          // name in current directory

  bool append(TCchar* webPath);                           // Append sftpTransport buffer to web
                                                          // host file
  bool retr(TCchar* webPath);                             // copy file from web host to
                                                          // sftpTransport buffer
  void store(Archive& ar);                                // Store file in the sftpTransport buffer
                                                          // to local file
  void closeTransport();                                  // Close Transport transaction

  void close();                                           // Close SftpSSL

  void openSSLThreadStop();

  String& firstResp();
  String& lastResp();
  };


extern SftpSSL sftpSSL;



//  void dspLines(String& s) {cmd.dspLines(s);}
//bool testStor(TCchar* webPath) {return cmd.testStor(webPath);}
//SftpTransport transport;

//SftpErr       err;

//  bool readPending();
//#include "SftpSocket.h"
//#include "SftpSSLi.h"
#if 0
  bool sendCmd(TCchar* cmmd, TCchar* args, String& response)
                                                        {return cmd.sendCmd(cmmd, args, response);}
  bool initPassiveMode(TCchar* cmmd, TCchar* arg)
                                           {return sftpTransport.initPassiveMode(cmmd, arg);}
#endif
//  String&        lastResp()                {return cmd.lastResp();}
#if 0
  // Transport Functions

//  SftpTransport& sftpTransport()           {return sftpTransport;}
  bool           getLocalFile(TCchar* src) {return sftpTransport.load(src);}
  bool           putLocalFile(TCchar* dst) {return sftpTransport.store(dst);}
  SftpStore&     fileData()                {return sftpTransport.sftpOps;}
#endif

