

SFTPv2.chm : *.htm SFTPv2.hhp SFTPv2.hhc App.css
  -1 "C:\Program Files (x86)\HTML Help Workshop\hhc" SFTPv2


Clean.cln    :
  @echo "Cleaning"
  cd $(MAKEDIR)
  del /F SFTPv2.chm


Release/SFTPv2.chm : SFTPv2.chm
  copy $(MAKEDIR)\SFTPv2.chm  $(MAKEDIR)\..\Release\SFTPv2.chm


Release/Clean.cln    : Clean.cln
  cd $(MAKEDIR)\..\Release
  del SFTPv2.chm
  cd $(MAKEDIR)


Debug/SFTPv2.chm : SFTPv2.chm
  copy $(MAKEDIR)\SFTPv2.chm  $(MAKEDIR)\..\Debug\SFTPv2.chm


Debug/Clean.cln    : Clean.cln
  cd $(MAKEDIR)\..\Debug
  del SFTPv2.chm
  cd $(MAKEDIR)


Release/Rebuild.rbd : Release/Clean.cln Release/SFTPv2.chm


Debug/Rebuild.rbd   : Debug/Clean.cln Debug/SFTPv2.chm


