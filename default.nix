{ lib
, stdenv
, cmake
, ninja
, doxygen
, graphviz
, clang-tools
, runCommand
, version
}:

let
  clang-scan-deps = runCommand "clang-scan-deps-wrapper" { } ''
    mkdir -p $out/bin
    ln -s ${clang-tools}/bin/clang-scan-deps $out/bin/clang-scan-deps
  '';
in

stdenv.mkDerivation {
  name = "tpcpp";
  version = version;

  src = ./.;

  nativeBuildInputs = [ cmake ninja doxygen graphviz ]
    ++ lib.optionals stdenv.hostPlatform.isDarwin [ clang-scan-deps ];
}
