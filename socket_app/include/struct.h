#include <string>
#include <vector>

namespace common
{
    struct NetData {
        std::string ip;
        std::string port;
        std::string mtu;
        std::string timeout;

        bool somethingIsEmpty() {
            return ip.empty() || port.empty() || mtu.empty() || timeout.empty();
        }

        bool emptyIpPort() {
            return ip.empty() || port.empty();
        }
    };

    inline bool inIpRange(int& num) {
        return num >= 0 && num <= 255;
    }

    inline bool inIpRange(std::vector<std::string>& nums) {
        for (auto& num : nums) {
            auto integer = std::stoi(num);
            if (!inIpRange(integer)) return false;
        }
        return true;
    }

    inline bool inIpRange(std::vector<int>& nums) {
        for (auto& num : nums)
            if (!inIpRange(num)) return false;
        return true;
    }

    inline bool inPortRange(int& num) {
        return num >= 0 && num <= 65535;
    }

    inline bool containsOnlyDigits(std::string& str) {
        // of course, it can be done clearly by iterating the string
        // not today though
        return !str.empty() && str.find_first_not_of("0123456789") == std::string::npos;
    }
}
