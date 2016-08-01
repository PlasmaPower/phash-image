
var compare = require('hamming-distance');
var assert = require("assert");

var pHash = require('..');

var examples = [{
  path: "./examples/a.jpg",
  hash: "27166fd624cb9439"
}, {
  path: "./examples/c.png",
  hash: "d63078d8de3236c6"
}, {
  path: "./examples/d.jpg",
  hash: "a71a2de6269b9469"
}];

describe("pHash", function() {
  // https://github.com/aaronm67/node-phash/issues/8
  describe("invalid file test", function() {
    it("should fail", function(done) {
      pHash("fake/path/here", function(err, hash) {
        assert(err);
        done();
      });
    });
  })

  describe("async test", function() {
    var test = examples[0];
    examples.forEach(function(i) {
      it('cb:' + i.path, function(done) {
        pHash(i.path, function(err, hash) {
          assert.ifError(err);
          assert.equal(i.hash, hash.toString('hex'));
          done();
        });
      });

      it('promise:' + i.path, function() {
        return pHash(i.path).then(function(hash) {
          assert.equal(i.hash, hash.toString('hex'));
        });
      });
    });

    it("cb: should fail", function(done) {
      pHash("../examples/f.png", function(err, hash) {
        assert(err);
        done();
      });
    });

    it("promise: should fail", function() {
      return pHash("../examples/f.png").then(function() {
        throw new Error('boom');
      }).catch(function(err) {
        assert(err.message !== 'boom');
      })
    });
  });

  describe('hammingDistance()', function() {
    it('should be done', function() {
      var hammingAA = compare(examples[0].hash, examples[0].hash);
      var hammingAC = compare(examples[0].hash, examples[1].hash);
      var hammingAD = compare(examples[0].hash, examples[2].hash);
      assert.equal(hammingAA, 0);
      assert.equal(hammingAC, 38);
      assert.equal(hammingAD, 12);
    });
  });

  it('should return a bigint', function () {
    return pHash('./examples/a.jpg', true).then(function (hash) {
      assert(/^[0-9]+$/.test(hash))
    })
  })
});

describe('MH', function () {
  it('should return a buffer as a promise', function () {
    return pHash.mh('./examples/a.jpg').then(function (buf) {
      assert(Buffer.isBuffer(buf))
      assert.equal(72, buf.length)
    })
  })

  it('should return a buffer in the callback', function (done) {
    pHash.mh('./examples/a.jpg', function (err, buf) {
      if (err) return done(err)
      assert(Buffer.isBuffer(buf))
      assert.equal(72, buf.length)
      done()
    })
  })
})
