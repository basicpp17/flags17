import qbs

Project {
    minimumQbsVersion: "1.7.1"

    StaticLibrary {
        name: "000_meta"
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

    StaticLibrary {
        name: "002_classic"
        Depends { name: "000_meta" }
        files: [
            "classic/Flags.cpp",
            "classic/Flags.h",
        ]
    }

    StaticLibrary {
        name: "003_bitnumber"
        Depends { name: "000_meta" }
        files: [
            "bitnumber/Flags.cpp",
            "bitnumber/Flags.h",
        ]
    }

    StaticLibrary {
        name: "004_tagtype"
        Depends { name: "000_meta" }
        files: [
            "tagtype/Flags.cpp",
            "tagtype/Flags.h",
        ]
    }
    StaticLibrary {
        name: "005_tagvalue"
        Depends { name: "000_meta" }
        files: [
            "tagvalue/Flags.cpp",
            "tagvalue/Flags.h",
        ]
    }

    StaticLibrary {
        name: "006_repeated"
        Depends { name: "000_meta" }
        files: [
            "repeated/Flags.cpp",
            "repeated/Flags.h",
        ]
    }

    Application {
        name: "flags_tests"
        Depends { name: "Qt.testlib" }
        Depends { name: "004_tagtype" }
        consoleApplication: true
        // Qt.core exports conflicting settings (see QBS-1225)
        Depends { name: "cpp" }
        cpp.cxxLanguageVersion: "c++17"
        cpp.includePaths: ["."]
        cpp.cxxStandardLibrary: {
            if (qbs.toolchain.contains('clang')) return "libc++";
        }
        files: [
            "flagstest.cpp"
        ]
    }

    Application {
        name: "flags_app"
        consoleApplication: true
        Depends { name: "000_meta" }
        Depends { name: "002_classic" }
        Depends { name: "003_bitnumber" }
        Depends { name: "004_tagtype" }
        Depends { name: "005_tagvalue" }
        Depends { name: "006_repeated" }
        files: [
            "main.cpp",
        ]
    }
}
