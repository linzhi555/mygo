#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// tokens
typedef int TkType;
const TkType TK_INT = 10;
const TkType TK_SYMBOL = 20;
const TkType TK_EQUAL = 30;
const TkType TK_LEFT_BRACE = 40;
const TkType TK_RIGHT_BRACE = 50;
const TkType Tk_ENDL = 60;
const TkType TK_PLUS = 70;

// ast nodes
const int AST_ROOT = 10;
const int AST_STATEMENT = 20;
const int AST_FUNTION = 30;
const int AST_IF = 40;
const int AST_EXPR = 50;
const int AST_LITERAL = 60;
const int AST_BLOCK = 70;

struct Token {
   public:
    TkType tk_type;
    int int_v;
    char char_v;
    float float_v;
    std::string str_v;
    void debug() {
        switch (tk_type) {
        case TK_INT:
            std::cout << "[int:" << int_v << "]";
            break;
        case TK_SYMBOL:
            std::cout << "[symbol:" << str_v << "]";
            break;
        case TK_EQUAL:
            std::cout << "[equal]";
            break;
        case TK_LEFT_BRACE:
            std::cout << "[left_brace]";
            break;
        case TK_RIGHT_BRACE:
            std::cout << "[right_brace]";
            break;
        case TK_PLUS:
            std::cout << "[plus]";
            break;
        case Tk_ENDL:
            std::cout << "[endl]";
            break;
        }
    }
};

class TokenStream {
   public:
    TokenStream() = default;
    ~TokenStream() = default;
    static std::optional<TokenStream> from_file(std::string_view filename) {
        TokenStream stream;
        FILE *fp = std::fopen(filename.data(), "r");
        if (!fp) {
            std::perror("File opening failed");
            return std::nullopt;
        }

        int c;
        while ((c = std::fgetc(fp)) != EOF) {
            stream.bytes.push_back(c);
        }

        std::optional<Token> tk = stream.next_token();
        while (tk) {
            tk.value().debug();
            tk = stream.next_token();
            stream.tokens.push_back(tk.value());
        }

        return stream;
    };

    void debug() {
        for (int i = cursor; i < bytes.size(); i++) {
            std::putchar(bytes[i]);
        }
    }

    std::optional<Token> next_token() {
        int temp_cursor = cursor;

        if (auto tk = next_symbol()) {
            Token res = { .tk_type = TK_SYMBOL, .str_v = tk.value() };
            return res;
        }

        if (auto tk = next_int()) {
            Token res = { .tk_type = TK_INT, .int_v = tk.value() };
            return res;
        }

        if (auto tk = next_expect_char('=')) {
            Token res = { .tk_type = TK_EQUAL };
            return res;
        }

        if (auto tk = next_expect_char('(')) {
            Token res = { .tk_type = TK_LEFT_BRACE };
            return res;
        }

        if (auto tk = next_expect_char('+')) {
            Token res = { .tk_type = TK_PLUS };
            return res;
        }

        if (auto tk = next_expect_char(')')) {
            Token res = { .tk_type = TK_RIGHT_BRACE };
            return res;
        }

        if (auto tk = next_expect_char('\n')) {
            Token res = { .tk_type = Tk_ENDL };
            return res;
        }

        return std::nullopt;
    }

   private:
    std::vector<uint8_t> bytes;
    std::vector<Token> tokens;
    int cursor = 0;

    bool is_az(char c) {
        if (c >= 'A' && c <= 'Z') return true;
        if (c >= 'a' && c <= 'z') return true;
        return false;
    }

    std::optional<std::string> next_symbol() {
        int temp_cursor = cursor;
        std::optional<std::string> ret;
        for (; temp_cursor < bytes.size(); temp_cursor++) {
            char c = bytes[temp_cursor];
            if (c == ' ') continue;
            if (!is_az(c)) {
                break;
            }
            if (!ret) {
                ret = std::string();
            }
            ret->push_back(c);
        }

        if (ret) {
            cursor = temp_cursor;
        }

        return ret;
    }

    std::optional<char> next_expect_char(char expect) {
        int temp_cursor = cursor;
        std::optional<int> ret;
        for (; temp_cursor < bytes.size(); temp_cursor++) {
            char c = bytes[temp_cursor];
            if (c == ' ') continue;
            if (c != expect) break;
            ret = expect;
        }

        if (ret) {
            cursor = temp_cursor;
        }
        return ret;
    }

    std::optional<int> next_int() {
        int temp_cursor = cursor;
        std::optional<int> ret;
        for (; temp_cursor < bytes.size(); temp_cursor++) {
            char c = bytes[temp_cursor];
            if (c == ' ') continue;
            if (c > '9' || c < '0') {
                break;
            }
            if (ret) {
                ret = ret.value() * 10 + c - '0';
            } else {
                ret = c - '0';
            }
        }

        if (ret) {
            cursor = temp_cursor;
        }
        return ret;
    }
};

class AstNode {
    virtual int parse(TokenStream *) = 0;
    virtual void debug() = 0;
};

class AstLeftvalueNode : public AstNode {
   public:
    virtual int parse(TokenStream *stream) {
        return -1;
    };
    virtual void debug() {
        std::cout << "leftV:" << name;
    };

   private:
    std::string name;
};
class AstExprNode : public AstNode {
   public:
    virtual int parse(TokenStream *stream) override {
        return -1;
    };

    virtual void debug() override {
        std::cout << "expr:" << int_v;
    };

   private:
    int int_v;
};

class AstStatementNode : public AstNode {
   public:
    AstStatementNode() = default;
    AstStatementNode(AstStatementNode &) {};
    std::unique_ptr<AstLeftvalueNode> left_value;
    std::unique_ptr<AstExprNode> expr;
    virtual int parse(TokenStream *) override;
    void debug() override {
        std::cout << "statement node";
    }
};

class AstRootNode : public AstNode {
    std::vector<std::unique_ptr<AstStatementNode>> childs;
    virtual int parse(TokenStream *stream) override {
        std::unique_ptr<AstStatementNode> newnode = std::make_unique<AstStatementNode>();
        int err = newnode->parse(stream);
        if (err == 0) {
            childs.push_back(std::move(newnode));
        }
        return 0;
    }
    void debug() override {
        std::cout << "statement node";
        for (std::unique_ptr<AstStatementNode> &child : childs) {
            std::cout << "----";
            child->debug();
            std::cout << std::endl;
        }
    };
};

enum class ValueType {
    None,
    Bool,
    Int,
    Float,
    Str,
    Obj,
    List,
    Function,
    Class,
};

struct Value {
    ValueType type;
    union {
        int int_v;
        double float_v;
        bool bool_v;
        std::vector<uint8_t> chars_v;
    } data;
};

class Storage {
   public:
    Value *load(std::string_view symbol) {
        return storage_[symbol];
    }
    void store(std::string_view symbol, Value *value) {
        storage_.insert({ symbol, value });
    }

   private:
    std::unordered_map<std::string_view, Value *> storage_;
};

class VM {
   public:
    int run(AstRootNode *program);
    Storage storage;
    std::vector<void *> call_stack;
    void memory_used();
    void gc();

   private:
};

int main() {
    std::optional<TokenStream> stream = TokenStream::from_file("test.txt");

    std::cout << "left is " << std::endl;

    stream.value().debug();
    VM vm;
    vm.storage.store("t1", nullptr);
    std::cout << "test vm value t1 is" << vm.storage.load("t1") << std::endl;
}
