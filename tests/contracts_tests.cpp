#include <vosp/contracts.hpp>

#include <cassert>
#include <stdexcept>
#include <string>
#include <unordered_set>

int main()
{
    using namespace vosp::error;

    const Error error{Category::DATABASE, 42, "transaction failed"};
    assert(error.category() == Category::DATABASE);
    assert(error.code() == 42);
    assert(error.message() == "transaction failed");
    assert(error.has_category());
    assert(to_string(error) == "[DATABASE:42] transaction failed");

    std::unordered_set<Error, ErrorHash> errors;
    errors.insert(error);
    assert(errors.contains(error));

    const auto success = attempt(Error{Category::NONE, 1, "failure"}, [] { return 7; });
    assert(success && *success == 7);

    const auto failure = attempt(Error{Category::FILESYSTEM, 2, "write"}, []() -> int {
        throw std::runtime_error{"denied"};
    });
    assert(!failure);
    assert(failure.error().message() == "write: denied");

    const OperationResult operation{};
    assert(operation);
}

