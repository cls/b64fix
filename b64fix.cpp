#include <array>
#include <charconv>
#include <deque>
#include <iostream>
#include <optional>
#include <utility>

namespace {

constexpr std::array<uint8_t, 64> base64 = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/',
};

template <typename InputIt, typename UnaryPred>
constexpr InputIt find_if_unique(InputIt first, InputIt last, UnaryPred p)
{
    InputIt found = last;
    for (InputIt it = first; it != last; ++it) {
        if (p(*it)) {
            if (found != last) {
                return last;
            }
            found = it;
        }
    }
    return found;
}

using Seed = std::array<uint8_t, 3>;

constexpr std::optional<Seed> find_seed()
{
    const auto it0 = find_if_unique(base64.begin(), base64.end(), [&](uint8_t c) {
        return base64[c >> 2] == c;
    });
    if (it0 != base64.end()) {
        const auto it1 = find_if_unique(base64.begin(), base64.end(), [&](uint8_t c) {
            return base64[((*it0 << 4) % 64) | (c >> 4)] == c;
        });
        if (it1 != base64.end()) {
            const auto it2 = find_if_unique(base64.begin(), base64.end(), [&](uint8_t c) {
                return base64[((*it1 << 2) % 64) | (c >> 6)] == c;
            });
            if (it2 != base64.end()) {
                return Seed{*it0, *it1, *it2};
            }
        }
    }
    return std::nullopt;
}

static_assert(find_seed(), "Seed is expected to be found");

constexpr Seed seed = *find_seed();

static_assert(seed == Seed{'V', 'm', '0'}, "Seed is expected to be 'Vm0'");

class Base64Queue
{
private:
    std::deque<uint8_t> bytes_;
    int state_;

public:
    Base64Queue() :
        state_(0)
    {}

    bool empty() const
    {
        return bytes_.empty() && state_ == 0;
    }

    void push_back(uint8_t c)
    {
        bytes_.push_back(c);
    }

    uint8_t pop_front()
    {
        size_t i;

        switch (state_) {
        case 0:
            state_ = 1;
            i = bytes_.front() >> 2;
            break;
        case 1:
            state_ = 2;
            i = (bytes_.front() << 4) % 64;
            bytes_.pop_front();
            if (!bytes_.empty()) {
                i |= bytes_.front() >> 4;
            }
            break;
        case 2:
            state_ = 3;
            if (bytes_.empty()) {
                return '=';
            }
            i = (bytes_.front() << 2) % 64;
            bytes_.pop_front();
            if (!bytes_.empty()) {
                i |= bytes_.front() >> 6;
            }
            break;
        case 3:
            state_ = 0;
            if (bytes_.empty()) {
                return '=';
            }
            i = bytes_.front() % 64;
            bytes_.pop_front();
            break;
        default:
            std::unreachable();
        }

        return base64[i];
    }
};

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "usage: b64fix <count>" << std::endl;
        return 2;
    }

    size_t n;
    const std::string_view arg(argv[1]);
    auto [ptr, ec] = std::from_chars(&*arg.begin(), &*arg.end(), n);

    if (ec != std::errc()) {
        std::cerr << "b64fix: Failed to parse count: " << arg << std::endl;
        return 2;
    }

    Base64Queue b64q;

    size_t i = 0;

    for (; i < seed.size() && i < n; ++i) {
        b64q.push_back(seed[i]);
        const uint8_t c = b64q.pop_front();
        std::cout.put(c);
    }

    for (; i < n; ++i) {
        const uint8_t c = b64q.pop_front();
        std::cout.put(c);
        b64q.push_back(c);
    }

    while (!b64q.empty()) {
        const uint8_t c = b64q.pop_front();
        std::cout.put(c);
    }

    std::cout.flush();

    return 0;
}
