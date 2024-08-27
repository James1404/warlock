{
  description = "A custom lisp dialect";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    zig.url = "github:mitchellh/zig-overlay";
    zls.url = "github:zigtools/zls";
  };

  outputs = { self, flake-utils, nixpkgs, zig, zls, ... }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          zlspkg = zls.packages.${system};
          pkgs = import nixpkgs { inherit system; overlays = [ zig.overlays.default]; };
        in {
          devShells.default = pkgs.mkShell {
            packages = with pkgs; [
              zigpkgs.master
              zlspkg.default
              
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
