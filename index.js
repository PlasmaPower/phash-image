
var Promise = require('native-or-bluebird');
var pHash = require('./build/Release/pHash');

module.exports = pHashImage;

function pHashImage(file, returnBigInt, cb) {
  if (typeof returnBigInt === 'function') {
    cb = returnBigInt;
    returnBigInt = false;
  }

  var promise = new Promise(function(resolve, reject) {
    pHash.imageHash(file, function(err, hash, bigint) {
      if (err) return reject(err);
      if (returnBigInt === true) return resolve(bigint);
      return resolve(new Buffer(hash.slice(2), 'hex'));
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
 * Export additional function
 * @param  {String}   file     - filename, sadly pHash lib doesn't support
 *                             	 accepting raw buffer
 * @param  {Function} callback <err, Buffer[hex]>
 */
pHashImage.mh = function (file, callback) {
  var promise = new Promise(function (resolve, reject) {
    pHash.imageHashMH(file, function (err, hash) {
      if (err) {
        return reject(err);
      }

      // optimize useless conversion?
      resolve(new Buffer(hash.slice(2), 'hex'));
    });
  });

  if (typeof callback === 'function') {
    promise.then(function phashReturned(hash) {
      callback(null, hash);
    }, callback);
  }

  return promise;
}
