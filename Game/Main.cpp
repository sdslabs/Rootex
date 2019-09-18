#include <common.h>

int main()
{
    print("Print");
    PANIC(false, "PrintIf");
    WARN("PrintW");
    ERR("PrintE");
    printLine("PrintLine");
    std::cin.get();

	return 0;
}
