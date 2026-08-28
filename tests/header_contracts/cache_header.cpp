#include <vosp/contracts/cache.hpp>

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>

namespace
{
struct CachePrototype
{
    using key_type = int;
    using mapped_type = std::string;

    [[nodiscard]] std::optional<mapped_type> get(const key_type &key)
    {
        const auto found = values.find(key);
        return found == values.end() ? std::nullopt : std::optional<mapped_type>{found->second};
    }

    void put(const key_type &key, mapped_type value)
    {
        values.insert_or_assign(key, std::move(value));
    }

    [[nodiscard]] bool contains(const key_type &key) { return values.contains(key); }
    [[nodiscard]] bool erase(const key_type &key) { return values.erase(key) != 0; }
    [[nodiscard]] std::size_t size() { return values.size(); }
    [[nodiscard]] std::size_t capacity() const { return 16; }
    [[nodiscard]] std::size_t purge_expired() { return 0; }
    void clear() { values.clear(); }

    std::unordered_map<key_type, mapped_type> values;
};

static_assert(vosp::contracts::KeyValueCache<CachePrototype>);
} // namespace
