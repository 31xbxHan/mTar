#include "mtar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAR_BLOCK_SIZE 512

typedef struct {
  uint8_t name[100];
  uint8_t mode[8];
  uint8_t owner[8];
  uint8_t group[8];
  uint8_t size[12];
  uint8_t mtime[12];
  uint8_t checkSum[8];
  uint8_t fileType;
  uint8_t linkName[100];
  uint8_t magic[6];
  uint8_t version[2];
  uint8_t uname[32];
  uint8_t gname[32];
  uint8_t devmajor[8];
  uint8_t devminor[8];
  uint8_t prefix[155];
  uint8_t padding[12];
} TarHeader;

// 解析文件大小（八进制转十进制）
static uint32_t ParseOctal(const uint8_t *field, size_t size) {
  uint32_t result = 0;
  for (size_t i = 0; i < size && field[i] != '\0'; i++) {
    if (field[i] < '0' || field[i] > '7') {
      fprintf(stderr, "Invalid octal digit: 0x%02X\n", field[i]);
      return 0;
    }
    result = result * 8 + (field[i] - '0');
  }
  return result;
}

// 读取并解压单个文件
static int ExtractFile(FILE *fp_src, const TarHeader *header,
                       const char *outputDirPath) {
  uint32_t size = ParseOctal(header->size, sizeof(header->size));
  if (size == 0) {
    fprintf(stderr, "Invalid file size for file: %s\n", header->name);
    return -1;
  }

  // 构建输出文件路径
  char filePath[256];
  snprintf(filePath, sizeof(filePath), "%s/%s", outputDirPath, header->name);

  // 打开目标文件
  FILE *fp_dst = fopen(filePath, "wb");
  if (fp_dst == NULL) {
    perror("fopen");
    return -1;
  }

  // 写入文件数据
  uint8_t buffer[TAR_BLOCK_SIZE];
  while (size > 0) {
    size_t toRead = size > TAR_BLOCK_SIZE ? TAR_BLOCK_SIZE : size;
    fread(buffer, 1, TAR_BLOCK_SIZE, fp_src);
    fwrite(buffer, 1, toRead, fp_dst);
    size -= toRead;
  }

  fclose(fp_dst);
  return 0;
}

// 解压 TAR 文件
int UnTar(const char *tarPath, const char *outputDirPath) {
  FILE *fp_src = fopen(tarPath, "rb");
  if (fp_src == NULL) {
    perror("fopen");
    return -1;
  }

  TarHeader header;
  while (fread(&header, 1, TAR_BLOCK_SIZE, fp_src) == TAR_BLOCK_SIZE) {
    if (header.name[0] == '\0') {
      break; // 结束
    }

    if (ExtractFile(fp_src, &header, outputDirPath) != 0) {
      fclose(fp_src);
      return -1;
    }
  }

  fclose(fp_src);
  return 0;
}