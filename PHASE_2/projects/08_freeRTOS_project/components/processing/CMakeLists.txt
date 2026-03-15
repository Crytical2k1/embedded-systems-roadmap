idf_component_register(
    SRCS "data_processor.c" "data_status_task.c"
    INCLUDE_DIRS "include"
    REQUIRES driver
)