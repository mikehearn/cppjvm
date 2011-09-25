
thirdparty_platform = win

compiler = cl
compiler_global_flags = -EHsc -nologo -c -DWIN32 -D_WINSOCKAPI_ -MD
compiler_object_name_prefix = -Fo
compiler_object_suffix = .obj

staticlib_suffix = .lib
static_linker = lib
static_linker_global_flags = -NOLOGO
static_linker_output_name_prefix = -OUT:

windows_thirdparty_dlls = $(notdir $(foreach tp,$(thirdparty_packages),$(wildcard $(tp)dll/*)))
windows_thirdparty_dlls_delayload = $(foreach tpd,$(windows_thirdparty_dlls),-DELAYLOAD:$(tpd))

executable_linker = link
executable_linker_global_flags = -INCREMENTAL:NO -NOLOGO Advapi32.lib ws2_32.lib Delayimp.lib -NODEFAULTLIB:libc.lib,libcmt.lib,libcd.lib,libcmtd.lib,msvcrtd.lib $(windows_thirdparty_dlls_delayload)
executable_linker_output_name_prefix = -OUT:
executable_linker_library_path_prefix = -LIBPATH:
executable_linker_library_name_prefix = 
executable_suffix = .exe

