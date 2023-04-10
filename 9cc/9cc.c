#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tokenの種類を定義
typedef enum
{
  TK_RESERVED, // 記号
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを示すトークン
} TokenKind;

// Token型を定義
typedef struct Token Token;

struct Token
{
  TokenKind kind; // Tokenの種類
  Token *next;    // 次のトークン
  int val;        // もしTK_NUMだったらここに整数が入る
  char *str;      // Token文字列
};

//ここからトークンについてのコード
// user_inputを定義
char *user_input;

// 現在のトークンを定義
Token *token;

/*
error関数

このコードは、エラーを報告するための関数 error を定義しています。この関数は、引数として char* 型のフォーマット文字列 fmt と可変長引数を受け取ります。

va_list 型の変数 ap を宣言し、va_start マクロを使って可変長引数の先頭のアドレスを取得します。

次に、可変長引数をフォーマット文字列に埋め込んで、その結果を stderr に出力するために、vfprintf 関数を呼び出します。

最後に、エラーメッセージの末尾に改行文字を追加し、exit 関数を呼び出してプログラムを終了します。引数として渡された整数値 1 は、プログラムが異常終了したことを示します。
*/
void error(char *fmt, ...) 
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

/*
error_at関数
この関数は、エラーがあった場所を指定してエラーメッセージを出力するために使用されます。
*/ 

void error_at(char *loc, char *fmt, ...)
/*
char *loc: エラーが発生した場所を示すポインタ。
char *fmt, ...: エラーメッセージのフォーマット指定文字列。
*/
{
  va_list ap;                           // 可変長引数のリスト
  va_start(ap, fmt);                    // va_listを初期化する。可変引数の処理を行う前に、必ず呼び出す必要がある。
  int pos = loc - user_input;           // エラーが発生した場所から、入力文字列の先頭までの文字数を計算する。
  fprintf(stderr, "%s\n", user_input);  // 入力文字列全体を出力する。
  fprintf(stderr, "%*s", pos, "");      // %*s はフォーマット指定文字列で、 * の部分は、次の可変引数の値で幅を指定するために使用されます。つまり、pos個の空白を出力します。 
  fprintf(stderr, "^ ");                // エラーの箇所を表す ^ を出力します。
  vfprintf(stderr, fmt, ap);            // 可変引数リストから引数を読み取り、指定された書式で出力します。
  fprintf(stderr, "\n");                // 改行を出力します。
  exit(1);                              // プログラムを異常終了させます。
}

