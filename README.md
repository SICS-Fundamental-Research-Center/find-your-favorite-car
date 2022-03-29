This project contains all source code for the web version of the demo system _Favorite+_.

Follow the intructions below to run the system.

1. Set up [Emscripten](https://emscripten.org) SDK.
   ```sh
   # in libraries/
   ./setup_emsdk.sh
   source emsdk/emsdk_env.sh
   ```

2. Compile GLPK for Emscripten
   ```sh
   # in libraries/
   ./make_lib.sh em++
   ```

3. Transcompile C++ code into JavaScript code.
   (The core algorithm is written in C/C++ by Min Xie).
   ```sh
   # in the project root folder
   make web
   ```

4. Install Yarn.
   [Installation Guide of Yarn](https://yarnpkg.com/lang/en/docs/install/)

5. Install dependencies
   ```sh
   # in react-app/
   yarn install
   ```

6. Run the demo
   ```sh
   # in react-app/
   yarn start
   ```
   The demo will be automatically opened in the browser. Follow this [YouTube video](https://www.youtube.com/watch?v=FjFbNcQYDFM) to interact with the demo.

7. Build the demo for deployment
   ```sh
   # in react-app/
   yarn run build
   ```
   The built webpages are placed in the `react-app/build/` folder.