# INFORMATION ON SAT SOLVER #
IPASIRSOLVER ?= cadical
IPASIRLIBDIR = lib/cadical-2.1.0/build/
# ========================= #
# INFORMATION ON c++ compiler #
CC	=	clang++
# =========================== #

TARGET=taas-banks
CFLAGS	?=	-Wall -DNDEBUG -O3 -std=c++0x
LINK	=	$(CC) $(CFLAGS)

all: $(TARGET)
clean:
	rm -f $(TARGET) *.o

taas-banks: taas-banks.o
	${LINK} -o $@ taas-fudge.o `pkg-config --libs glib-2.0` -lm -L$(IPASIRLIBDIR) -l$(IPASIRSOLVER)

taas-banks.o: taas-banks.cpp ipasir.h
	$(CC) $(CFLAGS) -c taas-banks.cpp `pkg-config --cflags glib-2.0`
