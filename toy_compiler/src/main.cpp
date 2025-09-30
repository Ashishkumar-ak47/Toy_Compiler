\
    #include <bits/stdc++.h>
    using namespace std;

    // Toy compiler: lexer, recursive-descent parser, AST, constant folding, TAC generation.
    // Compact single-file design for demonstration / interview.

    enum TokenType {TOK_EOF, TOK_ID, TOK_NUM, TOK_PLUS, TOK_MINUS, TOK_MUL, TOK_DIV, TOK_LP, TOK_RP, TOK_ASSIGN, TOK_SEMI};

    struct Token {
        TokenType type;
        string text;
    };

    struct Lexer {
        string s;
        size_t i = 0;
        Lexer(const string &src): s(src), i(0) {}
        char peek(){ return i < s.size() ? s[i] : '\\0'; }
        char get(){ return i < s.size() ? s[i++] : '\\0'; }
        void skip_ws(){ while(isspace(peek())) get(); }
        Token next_token(){
            skip_ws();
            char c = peek();
            if(c == '\\0') return {TOK_EOF, ""};
            if(isalpha(c)){
                string id;
                while(isalnum(peek()) || peek()=='_') id.push_back(get());
                return {TOK_ID, id};
            }
            if(isdigit(c)){
                string num;
                while(isdigit(peek())) num.push_back(get());
                return {TOK_NUM, num};
            }
            switch(get()){
                case '+': return {TOK_PLUS, "+"};
                case '-': return {TOK_MINUS, "-"};
                case '*': return {TOK_MUL, "*"};
                case '/': return {TOK_DIV, "/"};
                case '(': return {TOK_LP, "("};
                case ')': return {TOK_RP, ")"};
                case '=': return {TOK_ASSIGN, "="};
                case ';': return {TOK_SEMI, ";"};
                default: return next_token(); // ignore unknown characters
            }
        }
    };

    // AST
    struct Node {
        virtual ~Node() {}
    };
    struct Expr : Node {};
    struct Stmt : Node {};

    struct Num : Expr {
        long long val;
        Num(long long v): val(v) {}
    };
    struct Var : Expr {
        string name;
        Var(const string &n): name(n) {}
    };
    struct BinOp : Expr {
        char op;
        unique_ptr<Expr> left, right;
        BinOp(char o, unique_ptr<Expr> l, unique_ptr<Expr> r): op(o), left(move(l)), right(move(r)) {}
    };

    struct Assign : Stmt {
        string name;
        unique_ptr<Expr> expr;
        Assign(const string &n, unique_ptr<Expr> e): name(n), expr(move(e)) {}
    };

    // Parser (recursive descent)
    struct Parser {
        vector<Token> tokens;
        size_t pos = 0;
        Parser(const vector<Token>& tks): tokens(tks), pos(0) {}
        Token peek(){ return pos < tokens.size() ? tokens[pos] : Token{TOK_EOF,""}; }
        Token get(){ return pos < tokens.size() ? tokens[pos++] : Token{TOK_EOF,""}; }
        bool accept(TokenType tt){ if(peek().type==tt){ get(); return true;} return false; }
        bool expect(TokenType tt){
            if(accept(tt)) return true;
            cerr << "Parse error: expected token " << tt << "\\n"; exit(1);
        }

        unique_ptr<Expr> parse_primary(){
            Token t = peek();
            if(accept(TOK_NUM)){
                return make_unique<Num>(stoll(t.text));
            } else if(accept(TOK_ID)){
                return make_unique<Var>(t.text);
            } else if(accept(TOK_LP)){
                auto e = parse_expr();
                expect(TOK_RP);
                return e;
            } else {
                cerr << "Parse error: unexpected token in primary\\n"; exit(1);
            }
        }

        unique_ptr<Expr> parse_term(){
            auto left = parse_primary();
            while(true){
                if(accept(TOK_MUL)){
                    auto right = parse_primary();
                    left = make_unique<BinOp>('*', move(left), move(right));
                } else if(accept(TOK_DIV)){
                    auto right = parse_primary();
                    left = make_unique<BinOp>('/', move(left), move(right));
                } else break;
            }
            return left;
        }

        unique_ptr<Expr> parse_expr(){
            auto left = parse_term();
            while(true){
                if(accept(TOK_PLUS)){
                    auto right = parse_term();
                    left = make_unique<BinOp>('+', move(left), move(right));
                } else if(accept(TOK_MINUS)){
                    auto right = parse_term();
                    left = make_unique<BinOp>('-', move(left), move(right));
                } else break;
            }
            return left;
        }

        vector<unique_ptr<Stmt>> parse_all(){
            vector<unique_ptr<Stmt>> stmts;
            while(peek().type != TOK_EOF){
                Token t = peek();
                if(t.type == TOK_ID){
                    // assignment
                    Token id = get();
                    expect(TOK_ASSIGN);
                    auto expr = parse_expr();
                    expect(TOK_SEMI);
                    stmts.push_back(make_unique<Assign>(id.text, move(expr)));
                } else if(t.type == TOK_SEMI){
                    get(); // skip empty
                } else {
                    cerr << "Parse error: unexpected token at top-level\\n"; exit(1);
                }
            }
            return stmts;
        }
    };

    // Optimization: constant folding
    unique_ptr<Expr> const_fold(unique_ptr<Expr> e){
        if(auto b = dynamic_cast<BinOp*>(e.get())){
            b->left = const_fold(move(b->left));
            b->right = const_fold(move(b->right));
            if(auto L = dynamic_cast<Num*>(b->left.get())){
                if(auto R = dynamic_cast<Num*>(b->right.get())){
                    long long lv = L->val, rv = R->val;
                    long long res;
                    switch(b->op){
                        case '+': res = lv + rv; break;
                        case '-': res = lv - rv; break;
                        case '*': res = lv * rv; break;
                        case '/': res = rv!=0 ? lv / rv : 0; break;
                        default: res = 0;
                    }
                    return make_unique<Num>(res);
                }
            }
            return e;
        } else {
            return e;
        }
    }

    // TAC generator
    struct TACGen {
        vector<string> code;
        int tmp = 0;
        string newtmp(){ return "t" + to_string(++tmp); }
        string gen_expr(Expr* e){
            if(auto n = dynamic_cast<Num*>(e)){
                string t = newtmp();
                code.push_back(t + " = " + to_string(n->val));
                return t;
            } else if(auto v = dynamic_cast<Var*>(e)){
                return v->name;
            } else if(auto b = dynamic_cast<BinOp*>(e)){
                string L = gen_expr(b->left.get());
                string R = gen_expr(b->right.get());
                string t = newtmp();
                code.push_back(t + " = " + L + " " + b->op + " " + R);
                return t;
            }
            return "";
        }
        void gen_stmt(Stmt* s){
            if(auto a = dynamic_cast<Assign*>(s)){
                // fold constants first, but assume caller already did folding
                string r = gen_expr(a->expr.get());
                code.push_back(a->name + " = " + r);
            }
        }
    };

    int main(int argc, char** argv){
        ios::sync_with_stdio(false);
        cin.tie(nullptr);
        if(argc < 2){
            cerr << "Usage: " << argv[0] << " <input-file>\\n";
            return 1;
        }
        string inpath = argv[1];
        ifstream ifs(inpath);
        if(!ifs){ cerr << "Cannot open " << inpath << "\\n"; return 1; }
        string src((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());

        // Lex
        Lexer lex(src);
        vector<Token> toks;
        while(true){
            Token t = lex.next_token();
            toks.push_back(t);
            if(t.type == TOK_EOF) break;
        }

        // Parse
        Parser p(toks);
        auto stmts = p.parse_all();

        // Optimize (constant folding)
        for(auto &s : stmts){
            if(auto as = dynamic_cast<Assign*>(s.get())){
                as->expr = const_fold(move(as->expr));
            }
        }

        // Generate TAC
        TACGen gen;
        for(auto &s : stmts) gen.gen_stmt(s.get());

        // Print TAC (clean: avoid temporary definitions that are just immediate assignments)
        for(auto &line : gen.code){
            cout << line << '\\n';
        }
        return 0;
    }
