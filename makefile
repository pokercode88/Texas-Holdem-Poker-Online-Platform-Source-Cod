
#-----------------------------------------------------------------------

APP        := XGame
TARGET     := Log2DBServer
MFLAGS     :=
DFLAGS     :=
CONFIG     := 
STRIP_FLAG := N
J2CPP_FLAG := 
CFLAGS     += -lm
CXXFLAGS   += -lm

INCLUDE   += -I/usr/local/cpp_modules/wbl/include
LIB       += -L/usr/local/cpp_modules/wbl/lib -lwbl

INCLUDE   += -I/usr/local/mysql/include
LIB       += -L/usr/local/mysql/lib/mysql -lmysqlclient

INCLUDE   += -I/usr/local/cpp_modules/protobuf/include
LIB       += -L/usr/local/cpp_modules/protobuf/lib -lprotobuf

#-----------------------------------------------------------------------
include /home/tarsproto/XGame/Comm/Comm.mk
include /usr/local/tars/cpp/makefile/makefile.tars

xgame:
	cp -f $(TARGET) /usr/local/app/tars/tarsnode/data/XStats.Log2DBServer/bin/

100:
	sshpass -p 'awzs2022' scp ./Log2DBServer root@10.10.10.100:/home/yuj/server/log2dbserver
#-----------------------------------------------------------------------
