To use environment variables in the project, you need to create a `secrets.h` file in the `esp/include` directory.

The file should look like this:

```c
#define WIFI_SSID "YourNetworkName"
#define WIFI_PASSWORD "YourStrongPassword"
```

You can then use the `secrets.h` file in your code by including it with `#include "secrets.h"`.

The variables will be defined as macros and can be used in your code.
