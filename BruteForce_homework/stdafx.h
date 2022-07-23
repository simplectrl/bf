#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <exception>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <wctype.h>
#include <filesystem>
#include <execution>

#include "openssl/evp.h"
#include "openssl/aes.h"
#include "openssl/sha.h"
#include "openssl/conf.h"
#include "openssl/err.h"

using uchar = unsigned char;
using namespace std::chrono_literals;
namespace fs = std::filesystem;
