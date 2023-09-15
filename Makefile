EXE=connectFour
RM=rm -f

CPPFLAGS=-std=c++20
ifeq ($(opt),no)
    CXXFLAGS=-Wall -Wextra -g3 -O0 -fsanitize=thread -std=c++20
else
  ifeq ($(opt),nolto)
    CXXFLAGS=-Wall -Wextra -g3 -O3 -fsanitize=address -std=c++20
  else
    CXXFLAGS=-Wall -Wextra -g3 -O3 -flto=auto -std=c++20
  endif
endif
LDFLAGS=$(CXXFLAGS)


SRCS=main.cpp movemaker.cpp drawboard.cpp evaluation.cpp search.cpp\
	 transpositiontable.cpp movesorter.cpp protocol.cpp perft.cpp

OBJS=$(subst .cpp,.o,$(SRCS))

$(EXE): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(EXE) $(OBJS) 

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^ >>./.depend;

clean:
	$(RM) $(OBJS) $(EXE) .depend

distclean: clean
	$(RM) *~ .depend

include .depend
