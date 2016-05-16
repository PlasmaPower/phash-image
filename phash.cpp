#include <v8.h>
#include <node.h>
#include <nan.h>
#include <pHash.h>
#include <sstream>
#include <fstream>
#include <iomanip>
using namespace node;

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

// https://gist.github.com/rvagg/bb08a8bd2b6cbc264056#file-phash-cpp
class PhashRequest : public Nan::AsyncWorker {
 public:
  PhashRequest(Nan::Callback *callback, string file)
    : Nan::AsyncWorker(callback), error(false), file(file), bigint("") {}
  ~PhashRequest() {}

  void Execute () {
    // prevent segfault on an empty file, see https://github.com/aaronm67/node-phash/issues/8
    const char* _file = file.c_str();
    if (!fileExists(_file)) {
        error = true;
        return;
    }

    try {
        ph_dct_imagehash(_file, hash);
        bigint = NumberToString(hash);
    }
    catch(...) {
        error = true;
        // something went wrong with hashing
        // probably a CImg or ImageMagick IO Problem
    }
  }

  void HandleOKCallback () {
    Nan::HandleScope scope;

    v8::Local<v8::Value> argv[3];

    if (error) {
        argv[0] = Nan::Error("Error getting image phash.");
    }
    else {
        argv[0] = Nan::Null();
    }

    // A bit messy - converts the ulong64 into a char* (byte array) needed to create a Buffer
    // The problem is that the number's bytes are in the reverse of the needed order
    // That's why this loop pulls values in reverse
    size_t size = sizeof hash;
    char* hashPtr = (char*) &hash;
    char buffer[size];
    for (unsigned int i = 0; i < size; i++) {
      buffer[i] = hashPtr[size - i - 1];
    }

    argv[1] = Nan::NewBuffer(buffer, sizeof hash).ToLocalChecked();
    argv[2] = Nan::New<v8::String>(bigint).ToLocalChecked();

    callback->Call(3, argv);
  }

 private:
    bool error;
    string file;
    ulong64 hash;
    string bigint;
};

NAN_METHOD(ImageHashAsync) {
    v8::String::Utf8Value str(info[0]);
    Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());
    Nan::AsyncQueueWorker(new PhashRequest(callback, string(*str)));
    return;
}

// https://gist.github.com/rvagg/bb08a8bd2b6cbc264056#file-phash-cpp
class MHPhashRequest : public Nan::AsyncWorker {
 public:
  MHPhashRequest(Nan::Callback *callback, string file)
    : Nan::AsyncWorker(callback), error(false), file(file) {}
  ~MHPhashRequest() {}

  void Execute () {
    // prevent segfault on an empty file, see https://github.com/aaronm67/node-phash/issues/8
    const char* _file = file.c_str();
    if (!fileExists(_file)) {
        error = true;
        return;
    }

    try {
        int alpha = 2;
        int level = 1;
        hash = ph_mh_imagehash(_file, hashlen, alpha, level);
    }
    catch(...) {
        error = true;
        // something went wrong with hashing
        // probably a CImg or ImageMagick IO Problem
    }
  }

  void HandleOKCallback () {
    Nan::HandleScope scope;

    v8::Local<v8::Value> argv[2];

    if (error) {
        argv[0] = Nan::Error("Error getting image phash.");
    }
    else {
        argv[0] = Nan::Null();
    }

    argv[1] = Nan::NewBuffer((char*) hash, hashlen * sizeof hash[0]).ToLocalChecked();

    callback->Call(2, argv);

  }

 private:
    bool error;
    string file;
    uint8_t* hash;
    int hashlen = 0;
};

NAN_METHOD(MHImageHashAsync) {
    v8::String::Utf8Value str(info[0]);
    Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());
    Nan::AsyncQueueWorker(new MHPhashRequest(callback, string(*str)));
    return;
}

void RegisterModule(v8::Local<v8::Object> target) {
  Nan::SetMethod(target, "imageHash", ImageHashAsync);
  Nan::SetMethod(target, "imageHashMH", MHImageHashAsync);
}

NODE_MODULE(pHash, RegisterModule);
