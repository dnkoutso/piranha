#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Lex/Lexer.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;

class FindNamedClassVisitor
  : public RecursiveASTVisitor<FindNamedClassVisitor> {
public:
  explicit FindNamedClassVisitor(ASTContext *Context)
    : Context(Context) {}

  // bool VisitObjCObjectPointerType(ObjCObjectPointerType *type) {
  //   llvm::outs() << " VISITING TYPE bbbb "
  //                << " shit : " << type->getObjectType()->getBaseType().getTypePtr()->getTypeClassName() << "\n";
  //   // type->dump();
  //   return true;
  // }

  bool VisitParmVarDecl(ParmVarDecl *var) {
    std::string varName = var->getQualifiedNameAsString();
    std::string varType = var->getType().getAsString();
    std::string varOriginalType = var->getOriginalType().getAsString();

    var->getOriginalType().dump();

    // llvm::outs() << " Found variable declaration: " << varName << " of type " << varType << " of var type " << varOriginalType << "\n"
    //              << " canonicalType: " << var->getCanonicalDecl()->getType().getAsString() << "\n"
    //              << " definition: " << var->getDefinition()->getType().getAsString() << "\n"
    //              << " maybeshit: " << QualType::getAsString(var->getType().split()) << "\n";

                 // << " decl2str: " << this->decl2str(var, Context->getSourceManager()) << "\n"
                 // << " get_source_text " << this->get_source_text(var->getCanonicalDecl()->getSourceRange(), Context->getSourceManager()) << "\n";
    return true;
  }

  std::string decl2str(Decl *d, SourceManager &sm) {
    // (T, U) => "T,,"
    std::string text = Lexer::getSourceText(CharSourceRange::getTokenRange(d->getSourceRange()), sm, LangOptions(), 0);
    if (text.size() > 0 && (text.at(text.size()-1) == ',')) //the text can be ""
        return Lexer::getSourceText(CharSourceRange::getCharRange(d->getSourceRange()), sm, LangOptions(), 0);
    return text;
  }

  std::string get_source_text(SourceRange range, const SourceManager& sm) {
    // NOTE: sm.getSpellingLoc() used in case the range corresponds to a macro/preprocessed source.
    auto start_loc = sm.getSpellingLoc(range.getBegin());
    auto last_token_loc = sm.getSpellingLoc(range.getEnd());
    auto end_loc = clang::Lexer::getLocForEndOfToken(last_token_loc, 0, sm, Context->getLangOpts());
    auto printable_range = clang::SourceRange{start_loc, end_loc};
    return get_source_text_raw(printable_range, sm);
  }

  std::string get_source_text_raw(SourceRange range, const SourceManager& sm) {
    return clang::Lexer::getSourceText(clang::CharSourceRange::getCharRange(range), sm, Context->getLangOpts());
  }
  // std::string decl2str(Decl *d) {
  //   SourceLocation b(d->getLocStart()), _e(d->getLocEnd());
  //   SourceLocation e(clang::Lexer::getLocForEndOfToken(_e, 0, *sm, lopt));
  //   return std::string(sm->getCharacterData(b),
  //       sm->getCharacterData(e)-sm->getCharacterData(b));
  // }


//   std::string decl2str(Decl *d) {
//     SourceLocation b(d->getBeginLoc()), _e(d->getEndLoc());
//     SourceLocation e(clang::Lexer::getLocForEndOfToken(_e, 0, *Context.getSourceManager(), *Context.getLangOpts));
//     return std::string(Context.getSourceManager()->getCharacterData(b),
//         Context.getSourceManager()->getCharacterData(e)-Context.getSourceManager()->getCharacterData(b));
// }

private:
  ASTContext *Context;
};

class FindNamedClassConsumer : public clang::ASTConsumer {
public:
  explicit FindNamedClassConsumer(ASTContext *Context)
    : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  FindNamedClassVisitor Visitor;
};

class FindNamedClassPluginAction : public PluginASTAction {
  std::set<std::string> ParsedTemplates;
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return std::make_unique<FindNamedClassConsumer>(&CI.getASTContext());
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }

  void PrintHelp(llvm::raw_ostream& ros) {
    ros << "Help for OUR OWN STUFF!!!\n";
  }
};


static FrontendPluginRegistry::Add<FindNamedClassPluginAction>
X("find-named-square", "its uS!!!");

