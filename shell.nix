{
  mkShell,
  structz,
  clang-tools,
}:
mkShell {
  inputsFrom = [ structz ];
  packages = [ clang-tools ];
}
