# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "E:/esp-idf/container/v4.4.7/esp-idf/components/bootloader/subproject"
  "E:/Workspace/test/inmp411_audio_to_service/build/bootloader"
  "E:/Workspace/test/inmp411_audio_to_service/build/bootloader-prefix"
  "E:/Workspace/test/inmp411_audio_to_service/build/bootloader-prefix/tmp"
  "E:/Workspace/test/inmp411_audio_to_service/build/bootloader-prefix/src/bootloader-stamp"
  "E:/Workspace/test/inmp411_audio_to_service/build/bootloader-prefix/src"
  "E:/Workspace/test/inmp411_audio_to_service/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "E:/Workspace/test/inmp411_audio_to_service/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
