#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
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
const TkType TK_ENDL = 60;
const TkType TK_PLUS = 70;
const TkType TK_EOF = 80;

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
        case TK_ENDL:
            std::cout << "[endl]";
            break;
        case TK_EOF:
            std::cout << "[eof]";
        }
    }
};

class TokenStream {
   public:
    int cur_tk = 0;
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
            stream.tokens.push_back(tk.value());
            tk = stream.next_token();
        }

        return stream;
    };

    Token forward() {
        if (cur_tk < tokens.size() - 1) {
            auto temp = tokens[cur_tk];
            cur_tk++;
            return temp;
        }
        return { .tk_type = TK_EOF };
    }

    Token peek() {
        if (cur_tk < tokens.size() - 1) {
            auto temp = tokens[cur_tk];
            return temp;
        }
        return { .tk_type = TK_EOF };
    }

    void back(int index) {
        if (index >= 0 && index < tokens.size()) cur_tk = index;
    }

    void debug() {
        for (int i = cursor; i < bytes.size(); i++) {
            std::putchar(bytes[i]);
        }
    }

   private:
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
            Token res = { .tk_type = TK_ENDL };
            return res;
        }

        return std::nullopt;
    }

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
    std::string name;
    AstLeftvalueNode(std::string s) {
        this->name = s;
    };

    virtual int parse(TokenStream *stream) {
        return -1;
    };

    virtual void debug() {
        std::cout << "leftV:" << name;
    };

   private:
};
class AstExprNode : public AstNode {
   public:
    int int_v;
    AstExprNode(int v) {
        this->int_v = v;
    }

    virtual int parse(TokenStream *stream) override {
        return -1;
    };

    virtual void debug() override {
        std::cout << "expr:" << int_v;
    };

   private:
};

class AstStatementNode : public AstNode {
   public:
    AstStatementNode() = default;
    AstStatementNode(AstStatementNode &) {};
    std::unique_ptr<AstLeftvalueNode> left_value_;
    std::unique_ptr<AstExprNode> expr_;
    virtual int parse(TokenStream *s) override {
        if (s->peek().tk_type == TK_SYMBOL) {
            left_value_ = std::make_unique<AstLeftvalueNode>(s->peek().str_v);
            s->forward();
        } else {
            return -1;
        }

        if (s->peek().tk_type == TK_EQUAL) {
            s->forward();
        } else {
            return -1;
        }

        if (s->peek().tk_type == TK_INT) {
            expr_ = std::make_unique<AstExprNode>(s->peek().int_v);
            s->forward();
        } else {
            return -1;
        }

        if (s->peek().tk_type == TK_ENDL) {
            s->forward();
        } else {
            return -1;
        }

        return 0;
    };
    void debug() override {
        std::cout << "statement:";
        left_value_->debug();
        std::cout << "=";
        expr_->debug();
    }
};

class AstRootNode : public AstNode {
   public:
    std::vector<std::unique_ptr<AstStatementNode>> childs;
    virtual int parse(TokenStream *stream) override {
        int err = 0;
        while (err == 0) {
            std::unique_ptr<AstStatementNode> newnode = std::make_unique<AstStatementNode>();
            err = newnode->parse(stream);
            if (err == 0) {
                childs.push_back(std::move(newnode));
            }
        }
        return 0;
    }
    void debug() override {
        std::cout << "ast" << std::endl;
        for (std::unique_ptr<AstStatementNode> &child : childs) {
            std::cout << "---- ";
            child->debug();
            std::cout << std::endl;
        }
    };
};

enum class ValT {
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
    ValT type;
    int int_v;
    double float_v;
    bool bool_v;
    std::vector<uint8_t> chars_v;

    Value() {
        type = ValT::None;
    }

    Value(int v) {
        type = ValT::Int;
        int_v = v;
    }

    void debug() {
        std::cout << int_v;
    }
};

class Storage {
   public:
    Value &load(std::string_view symbol) {
        return storage_[symbol];
    }
    void store(std::string_view symbol, Value value) {
        storage_.insert({ symbol, value });
    }

    void debug() {
        for (auto &[key, value] : storage_) {
            std::cout << key << ":";
            value.debug();
            std::cout << std::endl;
        }
    }

   private:
    std::unordered_map<std::string_view, Value> storage_;
};

class VM {
   public:
    int run(AstRootNode &program) {
        for (const std::unique_ptr<AstStatementNode> &stmt : program.childs) {
            storage_.store(stmt->left_value_->name, Value(stmt->expr_->int_v));
        }

        return 0;
    }
    std::vector<void *> call_stack;
    void memory_used();
    void gc();

    Storage storage_;
};

int main() {
    std::optional<TokenStream> stream = TokenStream::from_file("test.txt");

    for (auto tk = stream->forward(); tk.tk_type != TK_EOF; tk = stream->forward()) {
        tk.debug();
        if (tk.tk_type == TK_ENDL) {
            std::cout << std::endl;
        }
    }

    std::cout << "parse finished" << std::endl;
    stream->back(0);
    AstRootNode ast;
    ast.parse(&stream.value());
    ast.debug();

    VM vm;
    vm.run(ast);

    vm.storage_.debug();
}
