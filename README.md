# draw-suffix-tree
Draws a suffix tree from an input text.
  - [webcola](http://marvl.infotech.monash.edu/webcola/) - graph layout engine for D3
  - [sdsl-lite](https://github.com/simongog/sdsl-lite) - construct the suffix tree

### Installation and Usage

```sh
git clone https://github.com/mpetri/draw-suffix-tree.git
git submodule update --init
mkdir build
cd build
make
./draw-suffix-tree.x "abracadabra$" > visualization.html
```

### Bugs

as the Suffix Tree becomes large it becomes more difficult for webcola to layout the Suffix Tree and "bugs" start to appear. 

If you are interested in working on this problem feel free to contact me.

### License

GPLv3


### Version
0.01
