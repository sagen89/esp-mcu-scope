#include "converter.h"

static union {
  int64_t value;
  uint8_t bytes[sizeof(int64_t)];
} bytes_and_int64;

static union {
  int32_t value;
  uint8_t bytes[sizeof(int32_t)];
} bytes_and_int32;

static union {
  int32_t value;
  uint8_t bytes[sizeof(int32_t)];
} int32_and_bytes;


int64_t  converter_bytes_to_int64(uint8_t * bytes) {
  for (int8_t i = 0; i < sizeof(int64_t); i++) {
    bytes_and_int64.bytes[sizeof(int64_t) - 1 - i] = bytes[i];
  }
  return bytes_and_int64.value;
}

int32_t converter_bytes_to_int32(uint8_t * bytes) {
  for (uint8_t i = 0; i < sizeof(int32_t); i++) {
    bytes_and_int32.bytes[sizeof(int32_t) - 1 - i] = bytes[i];
  }
  return bytes_and_int32.value;
}

void converter_int32_to_bytes(int32_t val, uint8_t * bytes) {
  int32_and_bytes.value = val;
  for(int8_t i = 0; i < sizeof(uint32_t); i++) {
    bytes[i] = int32_and_bytes.bytes[sizeof(int32_t) - 1 - i];
  }
}

