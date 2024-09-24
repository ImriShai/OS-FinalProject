# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++17 -Werror -Wsign-conversion
MEMCHECK_FLAGS = -v --leak-check=full --show-leak-kinds=all --error-exitcode=99
CACHEGRIND_FLAGS = -v --error-exitcode=99
HELGRIND_FLAGS = -v --error-exitcode=99

# Source files
graphSrc = $(wildcard GraphObj/*.cpp)
MSTSrc = $(wildcard MST/*.cpp)
DATASTRUCTSrc = $(wildcard DataStruct/*.cpp) DataStruct/BinaryHeap.hpp
UTILSrc = $(wildcard ServerUtils/*.cpp)


lf-serverSrc = LF-Server.cpp LFP/LFP.cpp 
PAO = PAO-server.cpp PAO/PAO.cpp


# Object files
LF-OBJ = $(graphSrc:.cpp=.o) $(lf-serverSrc:.cpp=.o) $(MSTSrc:.cpp=.o) $(DATASTRUCTSrc:.cpp=.o) $(UTILSrc:.cpp=.o)
PAO-OBJ = $(graphSrc:.cpp=.o) $(PAO:.cpp=.o) $(MSTSrc:.cpp=.o) $(DATASTRUCTSrc:.cpp=.o) $(UTILSrc:.cpp=.o)

.PHONY: all  pao-server valgrind clean
all: lf-server pao-server 

# Valgrind tools: we will check creating 3 graphs and 3 MSTs
# LF - Memory check:
lf-memcheck: lf-server
	@echo "Starting lf-server with Valgrind memcheck..."
	@trap 'kill $$valgrind_pid' SIGINT; \
	valgrind --tool=memcheck $(MEMCHECK_FLAGS ) ./lf-server 2>&1 | tee Valgrind-reports/lf-server_memcheck.txt & \
	valgrind_pid=$$!; \
	wait $$valgrind_pid

# LF - CacheGrind:
lf-cachegrind: lf-server
	@echo "Starting lf-server with Valgrind cachegrind..."
	@trap 'kill $$valgrind_pid' SIGINT; \
	valgrind --tool=cachegrind $(CACHEGRIND_FLAGS) ./lf-server 2>&1 | tee Valgrind-reports/lf-server_cachegrind.txt & \
	valgrind_pid=$$!; \
	wait $$valgrind_pid

# LF  - Helgrind:
lf-helgrind: lf-server
	@echo "Starting lf-server with Valgrind helgrind..."
	@trap 'kill $$valgrind_pid' SIGINT; \
	valgrind --tool=helgrind $(HELGRIND_FLAGS) ./lf-server 2>&1 | tee Valgrind-reports/lf-server_helgrind.txt & \
	valgrind_pid=$$!; \
	wait $$valgrind_pid

# PAO - Memory check:
pao-memcheck: pao-server
	@echo "Starting pao-server with Valgrind memcheck..."
	@trap 'kill $$valgrind_pid' SIGINT; \
	valgrind --tool=memcheck $(MEMCHECK_FLAGS) ./pao-server 2>&1 | tee Valgrind-reports/pao-server_memcheck.txt & \
	valgrind_pid=$$!; \
	wait $$valgrind_pid

# PAO - CacheGrind for pao-server:
pao-cachegrind: pao-server
	@echo "Starting pao-server with Valgrind cachegrind..."
	@trap 'kill $$valgrind_pid' SIGINT; \
	valgrind --tool=cachegrind $(CACHEGRIND_FLAGS) ./pao-server 2>&1 | tee Valgrind-reports/pao-server_cachegrind.txt & \
	valgrind_pid=$$!; \
	wait $$valgrind_pid

# PAO - Helgrind for pao-server:
pao-helgrind: pao-server
	@echo "Starting pao-server with Valgrind helgrind..."
	@trap 'kill $$valgrind_pid' SIGINT; \
	valgrind --tool=helgrind $(HELGRIND_FLAGS) ./pao-server 2>&1 | tee Valgrind-reports/pao-server_helgrind.txt & \
	valgrind_pid=$$!; \
	wait $$valgrind_pid

# Build target

lf-server: $(LF-OBJ)
	$(CC) $(CFLAGS) $(LF-OBJ)  -o lf-server

pao-server: $(PAO-OBJ)
	$(CC) $(CFLAGS) $(PAO-OBJ)  -o pao-server

# Compile source files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f *.o GraphObj/*.o MST/*.o DataStruct/*.o lf-server PAO-server PIPE/*.o LFP/*.o ServerUtils/*.o PAO/*.o pao-server 
