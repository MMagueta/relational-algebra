{
  description = "Relational algebra engine library (librelationalalgebra)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
  };

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forAllSystems = f: nixpkgs.lib.genAttrs systems (system:
        let
          pkgs = import nixpkgs { inherit system; };
          shared_lib_extension = if pkgs.stdenv.isDarwin then "dylib" else "so";
        in
        f pkgs shared_lib_extension
      );
    in
    {
      packages = forAllSystems (pkgs: shared_lib_extension:
        {
          librelationalalgebra = pkgs.stdenv.mkDerivation {
            pname = "librelationalalgebra";
            version = "0.1.0";
            src = ./.;

            nativeBuildInputs = with pkgs; [ clang patchelf ];

            buildPhase = ''
              clang -c -fPIC src/*.c -Iinclude

              ${if pkgs.stdenv.isDarwin then ''
                clang -dynamiclib -install_name @rpath/librelationalalgebra.${shared_lib_extension} \
                  -o librelationalalgebra.${shared_lib_extension} *.o -lm
              '' else ''
                clang -shared -o librelationalalgebra.${shared_lib_extension} *.o -lm
              ''}
            '';

            installPhase = ''
              mkdir -p $out/lib $out/include $out/lib/pkgconfig

              cp librelationalalgebra.${shared_lib_extension} $out/lib/
              cp include/*.h $out/include/

              cat > $out/lib/pkgconfig/librelationalalgebra.pc << EOF
prefix=$out
exec_prefix=\''${prefix}
libdir=\''${exec_prefix}/lib
includedir=\''${prefix}/include

Name: librelationalalgebra
Description: Relational algebra engine
Version: 0.1.0
Libs: -L\''${libdir} -lrelational -lm
Cflags: -I\''${includedir}
EOF
            '';

            postInstall = ''
              ${if pkgs.stdenv.isDarwin then ''
                install_name_tool -id $out/lib/librelationalalgebra.${shared_lib_extension} \
                  $out/lib/librelationalalgebra.${shared_lib_extension}
              '' else ''
                patchelf --set-soname librelationalalgebra.${shared_lib_extension} \
                  $out/lib/librelationalalgebra.${shared_lib_extension} || true
              ''}
            '';

            meta = with pkgs.lib; {
              description = "A pure relational algebra engine";
              platforms = platforms.unix;
            };
          };

          relational-algebra-engine = pkgs.stdenv.mkDerivation {
            pname = "relational-algebra-engine";
            version = "0.1.0";
            src = ./.;

            nativeBuildInputs = with pkgs; [ clang pkg-config ];
            buildInputs = [ self.packages.${pkgs.system}.librelationalalgebra ];

            buildPhase = ''
            clang -o relational-algebra-engine src/*.c \
              -Iinclude -lm
          '';

            installPhase = ''
            mkdir -p $out/bin
            cp relational-algebra-engine $out/bin/
          '';

            meta = with pkgs.lib; {
              description = "Binary for the Relational Algebra Engine";
              platforms = platforms.unix;
            };
          };

          default = self.packages.${pkgs.system}.relational-algebra-engine;
        });

      # For `nix develop` usage
      devShells = forAllSystems (pkgs: _: {
        default = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [ clang pkg-config ];
        };
      });
    };
}
