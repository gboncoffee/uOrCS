CPP = g++
LD = g++
CPPFLAGS = $(FLAGS)
BIN_NAME = uorcs
RM = rm -f

FLAGS =   -O3 -ggdb -Wall -Wextra -Werror -std=c++11
LDFLAGS = -ggdb

################################################################################

LIBRARY = -lz

SRC_PACKAGE = 		opcode_package.cpp 

SRC_TRACE_READER = 	trace_reader.cpp

SRC_PROCESSOR =	 	processor.cpp 

SRC_CORE =  simulator.cpp orcs_engine.cpp\
			$(SRC_TRACE_READER)	\
			$(SRC_PACKAGE) \
			$(SRC_PROCESSOR)

SRC_BTB = btb.cpp
OBJS_BTB = ${SRC_BTB:.cpp=.o}
SRC_PREDICTOR = predictor.cpp
OBJS_PREDICTOR = ${SRC_PREDICTOR:.cpp=.o}

################################################################################
OBJS_CORE = ${SRC_CORE:.cpp=.o}
OBJS = $(OBJS_CORE) $(OBJS_BTB) $(OBJS_PREDICTOR)
################################################################################
# Implicit rules.
%.o : %.cpp %.hpp
	$(CPP) -c $(CPPFLAGS) $< -o $@

################################################################################

all: uorcs

uorcs: $(OBJS)
	$(LD) $(LDFLAGS) -o $(BIN_NAME) $(OBJS) $(LIBRARY)

clean:
	-$(RM) $(OBJS)
	-$(RM) $(BIN_NAME)
	@echo OrCS cleaned!
	@echo
