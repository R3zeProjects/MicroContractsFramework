#include <vosp/contracts/error.hpp>

int main()
{
    const vosp::error::Result<int> result{42};
    return result && *result == 42 ? 0 : 1;
}

