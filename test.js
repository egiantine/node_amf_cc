/**
 * Unittests for node-amfast module.
 */

var sys = require('sys'); 
var amfast = require('./build/Release/amfast');

// To make this line work, "npm install amflib"
var amflib = require('amflib/node-amf/amf');


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

function dump(bin) {
  for (var i = 0; i < bin.length; i++) {
    sys.puts(i + "> " + bin[i].charCodeAt());
  }
}

for( var t = 0, n = 0; t < tests.length; t++ ){
	try {
		var descr = tests[t][0];
		var value = tests[t][1];
		var s = sys.inspect(value).replace(/\n/g,' ');
		sys.puts( ' > ' +descr+ ': ' + s);
		// serializing twice must not affect results
                amfast.serializer().serialize( value );
		// serialize and show AMF packet
		var Ser = amfast.serializer();
		var bin = Ser.serialize( value );
		//sys.puts( utils.hex(bin,16) );
		// deserialize and compare value
		var Des = amflib.deserializer(new Buffer(bin, 'binary'));
		var value2 = Des.readValue( amflib.AMF3 );
		var s2 =  sys.inspect(value2).replace(/\n/g,' ');
		// simple value test if value is scalar
		if( typeof value2 !== typeof value ){
			throw new Error('deserialized value of wrong type; ' + s2);
		}
		if( s !== s2 ){
			throw new Error('deserialized value does not match; ' + s2);
		}
		sys.puts('   OK');
		n++;
	}
	catch( Er ){
                console.log(Er.stack);
		sys.puts('**FAIL** ' + Er.message );
	}
}
sys.puts('Tests '+n+'/'+tests.length+' successful\n');



