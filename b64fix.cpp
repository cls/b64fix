#include <array>
#include <charconv>
#include <deque>
#include <iostream>

namespace {

const std::array<unsigned char, 64> Base64 = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/',
};

class Base64Queue
{
private:
    std::deque<unsigned char> bytes_;
    int state_;

public:
    Base64Queue() :
        state_(0)
    {}

    bool empty() const
    {
        return bytes_.empty() && state_ == 0;
    }

    void push_back(unsigned char c)
    {
        bytes_.push_back(c);
    }

    unsigned char pop_front()
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
            __builtin_unreachable();
        }

        return Base64[i];
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
    const std::string arg(argv[1]);
    auto [ptr, ec] = std::from_chars(&*arg.begin(), &*arg.end(), n);

    if (ec != std::errc()) {
        std::cerr << "b64fix: Failed to parse count: " << arg << std::endl;
        return 2;
    }

    Base64Queue b64q;

    size_t i = 0;

    const std::array<unsigned char, 3> seed = {'V', 'm', '0'};

    for (; i < seed.size() && i < n; ++i) {
        b64q.push_back(seed[i]);
        const auto c = b64q.pop_front();
        std::cout.put(c);
    }

    for (; i < n; ++i) {
        const auto c = b64q.pop_front();
        std::cout.put(c);
        b64q.push_back(c);
    }

    while (!b64q.empty()) {
        const auto c = b64q.pop_front();
        std::cout.put(c);
    }

    std::cout.flush();

    return 0;
}
