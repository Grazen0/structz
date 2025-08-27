{
  lib,
  stdenv,
  cmake,
  catch2_3,
}:
stdenv.mkDerivation {
  pname = "structz";
  version = "main";

  src = ./.;

  nativeBuildInputs = [
    cmake
    catch2_3
  ];

  cmakeFlags = [
    "-DCMAKE_BUILD_TYPE=Release"
  ];

  enableParallelBuilding = true;
  doCheck = true;

  meta = with lib; {
    description = "Data structure implementations in C++.";
    homepage = "https://github.com/Grazen0/structz";
    license = licenses.mit;
  };
}
