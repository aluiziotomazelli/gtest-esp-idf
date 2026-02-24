idf_component_register(
    SRCS 
        "main.cpp"

    REQUIRES 
        "03_class_mocks"   #The component being tested
    PRIV_REQUIRES
        driver

)