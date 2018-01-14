
# a "hello world" example showing how to read one hardware performance counter
add_executable(total_instruction_count
    total_instruction_count/main.c)
add_test(NAME examples::total_instruction_count COMMAND total_instruction_count)

