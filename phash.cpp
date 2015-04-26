#include <v8.h>
#include <node.h>
#include <nan.h>
#include <pHash.h>
#include <sstream>
#include <fstream>
#include <iomanip>
using namespace node;
using namespace v8;
using namespace Nan;

template < typename T >
std::string int_to_hex( T i ) {
  std::stringstream stream;
  stream << "0x"
         << std::setfill ('0') << std::setw(sizeof(T)*2)
         << std::hex << i;
  return stream.str();
}

bool fileExists(const char* filename) {
    ifstream file(filename);
    return !!file;
}

template <typename T>
string NumberToString ( T Number ) {
    ostringstream ss;
    ss << Number;
    return ss.str();
}

string uint_to_hex(uint8_t* input, int len)
{
    static const char* const lut = "0123456789ABCDEF";

    std::stringstream stream;
    stream << "0x";
    for (int i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        stream << lut[c >> 4] << lut[c & 15];
    }

    return stream.str();
}

// https://gist.github.com/rvagg/bb08a8bd2b6cbc264056#file-phash-cpp
class PhashRequest : public NanAsyncWorker {
 public:
  PhashRequest(NanCallback *callback, string file)
    : NanAsyncWorker(callback), error(false), file(file), hash(""), bigint("") {}
  ~PhashRequest() {}

  void Execute () {
    // prevent segfault on an empty file, see https://github.com/aaronm67/node-phash/issues/8
    const char* _file = file.c_str();
    if (!fileExists(_file)) {
        error = true;
        return;
    }

    try {
        ulong64 _hash = 0;
        ph_dct_imagehash(_file, _hash);
        hash = int_to_hex(_hash);
        bigint = NumberToString(_hash);
    }
    catch(...) {
        error = true;
        // something went wrong with hashing
        // probably a CImg or ImageMagick IO Problem
    }
  }

  void HandleOKCallback () {
    NanScope();

    Handle<Value> argv[3];

    if (error) {
        argv[0] = NanError("Error getting image phash.");
    }
    else {
        argv[0] = NanNull();
    }

    argv[1] = NanNew<String>(hash);
    argv[2] = NanNew<String>(bigint);

    callback->Call(3, argv);
  }

 private:
    bool error;
    string file;
    string hash;
    string bigint;
};

NAN_METHOD(ImageHashAsync) {
    String::Utf8Value str(args[0]);
    NanCallback *callback = new NanCallback(args[1].As<Function>());
    NanAsyncQueueWorker(new PhashRequest(callback, string(*str)));
    NanReturnUndefined();
}

// https://gist.github.com/rvagg/bb08a8bd2b6cbc264056#file-phash-cpp
class MHPhashRequest : public NanAsyncWorker {
 public:
  MHPhashRequest(NanCallback *callback, string file)
    : NanAsyncWorker(callback), error(false), file(file), hash("") {}
  ~MHPhashRequest() {}

  void Execute () {
    // prevent segfault on an empty file, see https://github.com/aaronm67/node-phash/issues/8
    const char* _file = file.c_str();
    if (!fileExists(_file)) {
        error = true;
        return;
    }

    try {
        int hashlen = 0;
        int alpha = 2;
        int level = 1;
        uint8_t* _hash = ph_mh_imagehash(_file, hashlen, alpha, level);
        hash = uint_to_hex(_hash, hashlen);
    }
    catch(...) {
        error = true;
        // something went wrong with hashing
        // probably a CImg or ImageMagick IO Problem
    }
  }

  void HandleOKCallback () {
    NanScope();

    Handle<Value> argv[2];

    if (error) {
        argv[0] = NanError("Error getting image phash.");
    }
    else {
        argv[0] = NanNull();
    }

    argv[1] = NanNew<String>(hash);

    callback->Call(2, argv);

  }

 private:
    bool error;
    string file;
    string hash;
};

NAN_METHOD(MHImageHashAsync) {
    String::Utf8Value str(args[0]);
    NanCallback *callback = new NanCallback(args[1].As<Function>());
    NanAsyncQueueWorker(new MHPhashRequest(callback, string(*str)));
    NanReturnUndefined();
}

void RegisterModule(Handle<Object> target) {
  NODE_SET_METHOD(target, "imageHash", ImageHashAsync);
  NODE_SET_METHOD(target, "imageHashMH", MHImageHashAsync);
}

NODE_MODULE(pHash, RegisterModule);
