
if (NOT EXISTS "C:/Users/65193/Desktop/assignment-4-environment-mapping-and-shadow-mapping-HaotianWu1/cmake-build-debug/glfw/install_manifest.txt")
    message(FATAL_ERROR "Cannot find install manifest: \"C:/Users/65193/Desktop/assignment-4-environment-mapping-and-shadow-mapping-HaotianWu1/cmake-build-debug/glfw/install_manifest.txt\"")
endif()

file(READ "C:/Users/65193/Desktop/assignment-4-environment-mapping-and-shadow-mapping-HaotianWu1/cmake-build-debug/glfw/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")

foreach (file ${files})
  message(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
  if (EXISTS "$ENV{DESTDIR}${file}")
    exec_program("C:/Users/65193/AppData/Local/JetBrains/Toolbox/apps/CLion/ch-0/213.6777.58/bin/cmake/win/bin/cmake.exe" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
                 OUTPUT_VARIABLE rm_out
                 RETURN_VALUE rm_retval)
    if (NOT "${rm_retval}" STREQUAL 0)
      MESSAGE(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
    endif()
  elseif (IS_SYMLINK "$ENV{DESTDIR}${file}")
    EXEC_PROGRAM("C:/Users/65193/AppData/Local/JetBrains/Toolbox/apps/CLion/ch-0/213.6777.58/bin/cmake/win/bin/cmake.exe" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
                 OUTPUT_VARIABLE rm_out
                 RETURN_VALUE rm_retval)
    if (NOT "${rm_retval}" STREQUAL 0)
      message(FATAL_ERROR "Problem when removing symlink \"$ENV{DESTDIR}${file}\"")
    endif()
  else()
    message(STATUS "File \"$ENV{DESTDIR}${file}\" does not exist.")
  endif()
endforeach()

