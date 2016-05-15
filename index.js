
var Promise = require('any-promise');

var pHash = require('./build/Release/pHash');

module.exports = pHashImage;

/**
 * http://www.phash.org/docs/design.html
 * Returns either an 8-byte buffer or 64-bit integer as a string.
 *
 * @param {String} filename
 * @param {Boolean} return bigint
 * @param {Function} callback(err, Buffer)
 * @return Promise => Buffer
 */

function pHashImage(file, returnBigInt, cb) {
  if (typeof returnBigInt === 'function') {
    cb = returnBigInt;
    returnBigInt = false;
  }

  var promise = new Promise(function(resolve, reject) {
    pHash.imageHash(file, function(err, hash, bigint) {
      if (err) return reject(err);
      if (returnBigInt === true) return resolve(bigint);
      return resolve(hash);
    });
  });

  if (typeof cb === 'function') {
    promise.then(function (hash) {
      cb(null, hash);
    }, cb);
  }

  return promise;
}

/**
 * http://www.phash.org/docs/design.html
 * Returns an 72-byte buffer.
 *
 * @param {String} filename
 * @param {Function} callback(err, Buffer)
 * @return Promise => Buffer
 */

pHashImage.mh = function (file, callback) {
  var promise = new Promise(function (resolve, reject) {
    pHash.imageHashMH(file, function (err, hash) {
      if (err) return reject(err);
      resolve(hash);
    });
  });

  if (typeof callback === 'function') {
    promise.then(function phashReturned(hash) {
      callback(null, hash);
    }, callback);
  }

  return promise;
}
