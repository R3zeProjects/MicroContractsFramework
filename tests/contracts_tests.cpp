#include <vosp/contracts.hpp>

#include <cassert>

namespace
{
class CountingSink final : public vosp::logger::ILogSink
{
  public:
    [[nodiscard]] bool write(const vosp::logger::LogEntry &) override
    {
        ++count;
        return true;
    }

    int count = 0;
};
} // namespace

int main()
{
    using namespace vosp::error;

    const Error error{Category::DATABASE, 42, "transaction failed"};
    assert(error.category() == Category::DATABASE);
    assert(error.code() == 42);
    assert(error.message() == "transaction failed");
    assert(error.has_category());

    const OperationResult operation{};
    assert(operation);

    CountingSink sink;
    const vosp::logger::LogEntry entry{
        .level = vosp::logger::Level::WARNING,
        .error = Error{Category::FILESYSTEM, 3, "persist"},
    };
    assert(sink.write(entry));
    assert(sink.count == 1);
    assert(entry.level == vosp::logger::Level::WARNING);
}
