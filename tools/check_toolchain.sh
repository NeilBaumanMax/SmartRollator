#!/bin/sh

# Read-only SmartRollator Phase 1 toolchain probe.
# It prints only product names and versions, never discovered absolute paths.

set -u

overall_status=0

print_found() {
    tool_name=$1
    tool_version=$2
    printf '%s=FOUND version=%s\n' "$tool_name" "$tool_version"
}

print_missing() {
    tool_name=$1
    printf '%s=NOT_FOUND\n' "$tool_name"
    overall_status=1
}

version_from_command() {
    version_command=$1
    "$version_command" --version 2>&1 |
        sed -n 's/.*\([0-9][0-9]*\(\.[0-9][0-9]*\)\{1,3\}\).*/\1/p' |
        sed -n '1p'
}

find_first_command() {
    for command_name in "$@"; do
        if command -v "$command_name" >/dev/null 2>&1; then
            printf '%s\n' "$command_name"
            return 0
        fi
    done
    return 1
}

printf 'HOST_OS=%s\n' "$(uname -s)"
printf 'HOST_ARCH=%s\n' "$(uname -m)"

cubemx_version=
cubemx_command=$(find_first_command STM32CubeMX stm32cubemx 2>/dev/null || true)
if [ -n "$cubemx_command" ]; then
    cubemx_version=$(version_from_command "$cubemx_command" || true)
elif [ -d /Applications/STMicroelectronics/STM32Cube/STM32CubeMX.app ]; then
    if command -v plutil >/dev/null 2>&1; then
        cubemx_version=$(plutil -extract CFBundleShortVersionString raw \
            /Applications/STMicroelectronics/STM32Cube/STM32CubeMX.app/Contents/Info.plist \
            2>/dev/null || true)
    fi
elif [ -d /Applications/STM32CubeMX.app ]; then
    if command -v plutil >/dev/null 2>&1; then
        cubemx_version=$(plutil -extract CFBundleShortVersionString raw \
            /Applications/STM32CubeMX.app/Contents/Info.plist 2>/dev/null || true)
    fi
fi

if [ -n "$cubemx_command" ] || [ -n "$cubemx_version" ]; then
    [ -n "$cubemx_version" ] || cubemx_version=UNVERIFIED
    print_found STM32CUBEMX "$cubemx_version"
    if [ "$cubemx_version" = UNVERIFIED ]; then
        overall_status=1
    fi
else
    print_missing STM32CUBEMX
fi

mdk_command=$(find_first_command uv4 UV4.exe 2>/dev/null || true)
if [ -z "$mdk_command" ]; then
    for mdk_candidate in \
        /c/Keil_v5/UV4/UV4.exe \
        /mnt/c/Keil_v5/UV4/UV4.exe; do
        if [ -x "$mdk_candidate" ]; then
            mdk_command=$mdk_candidate
            break
        fi
    done
fi

if [ -n "$mdk_command" ]; then
    mdk_version=${SMARTROLLATOR_MDK_VERSION-}
    [ -n "$mdk_version" ] || mdk_version=UNVERIFIED
    print_found KEIL_MDK "$mdk_version"
    if [ "$mdk_version" = UNVERIFIED ]; then
        overall_status=1
    fi
else
    print_missing KEIL_MDK
fi

arm_command=$(find_first_command armclang armclang.exe armcc armcc.exe 2>/dev/null || true)
if [ -z "$arm_command" ]; then
    for arm_candidate in \
        /c/Keil_v5/ARM/ARMCLANG/bin/armclang.exe \
        /mnt/c/Keil_v5/ARM/ARMCLANG/bin/armclang.exe; do
        if [ -x "$arm_candidate" ]; then
            arm_command=$arm_candidate
            break
        fi
    done
fi

if [ -n "$arm_command" ]; then
    arm_version=$(version_from_command "$arm_command" || true)
    [ -n "$arm_version" ] || arm_version=UNVERIFIED
    print_found ARM_COMPILER "$arm_version"
    if [ "$arm_version" = UNVERIFIED ]; then
        overall_status=1
    fi
else
    print_missing ARM_COMPILER
fi

fw_h7_version=
for repository_root in \
    "$HOME/STM32Cube/Repository" \
    "$HOME/STM32CubeMX/Repository"; do
    for fw_h7_candidate in "$repository_root"/STM32Cube_FW_H7_V*; do
        if [ -d "$fw_h7_candidate" ]; then
            fw_h7_version=${fw_h7_candidate##*/STM32Cube_FW_H7_V}
        fi
    done
done

if [ -n "$fw_h7_version" ]; then
    print_found STM32CUBE_FW_H7 "$fw_h7_version"
else
    print_missing STM32CUBE_FW_H7
fi

if [ "$overall_status" -eq 0 ]; then
    printf 'TOOLCHAIN_STATUS=COMPLETE\n'
else
    printf 'TOOLCHAIN_STATUS=INCOMPLETE\n'
fi

exit "$overall_status"
