idf_component_register(
    SRCS 
        "main.cpp"          #The main file
        "test_sum.cpp"      #The test file
    INCLUDE_DIRS 
        "."
    REQUIRES 
        gtest               #The GTest wrapper
        01_basic_test       #The component being tested
        
    WHOLE_ARCHIVE           # Force the linker to include all object files.
                            # Without this, test_sum.cpp might be skipped as they aren't explicitly called in main.cpp.
)