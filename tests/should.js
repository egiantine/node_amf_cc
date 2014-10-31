/**
 * Unittests for node_amf_cc module.
 */

var should = require('should');
var sys = require('sys'); 
var amfcc = require('../build/Release/node_amf_cc');

var amflib = require('amflib/node-amf/amf');  // npm install amflib


// data types to test with human-readable description
var tests = [
  // strings
  ['empty string', ''],
  ['ascii string', 'Hello World'],
  ['unicode string', '£今\u4ECA"\u65E5日'],
  // numbers
  ['zero',  0 ],
  ['integer in 1 byte u29 range', 0x7F ],
  ['integer in 2 byte u29 range', 0x00003FFF ],
  ['integer in 3 byte u29 range', 0x001FFFFF ],
  ['integer in 4 byte u29 range', 0x1FFFFFFF ],
  ['large integer', 4294967296 ],
  ['large negative integer', -4294967296 ],
  ['small negative integer', -1 ],
  ['small floating point', 0.123456789 ],
  ['small negative floating point', -0.987654321 ],
  ['Number.MIN_VALUE', Number.MIN_VALUE ],
  ['Number.MAX_VALUE',  Number.MAX_VALUE ],
  ['Number.NaN', Number.NaN],
  // other scalars
  ['Boolean false', false],
  ['Boolean true', true ],
  ['undefined', undefined ],
  ['null', null],
  // Arrays
  ['empty array', [] ],
  ['mixed array', [ 1, 'ab', true ] ],
  ['integer array', [ 1, -1, 2, -2, 3, -3, 4, -4, 5, -5 ] ],
  ['sparse array', [undefined,undefined,undefined,undefined,undefined,undefined] ],
  ['multi-dimensional array',  [[[],[]],[],] ],
  // special objects
  ['date object (epoch)', new Date(0) ],
  ['date object (now)', new Date() ],
  // plain objects
  ['empty object', {} ],
  ['keyed object', { foo:'bar', 'foo bar':'baz' } ],
  ['refs object', { foo: _ = { a: 12 }, bar: _ } ]
];



// Test each type individually through serializer and then deserializer
// note that this doesn't prove it works with Flash, just that it agrees with amflib.
sys.puts('Serializing and deserializing '+tests.length+' test values');

function dump(bin, prefix) {
  for (var i = 0; i < bin.length; i++) {
    sys.puts(prefix + i + "> " + bin[i].charCodeAt());
  }
}

function sanitize(value) {
  return sys.inspect(value).replace(/\n/g,' ');
}

var succeeded = 0;
var failed = 0;
for (var i = 0; i < tests.length; i++) {
  var test = tests[i];
  var descr = test[0];
  var value = test[1];
  sys.puts( ' > ' + descr + ': ' + sanitize(value));

  // Test serialization using amflib as baseline.
  var experimentBuffer = amfcc.serializer().serialize(value);
  var baselineBuffer = amflib.serializer().writeValue(value);
  try {
    experimentBuffer.should.be.exactly(baselineBuffer);  
    succeeded += 1;
  } catch (e) {
    sys.puts("Serialization error: " + e);
    failed += 1;
  }

  // Test deserialization using original value as baseline.
  var baselineValue = amflib.deserializer(baselineBuffer).readValue(amflib.AMF3);
  var experimentValue = amfcc.deserializer().deserialize(baselineBuffer);

  try {
    if (descr == "Number.NaN") {
      // NaN doesn't equal itself, so the Should library doesn't work for this one. 
      should(experimentValue).be.NaN;
    } else {
      should(experimentValue).eql(value);
    }
    succeeded += 1;
  } catch (e) {
    sys.puts("Deserialization error: " + e.message);
    sys.puts("Baseline: " + baselineValue);
    failed += 1;
  }
}

sys.puts(succeeded + "/" + (succeeded + failed) + " tests passing.");



