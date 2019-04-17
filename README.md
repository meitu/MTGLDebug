# MTGLDebug

MTGLDebug is an OpenGL debugging assist tool for iOS platform.

By inject tracking logic into the OpenGL API through [fishhook](https://github.com/facebook/fishhook), MTGLDebug provides the ability to track OpenGL resource memory usage, make it easier to detect the OpenGL resouce leaking, and it also helps to detect common OpenGL incorrect logic operating.

## Usage

It's recommended to use MTGLDebug with [MTHawkeye](https://github.com/meitu/MTHawkeye), `MTHawkeye` provides an user interface for `MTGLDebug` which make it easier to use during development.

View [MTHawkeye Readme](https://github.com/meitu/MTHawkeye#mthawkeye) to see how to add it to your project for detail, and check [Hawkeye - OpenGL Trace](https://github.com/meitu/MTHawkeye/blob/master/doc/graphics/opengl-trace.md#hawkeye---opengl-trace) for instructions.

## Contribute to MTGLDebug

For more information about contributing issues or pull requests, see [Contributing Guide](./Contributing.md).

## License

MTGLDebug is under the MIT license, See the [LICENSE](./LICENSE) file for detail.
