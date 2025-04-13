#!/bin/sh

BUILD_TYPE="standalone"
PROJECT_NAME=""

usage() {
  echo "Usage: $0 --name <project_name> [--build <cmake|standalone>]"
  exit 1
}

while [ $# -gt 0 ]; do
  case "$1" in
  --name)
    shift
    PROJECT_NAME="$1"
    ;;
  --build)
    shift
    BUILD_TYPE="$1"
    ;;
  --help)
    usage
    ;;
  esac
  shift
done

if [ -z "$PROJECT_NAME" ]; then
  echo "Error: Project name is required."
  usage
fi

mkdir -p "$PROJECT_NAME"

cd "$PROJECT_NAME" || {
  echo "Failed to change directory to $PROJECT_NAME"
  exit 1
}

cat <<EOF >main.cpp
#include "etna/etna_core.hpp"
#include "etna/scene.hpp"

constexpr uint32_t WINDOW_WIDTH{800};
constexpr uint32_t WINDOW_HEIGHT{600};

using namespace etna;

int main(void) {
	engine::init();

	Window window({
		.width = WINDOW_WIDTH,
		.height = WINDOW_HEIGHT,
		.title = "Basic Etna",
		.captureMouse = true,
	});

	Scene scene;

	MeshNode sphere = scene.createMeshNode({
		.name = "Sphere",
		.mesh = engine::getSphere(),
		.transform =
			{
				.position = {0, 0, -3},
				.scale = Vec3(0.5),
			},
		.material = engine::createGridMaterial({
			.color = BLUE * 0.08,
			.gridColor = BLUE,
			.gridSpacing = 0.1,
			.thickness = 0.005,
		}),
	});

	CameraNode playerCamera = scene.createCameraNode({
		.name = "PlayerCamera",
		.transform = {.position = {0, 0, 1}},
	});

	Renderer renderer({});

	while (!window.shouldClose()) {
		window.pollEvents();

		sphere->rotate(0, 0.01, 0.01);

		renderer.beginFrame(window);

		scene.render(renderer, playerCamera);

		renderer.endFrame();

		window.swapBuffers();
	}
}
EOF

if [ "$BUILD_TYPE" = "cmake" ]; then
  echo "Using CMake for build system"

  DOWNLOAD_URL="https://api.github.com/repos/nablaFox/Etna/releases/latest"

  TAG_VERSION=$(wget -q -O - "$DOWNLOAD_URL" | grep '"tag_name":' | cut -d '"' -f 4 2>&1) || {
    echo "Failed to fetch the latest release version"
    rm -rf "../$PROJECT_NAME"
    exit 1
  }

  ZIP_URL="https://github.com/nablaFox/Etna/archive/refs/tags/$TAG_VERSION.zip"

  echo "Downloading from: $ZIP_URL"

  wget "$ZIP_URL" -O Etna-latest.zip >/dev/null 2>&1 || {
    echo "Failed to download project"
    rm -rf "../$PROJECT_NAME"
    exit 1
  }

  unzip Etna-latest.zip >/dev/null 2>&1 || {
    echo "Failed to unzip project"
    rm -rf "../$PROJECT_NAME"
    exit 1
  }

  rm -rf Etna-latest.zip

  VERSION=$(echo "$TAG_VERSION" | cut -d 'v' -f 2)

  cat <<EOF >CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project($PROJECT_NAME)

add_executable($PROJECT_NAME main.cpp)
add_subdirectory(Etna-${VERSION})
target_link_libraries($PROJECT_NAME PRIVATE etna)
EOF

else
  echo "Using standalone build"

  OS=$(uname -s)

  if [ "$OS" = "Linux" ]; then
    RELEASE_VERSION="etna-linux_amd64"
  else
    echo "Unsupported operating system: $OS"
    exit 1
  fi

  DOWNLOAD_URL="https://github.com/nablaFox/Etna/releases/latest/download/$RELEASE_VERSION.zip"

  echo "Downloading etna release from $DOWNLOAD_URL ..."

  wget "$DOWNLOAD_URL" -O "$RELEASE_VERSION.zip" >/dev/null 2>&1 || {
    echo "Failed to download project"
    rm -rf "../$PROJECT_NAME"
    exit 1
  }

  unzip "$RELEASE_VERSION.zip" >/dev/null 2>&1 || {
    echo "Failed to unzip project"
    rm -rf "../$PROJECT_NAME"
    exit 1
  }

  CXX="${CXX:-c++}"
  LIBS="-lm -letna -lglfw3 -lvulkan"
  CXX_FLAGS="-I$RELEASE_VERSION/include -L${RELEASE_VERSION}/lib"

  if pkg-config --exists x11; then
    LIBS="$LIBS -lX11"
  fi

  if [ "$(uname)" = "Darwin" ]; then
    LIBS="$LIBS -framework Cocoa"
  fi

  cat <<EOF >build.sh
#!/bin/sh

set -xe

CXX="\${CXX:-c++}"
CXX_FLAGS="$CXX_FLAGS"
LIBS="$LIBS"

\$CXX \$CXX_FLAGS -o $PROJECT_NAME main.cpp \$LIBS
EOF

  chmod +x build.sh

  rm -rf "$RELEASE_VERSION.zip"
fi

echo "Project setup completed successfully."
