{
  description = "Data structure implementations in C++.";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
    systems.url = "github:nix-systems/default";
  };

  outputs =
    inputs@{
      self,
      flake-parts,
      ...
    }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = import inputs.systems;

      perSystem =
        {
          self',
          pkgs,
          system,
          ...
        }:
        {
          packages = {
            structz = pkgs.callPackage ./default.nix { };
            default = self'.packages.structz;
          };

          devShells.default = pkgs.callPackage ./shell.nix {
            inherit (self'.packages) structz;
          };
        };
    };
}
