node-amf-cc
===========

Under construction

To compile the addon:

    $ node-gyp configure
    $ node-gyp build

To run feature tests:

    $ npm install amflib should
    $ node tests/should.js

To run benchmarks:

    $ npm install amflib benchmark
    $ node tests/benchmark.js

Preliminary benchmark results (omitting unimplemented features):

    amflib/deserialize x 9.09 ops/sec ±2.26% (27 runs sampled)
    amfcc/deserialize x 43.88 ops/sec ±5.08% (57 runs sampled)
    amflib/serialize x 2.38 ops/sec ±7.85% (10 runs sampled)
    amfcc/serialize x 24.56 ops/sec ±8.42% (34 runs sampled)

