﻿#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "Token.hpp"
#include "TokenList.hpp"


namespace spp::frontend
{
    enum class ENodeType
    {
        Unknown,
        BinaryOp,
        StringLiteral,
        NumericLiteral,
        NullLiteral,
        ListLiteral,
        Function,
        Statement,
        Module,
        Return,
        Identifier,
        CreateAndAssign,
        Assign,
        Call,
        BooleanLiteral,
        When,
        Scope,
        For,
        While,
        Break,
        Continue,
        Class,
        Access,
        Index,
        Throw,
        TryCatch,
        NoOp
    };

    

    struct Node
    {
        ENodeType type = ENodeType::Unknown;
        
        TokenDebugInfo debugInfo;

        // May be used for optimization later
        bool isStatic = false;

        Node() = default;

        Node(const TokenDebugInfo& inDebugInfo);

        Node(const TokenDebugInfo& inDebugInfo,ENodeType inType);
        
        virtual ~Node() = default;
    };

    struct HasLeft : Node
    {
        std::shared_ptr<Node> left;
        HasLeft(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inLeft);
    };

    enum class EBinaryOp
    {
        Divide,
        Multiply,
        Add,
        Subtract,
        Mod,
        And,
        Or,
        Not,
        Equal,
        NotEqual,
        Less,
        LessEqual,
        Greater,
        GreaterEqual,
            
    };
    
    struct BinaryOpNode : HasLeft
    {
        
        
        std::shared_ptr<Node> right;
        EBinaryOp op;

        static EBinaryOp TokenTypeToBinaryOp(const ETokenType& tokType);
        
        BinaryOpNode(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inLeft, const std::shared_ptr<Node>& inRight, const EBinaryOp& inOp);
        

        BinaryOpNode(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inLeft, const std::shared_ptr<Node>& inRight, const ETokenType& inOp);
        
    };

    struct NumericLiteralNode : Node
    {
        std::string value;

        NumericLiteralNode(const TokenDebugInfo& inDebugInfo,const std::string& inValue);
    };

    struct BooleanLiteralNode : Node
    {
        bool value;

        BooleanLiteralNode(const TokenDebugInfo& inDebugInfo,bool inValue);
    };
    
    struct StringLiteralNode : Node
    {
        std::string value;

        StringLiteralNode(const TokenDebugInfo& inDebugInfo,const std::string& inValue);
    };

    struct NullLiteralNode : Node
    {
        NullLiteralNode(const TokenDebugInfo& inDebugInfo);
    };

    struct ListLiteralNode : Node
    {
        std::vector<std::shared_ptr<Node>> values;

        ListLiteralNode(const TokenDebugInfo& inDebugInfo,const std::vector<std::shared_ptr<Node>>& inValues);
    };

    struct CreateAndAssignNode : Node
    {
        std::string name;
        std::shared_ptr<Node> value;
        CreateAndAssignNode(const TokenDebugInfo& inDebugInfo,const std::string& inName,const std::shared_ptr<Node>& inValue);
        
    };

    struct AssignNode : HasLeft
    {
        std::shared_ptr<Node> value;
        AssignNode(const TokenDebugInfo& inDebugInfo,const  std::shared_ptr<Node>& inTarget,const std::shared_ptr<Node>& inValue);
        
    };

    struct NoOpNode : Node
    {
        NoOpNode();
        
    };

    struct IdentifierNode : Node
    {
        std::string value;

        IdentifierNode(const TokenDebugInfo& inDebugInfo,const std::string& inValue);
        
    };

    struct ScopeNode : Node
    {
        std::vector<std::shared_ptr<Node>> statements;

        ScopeNode(const TokenDebugInfo& inDebugInfo,const std::vector<std::shared_ptr<Node>>& inStatements = {});
        
    };

    struct AccessNode : HasLeft
    {
        std::shared_ptr<Node> right;

        AccessNode(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inLeft,const std::shared_ptr<Node>& inRight);
        
    };

    struct IndexNode : HasLeft
    {
        std::shared_ptr<Node> within;

        IndexNode(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inLeft,const std::shared_ptr<Node>& inWithin);
        
    };
    
    struct WhenNode : Node
    {
        
        struct Branch
        {
            std::shared_ptr<Node> expression;
            std::shared_ptr<Node> statement;
        };
        
        std::vector<Branch> branches;

        WhenNode(const TokenDebugInfo& inDebugInfo,const std::vector<Branch>& inBranches);
        
    };

    struct ReturnNode : Node
    {
        std::shared_ptr<Node> expression;

        ReturnNode(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inExpression);
    };

