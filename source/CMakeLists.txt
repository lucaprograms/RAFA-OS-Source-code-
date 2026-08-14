cmake_minimum_required(VERSION 3.16)
project(MyEfiApplication C)

# Read HOME from environment and sanity-check it
if(NOT DEFINED ENV{HOME})
  message(FATAL_ERROR "Environment variable HOME is not set; cannot expand ~")
endif()
set(MY_HOME $ENV{HOME})
message(STATUS "Expanded HOME -> ${MY_HOME}")

# FIXED: Include paths mapped to your cloned ~/gnu-efi location
set(INC_FLAGS
  "-I${MY_HOME}/gnu-efi/inc"
  "-I${MY_HOME}/gnu-efi/inc/x86_64"
  "-I${MY_HOME}/gnu-efi/inc/protocol"
  "-I${CMAKE_SOURCE_DIR}"
)

# EFI compile flags
set(EFI_CFLAGS
  "-Wno-error=pragmas" "-mno-red-zone" "-mno-avx" "-fPIE" "-g" "-O2"
  "-Wall" "-Wextra" "-Wno-pointer-sign" "-funsigned-char"
  "-fshort-wchar" "-fno-strict-aliasing" "-ffreestanding"
  "-fno-stack-protector" "-fno-stack-check" "-fno-merge-all-constants"
  "-DCONFIG_x86_64" "-std=c11" "-DGNU_EFI_USE_MS_ABI"
  "-maccumulate-outgoing-args" "-D__KERNEL__"
)

# Tools (prefer CMake-provided variables when available)
find_program(GCC_EXECUTABLE gcc)
if(NOT GCC_EXECUTABLE)
  set(GCC_EXECUTABLE "gcc") # fallback
endif()

find_program(OBJCOPY_EXECUTABLE objcopy)
if(NOT OBJCOPY_EXECUTABLE)
  set(OBJCOPY_EXECUTABLE "objcopy")
endif()

# Define multi-file source structure containing your calculator modules
set(SRC_FILES 
  ${CMAKE_SOURCE_DIR}/main.c
  ${CMAKE_SOURCE_DIR}/drivers.c
  ${CMAKE_SOURCE_DIR}/printf.c
  ${CMAKE_SOURCE_DIR}/calc.c
  ${CMAKE_SOURCE_DIR}/help.c
  ${CMAKE_SOURCE_DIR}/inputboxes.c
  ${CMAKE_SOURCE_DIR}/clock.c
  ${CMAKE_SOURCE_DIR}/resource.c
  ${CMAKE_SOURCE_DIR}/debug.c
  ${CMAKE_SOURCE_DIR}/error.c
  ${CMAKE_SOURCE_DIR}/execute.c
  ${CMAKE_SOURCE_DIR}/test.S
)
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(SO_FILE   ${BUILD_DIR}/main.so)
set(EFI_FILE  ${BUILD_DIR}/BOOTX64.EFI)

# FIXED: Structural locations matched directly to standard built gnu-efi outputs
set(CRT0 "${MY_HOME}/gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o")
set(LD_SCRIPT "${MY_HOME}/gnu-efi/gnuefi/elf_x86_64_efi.lds")
set(LIB_DIR1 "${MY_HOME}/gnu-efi/x86_64/lib")
set(LIB_DIR2 "${MY_HOME}/gnu-efi/x86_64/gnuefi")
set(LIBGCC "/usr/lib/gcc/x86_64-linux-gnu/15/libgcc.a")

# ----------------------------------------------------------------------------
# 1. Loop and Compile: Each source file -> its respective object file (.o)
set(OBJ_FILES "")
foreach(SRC_FILE ${SRC_FILES})
    # Extract the plain name (main or drivers) to generate build paths
    get_filename_component(FILE_NAME ${SRC_FILE} NAME_WE)
    set(OBJ_FILE ${BUILD_DIR}/${FILE_NAME}.o)
    
    add_custom_command(
      OUTPUT ${OBJ_FILE}
      COMMAND ${GCC_EXECUTABLE} ${INC_FLAGS} ${EFI_CFLAGS} -c ${SRC_FILE} -o ${OBJ_FILE}
      DEPENDS ${SRC_FILE} ${CMAKE_SOURCE_DIR}/main.h ${CMAKE_SOURCE_DIR}/drivers.h
      COMMENT "Compiling ${SRC_FILE} to ${OBJ_FILE}"
      VERBATIM
    )
    list(APPEND OBJ_FILES ${OBJ_FILE})
endforeach()

# 2. Link: Pass BOTH generated .o files into the final shared library translation block

# 2. Link: Pass BOTH generated .o files into the final shared library translation block
add_custom_command(
  OUTPUT ${SO_FILE}
  COMMAND ${CMAKE_LINKER} -nostdlib -entry _start --warn-common --no-undefined --fatal-warnings --build-id=sha1 -z nocombreloc -shared -Bsymbolic
          -L${LIB_DIR1} -L${LIB_DIR2}
          ${CRT0} ${OBJ_FILES} -o ${SO_FILE} -lefi -lgnuefi ${LIBGCC} -T ${LD_SCRIPT}
  DEPENDS ${OBJ_FILES}
  COMMENT "Linking objects -> ${SO_FILE}"
  VERBATIM
)

# 3. Objcopy: rafacalc.so -> rafacalc.efi
add_custom_command(
  OUTPUT ${EFI_FILE}
  COMMAND ${OBJCOPY_EXECUTABLE}
          -j .text -j .sdata -j .rodata -j .data -j .dynamic -j .dynsym
          -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc
          --input-target=elf64-x86-64
          --output-target=efi-app-x86_64
          --subsystem=10
          ${SO_FILE} ${EFI_FILE}
  DEPENDS ${SO_FILE}
  COMMENT "Converting ${SO_FILE} -> ${EFI_FILE} (keeping .rodata)"
  VERBATIM
)

# Top-level target
add_custom_target(rafacalc_efi ALL
  DEPENDS ${EFI_FILE}
  COMMENT "Build rafacalc.efi"
)

# Optional: prints useful verification commands after build
add_custom_command(TARGET rafacalc_efi POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "Verify: file ${EFI_FILE}"
  COMMAND ${CMAKE_COMMAND} -E echo "Strings in .so:"
  COMMAND sh -c "strings -el ${SO_FILE} | head -n 20 || true"
  COMMAND ${CMAKE_COMMAND} -E echo "Strings in .efi:"
  COMMAND sh -c "strings -el ${EFI_FILE} | head -n 20 || true"
  VERBATIM
)

