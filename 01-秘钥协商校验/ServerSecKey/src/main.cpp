#include <iostream>
#include <cstdio>
#include "ServerOP.h"

int main()
{
    ServerOP op("../conf/config.json");
    op.startServer();

    return 0;
}
