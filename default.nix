{ stdenv
, cmake
, version
}:

stdenv.mkDerivation {
  pname = "tpcpp";
  version = version;

  src = ./.;

  nativeBuildInputs = [ cmake ];
}
