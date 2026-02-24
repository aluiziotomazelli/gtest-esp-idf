idf_component_register(
    SRCS 
        "main.cpp"              #The main file
        "test_sum.cpp"          #The test file
        "test_sum_param.cpp"    #The parameterized test file
        "test_led_sargent.cpp"  #The led_sargent test file
        "test_sum_boss.cpp"     #The sum_boss test file
    INCLUDE_DIRS 
        "."
    REQUIRES 
        gtest                   #The GTest wrapper
        04_hal_and_leds         #The component being tested
        
    WHOLE_ARCHIVE               # Force the linker to include all object files.
                                # Without this, test_sum.cpp might be skipped as they aren't explicitly called in main.cpp.
)