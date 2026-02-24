set(COMPONENT_NAME 03_class_mocks)       #The name of the component

idf_component_register(                 #Register the component
    SRCS 
        "src/sum.cpp"                   #The source file
        "src/sum_boss.cpp"              #The source file
    
    INCLUDE_DIRS 
        "include"                       #The include directories

    REQUIRES 
        driver                          # Required to esp_err_t and ESP_LOGx
    
)