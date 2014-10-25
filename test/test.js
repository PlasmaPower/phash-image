var assert = require("assert");
var pHash = require('../index');
require("should");

var examples = [
    { path: "./examples/a.jpg", hash: "2816561582497829945" },
    { path: "./examples/a.jpg", hash: "2816561582497829945" },
    { path: "./examples/c.png", hash: "15433968795834791622" },
    { path: "./examples/d.jpg", hash: "12040987020241900649" }
];
describe("pHash", function() {
    // https://github.com/aaronm67/node-phash/issues/8
    describe("invalid file test", function() {
        it("should fail", function(done) {
            pHash.imageHash("fake/path/here", function(err, hash) {
                assert(err);
                done();
            });
        });
    })

    describe("async test", function() {
        var test = examples[0];
        examples.forEach(function(i) {
            it(i.path, function(done) {
                pHash.imageHash(i.path, function(err, hash) {
                    if (err) {
                        done(err);
                    }

                    assert.equal(i.hash, hash);
                    done();
                });
            });
        });

        it("should fail", function(done) {
            pHash.imageHash("./examples/f.png", function(err, hash) {
                assert(err);
                done();
            });
        });

        it("should throw", function() {
            assert.throws(function() {
                pHash.imageHash("not enough arguments");
            });
        })
    });

    describe('hammingDistance()', function() {
        it('should be done', function() {
            var hammingAB = pHash.hammingDistance(examples[0].hash,examples[1].hash);
            var hammingAC = pHash.hammingDistance(examples[0].hash,examples[2].hash);
            var hammingBC = pHash.hammingDistance(examples[1].hash,examples[2].hash);
            var hammingAD = pHash.hammingDistance(examples[0].hash,examples[3].hash);
            hammingAB.should.equal(0);
            hammingAC.should.equal(38);
            hammingBC.should.equal(38);
            hammingAD.should.equal(12);
        });
    });
});
