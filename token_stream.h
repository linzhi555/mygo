#include <optional>
#include <vector>
#include "token.h"
namespace mygo {
class TokenStream {
   public:
    TokenStream(std::vector<uint8_t>&& src);
    std::optional<token::Value> Next();
    std::optional<token::Value> Peek();
    bool Finish();

   private:
    std::optional<token::Value> cache_;
    std::vector<uint8_t> src_;
    int pos_;
    int line_;
    int colum_;
};

}  // namespace mygo
