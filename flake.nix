{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

    flake-parts.url = "github:hercules-ci/flake-parts";
    flake-parts.inputs.nixpkgs-lib.follows = "nixpkgs";

    git-hooks.url = "github:cachix/git-hooks.nix";
    git-hooks.inputs.nixpkgs.follows = "nixpkgs";

    dream2nix.url = "github:nix-community/dream2nix";
    dream2nix.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = { self, nixpkgs, flake-parts, git-hooks, dream2nix } @ inputs:
    flake-parts.lib.mkFlake { inherit inputs; } ({ ... }: {
      imports = [ git-hooks.flakeModule ];

      systems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];

      perSystem = { config, system, pkgs', ... }:
        let
          clang-tools-18 = pkgs'.llvmPackages_18.clang-tools;
          clang-format = pkgs'.runCommand "clang-format-wrapper" { } ''
            mkdir -p $out/bin
            ln -s ${clang-tools-18}/bin/clang-format $out/bin/clang-format
          '';
        in
        rec {
          _module.args.pkgs' = import nixpkgs { inherit system; };

          pre-commit = {
            check.enable = true;
            settings.src = ./.;
            settings.hooks = {
              clang-format = {
                enable = true;
                package = clang-format;
                types_or = pkgs'.lib.mkForce [ "c++" ];
              };
              editorconfig-checker.enable = true;
              nixpkgs-fmt.enable = true;
            };
          };

          packages = {
            pyprojectTpcpp = dream2nix.lib.evalModules {
              packageSets.nixpkgs = pkgs';
              modules = [
                ./pyproject.nix
                {
                  paths.projectRoot = ./.;
                  paths.projectRootFile = "flake.nix";
                  paths.package = ./.;
                }
              ];
            };

            tpcpp = pkgs'.callPackage ./default.nix {
              version = self.rev or self.dirtyRev or "git";
            };
          };

          devShells.default = pkgs'.mkShell {
            name = "template-project-cpp";

            inputsFrom = [
              packages.pyprojectTpcpp.devShell
              packages.tpcpp
            ];

            nativeBuildInputs = config.pre-commit.settings.enabledPackages;

            buildInputs =
              let
                helperB = pkgs'.writeShellScriptBin "B" ''
                  if [ -n "$DIRENV_DIR" ]; then cd ''${DIRENV_DIR:1}; fi
                  cmake --preset debug && cmake --build build/Debug
                '';
                helperD = pkgs'.writeShellScriptBin "D" ''
                  if [ -n "$DIRENV_DIR" ]; then cd ''${DIRENV_DIR:1}; fi
                  cmake --preset debug
                  compdb -p build/Debug/ list > compile_commands.json
                  strip-flags.py
                '';
                helperT = pkgs'.writeShellScriptBin "T" ''
                  if [ -n "$DIRENV_DIR" ]; then cd ''${DIRENV_DIR:1}; fi
                  cmake --preset debug && cmake --build build/Debug
                  ctest --test-dir build/Debug --output-on-failure
                '';

                debugTools = (with pkgs';
                  if stdenv.hostPlatform.isLinux
                  then [ gdb ] else [ lldb ]
                );

                clang-tools = pkgs'.llvmPackages.clang-tools;
                clangd = pkgs'.runCommand "clangd-wrapper" { } ''
                  mkdir -p $out/bin
                  ln -s ${clang-tools}/bin/clangd $out/bin/clangd
                '';
              in
              [ pkgs'.dos2unix clangd helperB helperD helperT ] ++ debugTools;

            hardeningDisable = [ "fortify" ];

            shellHook = ''
              ${config.pre-commit.installationScript}
              export PATH=$(pwd)/tools:$(pwd)/build/Debug:$PATH
            '';
          };
        };
      # end of perSystem
    });
}
