set(COMPONENT_NAME 01_basic_test)       #The name of the component

idf_component_register(                 #Register the component
    SRCS 
        "src/sum.cpp"                   #The source file
    
    INCLUDE_DIRS 
        "include"                       #The include directories

    REQUIRES 
        driver                          # Required to esp_err_t and ESP_LOGx
    
)