// include/cleaner/system/retry.h

typedef struct
{
    int max_retries;
    int attempt;
} retry_policy_t;