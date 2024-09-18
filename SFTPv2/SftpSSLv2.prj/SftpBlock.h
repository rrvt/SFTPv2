// Sftp Block of Data


#pragma once


typedef char FtpBfr[1024];


class SftpBlock {
public:

int    curX;                              // Current Index
int    n;                                 // Number of bytes stored in buffer
FtpBfr bfr;

  SftpBlock() : curX(0), n(0) {bfr[0] = 0;}
  SftpBlock(SftpBlock& b) {copy(b);}
 ~SftpBlock() { }

  SftpBlock& operator= (SftpBlock& b) {copy(b); return *this;}

private:

  void copy(SftpBlock& b) {curX = b.curX; n = b.n; memcpy(bfr, b.bfr, sizeof(FtpBfr));}
  };





