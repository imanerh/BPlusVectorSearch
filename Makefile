# Compiler and flags
CXX = g++
CXXFLAGS = -g -w 
# -O2 -fno-omit-frame-pointer

# Sources and output
SOURCES = main.cpp BPTree.cpp Search.cpp Display.cpp KNN/knn.cpp
OUTPUT = main

# Default rule
all: $(OUTPUT)

# Linking and compiling
$(OUTPUT): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(OUTPUT)

# Run the program and redirect errors
run: $(OUTPUT)
	.\$(OUTPUT) $(SOURCE_PATH) $(QUERY_PATH) $(KNN_SAVE_PATH) 2> logs/errors.txt

# Clean up
clean:
	del /f $(OUTPUT).exe logs\errors.txt
