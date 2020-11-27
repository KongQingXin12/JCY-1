#include "functions.h"

functions::functions()
{}

long functions::HexToInt(string hex)
{
    long ans = 0;

    for (int i = hex.size() - 1; i >= 0; i--) {
        if ((hex[i] >= '0') && (hex[i] <= '9')) {
            ans += (hex[i] - '0') * pow(16, hex.size() - 1 - i);
        } else {
            ans += (hex[i] - 'a' + 10) * pow(16, hex.size() - 1 - i);
        }
    }
    return ans;
}
