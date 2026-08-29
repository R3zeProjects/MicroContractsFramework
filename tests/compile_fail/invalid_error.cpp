#include <vosp/contracts/error.hpp>

static_assert(vosp::contracts::Error<int>,
              "an integer must not satisfy the owning Error contract");

int main() {}
