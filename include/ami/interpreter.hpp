#pragma once
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "parser.hpp"

namespace ami {
class Interpreter {
    std::map<std::string, double> builtin{{"pi", M_PI},
                                          {"eu", M_E},
                                          {"tau", M_PI * 2},
                                          {"inf", INFINITY},
                                          {"nan", NAN}};
    std::map<std::string, double>* userdefined;
    using u_ptr = std::shared_ptr<Expr>;
    Number m_VisitAdd(BinaryOpExpr* boe) {
        return Number(visit((boe->lhs)).val + visit((boe->rhs)).val);
    }
    Number m_VisitSub(BinaryOpExpr* boe) {
        return Number(visit((boe->lhs)).val - visit((boe->rhs)).val);
    }
    Number m_VisitDiv(BinaryOpExpr* boe) {
        return Number(visit((boe->lhs)).val / visit((boe->rhs)).val);
    }
    Number m_VisitMult(BinaryOpExpr* boe) {
        return Number(visit((boe->lhs)).val * visit((boe->rhs)).val);
    }
    Number m_VisitIdent(Identifier* ident) {
        bool is_negative = (ident->name[0] == '-');
        std::string name = ident->name;
        if (is_negative && name.size() > 1) {
            name = std::string(name.begin() + 1, name.end());
        }
        if (builtin.find(name) != builtin.end()) {
            if (is_negative)
                return Number(-(builtin.at(name)));
            else
                return Number(builtin.at(name));
        } else if ((!userdefined->empty()) &&
                   (userdefined->find(name) != userdefined->end())) {
            if (is_negative)
                return Number(-userdefined->at(name));
            else
                return Number(userdefined->at(name));
        } else {
            throw std::runtime_error(
                std::string("use of undeclared identifier ") + name);
        }
    }
    Number m_VisitUserDefinedIdentifier(UserDefinedIdentifier* udi) {
        bool is_builtin = builtin.find(udi->name) != builtin.end();
        if (is_builtin) {
            throw std::runtime_error("Can't assign to built-in identifier \"" +
                                     udi->name + "\"");
        } else {
            userdefined->operator[](udi->name) = visit((udi->value)).val;
            return Number(0);
        }
    }
    Number m_VisitMod(BinaryOpExpr* boe) {
        return Number(std::fmod(visit((boe->lhs)).val, visit((boe->rhs)).val));
    }
    Number m_VisitPow(BinaryOpExpr* boe) {
        return Number(std::pow(visit((boe->lhs)).val, visit((boe->rhs)).val));
    }
    Number m_VisitNumber(Number* num) { return Number(num->val); }

   public:
    explicit Interpreter(std::map<std::string, double>* scope = nullptr)
        : userdefined(scope) {}
    Interpreter() = default;
    Number visit(u_ptr expr) {
        std::cout << expr->str() << '\n';
        switch (expr->type()) {
            default: {
                throw std::runtime_error("invalid expr");
            }
            case AstType::BinaryOp: {
                BinaryOpExpr* bopexpr = static_cast<BinaryOpExpr*>(expr.get());
                switch (bopexpr->op) {
                    default:
                        throw std::runtime_error("invalid operator");
                        break;
                    case Op::Plus:
                        return m_VisitAdd(bopexpr);
                    case Op::Minus:
                        return m_VisitSub(bopexpr);
                    case Op::Div:
                        return m_VisitDiv(bopexpr);
                    case Op::Mult:
                        return m_VisitMult(bopexpr);
                    case Op::Pow:
                        return m_VisitPow(bopexpr);
                    case Op::Mod:
                        return m_VisitMod(bopexpr);
                }
            }
            case AstType::Number: {
                return m_VisitNumber(static_cast<Number*>(expr.get()));
            }
            case AstType::Identifier: {
                return m_VisitIdent(static_cast<Identifier*>(expr.get()));
            }
            case AstType::UserDefinedIdentifier: {
                return m_VisitUserDefinedIdentifier(
                    static_cast<UserDefinedIdentifier*>(expr.get()));
            }
        }
    }
    ~Interpreter() = default;
};
}  // namespace ami