/*
consume関数

このコードは、現在指しているトークンが与えられた演算子である場合にはそのトークンを消費して true を返し、そうでない場合には何もせずに false を返す関数 consume を定義しています。

具体的には、トークンが予約語でないか、かつその文字列の最初の文字が与えられた演算子でない場合には false を返します。そうでない場合には、トークンを一つ進めて、true を返します。

この関数を使うことで、次に来るトークンが期待する演算子であるかどうかを判定し、期待する演算子である場合にはトークンを消費することができます。
*/
bool consume(char op)
{
  if (token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

/*
expect関数

この関数は、次のトークンが特定の記号であることを確認します。もし次のトークンがその記号でなければエラーを報告します。

具体的には、まずトークンの種類が TK_RESERVED （予約語）であることをチェックします。次に、トークンの文字列の最初の文字が引数の記号であることを確認します。もし条件を満たさなければ、エラーを報告してプログラムを終了します。

例えば、以下のように使います。

void parse_add() {
  expect('+');
  // '+' が次のトークンであることが確定したので、次のトークンに進む
  // ... 以降の処理 ...
}

この例では、'+' が次のトークンでない場合にはエラーが報告され、プログラムが終了します。
*/
void expect(char op)
{
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error_at(token->str, "expected '%c'", op);
  token = token->next;
}

/*
expect_number関数
この関数は、現在のトークンが数値であることを期待し、その値を返します。もしトークンが数値でない場合は、エラーを報告してプログラムを終了します。

if (token->kind != TK_NUM)
現在のトークンの種類が、数値でない場合を判定しています。

error("数ではありません");

int val = token->val;
トークンが数値である場合は、その値を val 変数に代入します。

token = token->next;
トークンを1つ進めて、次のトークンに移ります。

return val;
数値の値を返します。
*/
int expect_number()
{
  if (token->kind != TK_NUM)                    // 現在のトークンの種類が、数値でない場合を判定しています。
    error_at(token->str, "expected a number");  // トークンが数値でない場合は、エラーメッセージを出力してプログラムを終了します。
  int val = token->val;                         // 現在のトークンの種類が、数値でない場合を判定しています。
  token = token->next;                          // トークンを1つ進めて、次のトークンに移ります。
  return val;                                   //
}

/*
at_eof関数
現在着目しているトークンがファイルの終わりを表すトークンであるかどうかを判定する関数です。

TK_EOFは、トークンの種類を表す列挙型TokenKindの一つで、入力の終わりを表すトークンであることを示します。

関数at_eof()は、現在着目しているトークンがTK_EOFである場合に真を返し、そうでない場合に偽を返します。
具体的には、tokenポインタが指すトークンのkindメンバがTK_EOFであるかどうかを判定しています。
つまり、トークン列の最後に到達したかどうかを判定するための関数です。
*/
bool at_eof()
{
  return token->kind == TK_EOF;
}

/*
*new_token関数

新しいトークンを作成してリンクドリストに追加するための関数です。トークンには、トークンの種類（kind）、トークンの文字列（str）などの情報が含まれます。

具体的には、この関数では、まず、新しいトークンのメモリを確保しています。
calloc関数は、指定されたサイズのメモリを確保し、すべてのビットをゼロで初期化する関数です。
次に、新しいトークンの種類と文字列を設定し、現在のトークンの次のトークンとしてリンクドリストに追加しています。
最後に、新しいトークンへのポインタを返しています。

この関数は、プログラムの字句解析器（lexer）で使用されます。字句解析器は、プログラムのソースコードを読み込み、トークンに分割します。
各トークンは、プログラムの文法規則に基づいて、解析のためにパーサー（parser）に渡されます。
*/

Token *new_token(TokenKind kind, Token *cur, char *str)
{
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

Token *tokenize()
{
  char *p = user_input;                        // トークナイズ対象の文字列を指すポインタ
  Token head;                                  // トークンのリストの先頭を示すダミーノード
  head.next = NULL;
  Token *cur = &head;                          // 現在のトークンを示すポインタ

  while (*p)                                   // 文字列の最後まで繰り返す
  {
    if (isspace(*p))                           // 空白文字をスキップ
    {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-'){                // 加算演算子をトークンとして追加
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p))                           // 整数をトークンとして追加
    {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error_at(p, "expected a number");          // それ以外の場合はエラーを出力して終了
  }

  new_token(TK_EOF, cur, p);                   // リストの最後にEOFトークンを追加してリストを完了
  return head.next;                            // トークンリストの先頭を返す
}



// 抽象構文木のノードの種類
typedef enum
{
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM  // 整数
} NodeKind;

typedef struct Node Node;

// 抽象構文木ノードの型
struct Node
{
  NodeKind kind; // ノードの型
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺
  int val;       // kindがND_NUMの場合のみ使う
};

// プロトタイプ宣言
Node *mul();
Node *expr();
Node *primary();

// 新しいノードを作成する関数
// 2項演算子用
Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}
// 数値用
Node *new_node_num(int val)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *mul(){
  Node *node = primary();

  for(;;){
    if(consume('*'))
      node = new_node(ND_MUL,node,primary());
    else if(consume('/'))
      node = new_node(ND_DIV,node,primary());
    else
      return node;
  }
}

Node *expr()
{
  Node *node = mul();

  for (;;)
  {
    if (consume('+'))
      node = new_node(ND_ADD, node, mul());
    else if (consume('-'))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *primary() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume('(')) {
    Node *node = expr();
    expect(')');
    return node;
  }

  // そうでなければ数値のはず
  return new_node_num(expect_number());
}


//スタックを利用した四則演算の関数
void gen(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  }

  printf("  push rax\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  // トークナイズしてパースする
  user_input = argv[1];
  token = tokenize(user_input);
  Node *node = expr();

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // 抽象構文木を下りながらコード生成
  gen(node);

  // スタックトップに式全体の値が残っているはずなので
  // それをRAXにロードして関数からの返り値とする
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
