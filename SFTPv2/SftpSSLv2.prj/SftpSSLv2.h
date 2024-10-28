// SSH FTP Interface


#pragma once
#include "SftpDataIter.h"

class Archive;
class Date;


enum SftpIO {NilSftpIO, ListSftpIO, PutSftpIO, GetSftpIO};


class SftpSSL {
WSADATA wsa;

public:

       SftpSSL();
      ~SftpSSL() {WSACleanup();}

  // SFTP Commands

  bool login(TCchar* host, TCchar* userId, TCchar* password);

  bool avbl(String& avail);
  bool stat(String& rslt);
  bool noop();

  bool pwd( String& rslt);
  bool cwd( TCchar* dir);                                 // Change Working Directory
  bool mkd( TCchar* dir);                                 // Make Directory
  bool rmd( TCchar* dir);                                 // Remove Directory
  bool rmda(TCchar* dir);                                 // Remove Directory Tree
  bool del(TCchar* webPath);                              // Delete File

  bool list(TCchar* webPath, SftpStore& store);           // Load directory for path into Transport
                                                          // Store, args may be  -l and/or -a
  bool size(TCchar* path, int&  size);                    // File size when true
  bool date(TCchar* path, Date& val);                     // File last modified date when true


  // Sending a file to the web and Retrieving a file from the web is performed in three steps:
  //   *  Open Transport buffer, specifying path and io to be done
  //   *  Loading a local file into a transport buffer or
  //   *  Reading a remote file into a transport buffer
  //
  //   *  Writing the content of the buffer to the remote host or
  //   *  Storing the content of the buffer in the local PC
  //
  //   *  Closing the Transport

  bool openTransport(SftpIO io, TCchar* webPath);         // Open Transport for a transfer, clears
                                                          // store
  void load(Archive& ar);                                 // load transport buffer from a local
                                                          // file
  void store(Archive& ar);                                // Store file in the sftpTransport buffer
                                                          // to local file
  bool writeTransport();                                  // Write from store to web host
  bool readTransport();                                   // Read from web host into store
  void closeTransport();                                  // Close Transport transaction


  bool stou(TCchar* webPath, String& fileName);           // copy transport buffer to unique file
                                                          // name in current directory
  String getName();                                       // Get Unique Name after opening
                                                          // StouSftpIO transport transaction
  bool append(TCchar* webPath);                           // Append sftpTransport buffer to web
                                                          // host file


  void close();                                           // Close SftpSSL

  void openSSLThreadStop();

  String& firstResp();
  String& lastResp();

private:

  bool open(TCchar* host);
  };


extern SftpSSL sftpSSL;