    struct ThrowNode : Node
    {
        std::shared_ptr<Node> expression;

        ThrowNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inExpression);
    };

    struct TryCatchNode : Node
    {
        std::shared_ptr<ScopeNode> tryScope;
        std::shared_ptr<ScopeNode> catchScope;
        std::string catchArgumentName;

        TryCatchNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<ScopeNode>& inTryScope,const std::shared_ptr<ScopeNode>& inCatchScope,const std::string& inCatchArgName);
    };

    struct FunctionNode : Node
    {
        std::string name;
        std::vector<std::string> args;
        std::shared_ptr<ScopeNode> body;

        FunctionNode(const TokenDebugInfo& inDebugInfo,const std::string& inName, const std::vector<std::string>& inArgs,const std::shared_ptr<ScopeNode>& inBody);
        
    };

    struct CallNode : HasLeft
    {
        std::vector<std::shared_ptr<Node>> args;

        CallNode(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inLeft,const std::vector<std::shared_ptr<Node>>& inArgs);
        
    };

    struct ForNode : Node
    {
        std::shared_ptr<Node> init;
        std::shared_ptr<Node> condition;
        std::shared_ptr<Node> update;
        std::shared_ptr<ScopeNode> body;

        ForNode(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inInit,const std::shared_ptr<Node>& inCondition,const std::shared_ptr<Node>& inUpdate,const std::shared_ptr<ScopeNode>& inBody);
        
    };

    struct WhileNode : Node
    {
        std::shared_ptr<Node> condition;
        std::shared_ptr<ScopeNode> body;

        WhileNode(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inCondition,const std::shared_ptr<ScopeNode>& inBody);
        
    };

    struct PrototypeNode : Node
    {
        std::string id;
        std::vector<std::string> parents;
        std::shared_ptr<ScopeNode> scope;

        PrototypeNode(const TokenDebugInfo& inDebugInfo,const std::string& inId,const std::vector<std::string>& inParents,const std::shared_ptr<ScopeNode>& inScope);
        
    };
    

    struct ModuleNode : Node
    {
        std::vector<std::shared_ptr<Node>> statements;

        ModuleNode(const TokenDebugInfo& inDebugInfo,const std::vector<std::shared_ptr<Node>>& inStatements);
        
    };

    void parseFunctionBody(TokenList &tokens,std::vector<std::shared_ptr<Node>>& body);

    std::shared_ptr<Node> parseParen(TokenList &tokens);
    
    std::shared_ptr<ListLiteralNode> parseList(TokenList &tokens);
    
    std::shared_ptr<Node> parsePrimary(TokenList &tokens);

    std::shared_ptr<Node> parseAccessors(TokenList &tokens);
    
    std::shared_ptr<Node> parseMultiplicativeExpression(TokenList &tokens);

    std::shared_ptr<Node> parseAdditiveExpression(TokenList &tokens);

    std::shared_ptr<Node> parseComparisonExpression(TokenList &tokens);

    std::shared_ptr<Node> parseLogicalExpression(TokenList &tokens);

    std::shared_ptr<Node> parseAssignmentExpression(TokenList &tokens);
    
    std::shared_ptr<Node> parseExpression(TokenList &tokens);

    std::shared_ptr<ReturnNode> parseReturn(TokenList &tokens);

    std::shared_ptr<ForNode> parseFor(TokenList &tokens);

    std::shared_ptr<WhileNode> parseWhile(TokenList &tokens);

    std::shared_ptr<TryCatchNode> parseTryCatch(TokenList& tokens);
    
    std::shared_ptr<Node> parseStatement(TokenList &tokens);

    std::vector<std::string> parseFunctionArguments(TokenList &tokens);
    
    std::shared_ptr<FunctionNode> parseFunction(TokenList &tokens);
    

    std::vector<std::shared_ptr<Node>> parseCallArguments(TokenList &tokens);
    
    std::shared_ptr<WhenNode> parseWhen(TokenList &tokens);
    
    std::shared_ptr<PrototypeNode> parseClass(TokenList& tokens);

    void getStatementTokens(TokenList& statement, TokenList& tokens);
    // Gets tokens till end (scope aware)
    void getTokensTill(TokenList& result,TokenList& tokens,const std::function<bool(const Token&,int)>& evaluator,int initialScope = 0,bool popEnd = true);
    void getTokensTill(TokenList& result,TokenList& tokens,const std::set<ETokenType>& ends,int initialScope = 0,bool popEnd = true);
    std::shared_ptr<ScopeNode> parseScope(TokenList& tokens);
    std::shared_ptr<ModuleNode> parse(TokenList tokens);
}
