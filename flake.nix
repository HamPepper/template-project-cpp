{
  description = "template-project-cpp";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

    flake-parts.url = "github:hercules-ci/flake-parts";
    flake-parts.inputs.nixpkgs-lib.follows = "nixpkgs";

    git-hooks.url = "github:cachix/git-hooks.nix";
    git-hooks.inputs.nixpkgs.follows = "nixpkgs";

    dream2nix.url = "github:nix-community/dream2nix";
    dream2nix.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = { self, nixpkgs, flake-parts, git-hooks, dream2nix, ... } @ inputs:
    let
      overlayThis = final: prev:
        {
          tpcpp = prev.callPackage ./default.nix {
            version = self.rev or self.dirtyRev or "git";
          };

          helperB = prev.writeShellScriptBin "B" ''
            cmake --preset debug && cmake --build build/Debug
          '';
          helperC = prev.writeShellScriptBin "C" ''
            rm -rf build
          '';
          helperD = prev.writeShellScriptBin "D" ''
            cmake --preset debug
            compdb -p build/Debug/ list > compile_commands.json
          '';
        };
    in
    flake-parts.lib.mkFlake { inherit inputs; } ({ getSystem, ... }: {
      imports = [
        git-hooks.flakeModule
      ];

      systems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];

      perSystem =
        { system, inputs', pkgs', config, ... }:
        rec {
          _module.args.pkgs' = import self.inputs.nixpkgs {
            inherit system;
            overlays = [ overlayThis ];
          };

          pre-commit = {
            check.enable = true;

            settings.src = ./.;
            settings.hooks = {
              clang-format = {
                enable = true;
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
            tpcpp = pkgs'.tpcpp;
          };

          devShells.default = pkgs'.mkShell {
            inputsFrom = [ packages.pyprojectTpcpp.devShell ];

            name = "template-project-cpp-dev";

            buildInputs = with pkgs'; [
              cmake
              gdb

              # utilities
              dos2unix
              time # measure memory usage, use \time to invoke in zsh

              helperB
              helperC
              helperD
            ] ++ config.pre-commit.settings.enabledPackages;

            shellHook = ''
              ${config.pre-commit.installationScript}
              export PATH=$(pwd)/tools:$(pwd)/build/Debug:$PATH

              # env vars set by devShells are not propagated to vscode?
              # install direnv the VS Code extension seems to fix this
              #export WSLENV=$WSLENV:PATH
            '';
          };
        };
      # end of perSystem
    });
}
