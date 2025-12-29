{
  description = "xv6 RISC-V development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            # RISC-V cross-compilation toolchain
            pkgsCross.riscv64.buildPackages.gcc
            pkgsCross.riscv64.buildPackages.binutils
            pkgsCross.riscv64.buildPackages.gdb
            
            # QEMU with RISC-V support
            qemu
            
            # Build essentials
            gnumake
            bc
            git
            
            # just wanted the clangd
            clang-tools
          ];

          shellHook = ''
            echo "xv6 RISC-V development environment loaded"
            echo "Available tools:"
            echo "  - riscv64-unknown-linux-gnu-gcc"
            echo "  - riscv64-unknown-linux-gnu-gdb"
            echo "  - qemu-system-riscv64"
            echo ""
            echo "You can now clone and build xv6:"
            echo "  git clone https://github.com/mit-pdos/xv6-riscv.git"
            echo "  cd xv6-riscv"
            echo "  make qemu"
          '';
        };
      }
    );
}
