#ifndef MTAR_H
#define MTAR_H

#include <stdint.h>

// 解压 TAR 文件
int UnTar(const char *tarPath, const char *outputPath);

#endif // MTAR_H