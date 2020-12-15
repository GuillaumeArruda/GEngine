#include "gtl/span.h"
#include <vector>
int main()
{
    std::vector<int> d;
    int array[] = { 1,2,3,4,5,6 };
    gtl::span<int> s = array;
    return s[0];
}