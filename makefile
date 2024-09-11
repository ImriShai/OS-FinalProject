# Compiler
CC = clang++

# Compiler flags
CFLAGS = -std=c++17 -Werror -Wsign-conversion
VALGRIND_FLAGS=-v --leak-check=full --show-leak-kinds=all  --error-exitcode=99

# Source files
graphSrc = $(wildcard GraphObj/*.cpp)
MSTSrc = $(wildcard MST/*.cpp)
DATASTRUCTSrc = $(wildcard DataStruct/*.cpp)
UTILSrc = $(wildcard ServerUtils/*.cpp)

serverSrc = Server.cpp
lf-serverSrc = LF-Server.cpp LFP/LFP.cpp 
pipeServerSrc = Pipe-Server.cpp PIPE/PipelineThreadPool.cpp PIPE/Pipeline.cpp 


# Object files
OBJ = $(graphSrc:.cpp=.o) $(serverSrc:.cpp=.o) $(MSTSrc:.cpp=.o) $(DATASTRUCTSrc:.cpp=.o) $(UTILSrc:.cpp=.o)
LF-OBJ = $(graphSrc:.cpp=.o) $(lf-serverSrc:.cpp=.o) $(MSTSrc:.cpp=.o) $(DATASTRUCTSrc:.cpp=.o) $(UTILSrc:.cpp=.o)
PIPE-OBJ = $(graphSrc:.cpp=.o) $(pipeServerSrc:.cpp=.o) $(MSTSrc:.cpp=.o) $(DATASTRUCTSrc:.cpp=.o) $(UTILSrc:.cpp=.o)
TARGET = server


.PHONY: all server pipe-server valgrind clean
all: lf-server pipe-server 

# Memory check using valgrind
valgrind: server 
	valgrind --tool=memcheck $(VALGRIND_FLAGS) ./demo <demoInput.txt 2>&1 | { egrep -i "HEAP|at exit" || true; } 	

# Build target
server: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

lf-server: $(LF-OBJ)
	$(CC) $(CFLAGS) $(LF-OBJ)  -o lf-server

pipe-server: $(PIPE-OBJ)
	$(CC) $(CFLAGS) $(PIPE-OBJ)  -o pipe-server

# Compile source files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f *.o server GraphObj/*.o MST/*.o DataStruct/*.o lf-server pipe-server PIPE/*.o LFP/*.o ServerUtils/*.o
