#include <memory>
namespace mygo {

class Backend {
    static std::unique_ptr<Backend> from_function_ast();
    virtual void run() = 0;
};

class CBackend : public Backend {
    virtual void run() override;
};

class ByteCodeBackend : public Backend {
    virtual void run() override;
};

}  // namespace mygo
