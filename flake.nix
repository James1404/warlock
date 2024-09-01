{
  description = "A custom lisp dialect";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, flake-utils, nixpkgs, ... }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          pkgs = import nixpkgs { inherit system; overlays = []; };
        in {
          devShells.default = pkgs.mkShell {
            packages = with pkgs; [
              pkg-config
	            bear
	            gdb
              gperf
              clang-tools
              llvm_18
              valgrind
            ];
          };
        }
      );
}
