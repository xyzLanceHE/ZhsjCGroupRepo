本目录取自Openssl库的一部分:

[openssl/openssl: TLS/SSL and crypto library (github.com)](https://github.com/openssl/openssl)

包含多个平台上的libcrypto

目前只引入了sha.h头文件，用来计算SHA256。

如果引用了Share共享项目，就可以直接包含头文件。

但是lib下的库要单独添加到自己的项目里(选择需要的平台)

Windows系统下，体积较小的静态库还需要链接：

WS2_32.LIB, GDI32.LIB, ADVAPI32.LIB, CRYPT32.LIB, USER32.LIB