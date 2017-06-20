Node-Sword
======================

## Instructions for UNIX based systems

Note: Tested on ubuntu

```bash
$ ./scripts/compile.sh
$ npm run build
$ node examples/sword.js
```

# Using docker

If facing some troubles when building the image from windows, make sure to first call `dos2unix scripts/compile_sources.sh`

add `AM_INIT_AUTOMAKE([subdir-objects])` to `lib/Makefile.am` in sword project to disable `subdir-objects` warnings.

Creating the image: `docker build . -t node-sword --force-rm --pull` build command creates the image `-t` set a name for the image `--force-rm` delete the container created when building the image `--pull` forces the image rebuild

Running the image: `docker run -v `pwd`/compiled:/usr/src/node_sword/compiled --rm -it node-sword` run command runs the image in a container. It compiles node-sword for linux x64 and set the binary in the compiled folder. Using `-v` shares the compiled folder between the host and the container and `--rm` deletes the container after it exits.

Running the image interactively: `docker run -v `pwd`/compiled:/usr/src/node_sword/compiled -v `pwd`/lib:/usr/src/node_sword/lib -v `pwd`/examples:/usr/src/node_sword/examples -v `pwd`/lib:/usr/src/node_sword/lib -v `pwd`/binding.gyp:/usr/src/node_sword/binding.gyp --rm -it node-sword bash` then run `npm run build` and `npm run package`

Testing the build: move <node-sword_package.tgz> to modules folder and run `docker run -v `pwd`/examples:/usr/src/sword-copy  --rm -it node bash`, then run it:

```
apt-get update
apt-get install libz-dev libclucene-dev libicu-dev libcurl4-gnutls-dev
cd /usr/src
cp -r sword-copy sword
cd sword
npm i <node-sword_package.tgz>
node run.js
```
