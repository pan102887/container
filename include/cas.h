#define A(result_type, header)

#define B(result_type, header) \
    A(result_type, header) \ 
    if (header->has_async_exception_condition()) {return (result_type)0; }