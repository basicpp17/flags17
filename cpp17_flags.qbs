import qbs

Project {
    minimumQbsVersion: "1.7.1"

    StaticLibrary {
        name: "flags"
        Depends { name: "cpp" }
        cpp.cxxLanguageVersion: "c++17"
        cpp.includePaths: ["."]
        cpp.cxxFlags: {
            if (qbs.toolchain.contains('msvc')) return "/await";
            if (qbs.toolchain.contains('clang')) return ["-fcoroutines-ts"];
        }
        cpp.cxxStandardLibrary: {
            if (qbs.toolchain.contains('clang')) return "libc++";
        }

        files: [
            "bitnumber/BitNumberFlags.cpp",
            "bitnumber/Flags.h",
            "classic/ClassicFlags.cpp",
            "classic/Flags.h",
            "meta/Name.cpp",
            "meta/Name.h",
            "meta/check.cpp",
            "meta/check.h",
            "meta/details/BitIntrinsics.cpp",
            "meta/details/BitIntrinsics.h",
            "meta/Type.cpp",
            "meta/Type.h",
            "meta/TypeList.cpp",
            "meta/TypeList.h",
            "meta/Value.cpp",
            "meta/Value.h",
            "meta/ValueList.cpp",
            "meta/ValueList.h",
            "meta/details/BitStorage.cpp",
            "meta/details/BitStorage.h",
            "repeated/Flags.h",
            "repeated/RepeatedFlags.cpp",
            "tagtype/Flags.h",
            "tagtype/TagTypeFlags.cpp",
            "tagvalue/Flags.h",
            "tagvalue/TagValueFlags.cpp",
        ]

        Export {
            Depends { name: "cpp" }
            cpp.cxxLanguageVersion: "c++17"
            cpp.includePaths: ["."]
            cpp.cxxFlags: {
                if (qbs.toolchain.contains('msvc')) return "/await";
                if (qbs.toolchain.contains('clang')) return ["-fcoroutines-ts"];
            }
            cpp.cxxStandardLibrary: {
                if (qbs.toolchain.contains('clang')) return "libc++";
            }
            cpp.staticLibraries: {
                if (qbs.toolchain.contains('clang')) return ["c++", "c++abi"];
            }
        }
    }

    Application {
        name: "flags_tests"
        Depends { name: "flags" }
        consoleApplication: true
        Depends { name: "Qt.testlib" }

        files: [
            "flagstest.cpp"
        ]
    }

    Application {
        name: "flags_app"
        consoleApplication: true
        Depends { name: "flags" }
        files: [
            "main.cpp",
        ]
    }
}
